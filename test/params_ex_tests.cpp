#include "params_ex_tests.h"

#include "PAC_info.h"

#ifdef LINUX_OS
#include <cerrno>
#include <unistd.h>
#endif

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

TEST( params_manager, evaluate_saves_after_intervals )
    {
    auto pm = params_manager::get_instance();
    pm->init( 0x12345678 );

    // Интервалы сохранения обнуляются, чтобы evaluate() сразу доходил до
    // проверки свободного места и сохранения параметров.
    auto* info = G_PAC_INFO();
    const auto stable_delay = info->par[ PAC_info::P_STABLE_SAVE_DELAY_MS ];
    const auto min_interval = info->par[ PAC_info::P_MIN_SAVE_INTERVAL_MS ];
    info->par[ PAC_info::P_STABLE_SAVE_DELAY_MS ] = 0;
    info->par[ PAC_info::P_MIN_SAVE_INTERVAL_MS ] = 0;

    pm->save();
    EXPECT_EQ( 0, pm->evaluate() );

    info->par[ PAC_info::P_STABLE_SAVE_DELAY_MS ] = stable_delay;
    info->par[ PAC_info::P_MIN_SAVE_INTERVAL_MS ] = min_interval;

    // Сохранённые файлы убираются, чтобы не влиять на последующие тесты.
    std::filesystem::remove( std::filesystem::current_path() / "nvram.bin" );
    std::filesystem::remove( std::filesystem::current_path() / "eeprom.bin" );
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

#ifdef LINUX_OS
    int bad_access( [[maybe_unused]] const char* pathname,
        [[maybe_unused]] int mode )
        {
        errno = EACCES;
        return -1;
        }
#endif

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

TEST( SRAM, safe_save_create_error_logged_once )
    {
    SRAM good_sram( "test_sram4.bin", DATA_SIZE );

    auto fopen_hook = subhook_new( reinterpret_cast<void*>( &fopen ),
        reinterpret_cast<void*>( &bad_fopen ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fopen_hook );

    testing::internal::CaptureStdout();

    // Сохранение выполняется циклически, поэтому второе обращение к закрытому
    // каталогу не должно приводить к повторному сообщению.
    EXPECT_EQ( 1, good_sram.safe_save() );
    EXPECT_EQ( 1, good_sram.safe_save() );

    subhook_remove( fopen_hook );
    subhook_free( fopen_hook );

    auto output = testing::internal::GetCapturedStdout();

    const std::string marker = "Can't open file";
    size_t count = 0;
    for ( auto pos = output.find( marker );
        pos != std::string::npos; pos = output.find( marker, pos + 1 ) )
        {
        count++;
        }

    EXPECT_EQ( 1u, count );

    // Путь в сообщении должен быть абсолютным, иначе невозможно понять,
    // в каком каталоге запрещена запись.
    auto expected = ( std::filesystem::current_path()
        / "test_sram4.bin.tmp" ).lexically_normal().string();
    EXPECT_NE( std::string::npos, output.find( expected ) );
    }

TEST( SRAM, get_directory )
    {
    SRAM sram( "subdir/test_sram5.bin", DATA_SIZE );

    // Путь хранится в абсолютном виде, поэтому каталог хранения файла данных
    // не зависит от текущего каталога процесса.
    EXPECT_EQ( std::filesystem::current_path() / "subdir",
        sram.get_directory() );

    std::filesystem::remove( sram.get_directory() );
    }

TEST( SRAM, storage_directory_created )
    {
    const std::filesystem::path directory{ "test_storage_dir/nested" };

    std::filesystem::remove_all( "test_storage_dir" );

    SRAM sram( directory / "test_sram6.bin", DATA_SIZE );

    // Если каталог хранения отсутствует, то он создается автоматически,
    // чтобы файлы параметров можно было разместить в новом каталоге без
    // ручных действий на контроллере.
    EXPECT_TRUE( std::filesystem::exists( sram.get_directory() ) );

    // Перед созданием файлов права на запись в каталог проверяются, поэтому
    // сохранение в созданный каталог должно завершиться успешно.
    EXPECT_EQ( 0, sram.safe_save() );
    EXPECT_TRUE(
        std::filesystem::exists( directory / "test_sram6.bin" ) );

    std::filesystem::remove_all( "test_storage_dir" );
    }

TEST( SRAM, storage_directory_create_error_logged )
    {
    // Обычный файл на пути к каталогу хранения приводит к ошибке его
    // создания, о которой конструктор должен сообщить в журнал.
    const char* blocker = "test_sram7.bin";

    auto file = fopen( blocker, "wb" );
    fclose( file );

    testing::internal::CaptureStdout();

    SRAM sram( std::filesystem::path( blocker ) / "nested/test.bin",
        DATA_SIZE );

    auto output = testing::internal::GetCapturedStdout();

    std::filesystem::remove( blocker );

    EXPECT_NE( std::string::npos,
        output.find( "Can't create storage directory" ) );

    // Сообщение должно содержать абсолютный путь каталога, иначе причину
    // отказа невозможно найти в файловой системе контроллера.
    auto expected = ( std::filesystem::current_path() / blocker / "nested" )
        .lexically_normal().string();
    EXPECT_NE( std::string::npos, output.find( expected ) );
    }

TEST( SRAM, storage_directory_write_error_logged )
    {
#ifdef LINUX_OS
    auto access_hook = subhook_new( reinterpret_cast<void*>( &access ),
        reinterpret_cast<void*>( &bad_access ), SUBHOOK_64BIT_OFFSET );
    subhook_install( access_hook );

    testing::internal::CaptureStdout();

    // Права на запись проверяются до создания файлов, поэтому об отказе
    // конструктор сообщает сразу при создании объекта.
    SRAM sram( "test_sram8.bin", DATA_SIZE );

    auto output = testing::internal::GetCapturedStdout();

    subhook_remove( access_hook );
    subhook_free( access_hook );

    EXPECT_NE( std::string::npos,
        output.find( "Can't write to storage directory" ) );

    // Причина отказа и пользователь, под которым запущен процесс, позволяют
    // отличить случай «каталог создан от root» от других отказов.
    EXPECT_NE( std::string::npos, output.find( "Permission denied" ) );
    EXPECT_NE( std::string::npos, output.find( "(uid=" ) );
#else
    GTEST_SKIP() << "Checking uses hooking of access() on POSIX only.";
#endif // LINUX_OS
    }
