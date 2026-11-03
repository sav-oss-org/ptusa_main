#include "param_ex.h"
#include "lua_manager.h"
#include "device/manager.h"

auto_smart_ptr< params_manager > params_manager::instance = 0;
char params_manager::is_init = 0;

#ifdef USE_SIMPLE_DEV_ERRORS
#include "g_errors.h"
#endif // USE_SIMPLE_DEV_ERRORS

#ifndef USE_NO_TANK_COMB_DEVICE
#include "tech_def.h"
#endif //USE_NO_TANK_COMB_DEVICE

#include "PAC_info.h"
#include "g_errors.h"

#include "log.h"

#include <chrono>
#include <cstring>
//-----------------------------------------------------------------------------
params_manager::params_manager(): par( 0 ), project_id( 0 )
    {
    last_idx = 0;

    CRC_mem = new SRAM( "./nvram.bin",
        static_cast<size_t>( CONSTANTS::C_SYS_MEM_SIZE ) );
    params_mem = new SRAM( "./eeprom.bin",
        static_cast<size_t>( CONSTANTS::C_TOTAL_PARAMS_SIZE ) );
    save_worker = std::thread( &params_manager::save_worker_loop, this );
    }
//-----------------------------------------------------------------------------
u_int_2 params_manager::solve_CRC()
    {
    char Flag;

    u_int_2 CRC = 65535;
    auto datlen = static_cast<unsigned int>( CONSTANTS::C_TOTAL_PARAMS_SIZE );
    int bufidx = 0;

    while ( datlen > 0 )
        {
        CRC = CRC ^ static_cast<u_int_2>( params_mem->get_data()[ bufidx ] );
        for ( int idx = 0; idx <= 7; idx++ )
            {
            Flag = CRC & 1;
            CRC = CRC >> 1;
            if ( Flag ) CRC = CRC ^ 0x0A001;
            }
        datlen--;
        bufidx++;
        }
    char* p = ( char* ) &project_id;
    CRC = CRC ^ p[ 0 ];
    for ( int idx = 0; idx <= 7; idx++ )
        {
        Flag = CRC & 1;
        CRC = CRC >> 1;
        if ( Flag ) CRC = CRC ^ 0x0A001;
        }
    CRC = CRC ^ p[ 1 ];
    for ( int idx = 0; idx <= 7; idx++ )
        {
        Flag = CRC & 1;
        CRC = CRC >> 1;
        if ( Flag ) CRC = CRC ^ 0x0A001;
        }

    return CRC;
    }
//-----------------------------------------------------------------------------
void params_manager::reset_CRC_mem()
    {
    CRC_mem->zero_fill();
    }
//-----------------------------------------------------------------------------
int params_manager::get_params_change_counter() const
    {
    return params_change_counter;
    }
//-----------------------------------------------------------------------------
int params_manager::get_params_save_counter() const
    {
    return params_save_counter;
    }
//-----------------------------------------------------------------------------
bool params_manager::was_successful_init() const
    {
    return successful_init;
    }
//-----------------------------------------------------------------------------
int params_manager::init( unsigned int project_id )
    {
    params_manager::project_id = project_id;

    auto res = params_mem->load_data();
    res += CRC_mem->load_data();

    successful_init = ( res == 0 );
    return res;
    }
