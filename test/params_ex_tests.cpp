#include "params_ex_tests.h"

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

    pm->save();
    pm->evaluate();
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
