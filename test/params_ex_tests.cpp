#include "params_ex_tests.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

using namespace ::testing;

TEST( params_manager, was_successful_init )
    {
    auto pm = params_manager::get_instance();

    // По умолчанию инициализация параметров считается неуспешной,
    // так как не было вызова метода `init`.
    EXPECT_FALSE( pm->was_successful_init() );
    }

TEST( params_manager, evaluate )
    {
    auto pm = params_manager::get_instance();
    pm->init( 0x12345678 );
    pm->par->save( 1, 0xDEADBEEF );
    EXPECT_EQ( 0xDEADBEEF, pm->par[ 0 ][ params_manager::P_IS_RESET_PARAMS ] );

    // Первая запись после запуска не должна ждать минимальный интервал между
    // обычными записями. Это необходимо после сброса параметров из-за
    // несовпадения их количества или CRC.
    const auto stable_delay =
        G_PAC_INFO()->par[ PAC_info::P_STABLE_SAVE_DELAY_MS ];
    const auto min_interval =
        G_PAC_INFO()->par[ PAC_info::P_MIN_SAVE_INTERVAL_MS ];
    G_PAC_INFO()->par[ PAC_info::P_STABLE_SAVE_DELAY_MS ] = 0;
    G_PAC_INFO()->par[ PAC_info::P_MIN_SAVE_INTERVAL_MS ] = 3'600'000;
    const auto save_counter = pm->get_params_save_counter();

    pm->save();
    EXPECT_EQ( 0, pm->evaluate() );
    for ( int i = 0;
        i < 1000 && pm->get_params_save_counter() == save_counter; ++i )
        {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        pm->evaluate();
        }
    EXPECT_EQ( save_counter + 1, pm->get_params_save_counter() );

    G_PAC_INFO()->par[ PAC_info::P_STABLE_SAVE_DELAY_MS ] = stable_delay;
    G_PAC_INFO()->par[ PAC_info::P_MIN_SAVE_INTERVAL_MS ] = min_interval;
    }

namespace
    {
    class snapshot_memory : public i_memory
        {
        public:
            explicit snapshot_memory( size_t size, bool block_first_write = false ) :
                data( size ), block_first_write( block_first_write ) {}

            int load_data() override { return 0; }
            int safe_save() override { return safe_save( data.data() ); }
            int safe_save( const std::byte* source ) override
                {
                std::unique_lock<std::mutex> lock( mutex );
                if ( block_first_write && writes.empty() )
                    {
                    first_write_started = true;
                    changed.notify_all();
                    changed.wait( lock, [this] { return first_write_released; } );
                    }
                writes.emplace_back( source, source + data.size() );
                changed.notify_all();
                return 0;
                }
            u_int get_size() const override { return static_cast<u_int>( data.size() ); }
            void zero_fill() override { std::fill( data.begin(), data.end(), std::byte{} ); }
            std::byte* get_data() override { return data.data(); }

            bool wait_for_first_write()
                {
                std::unique_lock<std::mutex> lock( mutex );
                return changed.wait_for( lock, std::chrono::seconds( 2 ),
                    [this] { return first_write_started; } );
                }
            void release_first_write()
                {
                std::lock_guard<std::mutex> lock( mutex );
                first_write_released = true;
                changed.notify_all();
                }
            bool wait_for_writes( size_t count )
                {
                std::unique_lock<std::mutex> lock( mutex );
                return changed.wait_for( lock, std::chrono::seconds( 2 ),
                    [this, count] { return writes.size() >= count; } );
                }
            std::vector<std::vector<std::byte>> saved_copies()
                {
                std::lock_guard<std::mutex> lock( mutex );
                return writes;
                }

        private:
            std::vector<std::byte> data;
            bool block_first_write;
            bool first_write_started{};
            bool first_write_released{};
            std::mutex mutex;
            std::condition_variable changed;
            std::vector<std::vector<std::byte>> writes;
        };
    }

class test_params_manager
    {
    public:
        static std::unique_ptr<params_manager> create(
            i_memory* crc_memory, i_memory* params_memory )
            {
            auto manager = std::unique_ptr<params_manager>( new params_manager );
            delete manager->CRC_mem;
            delete manager->params_mem;
            manager->CRC_mem = crc_memory;
            manager->params_mem = params_memory;
            return manager;
            }
    };

