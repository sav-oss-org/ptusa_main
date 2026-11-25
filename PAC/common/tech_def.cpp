#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include "fmt/format.h"

#include "tech_def.h"

#include "lua_manager.h"

#include "g_errors.h"
//-----------------------------------------------------------------------------
auto_smart_ptr < tech_object_manager > tech_object_manager::instance;

char i_tech_object::white_spaces[ 256 ] = "";
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
tech_object::tech_object( const char* new_name, u_int number, u_int type,
    const char *name_Lua,
    u_int operations_count,
    u_int timers_count,
    u_int par_float_count, u_int runtime_par_float_count,
    u_int par_uint_count,
    u_int runtime_par_uint_count) :
        par_float( saved_params_float( par_float_count, this ) ),
        rt_par_float( run_time_params_float( runtime_par_float_count, this ) ),
        par_uint( saved_params_u_int_4( par_uint_count, this ) ),
        rt_par_uint( run_time_params_u_int_4( runtime_par_uint_count, "RT_PAR_UI", this ) ),
        timers( timers_count ),
        number( number ),
        type( type ),
        cmd( 0 ),
        operations_count( operations_count ),
        operations_manager( 0 )
    {
    u_int state_size_in_int4 = operations_count / 32; // Размер состояния в double word.
    if ( operations_count % 32 > 0 ) state_size_in_int4++;
    for( u_int i = 0; i < state_size_in_int4; i++ )
        {
        state.push_back( 0 );
        }

    for ( u_int i = 0; i < operations_count; i++ )
        {
        available.push_back( 0 );
        }

    name = new char[ strlen( new_name ) + 1 ];
    strcpy( name, new_name );
    this->name_Lua = new char[ strlen( name_Lua ) + 1 ];
    strcpy( this->name_Lua, name_Lua );

    const char* sign_str = " №";
    const int MAX_LENGTH = strlen( new_name ) + strlen( sign_str ) +
        std::to_string( number ).length() + 1;
    full_name = new char[ MAX_LENGTH ];
    snprintf( full_name, MAX_LENGTH, "%s%s%d", new_name, sign_str, number );

    operations_manager = new operation_manager( this );
    }
//-----------------------------------------------------------------------------
tech_object::~tech_object()
    {
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        delete errors[ i ];
        errors[ i ] = nullptr;
        }

    delete[] name;
    name = nullptr;
    delete[] name_Lua;
    name_Lua = nullptr;
    delete[] full_name;
    full_name = nullptr;

    delete operations_manager;
    operations_manager = nullptr;
    }
