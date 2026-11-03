#include <algorithm>
#include <filesystem>

#include "fmt/format.h"

#ifdef WIN_OS
#include <Windows.h>

#endif // OS_WIN

#include "lua_manager.h"

#include "prj_mngr.h"
#include "device/device.h"
#include "tech_def.h"
#include "modbus_serv.h"
#include "lua_debugger.h"

#include "log.h"
//-----------------------------------------------------------------------------
auto_smart_ptr< lua_manager > lua_manager::instance;
bool lua_manager::is_print_stack_traceback = true;
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
void lua_manager::set_Lua( lua_State* l )
    {
    L = l;
    }

void lua_manager::free_Lua()
    {
    if ( L )
        {
        G_LUA_DEBUGGER->reset( L );
        lua_close( L );
        L = nullptr;
        }
    }
#endif
//-----------------------------------------------------------------------------
lua_manager* lua_manager::get_instance()
    {
    if ( static int is_init = 0; 0 == is_init )
        {
        instance = new lua_manager();
        is_init = 1;
        }

    return instance;
    }
//-----------------------------------------------------------------------------
int check_file( const char* file_name, char* err_str )
    {
    strcpy( err_str, "" );

    FILE *f = fopen( file_name, "r");
    if ( !f )
        {
        G_LOG->error( err_str, "File \"%s\" not found!", file_name );
        return -1;
        }
    int version = 0;

    if ( char str[ 100 ] = ""; fgets( str, sizeof( str ), f ) )
        {
        int err = sscanf( str, "--version = %d", &version );
        }

    fclose( f );
    f = nullptr;

    if ( G_DEBUG )
        {
        G_LOG->notice( "File \"%s\" version %d.", file_name, version );
        }

    return version;
    }
//-----------------------------------------------------------------------------
const char *FILES[ FILE_CNT ] =
    {
    "sys.io.lua",
    "sys.devices.lua",
    "sys.objects.lua",

    "main.io.lua",
    "main.objects.lua",
    "main.modbus_srv.lua",
    "main.restrictions.lua", // Keep last for RESTRICTIONS_SCRIPT_N.
    };
//-----------------------------------------------------------------------------
//I
//Загружаем последовательно Lua-скрипты. Их структура такова, что все необходимые
//действия реализованы в виде функций, которые потом в определенном порядке
//будут выполнятся.
//Скрипты разделены на системные (общие
//для всех проектов) и проекта (описание отдельного проекта). Системные скрипты
//имеют префикс sys и хранятся в папке "system scripts" на PC. Список скриптов:
// 1. sys.io.lua - функции получения описания I/O и устройств с привязкой к
// каналам ввода\вывода. Они вызываются исполняющей программой (С++).
// 2. sys.devices.lua - функции получения описания технологических устройств
// (клапана, насосы и т.д.). Они вызываются исполняющей программой
// (С++).
// 3. sys.objects.lua - функции получения описания технологических
// объектов (режимы, параметры и т.д.). Они вызываются исполняющей программой
// (С++).
//Пользовательские скрипты имеют префикс main и хранятся в папке конкретного
//проекта. Список скриптов:
// 1. main.io.lua - описание I/O и устройств проекта с привязкой к каналам
// ввода\вывода.
// 2. main.objects.lua - описание технологических объектов (режимы,
// параметры и т.д.) проекта.
// 3. main.plua - пользовательская логика работы проекта.
//Для каждого файла проверяется его наличие и версия (первая строка файла должна
//быть такой: "--version = 1"). Если файла нет или он имеет не поддерживаемую
//версию, то выдается соответствующее сообщение и работа приложения завершается.
//II
//Экспортируем в Lua классы и функции из C++.