//-----------------------------------------------------------------------------
void params_manager::final_init( int auto_init_params /*= 1*/,
                                int auto_init_work_params /*= 1*/,
                                void ( *custom_init_params_function )() /*= 0 */ )
    {
    sprintf( G_LOG->msg, "Total memory used: %u of %u bytes[ %.2f%c ].",
            last_idx, static_cast<unsigned int>( CONSTANTS::C_TOTAL_PARAMS_SIZE ),
            100. * last_idx / static_cast<float>( CONSTANTS::C_TOTAL_PARAMS_SIZE ), '%' );
    G_LOG->write_log( i_log::P_DEBUG );

    //Проверка на изменение количества параметров.
    u_int last_idx_{};
    std::memcpy( &last_idx_, CRC_mem->get_data(), sizeof( last_idx ) );
    if ( last_idx_ != last_idx )
        {
        G_LOG->notice( "Params count is changed (%d != %d), re-initialization.",
            last_idx_, last_idx );

        //Запись количества параметров.
        std::memcpy( CRC_mem->get_data(), &last_idx, sizeof( last_idx ) );

        reset_to_default( custom_init_params_function, auto_init_params,
            auto_init_work_params );
        }

    // Проверка контрольной суммы.
    u_int_2 saved_CRC{};
    constexpr std::size_t OFFSET = sizeof( last_idx );
    std::memcpy( &saved_CRC, CRC_mem->get_data() + OFFSET,
        sizeof( saved_CRC ) );
    auto solved_CRC = solve_CRC();
    if ( saved_CRC != solved_CRC )
        {
        G_LOG->notice(
            "Parameters CRC is not valid (saved %d != solved %d), "
            "re-initialization.",
            saved_CRC, solved_CRC );

        reset_to_default( custom_init_params_function, auto_init_params,
            auto_init_work_params );
        }
    }
//-----------------------------------------------------------------------------
void params_manager::reset_to_default( void( *custom_init_params_function )( ),
    int auto_init_params, int auto_init_work_params )
    {
    params_mem->zero_fill();

    if ( custom_init_params_function != 0 )
        {
        ( *custom_init_params_function )( );
        }

    PAC_info::get_instance()->reset_params();

    if ( auto_init_params )
        {
#ifndef USE_NO_TANK_COMB_DEVICE
        tech_object_manager::get_instance()->init_params();
#endif // USE_NO_TANK_COMB_DEVICE

        G_ERRORS_MANAGER->reset_errors_params();

        G_DEVICE_MANAGER()->init_params();
        }

    if ( auto_init_work_params )
        {
#ifndef USE_NO_TANK_COMB_DEVICE
        tech_object_manager::get_instance()->init_runtime_params();
#endif // USE_NO_TANK_COMB_DEVICE
        }

    par[ 0 ][ P_IS_RESET_PARAMS ] = 0;

    save();
#ifdef KEY_CONFIRM
    printf( "Press any key to continue..." );
    get_char();
    printf( "\n" );
#endif // KEY_CONFIRM
        }
//-----------------------------------------------------------------------------
void params_manager::save()
    {
    params_change_counter++;
    change_generation++;

    is_changed = true;
    last_change_ms = get_millisec();
    }
//-----------------------------------------------------------------------------
std::byte* params_manager::reserve_params_region( int size, int &start_pos )
    {
    if ( last_idx + size > params_mem->get_size() )
        {
        G_LOG->debug( "params_manager::reserve_params_region() - is not enough "
            "memory ( %d + %d > %d ) !",
            last_idx, size, params_mem->get_size() );

        return nullptr;
        }

    auto res = params_mem->get_data() + last_idx;
    start_pos = last_idx;
    last_idx += size;

    return res;
    }
//-----------------------------------------------------------------------------
params_manager* params_manager::get_instance()
    {
    if ( 0 == is_init )
        {
        is_init = 1;
        instance = new params_manager();

        instance->par = new saved_params_u_int_4( P_COUNT );
        }
    return instance;
    }
//-----------------------------------------------------------------------------
params_manager::~params_manager()
    {
    {
    std::lock_guard<std::mutex> lock( save_mutex );
    stop_save_worker = true;
    }
    save_cv.notify_one();
    if ( save_worker.joinable() ) save_worker.join();

    if ( params_mem )
        {
        delete params_mem;
        params_mem = nullptr;
        }
    if ( CRC_mem )
        {
        delete CRC_mem;
        CRC_mem = nullptr;
        }

    delete par;
    par = nullptr;
    }