//-----------------------------------------------------------------------------
int tech_object::init_params()
    {
    par_float.reset_to_0();
    par_uint.reset_to_0();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::init_runtime_params()
    {
    rt_par_float.reset_to_0();
    rt_par_uint.reset_to_0();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::set_mode( u_int operation_n, int newm )
    {
    int res = 0;

    static u_char idx = 0;
    if ( operation_n > operations_count )
        {
        res = 3;
        }
    if ( 0 == operation_n )
        {
        res = 4;
        }

    if ( G_DEBUG )
        {
        SetColor( GREEN );
        printf( R"(%sBEGIN "%s %d" (%s) set operation № %u ("%s") --> %s.)",
            white_spaces, name, number, name_Lua, operation_n,
            0 == res ? ( *operations_manager )[ operation_n ]->get_name() : "",
            newm == 0 ? "OFF" : ( newm == 1 ? "ON" : ( newm == 2 ? "PAUSE" :
            ( newm == 3 ? "STOP" : ( newm == 4 ? "WAIT" : "?" ) ) ) ) );

        white_spaces[ idx++ ] = ' ';
        white_spaces[ idx++ ] = ' ';
        white_spaces[ idx++ ] = ' ';
        white_spaces[ idx++ ] = ' ';
        white_spaces[ idx + 1 ] = 0;
        SetColor( RESET );
        printf( "\n" );
        }

    int i = operation_n - 1;
    if ( 0 == res )
        {
        operation* op = ( *operations_manager )[ operation_n ];
        switch ( newm )
            {
            case operation::PAUSE:
                // Ignore if operation is in IDLE state.
                if ( op->get_state() == operation::IDLE )
                    {
                    res = 5;
                    break;
                    }
                // Check if possible.
                if ( ( res = lua_check_on_pause( operation_n ) ) == 0 )
                    {
                    state[ i / 32 ] = state[ i / 32 ] | 1UL << i % 32;
                    op->pause();
                    lua_on_pause( operation_n );
                    }
                else
                    {
                    res += 200;
                    }
                break;

            case operation::STOP:
                // Ignore if operation is in IDLE state.
                if ( op->get_state() == operation::IDLE )
                    {
                    res = 6;
                    break;
                    }
                // Check if possible.
                if ( ( res = lua_check_on_stop( operation_n ) ) == 0 )
                    {
                    state[ i / 32 ] = state[ i / 32 ] | 1UL << i % 32;
                    op->stop();
                    lua_on_stop( operation_n );
                    }
                else
                    {
                    res += 300;
                    }
                break;

            default:
                if ( newm != 0 ) newm = 1;

                if ( get_mode( operation_n ) == newm )
                    {
                    if ( newm == 1 && op->get_state() != operation::RUN )
                        {
                        // Check if possible.
                        if ( ( res = lua_check_on_start( operation_n ) ) == 0 )
                            {
                            auto run_step =
                                lua_get_run_step_after_pause( operation_n );
                            op->start( run_step );
                            lua_on_start( operation_n );
                            }
                        else
                            {
                            res += 400;
                            }
                        }
                    else
                        {
                        res = 1;
                        }
                    }
                else
                    {
                    if ( newm == 0 ) // Off mode.
                        {
                        // Check if possible.
                        if ( ( res = lua_check_off_mode( operation_n ) ) <= 0 )
                            {
                            int idx = operation_n - 1;
                            state[ idx / 32 ] = state[ idx / 32 ] & ~( 1UL << idx % 32 );

                            op->finalize();
                            lua_final_mode( operation_n );
                            }
                        else
                            {
                            res += 100;
                            }
                        }
                    else
                        {
                        //Проверка режима на проверку ОС устройств.
                        const int ERR_STR_SIZE = 41;
                        char res_str[ ERR_STR_SIZE ] = "авария ";

                        int len = strlen( res_str );
                        res = op->check_devices_on_run_state(
                            res_str + len, ERR_STR_SIZE - len );
                        if ( res && is_check_mode( operation_n ) == 1 )
                            {
                            set_err_msg( res_str, operation_n );
                            res = operation_n + 1000;
                            }
                        else
                            {
                            bool is_dev_err = res ? true : false;
                            //Проверка режима на проверку ОС устройств.

                            if ( ( res = lua_check_on_mode( operation_n ) ) == 0 )
                                {
                                op->start();
                                lua_init_mode( operation_n );
                                lua_on_start( operation_n );

                                int idx = operation_n - 1;
                                state[ idx / 32 ] = state[ idx / 32 ] | 1UL << idx % 32;

                                //Проверка режима на проверку ОС устройств.
                                if ( is_dev_err )
                                    {
                                    set_err_msg( res_str, operation_n, 0, ERR_ON_WITH_ERRORS );
                                    }
                                //Проверка режима на проверку ОС устройств.


                                operations_manager->reset_active_operation_or_idle_time();
                                }
                            else
                                {
                                res += 100;
                                }
                            }
                        }
                    }
                break;
            }
        }

    if ( G_DEBUG )
        {
        idx -= 4;
        white_spaces[ idx ] = 0;

        SetColor( GREEN );
        auto current_op_state = operation::state_idx::IDLE;
        if ( res <= 1 )
            {
            current_op_state = ( *operations_manager )[ operation_n ]->get_state();
            }
        const auto str = current_op_state < operation::state_idx::STATES_MAX ?
            operation::en_state_str.at( current_op_state ) : "?";

        printf( "%sEND \"%s %d\" set operation № %u --> %s, res = %d",
            white_spaces, name, number, operation_n, str, res);

        switch ( res )
            {
            case 1:
                printf( " (is already %s).", newm == 0 ? "OFF" : "ON" );
                break;

            case 3:
                printf( " (mode %d > modes count %d).",  operation_n,
                    operations_count );
                break;

            case 4:
                printf( " (mode index must be in [1..%d], got 0).",
                    operations_count );
                break;

            case 5:
                printf( " (IDLE --> PAUSE is not possible)." );
                break;

            case 6:
                printf( " (IDLE --> STOP is not possible)." );
                break;

             default:
                 if ( res > 100 )
                    {
                    printf( " (can't on)." );
                    break;
                    }

                printf( "." );
                break;
            }
        SetColor( RESET );
        printf( "\n" );

        for ( u_int i = 0; i < state.size(); i++ )
            {
            printf( "%sstate[ %d ] = %u (",
                white_spaces, i, state[ i ] );
            print_binary( state[ i ] );
            printf( ")\n" );
            }
        printf( "\n" );

        }

    return res;
    }
//-----------------------------------------------------------------------------
int tech_object::get_mode( u_int operation )
    {
    if ( operation > operations_count || 0 == operation ) return 0;

    int idx = operation - 1;
    return ( int )( ( u_int_4 ) ( state.at( idx / 32 ) >> idx % 32 ) & 1 );
    }
//-----------------------------------------------------------------------------
int tech_object::get_operation_state( u_int operation ) const
    {
    if ( operation > operations_count || 0 == operation ) return 0;

    return ( *operations_manager )[ operation ]->get_state();
    }
//-----------------------------------------------------------------------------
int tech_object::check_on_mode( u_int operation, char* reason, int max_len )
    {
    if ( operation > operations_count || 0 == operation ) return 0;

    return (*operations_manager)[ operation ]->check_on_run_state( reason, max_len );
    }
//-----------------------------------------------------------------------------
int tech_object::evaluate()
    {
    for ( u_int i = 0; i < operations_count; i++ )
        {
        int idx = i + 1;
        auto op = ( *operations_manager )[ idx ];
        op->evaluate();

        const unsigned int ERR_STR_SIZE = 150;

        if ( G_PAC_INFO()->par[ PAC_info::P_AUTO_PAUSE_OPER_ON_DEV_ERR ] == 0 &&
            op->get_state() == operation::RUN )
        	{
            //Проверка режима на проверку ОС устройств.
            char res_str[ ERR_STR_SIZE ] = "авария устройств ";

            const int OFFSET = strlen( res_str );
            int res = op->check_devices_on_run_state( res_str + OFFSET,
                ERR_STR_SIZE - OFFSET );
            if ( res && is_check_mode( idx ) == 1 )
                {
                set_err_msg( res_str, idx, 0, ERR_TO_FAIL_STATE );
                op->pause();
                lua_on_pause( idx );
                }
        	}

        if ( op->get_state() == operation::RUN )
            {
            //Проверка операции на корректные параметры шагов.
            char res_str[ ERR_STR_SIZE ] = "";

            int res = op->check_steps_params( res_str, ERR_STR_SIZE );
            if ( res )
                {
                set_err_msg( res_str, idx, 0, ERR_TO_FAIL_STATE );
                op->pause();
                lua_on_pause( idx );
                }

            //Проверка на превышение максимального времени шага.
            res_str[ 0 ] = '\0';
            res = op->check_max_step_time( res_str, ERR_STR_SIZE );
            if ( res )
                {
                set_err_msg( res_str, idx, 0, ERR_TO_FAIL_STATE );
                op->pause();
                lua_on_pause( idx );
                }
            }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::check_off_mode( u_int mode )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::final_mode( u_int mode )
    {
    if ( mode > operations_count || 0 == mode ) return 1;

    operations_manager->reset_active_operation_or_idle_time();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_exec_cmd( u_int cmd )
    {
    tech_object::exec_cmd( cmd );

    return lua_manager::get_instance()->int_exec_lua_method( name_Lua, "exec_cmd",
        cmd, "int tech_object::lua_exec_cmd( u_int cmd )" );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_function( const char* function_name,
    const char* comment, u_int mode, bool show_error )
    {
    //Проверка на наличии функции function_name
    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        name_Lua );
    if ( lua_isnoneornil( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_remove( lua_manager::get_instance()->get_Lua(), -1 );
        return 0;
        }
    lua_getfield( lua_manager::get_instance()->get_Lua(), -1, function_name );
    lua_remove( lua_manager::get_instance()->get_Lua(), -2 );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        return lua_manager::get_instance()->int_2_exec_lua_method( name_Lua,
            function_name, mode, show_error ? 1 : 0, comment );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_get_run_step_after_pause( u_int mode ) const
    {
    if ( auto function_name = "get_run_step_after_pause";
        G_LUA_MANAGER->is_exist_lua_function( name_Lua, function_name ) )
        {
        return G_LUA_MANAGER->int_exec_lua_method( name_Lua,
            function_name, mode,
            "int tech_object::lua_get_run_step_after_pause( u_int mode )" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_on_start( u_int mode, bool show_error )
    {
    return lua_check_function( "check_on_start",
        "int tech_object::check_on_start( u_int mode )", mode, show_error );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_on_pause( u_int mode, bool show_error )
    {
    return lua_check_function( "check_on_pause",
        "int tech_object::check_on_pause( u_int mode )", mode, show_error );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_on_stop( u_int mode, bool show_error )
    {
    return lua_check_function( "check_on_stop",
        "int tech_object::check_on_stop( u_int mode )", mode, show_error );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_on_mode( u_int mode, bool show_error )
    {
    char err_msg[ 200 ] = "";

    if ( G_PAC_INFO()->par[ PAC_info::P_RESTRICTIONS_MODE ] == 0 )
        {
        //Проверка на наличие уже включенной операции, запрещающей текущую.
        //1. Операции текущего объекта.
        lua_State *L = lua_manager::get_instance()->get_Lua();
        lua_getfield( L, LUA_GLOBALSINDEX, "restrictions" );
        if ( lua_istable( L, -1 ) )
            {
            lua_rawgeti( L, -1, serial_idx );           //Номер объекта.
            if ( lua_istable( L, -1 ) )
                {
                lua_rawgeti( L, -1, mode );             //Номер операции.
                if ( lua_istable( L, -1 ) )
                    {
                    //1 Ограничения внутри объекта.
                    lua_rawgeti( L, -1, 1 );
                    if ( lua_istable( L, -1 ) )
                        {
                        for ( u_int i = 1; i <= get_modes_count(); i++ )
                            {
                            lua_rawgeti( L, -1, i );    //Номер ограничения.
                            if ( lua_isnumber( L, -1 ) )
                                {
                                //Есть ограничение, проверяем на активность операции.
                                if ( get_mode( i ) )
                                    {
                                    if ( show_error ) set_err_msg( err_msg,
                                        mode, i, ERR_CANT_ON_2_OPER );
                                    lua_pop( L, 5 );
                                    return 1000 + i;
                                    }
                                }
                            lua_pop( L, 1 );
                            }
                        }
                    lua_pop( L, 1 );

                    //2 Ограничения от других объектов.
                    lua_rawgeti( L, -1, 2 );
                    if ( lua_istable( L, -1 ) )
                        {
                        for ( u_int k = 1; k <= G_TECH_OBJECT_MNGR()->get_count(); k++ )
                            {
                            lua_rawgeti( L, -1, k ); //Объект с номером k.
                            if ( lua_istable( L, -1 ) )
                                {
                                tech_object* t_obj = G_TECH_OBJECT_MNGR()->get_tech_objects( k - 1 );
                                for ( u_int i = 1;
                                    i <= t_obj->get_modes_count(); i++ )
                                    {
                                    lua_rawgeti( L, -1, i );    //Номер ограничения.
                                    if ( lua_isnumber( L, -1 ) )
                                        {
                                        //Есть ограничение, проверяем на активность операции.
                                        if ( t_obj->get_mode( i ) )
                                            {
                                            if ( show_error )
                                                {
                                                snprintf( err_msg, sizeof( err_msg ),
                                                    "уже включена операция %.1d \'%s\' для объекта \'%s %d\'",
                                                    i,
                                                    ( *t_obj->get_modes_manager() )[ i ]->get_name(),
                                                    t_obj->get_name(), t_obj->get_number() );

                                                set_err_msg( err_msg, mode, i, ERR_CANT_ON_2_OBJ );
                                                }

                                            lua_pop( L, 6 );
                                            return 10000 + i;
                                            }
                                        }
                                    lua_pop( L, 1 );
                                    }
                                }
                            lua_pop( L, 1 );
                            }
                        }
                    lua_pop( L, 1 );
                    }
                lua_pop( L, 1 );
                }
            lua_pop( L, 1 );
            }
        lua_pop( L, 1 );
        }

    if ( int res = tech_object::check_on_mode( mode, err_msg, sizeof( err_msg ) ) )
        {
        if ( show_error ) set_err_msg( err_msg, mode );

        return 1000 + res;
        }

    //TODO. Устаревшее название функции. Оставлено для совместимости.
    //Проверка на наличии функции check_on_mode.
    if ( auto old_function_name = "check_on_mode";
        G_LUA_MANAGER->is_exist_lua_function( name_Lua, old_function_name ) )
        {
        return lua_manager::get_instance()->int_2_exec_lua_method( name_Lua,
            old_function_name, mode, show_error ? 1 : 0,
            "int tech_object::lua_check_on_mode( u_int mode )" );
        }

    //Проверка на наличии функции user_check_operation_on.
    if ( auto new_function_name = "user_check_operation_on";
        G_LUA_MANAGER->is_exist_lua_function( name_Lua, new_function_name ) )
        {
        return lua_manager::get_instance()->int_2_exec_lua_method( name_Lua,
            new_function_name, mode, show_error ? 1 : 0,
            "int tech_object::lua_check_on_mode( u_int mode )" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void tech_object::lua_init_mode( u_int mode )
    {
    lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "init_mode", mode, "void tech_object::lua_init_mode( u_int mode )" );

    //Обновляем состояние ограничений.
    for ( u_int i = 1; i <= get_modes_count(); i++ )
        {
        available[ i - 1 ] = 0;
        }
    for ( u_int i = 1; i <= get_modes_count(); i++ )
        {
        if ( get_mode( i ) == 1 )
            {
            check_availability( i );
            }
        }
    check_availability( mode );
    }
//-----------------------------------------------------------------------------
void tech_object::check_availability( u_int operation_n )
    {
    //Запрет на включение других операций, запрещающаемых текущей.
    //1. Операции текущего объекта.
    lua_State *L = lua_manager::get_instance()->get_Lua();
    lua_getfield( L, LUA_GLOBALSINDEX, "restrictions" );
    if ( lua_istable( L, -1 ) )
        {
        lua_rawgeti( L, -1, serial_idx );           //Номер объекта.
        if ( lua_istable( L, -1 ) )
            {
            //Проход по всем операциям.
            for ( u_int i = 1; i <= get_modes_count(); i++ )
                {
                if ( i == operation_n ) continue;

                lua_rawgeti( L, -1, i );            //Номер операции.
                if ( lua_istable( L, -1 ) )
                    {
                    //1 Ограничения внутри объекта.
                    lua_rawgeti( L, -1, 1 );
                    if ( lua_istable( L, -1 ) )
                        {
                        lua_rawgeti( L, -1, operation_n );
                        if ( lua_isnumber( L, -1 ) )
                            {
                            available[ i - 1 ]++;
                            }
                        lua_pop( L, 1 );
                        }
                    lua_pop( L, 1 );
                    }
                lua_pop( L, 1 );
                }
            }
        lua_pop( L, 1 );
        }
    lua_pop( L, 1 );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_off_mode( u_int mode )
    {
    return lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "check_off_mode", mode, "int tech_object::lua_check_off_mode( u_int mode )" );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_final_mode( u_int mode )
    {
    tech_object::final_mode( mode );

    lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "final_mode", mode, "int tech_object::lua_final_mode( u_int mode )" );

    //Обрабатываем запрет на включение других операций, запрещающаемых
    //текущей, и запрет на последующие операции.
    //1. Операции текущего объекта.
    lua_State *L = lua_manager::get_instance()->get_Lua();
    lua_getfield( L, LUA_GLOBALSINDEX, "restrictions" );
    if ( lua_istable( L, -1 ) )
        {
        lua_rawgeti( L, -1, serial_idx );           //Номер объекта.
        if ( lua_istable( L, -1 ) )
            {
            //Проход по всем операциям.
            for ( u_int i = 1; i <= get_modes_count(); i++ )
                {
                if ( i == mode ) continue;

                lua_rawgeti( L, -1, i );            //Номер операции.
                if ( lua_istable( L, -1 ) )
                    {
                    //1 Ограничения внутри объекта.
                    lua_rawgeti( L, -1, 1 );
                    if ( lua_istable( L, -1 ) )
                        {
                        lua_rawgeti( L, -1, mode );
                        if ( lua_isnumber( L, -1 ) )
                            {
                            available[ i - 1 ]--;
                            }
                        lua_pop( L, 1 );
                        }
                    lua_pop( L, 1 );
                    }
                lua_pop( L, 1 );
                }

            lua_rawgeti( L, -1, mode );            //Текущая операция.
            if ( lua_istable( L, -1 ) )
                {
                //1 Ограничения на последующие операции внутри объекта.
                lua_rawgeti( L, -1, 3 );
                if ( lua_istable( L, -1 ) )
                    {
                    //Проход по всем операциям.
                    for ( u_int i = 1; i <= get_modes_count(); i++ )
                        {
                        lua_rawgeti( L, -1, i );
                        if ( lua_isnumber( L, -1 ) )
                            {
                            available[ i - 1 ]++;
                            }
                        lua_pop( L, 1 );
                        }
                    }
                lua_pop( L, 1 );
                }
            lua_pop( L, 1 );
            }

        lua_pop( L, 1 );
        }
    lua_pop( L, 1 );

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_init_params()
    {
    init_params();

    static const auto LUA_INIT_FUNCTIONS =
        {
        "init_params_uint",
        "init_params_float",
        "init_rt_params_uint",
        "init_rt_params_float"
        };

    auto res = 0;
    for ( auto f_name : LUA_INIT_FUNCTIONS )
        {
        if ( G_LUA_MANAGER->is_exist_lua_function( name_Lua, f_name ) )
            {
            auto init_res = lua_manager::get_instance()->int_exec_lua_method( name_Lua,
                f_name, 0, fmt::format(
                "int tech_object::lua_init_params() for function '{}'",
                f_name ).c_str() );

            if ( init_res != 0 )
                {
                res = init_res;
                }
            }
        }

    return res;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_init_runtime_params()
    {
    init_runtime_params();

    return lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "init_runtime_params", 0, "int tech_object::lua_init_runtime_params()" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_on_pause( u_int mode )
    {
    lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "on_pause", mode, "int tech_object::lua_on_pause( u_int mode )" );
    return 0;
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_on_stop( u_int mode )
    {
    lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "on_stop", mode, "int tech_object::lua_on_stop( u_int mode )" );
    return 0;
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_on_start( u_int mode )
    {
    lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "on_start", mode, "int tech_object::lua_on_start( u_int mode )" );
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::save_device( char *buff ) const
    {
    int res =
        sprintf( buff, "t.%s = t.%s or {}\nt.%s=\n\t{\n", name_Lua, name_Lua,
        name_Lua );

    //Состояние и команда.
    res += sprintf( buff + res, "\tCMD=%lu,\n", ( u_long ) cmd );
    //Номер последнего загруженного рецепта.
    res += sprintf( buff + res, "\tLASTRECNMR=%d,\n", lastLoadedRecipeNmr );
    //Имя последнего загруженного рецепта.
    res += sprintf( buff + res, "\tLASTRECNAME='%s',\n", lastLoadedRecipeName.c_str() );
    res += sprintf( buff + res, "\tST=\n\t\t{\n\t\t" );

    for ( u_int i = 0; i < state.size(); i++ )
        {
        res += sprintf( buff + res, "%lu, ", ( long int ) state[ i ] );
        }
    res += sprintf( buff + res, "\n\t\t},\n" );

    //Операции.
    res += sprintf( buff + res, "\tMODES=\n\t\t{\n\t\t" );
    for ( u_int i = 1; i <= operations_count; i++ )
        {
        res += sprintf( buff + res, "%d, ", get_operation_state( i ) ? 1 : 0 );
        }
    res += sprintf( buff + res, "\n\t\t},\n" );
    res += sprintf( buff + res, "\tOPERATIONS=\n\t\t{\n\t\t" );
    for ( u_int i = 1; i <= operations_count; i++ )
        {
        res += sprintf( buff + res, "%d, ", get_operation_state( i ) );
        }
    res += sprintf( buff + res, "\n\t\t},\n" );

    //Доступность операций.
    res += sprintf( buff + res, "\tAVAILABILITY=\n\t\t{\n\t\t" );
    for ( u_int i = 1; i <= operations_count; i++ )
        {
        res += sprintf( buff + res, "%d, ", available.at( i - 1 ) == 0 ? 1 : 0 );
        }
    res += sprintf( buff + res, "\n\t\t},\n" );

    //Время простоя или текущей активной операции.
    auto duration = operations_manager->get_idle_time_sec();
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "\tACTIVE_OPERATION_OR_IDLE_TIME={},\n", duration ).size;

    //Время главного шага текущей активной операции.
    duration = 0L;
    for ( u_int i = 1; i <= operations_count; i++ )
        {
        if ( get_operation_state( i ) == 1 )
            {
            auto op = ( *operations_manager )[ i ];
            duration = op->active_step_evaluation_time() / 1000;
            }
        }
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "\tACTIVE_STEP_TIME={},\n", duration ).size;

    //Время операций.
    res += sprintf( buff + res, "\tMODES_TIME=\n\t\t{\n\t\t" );
    for ( u_int i = 0; i < operations_count; i++ )
        {
        auto op = ( *operations_manager )[ i + 1 ];
        auto t = op->evaluation_time() / 1000;
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "{}, ", t ).size;
        }
    res += sprintf( buff + res, "\n\t\t},\n" );

    //Шаги.
    res += sprintf( buff + res, "\tMODES_STEPS=\n\t\t{\n\t\t" );
    for ( u_int i = 0; i < operations_count; i++ )
        {
        res += sprintf( buff + res, "%d, ",
            (*operations_manager)[ i + 1 ]->active_step());
        }
    res += sprintf( buff + res, "\n\t\t},\n" );

    res += sprintf( buff + res, "\tMODES_RUN_STEPS=\n\t\t{\n\t\t" );
    for ( u_int i = 0; i < operations_count; i++ )
        {
        res += sprintf( buff + res, "%d, ",
            ( *operations_manager )[ i + 1 ]->get_run_step() );
        }
    res += sprintf( buff + res, "\n\t\t},\n" );


    for ( u_int i = 1; i <= operations_count; i++ )
        {
        auto operation = ( *operations_manager )[ i ];

        u_int steps_count = operation->get_run_steps_count();
        if ( steps_count == 0 )
            {
            continue;
            }

        res += sprintf( buff + res, "\tSTEPS%d=\n\t\t{\n\t\t", i );
        u_int run_step = operation->get_run_active_step();

        for ( u_int j = 1; j <= steps_count; j++ )
            {
            if ( run_step == j )
                {
                res += sprintf( buff + res, "1, " );
                }
            else
                {
                if ( operation->is_active_run_extra_step( j ) )
                    {
                    res += sprintf( buff + res, "1, " );
                    }
                else
                    {
                    res += sprintf( buff + res, "0, " );
                    }
                }
            }
        res += sprintf( buff + res, "\n\t\t},\n" );
        }

    //States' steps.
    for ( u_int s = operation::RUN; s < operation::STATES_MAX; s++ )
        {
        for ( u_int i = 1; i <= operations_count; i++ )
            {
            auto operation = ( *operations_manager )[ i ];
            auto oper_state = ( *operation )[ s ];
            u_int steps_count = oper_state->steps_count();
            if ( steps_count == 0 ) continue;

            res += sprintf( buff + res, "\t%s_STEPS%d=\n\t\t{\n\t\t",
                operation::en_state_str[ s ], i );
            u_int static_step = oper_state->active_step();
            for ( u_int j = 1; j <= steps_count; j++ )
                {
                if ( static_step == j )
                    {
                    res += sprintf( buff + res, "1, " );
                    }
                else
                    {
                    res += sprintf( buff + res,
                        oper_state->is_active_extra_step( j ) ? "1, " : "0, " );
                    }
                }
            res += sprintf( buff + res, "\n\t\t},\n" );
            }
        }

    //Параметры.
    res += par_float.save_device( buff + res, "\t" );
    res += par_uint.save_device( buff + res, "\t" );
    res += rt_par_float.save_device( buff + res, "\t" );
    res += rt_par_uint.save_device( buff + res, "\t" );

    res += sprintf( buff + res, "\t}\n" );
    return res;
    }
//-----------------------------------------------------------------------------
int tech_object::set_cmd( const char *prop, u_int idx, double val )
    {
    if ( strcmp( prop, "CMD" ) == 0 )
        {
        if ( G_DEBUG )
            {
            printf( "tech_object::set_cmd() - prop = \"%s\", val = %f\n",
                prop, val );
            }

        if ( 0. == val )
            {
            cmd = 0;
            return 0;
            }

        auto mode = static_cast<u_int>( val );
        char new_state = 0;

        if ( mode >= 200'000 && mode < 300'000 )      // On extra step.
            {
            auto step_n = mode % 100;
            auto operation_n = mode / 100 - 2'000;

            return set_extra_step( operation_n, step_n, 1 );
            }
        else
            {
            if ( mode >= 300'000 && mode < 400'000 )      // Off extra step.
                {
                auto step_n = mode % 100;
                auto operation_n = mode / 100 - 3'000;

                return set_extra_step( operation_n, step_n, 0 );
                }

            if ( mode >= 400'000 && mode < 500'000 )      // To step.
                {
                if ( auto step_n = mode % 100, operation_n = mode / 100 - 4'000;
                    operation_n > 0 && operation_n <= operations_count )
                    {
                    if ( auto op = ( *operations_manager )[ operation_n ];
                        op->get_state() != operation::IDLE )
                        {
                        op->to_step( step_n );
                        return 0;
                        }
                    }

                return 1;
                }
            }

        if ( mode >= 1'000 && mode < 2'000 )      // On mode.
            {
            mode = mode - 1'000;
            new_state = 1;
            }
        else
            {
            if ( mode >= 2'000 && mode < 3'000 )  // Off mode.
                {
                mode = mode - 2'000;
                new_state = 0;
                }
            else
                {
                if ( mode >= 100'000 && mode < 200'000 )      // On state mode.
                    {
                    new_state = mode % 100;
                    mode = mode / 100 - 1'000;
                    cmd = ( int ) val;
                    return set_mode( mode, new_state );
                    }
                else
                    {
                    if ( G_DEBUG )
                        {
                        printf( "Error complex_state::parse_cmd - new_mode = %lu\n",
                            ( unsigned long int ) mode );
                        }
                    return 1;
                    }
                }
            }

        if ( mode > get_modes_count() + 1 )
            {
            // Command.
            cmd = lua_exec_cmd( mode );
            }
        else
            {
            // On/off mode.
            int res = set_mode( mode, new_state );
            if ( 0 == res )
                {
                cmd = ( int ) val;  // Ok.
                }
            else
                {
                cmd = res;          // Ошибка.
                }
            }

        return 0;
        }

    if ( strcmp( prop, "S_PAR_F" ) == 0 )
        {
        if ( G_DEBUG )
            {
            printf( "\"%s%d\" %s.S_PAR_F[%d]=%f\n",
                name, number, name_Lua, idx, val );
            }

        par_float.save( idx, ( float ) val );
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_F" ) == 0 )
        {
        rt_par_float[ idx ] = ( float ) val;
        return 0;
        }

    if ( strcmp( prop, "S_PAR_UI" ) == 0 )
        {
        par_uint.save( idx, ( u_int_4 ) val );
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_UI" ) == 0 )
        {
        rt_par_uint[ idx ] = ( u_int_4 ) val;
        return 0;
        }

    if ( strcmp( prop, "MODES" ) == 0 )
        {
        set_mode( idx, ( int ) val );
        return 0;
        }

    if ( G_DEBUG )
        {
        printf( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %f\n",
            prop, idx, val );
        }

    return 1;
    }
//-----------------------------------------------------------------------------
int tech_object::save_params_as_Lua_str( char* str )
    {
    int res = sprintf( str, "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "par_float", ID_PAR_FLOAT );
    res += par_float.save_device_ex( str + res, "", "values"  ) - 2;
    res += sprintf( str + res, "%s", " }\n" );

    res += sprintf( str + res, "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "rt_par_float", ID_RT_PAR_FLOAT );
    res += rt_par_float.save_device_ex( str + res, "", "values"  ) - 2;
    res += sprintf( str + res, "%s", " }\n" );

    res += sprintf( str + res, "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "par_uint", ID_PAR_UINT );
    res += par_uint.save_device_ex( str + res, "", "values"  ) - 2;
    res += sprintf( str + res, "%s", " }\n" );

    res += sprintf( str + res, "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "rt_par_uint", ID_RT_PAR_UINT );
    res += rt_par_uint.save_device_ex( str + res, "", "values"  ) - 2;
    res += sprintf( str + res, "%s", " }\n" );

    return res;
    }
//-----------------------------------------------------------------------------
int tech_object::set_param( int par_id, int index, double value )
    {
    switch ( par_id )
        {
    case ID_PAR_FLOAT:
        par_float.save( index, ( float ) value );
        break;

    case ID_RT_PAR_FLOAT:
        rt_par_float[ index ] = ( float ) value;
        break;

    case ID_PAR_UINT:
        par_uint.save( index, ( u_int ) value );
        break;

    case ID_RT_PAR_UINT:
        rt_par_uint[ index ] = ( u_int ) value;
        break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::is_check_mode( int mode ) const
    {
    if ( auto res = lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "is_check_mode", mode, "int tech_object::is_check_mode( u_int mode )" );
        res >= 0 )
        {
        return res;
        }

    return 1;
    }
//-----------------------------------------------------------------------------
int tech_object::set_err_msg( const char *err_msg, int mode, int new_mode,
                             ERR_MSG_TYPES type /*= ERR_CANT_ON */ )
    {
    err_info *new_err = new err_info;
    static int error_number = 0;

    error_number++;
    new_err->n = error_number;
    new_err->type = type;

    switch ( type )
        {
        case ERR_CANT_ON:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - не включена операция %.1d \'%s\' - %s.",
                name, number, mode, ( *operations_manager )[ mode ]->get_name(),
                err_msg );
            break;

        case ERR_ON_WITH_ERRORS:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - включена с ошибкой операция %.1d \'%s\' - %s.",
                name, number, mode, ( *operations_manager )[ mode ]->get_name(),
                err_msg );
            break;

        case ERR_OFF:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - отключена операция %.1d \'%s\' - %s.",
                name, number, mode, ( *operations_manager )[ mode ]->get_name(),
                err_msg );
            break;

        case ERR_OFF_AND_ON:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - переход от %.1d \'%s\' к %.1d \'%s\'.",
                name, number, mode, ( *operations_manager )[ mode ]->get_name(),
                new_mode, ( *operations_manager )[ new_mode ]->get_name() );

            if ( strcmp( err_msg, "" ) != 0 )
                {
                snprintf( new_err->msg + strlen( new_err->msg ) - 1,
                    sizeof( new_err->msg ) - strlen( new_err->msg ) - 1,
                    " - %s.", err_msg );
                }
            break;

        case ERR_DURING_WORK:
            if ( mode > 0 )
                {
                snprintf( new_err->msg, sizeof( new_err->msg ),
                    "\'%s %d\' - операция %.1d \'%s\' - %s.",
                    name, number, mode, ( *operations_manager )[ mode ]->get_name(),
                    err_msg );
                }
            else
                {
                snprintf( new_err->msg, sizeof( new_err->msg ),
                    "\'%s %d\' - %s.",
                    name, number, err_msg );
                }

            break;

        case ERR_ALARM:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - %s.", name, number, err_msg );
            break;

        case ERR_TO_FAIL_STATE:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - авария операции %.1d \'%s\' - %s.",
                name, number, mode, ( *operations_manager )[ mode ]->get_name(),
                err_msg );
            break;

        case ERR_CANT_ON_2_OPER:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - не включена операция %.1d \'%s\' - "
                "уже включена операция %.1d \'%s\'.",
                name, number,
                mode, ( *operations_manager )[ mode ]->get_name(),
                new_mode, ( *operations_manager )[ new_mode ]->get_name() );

            if ( strcmp( err_msg, "" ) != 0 )
                {
                snprintf( new_err->msg + strlen( new_err->msg ) - 1,
                    sizeof( new_err->msg ) - strlen( new_err->msg ) - 1,
                    " - %s.", err_msg );
                }
            break;

        case ERR_CANT_ON_2_OBJ:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s %d\' - не включена операция %.1d \'%s\' - %s.",
                name, number,
                mode, ( *operations_manager )[ mode ]->get_name(),
                err_msg );
            break;

        default:
            if ( G_DEBUG )
                {
                printf( "Error tech_object::set_err_msg(...) - unknown error type!\n" );
                debug_break;
                }
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%s\' - операция %.1d \'%s\' - %s.",
                name, mode, ( *operations_manager )[ mode ]->get_name(), err_msg );
            break;
        }

    if ( G_DEBUG )
        {
        printf( "Событие -> %s\n", new_err->msg );
        }

    if ( errors.size() < E_MAX_ERRORS_SIZE )
        {
        errors.push_back( new_err );
        }
    else
        {
        if ( G_DEBUG )
            {
            printf( "Error: max errors count (%d) is reached.",
                E_MAX_ERRORS_SIZE );
            }
        delete new_err;
        new_err = nullptr;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
bool tech_object::is_any_error() const
    {
    auto iter = errors.begin();
    while ( iter != errors.end() )
        {
        if ( auto error = *iter;
            tech_obj_error::get_priority( error->type ) ==
            ALARM_CLASS_PRIORITY::P_ALARM )
            return true;
        ++iter;
        }

    return false;
    }
//-----------------------------------------------------------------------------
void tech_object::print() const
    {
    printf( "#%2d\n", serial_idx );
    printf( "Object \'%s\' [%d], type %d\n", name, number, type );
    printf( "par_float[%d] par_uint[%d] rt_par_float[%d] rt_par_uint[%d]\n",
        par_float.get_count(), par_uint.get_count(),
        rt_par_float.get_count(), rt_par_uint.get_count() );
    printf( "timers[%d]\n", timers.get_count() );

    par_float.print();
    par_uint.print();
    rt_par_float.print();
    rt_par_uint.print();

    operations_manager->print();
    printf( "\n" );
    }
//-----------------------------------------------------------------------------
bool tech_object::is_idle() const
    {
    for ( u_int i = 0; i < state.size(); i++ )
        {
        if ( state[ i ] > 0 )
            {
            return false;
            }
        }

    return true;
    }
//-----------------------------------------------------------------------------
bool tech_object::is_any_important_mode()
    {
    static char has_Lua_impl = 0;
    if ( has_Lua_impl == 0 )
        {
        //Проверка на наличии Lua-функции is_any_important_mode().
        lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
            name_Lua );
        lua_getfield( lua_manager::get_instance()->get_Lua(), -1,
            "is_any_important_mode" );
        lua_remove( lua_manager::get_instance()->get_Lua(), -2 );

        if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
            {
            has_Lua_impl = 2;
            }
        else
            {
            has_Lua_impl = 1;
            }
        //Удаляем "is_any_important_mode" со стека.
        lua_remove( lua_manager::get_instance()->get_Lua(), -1 );
        }

    if ( has_Lua_impl == 2 )
        {
        int res = lua_manager::get_instance()->int_no_param_exec_lua_method( name_Lua,
            "is_any_important_mode",
            "void tech_object::is_any_important_mode()" );

        return res > 0;
        }

    for ( u_int i = 0; i < state.size(); i++ )
        {
        if ( state[ i ] > 0 )
            {
            return true;
            }
        }

    return false;
    }
//-----------------------------------------------------------------------------
int tech_object::check_operation_on( u_int operation_n, bool show_error )
    {
    operation* op = ( *operations_manager )[ operation_n ];

    //Проверка режима на проверку ОС устройств.
    const int S_SIZE = 41;
    char res_str[ S_SIZE ] = "авария ";

    int l = strlen( res_str );
    int res = op->check_devices_on_run_state( res_str + l, S_SIZE - l );
    if ( res && is_check_mode( operation_n ) == 1 )
        {
        if ( show_error ) set_err_msg( res_str, operation_n );
        res = operation_n + 1000;
        }
    else
        {
        if ( ( res = lua_check_on_mode( operation_n, show_error ) ) != 0 ) // Check if possible.
            {
            res += 100;
            }
        }

    return res;
    }
//-----------------------------------------------------------------------------
void tech_object::set_serial_idx( u_int idx )
    {
    serial_idx = idx;
    }
//-----------------------------------------------------------------------------
int tech_object::set_extra_step( u_int operation, int step, int cmd )
    {
    static u_char idx = 0;
    if ( operation > operations_count || 0 == operation ) return 1;

    switch ( cmd )
        {
        case 0:
            ( *operations_manager )[ operation ]->off_extra_step( step );
            break;

        case 1:
            ( *operations_manager )[ operation ]->on_extra_step( step );
            break;

        default:
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int tech_object_manager::init_params()
    {
    for( u_int i = 0; i < tech_objects.size(); i++ )
        {
        tech_objects[ i ]->lua_init_params();
        }

    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        "init_params" );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_manager::get_instance()->void_exec_lua_method( "", "init_params",
            "int tech_object_manager::init_params()" );
        }
    lua_remove( lua_manager::get_instance()->get_Lua(), -1 ); // stack: init_params

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::init_runtime_params()
    {
    for ( u_int i = 0; i < tech_objects.size(); i++ )
        {
        tech_objects[ i ]->lua_init_runtime_params();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
tech_object_manager* tech_object_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new tech_object_manager();
        }

    return instance;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::get_object_with_active_mode( u_int mode,
    u_int start_idx, u_int end_idx )
    {
    for ( u_int i = start_idx;
        i <= end_idx && i < tech_objects.size(); i++ )
        {
        if ( tech_objects.at( i )->get_mode( mode ) ) return i;
        }

    return -1;
    }
//-----------------------------------------------------------------------------
tech_object* tech_object_manager::get_tech_objects( u_int idx )
    {
    try
        {
        return tech_objects.at( idx );
        }
    catch (...)
        {
#ifdef PAC_PC
        debug_break;
#endif // PAC_PC

        return stub;
        }
    }
//-----------------------------------------------------------------------------
u_int tech_object_manager::get_count() const
    {
    return tech_objects.size();
    }
//-----------------------------------------------------------------------------
int tech_object_manager::evaluate()
    {
    int res = 0;
    for ( u_int i = 0; i < tech_objects.size(); i++ )
        {
        tech_objects.at( i )->evaluate();
        }

    static char has_Lua_eval = 0;
    if ( has_Lua_eval == 0 )
        {
        lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
            "eval" );

        if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
            {
            has_Lua_eval = 2;
            }
        else
            {
            has_Lua_eval = 1;
            }

        lua_remove( lua_manager::get_instance()->get_Lua(), -1 ); // stack: eval
        }

    if ( has_Lua_eval == 2 )
        {
        res = lua_manager::get_instance()->void_exec_lua_method( "", "eval",
            "void tech_object_manager::evaluate()" );
        }

    return res;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::init_objects()
    {
    //-Вызов пользовательской функции инициализации.
    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        "init" );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_manager::get_instance()->void_exec_lua_method( "", "init",
            "int tech_object_manager::init_objects()" );
        }
    lua_remove( lua_manager::get_instance()->get_Lua(), -1 ); // stack: init

    return 0;
    }
//-----------------------------------------------------------------------------
tech_object_manager::~tech_object_manager()
    {
    delete stub;
    stub = nullptr;

// Удаляются в Lua.
//    for ( u_int i = 0; i < tech_objects.size(); i++ )
//        {
//        delete tech_objects[ i ];
//        }
    }
//-----------------------------------------------------------------------------
tech_object_manager::tech_object_manager()
    {
    stub = new tech_object( "Заглушка", 1, 1000, "STUB", 1, 1, 1, 1, 1, 1 );
    }
//-----------------------------------------------------------------------------
void tech_object_manager::add_tech_object( tech_object* new_tech_object )
    {
    tech_objects.push_back( new_tech_object );
    new_tech_object->set_serial_idx( tech_objects.size() );

    G_ERRORS_MANAGER->add_error( new tech_obj_error( new_tech_object ) );
    }

//-----------------------------------------------------------------------------
int tech_object_manager::save_params_as_Lua_str( char* str )
    {
    int res = 0;
    str[ 0 ] = 0;

    for ( u_int i = 0; i < tech_objects.size(); i++ )
        {
        res += tech_objects[ i ]->save_params_as_Lua_str( str + res );
        }
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
tech_object_manager* G_TECH_OBJECT_MNGR()
    {
    return tech_object_manager::get_instance();
    }
//-----------------------------------------------------------------------------
tech_object* G_TECH_OBJECTS( u_int idx )
    {
    return tech_object_manager::get_instance()->get_tech_objects( idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
