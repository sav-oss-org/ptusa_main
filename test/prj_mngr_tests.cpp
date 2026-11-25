#include <array>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdio>

#include "prj_mngr_tests.h"
#include "PAC_info.h"
#include "lua_manager.h"
#include "dtime.h"

const char * const PROGRAM_STARTED =
#ifndef WIN_OS
    "\x1B[32m"
#endif // !WIN_OS
    "INFO   (6) -> Program started (version " PRODUCT_VERSION_FULL_STR ").\n"
#ifndef WIN_OS
    "\x1B[0m"
#endif // !WIN_OS
    ;

const char* const BUS_COUPLERS_DISABLED =
#ifndef WIN_OS
    "\x1B[33m"
#endif // !WIN_OS
    "WARNING(4) -> Bus couplers are disabled.\n"
#ifndef WIN_OS
    "\x1B[0m"
#endif // !WIN_OS
    ;

const char* const OPC_RO =
#ifndef WIN_OS
    "\x1B[33m"
#endif
    "WARNING(4) -> OPC UA server is activated (only read).\n"
#ifndef WIN_OS
    "\x1B[0m"
#endif
    ;

const char* const OPC_RW =
#ifndef WIN_OS
    "\x1B[33m"
#endif

"WARNING(4) -> OPC UA server is activated (read-write).\n"
#ifndef WIN_OS
    "\x1B[0m"
#endif
    ;

const char* const OPC_OFF =
#ifndef WIN_OS
    "\x1B[32m"
#endif
    "INFO   (6) -> OPC UA server is disabled.\n"
#ifndef WIN_OS
    "\x1B[0m"
#endif
    ;

const char* const OPC_ERR_ST =
#ifndef WIN_OS
    "\x1B[31m"
#endif
    "ERROR  (3) -> Unknown OPC UA mode: 'st'. Valid values: off, r, rw.\n"
#ifndef WIN_OS
    "\x1B[0m"
#endif
    ;

const char* const BUS_COUPLERS_ENABLED =
#ifndef WIN_OS
    "\x1B[33m"
#endif
    "WARNING(4) -> Bus couplers are enabled.\n"
#ifndef WIN_OS
    "\x1B[0m"
#endif
    ;

using namespace ::testing;

TEST( project_manager, lua_load_configuration )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );


    // Should fail without correct data - no all required Lua-functions.
    auto res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no create_devices() Lua-function.
    auto system_Lua_script_create_io = R"(
system =
    {
    create_io = function( self )
        return 0
    end
    }
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_create_io ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );


    // Should fail without correct data - no init_devices_properties() Lua-function.
    auto system_Lua_script_create_devices = R"(
system.create_devices = function( self )
        return 0
    end
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_create_devices ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no init_tech_objects() Lua-function.
    auto system_Lua_script_init_devices_properties = R"(
system.init_devices_properties = function( self )
        return 0
    end
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_init_devices_properties ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no get_objects_count() Lua-function.
    auto system_Lua_script_init_tech_objects = R"(
init_tech_objects = function( self )
    return 0
end
)";

    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_init_tech_objects ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should not fail - all required Lua-functions. They (Lua) return 0 -
    // just everthing is OK.
    auto object_manager_Lua_script_get_objects_count = R"(
object_manager =
    {
    --Получение количества пользовательских технологических объектов.
    get_objects_count = function( self )
        return 0
    end
    }
)";
    EXPECT_EQ( 0, luaL_dostring( L, object_manager_Lua_script_get_objects_count ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 0, res );


    G_LUA_MANAGER->free_Lua();
    }

TEST( project_manager, proc_main_params )
    {
    // Для тестов создаем файл main.plua.
    std::ofstream main_plua( "main.plua", std::ofstream::out );
    main_plua << "system = {}" << std::endl;
    main_plua.close();

    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );

    auto res = G_PROJECT_MANAGER->proc_main_params( 1, nullptr );
    ASSERT_EQ( 2, res );

    const char* empty_first_param[] = { "" };
    res = G_PROJECT_MANAGER->proc_main_params( 0, empty_first_param );
    ASSERT_EQ( 2, res );

    const char* null_first_param[] = { nullptr };
    res = G_PROJECT_MANAGER->proc_main_params( 1, null_first_param );
    ASSERT_EQ( 2, res );

    char argv0[] = "ptusa_main.exe";
    char argv1[] = "--help";
    const char* argv[] = { argv0, argv1 };

    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( 2, argv );
    auto output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 1, res );

    auto help =
