// main_control_dll_vc.cpp: определяет экспортированные функции для приложения DLL.
//
#include <stdlib.h>
#include "fcntl.h"

#include "dtime.h"

#include "prj_mngr.h"
#include "device/device.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"
#include "version_info.h"

#include "log.h"

#ifndef PTUSA_TEST
int G_DEBUG = 0;    //Вывод дополнительной отладочной информации.

bool G_NO_IO_NODES = true; // По умолчанию обмен с модулями отключен.
bool G_READ_ONLY_IO_NODES = false;
#endif

int lua_init( lua_State* L )
    {
    int top = lua_gettop( L );
    int p_size = !top ? 1 : top;
    auto argv = new const char* [ p_size ] { nullptr };

    int argc = 0;
    for ( int i = 1; i <= top; i++ )
        {
        switch ( auto t = lua_type( L, 1 ); t )
            {
            case LUA_TSTRING:   //Strings
                {
                const char* str = lua_tostring( L, 1 );
                char* tmp_str = new char[ strlen( str ) + 1 ];
                strcpy( tmp_str, str );
                argv[ argc ] = tmp_str;
                argc++;
                break;
                }

            default:            //Other values.
                break;
            }
        lua_remove( L, 1 );
        }

    int res = G_PROJECT_MANAGER->proc_main_params( argc, argv );

    for ( int i = 0; i < argc; i++ )
        {
        delete[] argv[ i ];
        argv[ i ] = nullptr;
        }
    delete[] argv;
    argv = nullptr;

    if ( res )
        {
        lua_pushnumber( L, EXIT_FAILURE );
        return 1;
        }

    //-Инициализация Lua.
    res = G_LUA_MANAGER->init( L, G_PROJECT_MANAGER->main_script.c_str(),
        G_PROJECT_MANAGER->path.c_str(), G_PROJECT_MANAGER->sys_path.c_str(),
        G_PROJECT_MANAGER->extra_paths.c_str() );

    if ( res ) //-Ошибка инициализации.
        {
        sprintf( G_LOG->msg, "Lua init returned error code %d!", 1 );
        G_LOG->write_log( i_log::P_CRIT );

#ifdef _DEBUG
        debug_break;
#endif
        lua_pushnumber( L, EXIT_FAILURE );
        return 1;
        }

    G_PROJECT_MANAGER->apply_opc_mode( false );

    lua_pushnumber( L, EXIT_SUCCESS );
    return 1;
    }

int eval( lua_State* L )
    {
    if ( G_DEBUG )
        {
        fflush( stdout );
        }

#ifdef TEST_SPEED
    static uint32_t st_time;
    static uint32_t all_time = 0;
    static uint32_t cycles_cnt = 0;

    st_time = get_millisec();
    cycles_cnt++;
#endif // TEST_SPEED

    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, 200 );

    valve::evaluate();
    valve_bottom_mix_proof::evaluate();
    int res = G_TECH_OBJECT_MNGR()->evaluate();

    PAC_info::get_instance()->eval();
    PAC_critical_errors_manager::get_instance()->show_errors();
    G_ERRORS_MANAGER->evaluate();
    G_SIREN_LIGHTS_MANAGER()->eval();

    lua_pushnumber( L, res );
    return 1;
    }

int no_print_stack_traceback( lua_State* L )
    {
    lua_manager::no_print_stack_traceback();
    return 0;
    }

int get_lua_error_count( lua_State* L )
    {
    lua_pushnumber( L, lua_manager::get_lua_error_count() );
    return 1;
    }

int use_print_stack_traceback( lua_State* L )
    {
    lua_manager::use_print_stack_traceback();
    return 0;
    }

int switch_on_verbose( lua_State* L )
    {
    G_DEBUG = 1;
    return 0;
    }

int switch_off_verbose( lua_State* L )
    {
    G_DEBUG = 0;
    return 0;
    }

//Регистрация реализованных в dll функций, что бы те стали доступны из lua.
struct luaL_reg ls_lib[] =
    {
    { "init", lua_init },
    { "eval", eval },
    { "no_print_stack_traceback", no_print_stack_traceback },
    { "get_lua_error_count", get_lua_error_count },
    { "use_print_stack_traceback", use_print_stack_traceback },
    { "switch_on_verbose", switch_on_verbose },
    { "switch_off_verbose", switch_off_verbose },
    { NULL, NULL },
    };

extern "C" int
#ifdef WIN32
__declspec( dllexport )
#endif
    luaopen_ptusa_main( lua_State* L )
    {
    luaL_openlib( L, "ptusa_main", ls_lib, 0 );
    return 1;
    }