//-----------------------------------------------------------------------------
int params_manager::save_params()
    {
    auto snapshot = std::unique_ptr<save_snapshot>( new save_snapshot );
    const auto CRC = solve_CRC();
    constexpr std::size_t OFFSET = sizeof( last_idx );
    std::memcpy( CRC_mem->get_data() + OFFSET, &CRC, sizeof( CRC ) );
    std::memcpy( snapshot->params.data(), params_mem->get_data(),
        snapshot->params.size() );
    std::memcpy( snapshot->crc_mem.data(), CRC_mem->get_data(),
        snapshot->crc_mem.size() );
    snapshot->generation = change_generation;

    {
    std::lock_guard<std::mutex> lock( save_mutex );
    // Keep only the newest snapshot while a previous one is being written.
    pending_save = std::move( snapshot );
    }
    save_cv.notify_one();

    return 0;
    }
//-----------------------------------------------------------------------------
void params_manager::save_worker_loop()
    {
    for ( ;; )
        {
        std::unique_ptr<save_snapshot> snapshot;
        {
        std::unique_lock<std::mutex> lock( save_mutex );
        save_cv.wait( lock, [this]
            { return stop_save_worker || pending_save != nullptr; } );
        if ( !pending_save ) return;
        snapshot = std::move( pending_save );
        save_in_progress = true;
        }

        save_result result;
        result.generation = snapshot->generation;
        std::error_code ec;
        const auto current_path = std::filesystem::current_path( ec );
        auto available = uintmax_t{};
        if ( !ec )
            available = std::filesystem::space( current_path, ec ).available;
        if ( ec || available < snapshot->params.size() )
            {
            result.error_stage = 1;
            result.error_code = ec ? ec.value() : 1;
            }
        else
            {
            auto start = std::chrono::steady_clock::now();
            result.error_code = CRC_mem->safe_save( snapshot->crc_mem.data() );
            result.crc_write_us = std::chrono::duration_cast<
                std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - start ).count();
            if ( result.error_code != 0 ) result.error_stage = 2;
            else
                {
                start = std::chrono::steady_clock::now();
                result.error_code = params_mem->safe_save(
                    snapshot->params.data() );
                result.params_write_us = std::chrono::duration_cast<
                    std::chrono::microseconds>(
                        std::chrono::steady_clock::now() - start ).count();
                if ( result.error_code != 0 ) result.error_stage = 3;
                }
            }

        {
        std::lock_guard<std::mutex> lock( save_mutex );
        completed_saves.push_back( result );
        save_in_progress = false;
        }
        }
    }
//-----------------------------------------------------------------------------
bool params_manager::save_is_pending() const
    {
    std::lock_guard<std::mutex> lock( save_mutex );
    return pending_save != nullptr || save_in_progress;
    }
//-----------------------------------------------------------------------------
void params_manager::collect_save_results()
    {
    std::deque<save_result> results;
    {
    std::lock_guard<std::mutex> lock( save_mutex );
    results.swap( completed_saves );
    }

    for ( const auto& result : results )
        {
        if ( result.error_stage != 0 )
            {
            save_failed = true;
            is_changed = true;
            last_failed_save_ms = get_millisec();
            const char* stage = result.error_stage == 1 ? "free space check" :
                result.error_stage == 2 ? "nvram.bin" : "eeprom.bin";
            G_LOG->error( "params_manager::save_params() - background save "
                "failed at %s (code %d).", stage, result.error_code );
            continue;
            }

        save_failed = false;
        last_save_ms = get_millisec();
        params_save_counter++;
        if ( result.generation == change_generation ) is_changed = false;
        if ( G_DEBUG )
            {
            G_LOG->debug( "SRAM::safe_save() - write time: %lld us (./nvram.bin).",
                result.crc_write_us );
            G_LOG->debug( "SRAM::safe_save() - write time: %lld us (./eeprom.bin).",
                result.params_write_us );
            G_LOG->debug( "params_mem::safe_save() - call %d",
                params_save_counter );
            }
        }
    }