int lua_manager::init( lua_State* lua_state, const char* script_name,
    const char* dir, const char* sys_dir, const char* extra_dirs )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "Init Lua..." );
        }

    G_LOG->notice( "script_name = \"%s\"",
        std::filesystem::path( script_name ).make_preferred().u8string().c_str() );

    std::string dir_str( dir );
    std::string sys_dir_str( sys_dir );
    std::string extra_dirs_str( extra_dirs );


    G_LOG->notice( "current working directory: \"%s\"",
        std::filesystem::current_path().u8string().c_str() );
    if ( !dir_str.empty() || !sys_dir_str.empty() || !extra_dirs_str.empty() )
        {
        G_LOG->notice( "path = \"%s\", sys_path = \"%s\", extra_paths = \"%s\"",
            std::filesystem::path( dir ).make_preferred().u8string().c_str(),
            std::filesystem::path( sys_dir ).make_preferred().u8string().c_str(),
            std::filesystem::path( extra_dirs ).make_preferred().u8string().c_str() );
        }

    if ( 0 == lua_state )
        {
        //Инициализация Lua.
        L = lua_open();   // Create Lua context.

        if ( NULL == L )
            {
            G_LOG->critical( "Error creating Lua context." );
            return 1;
            }
        is_free_lua = 1;

        lua_gc( L, LUA_GCSTOP, 0 );
        luaL_openlibs( L );    // Open standard libraries.
        }
    else
        {
        L = lua_state;
        is_free_lua = 0;
        }

    const std::string cmd = "package.path = package.path..';";
    std::string package_path = "";
    if ( !dir_str.empty() )
        {
        //Добавление каталога для поиска.
        package_path = dir_str + "?.lua";
        }
    if ( !sys_dir_str.empty() )
        {
        //Добавление каталога с системными скриптами.
        package_path += ";" + sys_dir_str + "?.lua";
        }
    if ( !extra_dirs_str.empty() )
        {
        //Добавление каталога с пользовательскими скриптами.
        package_path += ";" + extra_dirs_str + "?.lua";
        }
    if ( !package_path.empty() )
        {
        package_path += "'";
        if ( luaL_dostring( L, ( cmd + package_path ).c_str() ) != 0 )
            {
            G_LOG->critical(
                "Error during C++ call - \"lua_manager::init\" - %s",
                lua_tostring( L, -1 ) );

            lua_pop( L, 1 );
            return 1;
            }
        }

    //I
    //Проверка наличия и версии скриптов.
    if ( G_DEBUG )
        {
        printf( "Проверка наличия и версии скриптов.\n" );
        }
    char err_str[ 100 ] = "";

    int res = 0;
    for ( int i = 0; i < FILE_CNT; i++ )
        {
        char path[ 100 ];
        std::string& path_str = i < SYS_FILE_CNT ? sys_dir_str : dir_str;
        auto r = fmt::format_to_n( path, sizeof( path ), "{}{}", path_str, FILES[ i ] );
        *r.out = 0;
        res = check_file( path, err_str );

        if ( -1 == res )
            {
            sprintf( G_LOG->msg, "File \"%s\" not found.", path );
            G_LOG->write_log( i_log::P_CRIT );
            return 1;
            }

        if ( FILES_VERSION[ i ] != res )
            {
            if ( FILES_VERSION[ i ] < res )
                {
                G_LOG->critical( "File \"%s\" has version %d, must be %d "
                    "(consider updating \"ptusa_main\").",
                    FILES[ i ], res, FILES_VERSION[ i ] );
                }
            else
                {
                G_LOG->critical( "File \"%s\" has version %d, must be %d "
                    "(use previous version \"ptusa_main\").",
                    FILES[ i ], res, FILES_VERSION[ i ] );
                }
            return 1;
            }
        }

    //-Выполнение системных скриптов sys.lua.
    if ( G_DEBUG )
        {
        printf( "Выполнение системных скриптов sys.lua.\n" );
        }

    for ( int i = 0; i < FILE_CNT; i++ )
        {
        char path[ 100 ] = "";
        std::string& path_str = i < SYS_FILE_CNT ? sys_dir_str : dir_str;
        auto r = fmt::format_to_n(
            path, sizeof( path ), "{}{}", path_str, FILES[ i ] );
        *r.out = 0;

        if ( luaL_dofile( L, path ) != 0 )
            {
            G_LOG->critical( "%s", lua_tostring( L, -1 ) );
            lua_pop( L, 1 );

            return 1;
            }
        }

    //II
    //Экспорт в Lua необходимых объектов.
    if ( G_DEBUG )
        {
        printf( "Экспорт в Lua необходимых объектов.\n" );
        }
    tolua_PAC_dev_open( L );

    //-Загрузка параметров.
    if ( G_DEBUG )
        {
        printf( "Загрузка параметров.\n" );
        }
    const int PAC_ID =
        lua_manager::get_instance()->int_no_param_exec_lua_method( "system",
        "get_PAC_id", "main" );
    params_manager::get_instance()->init( PAC_ID );

    //III
    //Выполнение процедуры инициализации.
    if ( G_DEBUG )
        {
        printf( "Выполнение процедуры инициализации.\n" );
        }
    res = G_PROJECT_MANAGER->lua_load_configuration();
    if ( res )
        {
        return res;
        }

    //Инициализация параметров для корректной работы устройств.
    G_DEVICE_MANAGER()->init_rt_params();

    //IV Выполнение основного скрипта ('main.plua').
    if ( G_DEBUG )
        {
        printf( "Выполнение основного скрипта (\"%s\").\n", script_name );
        }

    if( luaL_loadfile( L, script_name ) != 0 )
        {
        G_LOG->critical( "%s", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
        return 1;
        }

    if ( int i_line = lua_pcall(L, 0, LUA_MULTRET, 0); i_line != 0 )
        {
        G_LOG->critical( "%s", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
        return 1;
        }
    //-Инициализация Lua.--!>

    //Выполнение пользовательской функции инициализации.
    if ( G_DEBUG )
        {
        printf( "Выполнение пользовательской функции инициализации.\n" );
        }
    res = G_TECH_OBJECT_MNGR()->init_objects();
    if ( res )
        {
        return res;
        }

    const char *PAC_name_rus =
        G_LUA_MANAGER->char_no_param_exec_lua_method( "system",
        "get_PAC_name_rus", "lua_manager::init" );
    if ( 0 == PAC_name_rus )
        {
        G_LOG->critical( "Lua init error - error reading PAC name (rus)." );
        return 1;
        }
    const char *PAC_name_eng =
        G_LUA_MANAGER->char_no_param_exec_lua_method( "system",
        "get_PAC_name_eng", "lua_manager::init" );
    if ( 0 == PAC_name_eng )
        {
        G_LOG->critical( "Lua init error - error reading PAC name (eng)." );
        return 1;
        }

#ifndef _USRDLL
    tcp_communicator::init_instance( PAC_name_rus, PAC_name_eng );

    G_CMMCTR->reg_service( device_communicator::C_SERVICE_N,
        device_communicator::write_devices_states_service );
    G_CMMCTR->reg_service( lua_debugger::C_SERVICE_N,
        lua_debugger::process_service );
    G_CMMCTR->reg_service( 15, ModbusServ::ModbusService );
#endif

    lua_gc( L, LUA_GCRESTART, 0 );
    lua_gc( L, LUA_GCCOLLECT, 0 );

    //Инициализация параметров при необходимости.
    if ( G_DEBUG )
        {
        printf( "Инициализация параметров при необходимости.\n" );
        }
    params_manager::get_instance()->final_init();

    if ( G_DEBUG )
        {
        printf( "Init Lua Ok.\n" );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
lua_manager::~lua_manager()
    {
    if ( 1 == is_free_lua )
        {
        if ( L )
            {
            G_LUA_DEBUGGER->reset( L );
            lua_close( L );
            L = nullptr;
            }
        }
    }
//-----------------------------------------------------------------------------
bool lua_manager::is_exist_lua_function( const char* object_name,
    const char* function_name ) const
    {
    auto res = false;
    lua_getfield( L, LUA_GLOBALSINDEX, object_name );
    if ( !lua_isnil( L, -1 ) )
        {
        lua_getfield( L, -1, function_name );
        res = lua_isfunction( L, -1 );
        lua_pop( L, 1 );
        }
    lua_pop( L, 1 );
    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::void_exec_lua_method( const char *object_name,
                                      const char *function_name, const char *c_function_name ) const
    {
    return exec_lua_method( object_name, function_name, 0, 0, 0 );
    }
//-----------------------------------------------------------------------------
int lua_manager::int_exec_lua_method( const char *object_name,
                                     const char *function_name, int param, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, param ) )
        {
        res = ( int ) tolua_tonumber( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
        res = -1;
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::int_2_exec_lua_method( const char* object_name,
    const char* function_name, int param1, int param2,
    const char* c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method_var( object_name, function_name, 1, 2, param1, param2 ) )
        {
        res = (int)tolua_tonumber( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
        res = -1;
        }

    return res;
    }
//-----------------------------------------------------------------------------
void* lua_manager::user_object_exec_lua_method( const char *object_name,
    const char *function_name, int param, const char *c_function_name ) const
    {
    void* res = NULL;
    if ( 0 == exec_lua_method( object_name, function_name, param ) )
        {
        res = tolua_tousertype( L, -1, NULL );
        lua_remove( L, -1 );
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::exec_lua_method( const char *object_name,
                                 const char *function_name, int param, int is_use_param,
                                 int is_use_lua_return_value ) const
    {
    return exec_lua_method_var( object_name,
        function_name, is_use_lua_return_value, is_use_param ? 1 : 0, param );
    }
//-----------------------------------------------------------------------------
int lua_manager::exec_lua_method_var( const char* object_name,
    const char* function_name, int is_use_lua_return_value, int cnt, ... ) const
    {
    //-Вычисление времени выполнения функций Lua.
    //LARGE_INTEGER start_time;
    //QueryPerformanceCounter( &start_time );

    //    u_long start_time;
    //    start_time = get_millisec();

    lua_pushcclosure( lua_manager::L, error_trace, 0 );
    instance->err_func = lua_gettop( L );

    int param_count = 0;

    if ( object_name && strcmp( object_name, "" ) != 0 )
        {
        lua_getfield( L, LUA_GLOBALSINDEX, object_name );
        if ( lua_type( L, -1 ) == LUA_TNIL )
            {
            lua_pop( L, 1 ); //Удаляем функцию error_trace.
            return 1;
            }

        lua_getfield( L, -1, function_name );
        if ( lua_type( L, -1 ) == LUA_TNIL )
            {
            lua_pop( L, 1 ); //Удаляем object_name.
            lua_pop( L, 1 ); //Удаляем функцию error_trace.
            return 1;
            }

        lua_remove( L, -2 );
        lua_getfield( L, LUA_GLOBALSINDEX, object_name );

        param_count++;
        }
    else
        {
        lua_getfield( L, LUA_GLOBALSINDEX, function_name );
        }

    va_list param;
    va_start( param, cnt );
    while ( cnt > 0 )
        {
        int par = va_arg( param, int );
        lua_pushnumber( L, par );
        param_count++;
        cnt--;
        }
    va_end( param );

    int results_count = is_use_lua_return_value == 1 ? 1 : 0;
    int res = lua_pcall( L, param_count, results_count, err_func );

    lua_remove( L, -results_count - 1 ); //Удаляем функцию error_trace.

    //LARGE_INTEGER finish_time;
    //QueryPerformanceCounter( &finish_time );
    //LARGE_INTEGER tick_per_sec;
    //QueryPerformanceFrequency( &tick_per_sec );
    //LONGLONG call_time = finish_time.QuadPart - start_time.QuadPart;
    //int tiks_per_mcsec = tick_per_sec.QuadPart / ( 1000 * 1000 );
    //double dt = call_time / tiks_per_mcsec;
    //dt += 1;

    //    u_long finish_time;
    //    finish_time = get_millisec();
    //    u_long call_time = finish_time - start_time;
    //    printf( "Lua call time - %lums\n", call_time );

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::error_trace( lua_State * L )
    {
    static std::vector< std::string > errors;

    std::string err_str = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    if ( std::binary_search( errors.begin(), errors.end(), err_str ) != true )
        {
        sprintf( G_LOG->msg, "Lua error: %s", err_str.c_str() );
        if ( G_LOG->msg[ strlen( G_LOG->msg ) - 1 ] == '\n' )
            {
            //Удаляем последний CR
            G_LOG->msg[ strlen( G_LOG->msg ) - 1 ] = 0;
            }

        G_LOG->write_log( i_log::P_ERR );

        if ( is_print_stack_traceback )
            {
            int res = sprintf( G_LOG->msg, "Lua stack traceback:" );

            lua_Debug ar;
            int level = 1;

            while ( lua_getstack( L, level, &ar ) && i_log::C_BUFF_SIZE - res > 0 )
                {
                lua_getinfo( L, "Sln", &ar );

                res += snprintf( G_LOG->msg + res, i_log::C_BUFF_SIZE - res,
                    "\n\t%s:%d: in function '%s' ('%s')",
                    ar.short_src, ar.currentline, ar.name, ar.namewhat );
                level++;
                }

            G_LOG->write_log( i_log::P_ERR );
            }

        errors.push_back( err_str );
        if ( errors.size() > MAX_ERRORS )
            {
            errors.erase( errors.begin(), errors.end() - MAX_ERRORS / 2 );
            }

        std::sort( errors.begin(), errors.end() );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int lua_manager::int_no_param_exec_lua_method( const char *object_name,
                                              const char *function_name, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 1 ) )
        {
        res = ( int ) tolua_tonumber( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
        res = 1;
        }

    return res;
    }
//-----------------------------------------------------------------------------
const char* lua_manager::char_exec_lua_method( const char *object_name,
                                              const char *function_name, int param, const char *c_function_name ) const
    {
    const char* res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, param, 1, 1 ) )
        {
        res = tolua_tostring( L, -1, 0 );
        lua_remove( L, -1 );
        }

    return res;
    }
//-----------------------------------------------------------------------------
const char* lua_manager::char_no_param_exec_lua_method( const char *object_name,
                                                       const char *function_name, const char *c_function_name ) const
    {
    const char* res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 1 ) )
        {
        res = tolua_tostring( L, -1, 0 );
        lua_remove( L, -1 );
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::exec_Lua_str( const char *Lua_str, const char *error_str,
                              bool is_print_error_msg /*= true */ ) const
    {
    if( int res = luaL_dostring(L, Lua_str); res != 0  )
        {
        if ( is_print_error_msg )
            {
            sprintf( G_LOG->msg, "Error during C++ call - \"%s\" - %s\n",
                error_str, lua_tostring( L, -1 ) );
            G_LOG->write_log( i_log::P_ERR );
            }

        lua_pop( L, 1 );
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
lua_State * lua_manager::get_Lua() const
    {
    return L;
    }
//-----------------------------------------------------------------------------
int lua_manager::reload_script( int script_n, const char* script_function_name,
    char *res_str, int max_res_str_length )
    {
    res_str[ 0 ] = 0;

    G_LOG->notice( "Start reload Lua script №%d.", script_n );

    if ( 0 == L )
        {
        return 1;
        }

    if ( script_n < 0 || script_n >= FILE_CNT )
        {
        G_LOG->error( "Reload Lua script error - script_n out of range "
            "(%d, FILE_CNT=%d).", script_n, FILE_CNT );
        return 1;
        }

    //Проверка наличия и версии скрипта.
    char err_str[ 100 ] = "";

    int res = 0;

    char path[ 100 ];
    if ( script_n < SYS_FILE_CNT )
        {
        sprintf( path, "%s%s",
            G_PROJECT_MANAGER->sys_path.c_str(), FILES[ script_n ] );
        }
    else
        {
        sprintf( path, "%s%s",
            G_PROJECT_MANAGER->path.c_str(), FILES[ script_n ] );
        }

    res = check_file( path, err_str );

    if ( -1 == res )                             // ../system scripts
        {
        res = check_file( FILES[ script_n ], err_str ); // .
        if ( -1 == res )
            {
            G_LOG->error( "%s", err_str );
            strcpy( res_str, err_str );
            return 1;
            }
        }

    if ( FILES_VERSION[ script_n ] != res )
        {
        sprintf( err_str, "File \"%s\" has version %d, must be %d "
            "(consider updating program \"ptusa_main\").",
            FILES[ script_n ], res, FILES_VERSION[ script_n ] );
        G_LOG->error( "%s", err_str );
        strcpy( res_str, err_str );
        return 1;
        }

    ////Сохранение предыдущей функции скрипта. Оставлен шаблон, все работает
    // и так при ошибке в новой функции.
    //char f_name[ 100 ];
    //sprintf( f_name, "%s_old = %s\n",
    //    script_function_name, script_function_name );
    //if ( luaL_dostring( L, f_name ) != 0 )
    //    {
    //    sprintf( G_LOG->msg, "\nReload Lua script - %s", lua_tostring( L, -1 ) );
    //    G_LOG->write_log( i_log::P_ERR );
    //    lua_pop( L, 1 );
    //    return 1;
    //    }

    //-Выполнение скрипта.
    if ( luaL_dofile( L, path ) != 0 )
        {
        lua_pop( L, 1 );
        if ( luaL_dofile( L, FILES[ script_n ] ) != 0 )
            {
            sprintf( G_LOG->msg, "Reload Lua script - %s", lua_tostring( L, -1 ) );
            G_LOG->write_log( i_log::P_ERR );

            strncpy( res_str, G_LOG->msg + 1 /*не копируем первый \n*/,
                max_res_str_length );
            lua_pop( L, 1 );

            ////Выполнение предыдущей функции скрипта. Оставлен шаблон, все работает
            // и так при ошибке в новой функции.
            //sprintf( f_name, "%s = %s_old\n",
            //    script_function_name, script_function_name );
            //if ( luaL_dostring( L, f_name ) != 0 )
            //    {
            //    sprintf( G_LOG->msg, "\nReload Lua script - %s", lua_tostring( L, -1 ) );
            //    G_LOG->write_log( i_log::P_ERR );
            //    lua_pop( L, 1 );
            //    }

            return 1;
            }
        }

    G_LOG->notice( "Reload Lua script \"%s\" - Ok.", path );
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