TEST( params_manager, background_save_uses_snapshot_and_does_not_block )
    {
    auto crc_memory = new snapshot_memory( 10, true );
    auto params_memory = new snapshot_memory(
        static_cast<size_t>( params_manager::CONSTANTS::C_TOTAL_PARAMS_SIZE ) );
    auto manager = test_params_manager::create( crc_memory, params_memory );

    params_memory->get_data()[ 0 ] = std::byte{ 0x11 };
    manager->save();
    EXPECT_EQ( 0, manager->save_params() );
    const auto first_write_started = crc_memory->wait_for_first_write();
    if ( !first_write_started )
        {
        crc_memory->release_first_write();
        FAIL() << "Background writer did not start";
        }

    // The first disk write is still blocked; the forced request must return.
    params_memory->get_data()[ 0 ] = std::byte{ 0x22 };
    manager->save();
    auto forced = std::async( std::launch::async,
        [&manager] { return manager->save_params(); } );
    const auto ready = forced.wait_for( std::chrono::seconds( 1 ) );
    EXPECT_EQ( std::future_status::ready, ready );
    if ( ready == std::future_status::ready )
        EXPECT_EQ( 0, forced.get() );

    // A newer pending snapshot replaces the older pending request.
    params_memory->get_data()[ 0 ] = std::byte{ 0x33 };
    manager->save();
    EXPECT_EQ( 0, manager->save_params() );
    crc_memory->release_first_write();
    if ( ready != std::future_status::ready )
        EXPECT_EQ( 0, forced.get() );

    ASSERT_TRUE( params_memory->wait_for_writes( 2 ) );
    const auto copies = params_memory->saved_copies();
    ASSERT_GE( copies.size(), 2u );
    EXPECT_EQ( std::byte{ 0x11 }, copies[ 0 ][ 0 ] );
    EXPECT_EQ( std::byte{ 0x33 }, copies.back()[ 0 ] );
    EXPECT_EQ( 2u, copies.size() );

    for ( int i = 0;
        i < 1000 && manager->get_params_save_counter() < 2; ++i )
        {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        manager->evaluate();
        }
    EXPECT_GE( manager->get_params_save_counter(), 2 );
    }

TEST( params_manager, reserve_params_region )
    {
    auto pm = params_manager::get_instance();
    int start_pos;

    // Запрос большего количества параметров, чем есть в памяти. Результат
    // должен быть `nullptr`.
    auto data = pm->reserve_params_region(
        static_cast<int>( params_manager::CONSTANTS::C_TOTAL_PARAMS_SIZE ) + 1,
        start_pos );
    EXPECT_EQ( data, nullptr );
    }

namespace
    {
    FILE* bad_fopen( [[maybe_unused]] const char* filename,
        [[maybe_unused]] const char* mode )
        {
        return nullptr;
        }

    size_t bad_fread( [[maybe_unused]] void* ptr,
        [[maybe_unused]] size_t size,
        [[maybe_unused]] size_t count,
        [[maybe_unused]] FILE* stream )
        {
        return 0;
        }

    const auto DATA_SIZE = 1024;
    }

TEST( SRAM, constructor )
    {
    SRAM sram( "test_sram1.bin", DATA_SIZE );

    // Нет файла с параметрами, поэтому загрузка должна завершиться с ошибкой.
    EXPECT_EQ( 1, sram.load_data() );
    }

TEST( SRAM, load )
    {
    auto test_file = "test_sram2.bin";

    SRAM good_sram( test_file, DATA_SIZE );

    // Нет файла с параметрами, поэтому загрузка должна завершиться с ошибкой.
    EXPECT_EQ( 1, good_sram.load_data() );

    // Создаём файл с параметрами, чтобы проверить успешную загрузку.
    auto file = fopen( test_file, "wb" );
    fclose( file );

    // Подменяем функцию `fopen` на фиктивную, которая всегда возвращает
    // `nullptr`. Тест должен завершиться с ошибкой, так как файл не может
    // быть открыт.
    auto fopen_hook = subhook_new( reinterpret_cast<void*>( &fopen ),
        reinterpret_cast<void*>( &bad_fopen ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fopen_hook );
    EXPECT_EQ( 2, good_sram.load_data() );
    subhook_remove( fopen_hook );
    subhook_free( fopen_hook );

    // Подменяем функцию `fread` на фиктивную, которая всегда возвращает 0.
    // Тест должен завершиться с ошибкой, так как файл не может быть прочитан.
    auto fread_hook = subhook_new( reinterpret_cast<void*>( &fread ),
        reinterpret_cast<void*>( &bad_fread ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fread_hook );
    EXPECT_EQ( 3, good_sram.load_data() );
    subhook_remove( fread_hook );
    subhook_free( fread_hook );

    // Записываем данные в файл, чтобы проверить успешную загрузку.
    file = fopen( test_file, "wb" );
    std::array<std::byte, DATA_SIZE> data = {};
    fwrite( data.data(), sizeof( std::byte ), data.size(), file );
    fclose( file );

    EXPECT_EQ( 0, good_sram.load_data() );

    std::filesystem::remove( test_file );
    }

TEST( SRAM, safe_save )
    {
    SRAM good_sram( "test_sram3.bin", DATA_SIZE );

    auto fopen_hook = subhook_new( reinterpret_cast<void*>( &fopen ),
        reinterpret_cast<void*>( &bad_fopen ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fopen_hook );

    EXPECT_EQ( 1, good_sram.safe_save() );

    subhook_remove( fopen_hook );
    subhook_free( fopen_hook );
    }