//-----------------------------------------------------------------------------
int params_manager::evaluate()
    {
    collect_save_results();
    if ( is_changed && !save_is_pending() )
        {
        auto since_save = get_delta_millisec( last_save_ms );
        auto since_change = get_delta_millisec( last_change_ms );
        const auto min_interval =
             G_PAC_INFO()->par[ PAC_info::P_MIN_SAVE_INTERVAL_MS ];
        const auto stable_delay =
             G_PAC_INFO()->par[ PAC_info::P_STABLE_SAVE_DELAY_MS ];
        constexpr uint32_t RETRY_DELAY_MS = 5'000;

        // The first save must not be delayed by the regular minimum interval.
        // In particular, final_init() can successfully read the files and then
        // reject their parameter count or CRC. In that case reset_to_default()
        // marks the new defaults as changed, and they have to reach disk after
        // stable_delay instead of remaining invalid until min_interval expires.
        if ( ( params_save_counter == 0 || since_save >= min_interval ) &&
            since_change >= stable_delay &&
            ( !save_failed ||
                get_delta_millisec( last_failed_save_ms ) >= RETRY_DELAY_MS ) )
            {
            return save_params();
            }
        }

    return 1;
    }
//-----------------------------------------------------------------------------
int params_manager::save_params_as_Lua_str( char* str )
    {
    int res = 0;
    //res += G_DEVICE_MANAGER()->save_params_as_Lua_str( str );
    res += G_TECH_OBJECT_MNGR()->save_params_as_Lua_str( str + res );

    return res;
    }
//-----------------------------------------------------------------------------
int params_manager::restore_params_from_server_backup( char *backup_str )
    {
    static bool is_init = false;
    if ( false == is_init )
        {
        const char *extra_cmd = "function params ( par_info )\n"
            "\n"
            "local cmd\n"
            "--if par_info == nil then return\n"
            "\n"
            "for index, value in pairs( par_info.values ) do\n"
            "    cmd = 'sys.'..par_info.object..':set_param('..par_info.par_id..','..\n"
            "        ( index - 1 )..','..value..')'\n"
            "    assert( loadstring( cmd ) )()\n"
            "    end\n"
            "end\n";

        lua_manager::get_instance()->exec_Lua_str( extra_cmd,
            "params_manager::restore_params_from_server_backup - init block");

        is_init = true;
        }


    int res = lua_manager::get_instance()->exec_Lua_str( backup_str,
        "params_manager::restore_params_from_server_backup ");

    if ( 0 == res )
        {
        par[ 0 ][ P_IS_RESET_PARAMS ] = 0;
        par->save_all();
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int params_test::make_test()
    {
    if ( G_DEBUG )
        {
        printf( "Start params test.\n" );
        }

    const u_int POJECT_ID = 2;
    params_manager::get_instance()->init( POJECT_ID );

    saved_params_u_int_4 test1( 10 );
    //test1.save( 0, 5120 );
    //test1.save( 1, 120 );
    //test1.save( 2, 130 );

    saved_params_float test( 10 );
    //test.save( 0, 512 );
    //test.save( 1, 12 );
    //test.save( 2, 13 );

    params_manager::get_instance()->init( POJECT_ID );
    params_manager::get_instance()->final_init();

    if (
        test1[ 0 ] != 5120 ||
        test1[ 1 ] != 120 ||
        test1[ 2 ] != 130 ||
        test[ 0 ] != 512 ||
        test[ 1 ] != 12 ||
        test[ 2 ] != 13 )
        {
        if ( G_DEBUG )
            {
            printf( "Error passing params test!\n" );
            printf( "test[ 0 ] = %f\n", test[ 0 ] );
            printf( "test[ 1 ] = %f\n", test[ 1 ] );
            printf( "test[ 2 ] = %f\n", test[ 2 ] );

            printf( "test1[ 0 ] = %lu\n", ( u_long ) test1[ 0 ] );
            printf( "test1[ 1 ] = %lu\n", ( u_long ) test1[ 1 ] );
            printf( "test1[ 2 ] = %lu\n", ( u_long ) test1[ 2 ] );
            get_char();
            }
        return 1;
        }

    if ( G_DEBUG )
        {
        printf( "Passing params test - ok!\n" );

#ifdef KEY_CONFIRM
        printf( "Press any key to continue..." );
        get_char();
        printf( "\n" );
#endif // KEY_CONFIRM
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
