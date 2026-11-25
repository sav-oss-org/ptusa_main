#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif

#include <string.h>
#include <stdlib.h>
#include <filesystem>

#include <cxxopts.hpp>
#include "fmt/format.h"

#include "prj_mngr.h"
#include "bus_coupler_io.h"
#include "device/device.h"
#include "device/manager.h"
#include "param_ex.h"
#include "params_recipe_manager.h"
#include "OPCUAServer.h"

#include "lua_manager.h"

#include "tech_def.h"

#include "log.h"

#include "g_errors.h"

extern bool G_NO_IO_NODES;
extern bool G_READ_ONLY_IO_NODES;

auto_smart_ptr < project_manager > project_manager::instance;
//-----------------------------------------------------------------------------
int project_manager::proc_main_params( int argc, const char* argv[] )
    {
    if ( !argc || !argv || !argv[ 0 ] ) return 2;

    opc_mode = OPC_MODE::UNDEFINED;

    //-Работа с параметрами командной строки.
    cxxopts::Options options( argv[ 0 ], "Main control program" );

    constexpr auto DEFAULT_NO_IO =
#if defined WIN_OS
        "true";
#else
        "false";
#endif // defined WIN_OS

    options.add_options()
        ( "v,version", "Print version info" )
        ( "d,debug", "Enable debugging",
            cxxopts::value<bool>()->default_value( "false" ) )

        ( "no_io", "No communicate with I\\O nodes",
                cxxopts::value<bool>()->default_value( DEFAULT_NO_IO ) )
        ( "read_only_io", "Read only from I\\O nodes",
            cxxopts::value<bool>()->default_value( DEFAULT_NO_IO ) )

        ( "p,port", "Param port",

            cxxopts::value<int>()->default_value( "10000" ) )
        ( "h,help", "Print help info" )
        ( "r,rcrc", "Reset params" )

        ( "opc", "OPC UA server behavior (off, r, rw)",
            cxxopts::value<std::string>() )

        ( "sys_path", "Sys path",
            cxxopts::value<std::string>()->default_value( "./sys" ) )
        ( "path", "Path",
            cxxopts::value<std::string>()->default_value( "." ) )
        ( "extra_paths", "Extra paths",
            cxxopts::value<std::string>()->default_value( "./dairy-sys" ) )
        ( "sleep_time", "Sleep time, ms",
            cxxopts::value<unsigned int>()->default_value( "2" ) )

        ( "script", "The script file to execute",
            cxxopts::value<std::string>()  );

    options.parse_positional( { "script" } );
    options.positional_help( "<script>" );
    options.allow_unrecognised_options();
    auto result = options.parse( argc, argv );

    if ( result.count( "version" ) )
        {
        fmt::print( "{}\n", PRODUCT_VERSION_FULL_STR );
        return 1;
        }

    if ( result.count( "help" ) || result.count( "script" ) == 0 )
        {
        fmt::print( "{}", options.help() );
        return 1;
        }

    // Проверка на наличие файла @main_script.
    std::filesystem::path s{ result[ "script" ].as<std::string>() };
    if ( std::error_code ec; !std::filesystem::exists( s, ec ) )
        {
        fmt::print( "Error: Script file '{}' does not exist.\n", s.string() );
        return 1;
        }
    main_script = s.lexically_normal().generic_string();

    G_LOG->info( "Program started (version %s).", PRODUCT_VERSION_FULL_STR );

    if ( result[ "debug" ].as<bool>() )
        {
        G_DEBUG = 1;
        fmt::print( "DEBUG ON.\n" );
        }

    if ( result.count( "rcrc" ) )
        {
        G_LOG->debug( "Resetting parameters (command line parameter 'rcrc')." );
        params_manager::get_instance()->reset_CRC_mem();
        }

    if ( result.count( "port" ) )
        {
        int p = result[ "port" ].as<int>();
        if ( p > 0 )
            {
            tcp_communicator::set_port( p, p + 502 );
            G_LOG->notice( "New tcp_communicator ports: %d [modbus %d].",
                p, p + 502 );
            }
        }

    if ( result.count( "opc" ) )
        {
        if ( auto opc_mode_arg = result[ "opc" ].as<std::string>();
            opc_mode_arg == "rw" )
            {
            opc_mode = OPC_MODE::READ_WRITE;
            }
        else if ( opc_mode_arg == "r" )
            {
            opc_mode = OPC_MODE::READ_ONLY;
            }
        else if ( opc_mode_arg == "off" )
            {
            opc_mode = OPC_MODE::OFF;
            }
        else
            {
            G_LOG->error( "Unknown OPC UA mode: '%s'. "
                "Valid values: off, r, rw.", opc_mode_arg.c_str() );
            return 1;
            }

        log_opc_mode();
        }

    sleep_time_ms = result[ "sleep_time" ].as<unsigned int>();

    // Нормализуем пути и гарантируем слеш на конце через /= "".
    auto p_norm = std::filesystem::path(
        result[ "path" ].as<std::string>() ).lexically_normal() / "";
    auto s_norm = std::filesystem::path(
        result[ "sys_path" ].as<std::string>() ).lexically_normal() / "";
    auto e_norm = std::filesystem::path(
        result[ "extra_paths" ].as<std::string>() ).lexically_normal() / "";

    path = p_norm.generic_string();
    sys_path = s_norm.generic_string();
    extra_paths = e_norm.generic_string();

    // Отключить/включить обмен с модулями ввода/вывода.
    G_NO_IO_NODES = result[ "no_io" ].as<bool>() ? true : false;

    // Только чтение/запись+чтение данных с модулей ввода/вывода.
    G_READ_ONLY_IO_NODES = result[ "read_only_io" ].as<bool>() ? true : false;

    if ( G_NO_IO_NODES )
        G_LOG->warning( "Bus couplers are disabled." );
    else
        {
        G_LOG->warning( "Bus couplers are enabled." );
        if ( G_READ_ONLY_IO_NODES )
            G_LOG->warning( "Bus couplers are read only." );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void project_manager::log_opc_mode() const
    {
    switch ( opc_mode )
        {
        case OPC_MODE::OFF:
            G_LOG->info( "OPC UA server is disabled." );
            break;

        case OPC_MODE::READ_ONLY:
            G_LOG->warning( "OPC UA server is activated (only read)." );
            break;

        case OPC_MODE::READ_WRITE:
            G_LOG->warning( "OPC UA server is activated (read-write)." );
            break;

        case OPC_MODE::UNDEFINED:
            break;
        }
    }
//-----------------------------------------------------------------------------
int project_manager::apply_opc_mode( bool show_msg /* = true */ ) const
    {
    auto pac_info = G_PAC_INFO();
    auto save_param_if_changed = [ pac_info ]( unsigned int param_idx,
        u_int_4 value )
        {
        if ( pac_info->par[ param_idx ] != value )
            {
            pac_info->par.save( param_idx, value );
            }
        };

    switch ( opc_mode )
        {
        case OPC_MODE::OFF:
            save_param_if_changed( PAC_info::P_IS_OPC_UA_SERVER_ACTIVE, 0 );
            save_param_if_changed( PAC_info::P_IS_OPC_UA_SERVER_CONTROL, 0 );
            break;

        case OPC_MODE::READ_ONLY:
            save_param_if_changed( PAC_info::P_IS_OPC_UA_SERVER_ACTIVE, 1 );
            save_param_if_changed( PAC_info::P_IS_OPC_UA_SERVER_CONTROL, 0 );
            break;

        case OPC_MODE::READ_WRITE:
            save_param_if_changed( PAC_info::P_IS_OPC_UA_SERVER_ACTIVE, 1 );
            save_param_if_changed( PAC_info::P_IS_OPC_UA_SERVER_CONTROL, 1 );
            break;

        case OPC_MODE::UNDEFINED:
            break;
        }

    if ( show_msg )
        {
        log_opc_mode();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
project_manager* project_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new project_manager();
        }

    return instance;
    }
//-----------------------------------------------------------------------------
int project_manager::init_path( const char* path )
    {
    if ( path )
        {
        this->path = path;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int project_manager::init_sys_path( const char* sys_path )
    {
    if ( sys_path )
        {
        this->sys_path = sys_path;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int project_manager::init_extra_paths( const char* paths )
    {
    if ( paths )
        {
        this->extra_paths = paths;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//Порядок загрузки:
//1.Модули.
//2.Устройства.
//3.Переменные для доступа к устройствам из Lua (совпадают с именем устройства).
int project_manager::lua_load_configuration()
    {
    if ( G_DEBUG )
        {
        printf( "\nProject manager - processing configuration...\n" );
        }

    //-I/O modules data.
    auto res = lua_manager::get_instance()->void_exec_lua_method( "system",
        "create_io", "project_manager::lua_load_configuration()" );

    if ( res ) return 1;

    if ( G_DEBUG )
        {
        io_manager::get_instance()->print();
        printf( "\n" );
        }

    //-Devices data.
    res = lua_manager::get_instance()->void_exec_lua_method( "system",
        "create_devices", "project_manager::lua_load_configuration()" );

    if ( res ) return 1;

    //-Devices properties.
    res = lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_properties", "project_manager::lua_load_configuration()" );

    if ( res ) return 1;

    if ( G_DEBUG )
        {
        printf( "Oк.\n" );
        }

    if ( G_DEBUG )
        {
        G_DEVICE_MANAGER()->print();
        printf( "\n" );
        }

    res = lua_manager::get_instance()->int_exec_lua_method( "",
        "init_tech_objects", 0, "project_manager::lua_load_configuration()" );
    if ( res )
        {
        printf( "Fatal error!\n" );
        return 1;
        }

    res = lua_manager::get_instance()->int_exec_lua_method( "object_manager",
        "get_objects_count", 0, "project_manager::lua_load_configuration()" );
    if ( res < 0 )
        {
        printf( "Fatal error!\n" );
        return 1;
        }

    int objects_count = res;
    for ( int i = 1; i <= objects_count; i++ )
        {
        void * res_object = lua_manager::get_instance()->user_object_exec_lua_method(
            "object_manager", "get_object", i,
            "project_manager::lua_load_configuration()" );

        if ( 0 == res_object )
            {
            printf( "Fatal error!\n" );
            return 1;
            }

        G_TECH_OBJECT_MNGR()->add_tech_object( ( tech_object * ) res_object );
        }

    if ( G_DEBUG )
        {
        G_TECH_OBJECT_MNGR()->print();
        printf( "\n" );
        }

    //-Добавление технологических объектов проекта.
    for ( u_int i = 0; i < G_TECH_OBJECT_MNGR()->get_count(); i++ )
        {
        G_DEVICE_CMMCTR->add_device( G_TECH_OBJECTS( i ) );
        }
    //-Добавление системных тегов контроллера.
    G_DEVICE_CMMCTR->add_device( PAC_info::get_instance() );

    G_DEVICE_CMMCTR->add_device( siren_lights_manager::get_instance() );

    G_DEVICE_CMMCTR->add_device( ParamsRecipeManager::getInstance());

    if ( G_DEBUG )
        {
        printf( "Project manager - processing configuration completed.\n" );
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
