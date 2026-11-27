#include <benchmark/benchmark.h>
#include <clocale>

#include "g_device.h"
#include "lua_manager.h"
#include "log.h"

int G_DEBUG = 0;            // Вывод дополнительной отладочной информации.
bool G_NO_IO_NODES = true;  // По умолчанию обмен с модулями отключен.
bool G_READ_ONLY_IO_NODES = false;

lua_State* L = nullptr;
u_char in_data_devices[] = { device_communicator::CMD_GET_DEVICES };
u_char out_data[ 5 * 1024 ] = { 0 };

static void DoSetup( const benchmark::State& state )
    {
    static bool is_init = false;
    if ( !is_init )
        {
        is_init = true;
#ifdef WIN_OS
        setlocale( LC_ALL, "ru_RU.UTF-8" );
        setlocale( LC_NUMERIC, "C" );
#endif
        L = lua_open();             // Create Lua context.
        lua_gc( L, LUA_GCSTOP, 0 );
        luaL_openlibs( L );         // Open standard libraries.

        G_LUA_MANAGER->init( L, "main.plua", "", "./sys/" );
        }
    }

static void write_devices_service( benchmark::State& state, bool use_compression )
    {
    if ( use_compression ) device_communicator::switch_on_compression();
    else device_communicator::switch_off_compression();

    auto size = G_DEVICE_CMMCTR->write_devices_states_service( 1,
        in_data_devices, out_data );

    for ( auto _ : state )
        G_DEVICE_CMMCTR->write_devices_states_service( 1,
            in_data_devices, out_data );

    state.counters.insert( { {"Size", size} } );
    }

// Register the function as a benchmark
BENCHMARK_CAPTURE( write_devices_service, "no compression", false )->
    Setup( DoSetup )->Unit( benchmark::kMicrosecond );
BENCHMARK_CAPTURE( write_devices_service, "with compression", true )->
    Setup( DoSetup )->Unit( benchmark::kMicrosecond );


namespace
    {

/// @brief Sink that records nothing: the benchmark times the shared
/// write_log() funnel (sanitize_msg() included) without the platform
/// stdio and syslog latency, so runs stay comparable across the CI
/// matrix. The console rendering itself stays out of the gate.
class bench_log : public i_log
    {
    public:
        void write_log_impl( PRIORITIES priority ) override
            {
            benchmark::DoNotOptimize( priority );
            }
    };

    } // namespace

static void write_log_record( benchmark::State& state, bool injected )
    {
    bench_log log;

    const int len = static_cast< int >( state.range( 0 ) );
    for ( int i = 0; i < len; ++i )
        {
        log.msg[ i ] = ( injected && i % 10 == 0 ) ? '\\n'
                                                   : char( 65 + i % 26 );
        }
    log.msg[ len ] = 0;

    for ( auto _ : state )
        log.write_log( i_log::P_INFO );

    state.SetBytesProcessed(
        static_cast< long long >( state.iterations() ) * len );
    }

// Register the function as a benchmark
BENCHMARK_CAPTURE( write_log_record, "clean", false )
    ->Arg( 40 )->Arg( 200 )->Arg( 1000 )->Unit( benchmark::kNanosecond );
BENCHMARK_CAPTURE( write_log_record, "LF every 10th char", true )
    ->Arg( 40 )->Arg( 200 )->Arg( 1000 )->Unit( benchmark::kNanosecond );

BENCHMARK_MAIN();