#if defined WIN_OS
        R"(Main control program
Usage:
  ptusa_main.exe [OPTION...] <script>

  -v, --version          Print version info
  -d, --debug            Enable debugging
      --no_io            No communicate with I\O nodes (default: true)
      --read_only_io     Read only from I\O nodes (default: true)
  -p, --port arg         Param port (default: 10000)
  -h, --help             Print help info
  -r, --rcrc             Reset params
      --opc arg          OPC UA server behavior (off, r, rw)
      --sys_path arg     Sys path (default: ./sys)
      --path arg         Path (default: .)
      --extra_paths arg  Extra paths (default: ./dairy-sys)
      --sleep_time arg   Sleep time, ms (default: 2)
)";
#else
        R"(Main control program
Usage:
  ptusa_main.exe [OPTION...] <script>

  -v, --version          Print version info
  -d, --debug            Enable debugging
      --no_io            No communicate with I\O nodes
      --read_only_io     Read only from I\O nodes
  -p, --port arg         Param port (default: 10000)
  -h, --help             Print help info
  -r, --rcrc             Reset params
      --opc arg          OPC UA server behavior (off, r, rw)
      --sys_path arg     Sys path (default: ./sys)
      --path arg         Path (default: .)
      --extra_paths arg  Extra paths (default: ./dairy-sys)
      --sleep_time arg   Sleep time, ms (default: 2)
)";
#endif // defined WIN_OS

    EXPECT_EQ( output, help );


    const char* argv_v[] = { "ptusa_main.exe", "--version" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( 2, argv_v );
    ASSERT_EQ( 1, res );
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, PRODUCT_VERSION_FULL_STR "\n" );


    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    std::tm tm = get_time();
    std::stringstream tmp;
    tmp << std::put_time( &tm, "%Y-%m-%d %H.%M.%S " );

    // Отключаем работу с модулями ввода/вывода, сбрасываем параметры,
    // запускаем в отладочном режиме.
    std::array<const char*, 4> argv_ex = { "ptusa_main.exe", "--debug",
        "--rcrc", "main.plua" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    std::string debug = tmp.str() + PROGRAM_STARTED +
        "DEBUG ON.\n" +
        tmp.str() +
#if !defined WIN_OS
        "\x1B[37m" +
#endif
        "DEBUG  (7) -> "
        "Resetting parameters (command line parameter 'rcrc').\n"
#if !defined WIN_OS
        "\x1B[0m"
#endif
        ;

    debug += tmp.str() +
#ifdef WIN_OS
        BUS_COUPLERS_DISABLED;
#else
        BUS_COUPLERS_ENABLED;
#endif // WIN_OS

    EXPECT_EQ( output, debug );

    // Выключаем OPC UA.
    argv_ex = { "ptusa_main.exe", "main.plua", "--opc=off", "" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    debug = tmp.str() + PROGRAM_STARTED;
    debug += tmp.str() + OPC_OFF;
    debug += tmp.str() +
#ifdef WIN_OS
        BUS_COUPLERS_DISABLED;
#else
        BUS_COUPLERS_ENABLED;
#endif // WIN_OS

    EXPECT_EQ( output, debug );

    // Проверяем, что после инициализации стандартных параметров
    // сохраненный режим "--opc=off" можно применить повторно
    // без лишнего сообщения. Меняем только OPC UA параметры,
    // чтобы не вызывать глобальный reset_params() и лишние save_all().
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] = 1;
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = 0;
    EXPECT_EQ( 1, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] );

    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->apply_opc_mode( false );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    EXPECT_TRUE( output.empty() );
    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] );

    // Передаем некорректный режим OPC UA.
    argv_ex = { "ptusa_main.exe", "main.plua", "--opc=st", "" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 1, res );

    debug = tmp.str() + PROGRAM_STARTED;
    debug += tmp.str() + OPC_ERR_ST;
    EXPECT_EQ( output, debug );

    // Включаем OPC UA в режиме чтения.
    argv_ex = { "ptusa_main.exe", "main.plua", "--opc=r", "" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    debug = tmp.str() + PROGRAM_STARTED;
    debug += tmp.str() + OPC_RO;
    debug += tmp.str() +
#ifdef WIN_OS
        BUS_COUPLERS_DISABLED;
#else
        BUS_COUPLERS_ENABLED;
#endif // WIN_OS
    EXPECT_EQ( output, debug );

    // Включаем OPC UA в режиме чтения и записи.
    argv_ex = { "ptusa_main.exe", "main.plua", "--opc=rw", "" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    debug = tmp.str() + PROGRAM_STARTED;
    debug += tmp.str() + OPC_RW;
    debug += tmp.str() +
#ifdef WIN_OS
        BUS_COUPLERS_DISABLED;
#else
        BUS_COUPLERS_ENABLED;
#endif // WIN_OS
    EXPECT_EQ( output, debug );

    // Включаем работу с модулями ввода/вывода.
    argv_ex = { "ptusa_main.exe", "main.plua", "--no_io=false", "" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    debug = tmp.str() + PROGRAM_STARTED;
    debug += tmp.str() + BUS_COUPLERS_ENABLED;
#if defined WIN_OS
    debug += tmp.str() + "WARNING(4) -> Bus couplers are read only.\n";
#endif
    EXPECT_EQ( output, debug );

    // Включаем работу с модулями ввода/вывода, включаем только чтение.
    argv_ex = { "ptusa_main.exe", "main.plua", "--no_io=false",
        "--read_only_io" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    debug = tmp.str() + PROGRAM_STARTED;
    debug += tmp.str() + BUS_COUPLERS_ENABLED;
#if defined WIN_OS
    debug += tmp.str() + "WARNING(4) -> Bus couplers are read only.\n";
#else
    debug += tmp.str() + "\x1B[33m"
        + "WARNING(4) -> Bus couplers are read only.\n" + "\x1B[0m";
#endif
    EXPECT_EQ( output, debug );

    // Включаем работу с модулями ввода/вывода, отключаем только чтение.
    argv_ex = { "ptusa_main.exe", "main.plua", "--no_io=false",
        "--read_only_io=false" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    debug = tmp.str() + PROGRAM_STARTED;
    debug += tmp.str() + BUS_COUPLERS_ENABLED;
    EXPECT_EQ( output, debug );

    std::array<const char*, 14> argv_path{ "ptusa_main.exe", "--port", "20000",
        "--sys_path", "./sys/", "--path", "./", "--extra_paths", "./dairy_sys/",
        "--sleep_time", "5", "--no_io", "--read_only_io", "main.plua" };
    res = G_PROJECT_MANAGER->proc_main_params( argv_path.size(), argv_path.data() );
    ASSERT_EQ( 0, res );

    G_PAC_INFO()->reset_params();

    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    G_LUA_MANAGER->free_Lua();

    std::remove( "main.plua" );
    }


TEST( project_manager, apply_opc_mode )
    {
    auto ua_server_active =
        G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ];
    auto ua_server_control =
        G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ];

    // Используемый режим OPC UA по умолчанию - UNDEFINED, при его применении
    // сохранённые параметры не изменяются и нет никаких сообщений.
    testing::internal::CaptureStdout();
    auto res = G_PROJECT_MANAGER->apply_opc_mode();
    auto output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( 0, res );

    EXPECT_TRUE( output.empty() );
    EXPECT_EQ( ua_server_active,
        G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    EXPECT_EQ( ua_server_control,
        G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] );


    // Для тестов создаем файл main.plua.
    std::ofstream main_plua( "main.plua", std::ofstream::out );
    main_plua << "system = {}" << std::endl;
    main_plua.close();


    // Отключаем работу OPC UA.
    const auto NO_SHOW_LOG_MESSAGE = false;
    std::array<const char*, 3> argv_ex = { "ptusa_main.exe", "--opc=off",
        "main.plua" };
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );
    res = G_PROJECT_MANAGER->apply_opc_mode( NO_SHOW_LOG_MESSAGE );
    ASSERT_EQ( 0, res );

    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] );

    // Включаем работу OPC UA.
    argv_ex[ 1 ] = "--opc=rw";
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );
    res = G_PROJECT_MANAGER->apply_opc_mode( NO_SHOW_LOG_MESSAGE );
    ASSERT_EQ( 0, res );

    EXPECT_EQ( 1, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    EXPECT_EQ( 1, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] );

    // Включаем только чтение для OPC UA.
    argv_ex[ 1 ] = "--opc=r";
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );
    res = G_PROJECT_MANAGER->apply_opc_mode( NO_SHOW_LOG_MESSAGE );
    ASSERT_EQ( 0, res );

    EXPECT_EQ( 1, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] );

    // Возвращаем предыдущие значения.
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] = ua_server_active;
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] = ua_server_control;

    std::remove( "main.plua" );
    }
