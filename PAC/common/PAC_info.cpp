#include <string.h>
#include "fmt/format.h"
#include <inttypes.h>

#include "PAC_info.h"
#include "PAC_err.h"

#include "tech_def.h"

#include "lua_manager.h"
#include "bus_coupler_io.h"
#include "device/manager.h"

#include "OPCUAServer.h"


extern bool G_NO_IO_NODES;

auto_smart_ptr < PAC_info > PAC_info::instance;///< Экземпляр класса.

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void PAC_info::eval()
    {
    if ( restrictions_set_to_off_time &&
        get_delta_millisec( restrictions_set_to_off_time ) >
        par[ P_RESTRICTIONS_MANUAL_TIME ] )
        {
        par[ P_RESTRICTIONS_MODE ] = 0;
        restrictions_set_to_off_time = 0;
        }

    if ( get_delta_millisec( last_check_time ) > 1000 )
        {
        up_msec += get_delta_millisec( last_check_time );
        if ( up_msec >= MSEC_IN_DAY )
            {
            up_days++;
            up_msec %= MSEC_IN_DAY;
            }
        last_check_time = get_millisec();

        up_hours = up_msec / ( 1000UL * 60 * 60 );
        up_mins = up_msec / ( 1000UL * 60 ) % 60;
        up_secs = up_msec / 1000 % 60;

        auto res = fmt::format_to_n( up_time_str, C_MAX_STR_LENGTH - 1,
            "{} дн. {:02}:{:02}:{:02}", up_days, up_hours, up_mins, up_secs );
        *res.out = '\0';

        // Check I/O nodes communication status.
        nodes_comm_error = 0;
        const auto IO_MANAGER = io_manager::get_instance();
        unsigned int nc = IO_MANAGER->get_nodes_count();
        for ( unsigned int i = 0; i < nc; i++ )
            {
            const auto node = IO_MANAGER->get_node( i );
            // Error or warning state detected.
            if ( auto state = node->get_display_state();
                state == io_manager::io_node::DISPLAY_STATES::DST_ERROR ||
                state == io_manager::io_node::DISPLAY_STATES::DST_WARNING )
                {
                nodes_comm_error = 1;
                break;
                }
            }

        // Check watchdog devices status.
        watchdog_error = 0;

        auto dev_count = G_DEVICE_MANAGER()->get_device_count();
        for ( size_t i = 0; i < dev_count; i++ )
            {
            const auto dev = G_DEVICE_MANAGER()->get_device( i );
            if ( dev && dev->get_type() == device::DT_WATCHDOG &&
                dev->get_state() < 0 )
                {
                watchdog_error = 1;
                break;
                }
            }

        commun_error = nodes_comm_error || watchdog_error ? 1 : 0;
        }
    }
//-----------------------------------------------------------------------------
void PAC_info::reset_params()
    {
    par[ P_MIX_FLIP_PERIOD ] = 180;
    par[ P_MIX_FLIP_UPPER_TIME ] = 2'000;
    par[ P_MIX_FLIP_LOWER_TIME ] = 1'000;
    par[ P_V_OFF_DELAY_TIME ] = 1'000;
    par[ P_V_BOTTOM_OFF_DELAY_TIME ] = 1'200;

    par[ P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ] = 50;
    par[ P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME ] = 300;

    par[ P_RESTRICTIONS_MODE ] = 0;
    par[ P_RESTRICTIONS_MANUAL_TIME ] = 2 * 60 * 1'000; // 2 minutes.

    par[ P_AUTO_PAUSE_OPER_ON_DEV_ERR ] = 0;

    par[ P_AUTO_OPERATION_WAIT_TIME ] = 60'000;
    par[ P_AUTO_OPERATION_WARN_TIME ] = 20'000;

    par[ P_IS_OPC_UA_SERVER_ACTIVE ] = 1;
    par[ P_IS_OPC_UA_SERVER_CONTROL ] = 0;

    par[ P_BK_ANSWER_MAX_WAIT_TIME ] = 6'000;   // 6 seconds.

    par[ P_STABLE_SAVE_DELAY_MS ] = 60'000;     // 1 minute.
    par[ P_MIN_SAVE_INTERVAL_MS ] = 3'600'000;  // 1 hour (60 * 60 * 1'000).

    par[ P_POST_START_ERROR_PROCESSING_DELAY_MS ] = 5'000; // 5 seconds.

    par.save_all();
    }
//-----------------------------------------------------------------------------
int PAC_info::save_device( char* buff ) const
    {
    int size = fmt::format_to_n( buff, MAX_COPY_SIZE, "t.SYSTEM = \n\t{{\n" ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tRESET_BY={},\n", reset_type ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_DAYS={},\n", up_days ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_HOURS={},\n", up_hours ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_MINS={},\n", up_mins ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_SECS={},\n", up_secs ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_TIME=\"{}\",\n", up_time_str ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tCYCLE_TIME={},\n", cycle_time ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tWASH_VALVE_SEAT_PERIOD={},\n", par[ P_MIX_FLIP_PERIOD ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tWASH_VALVE_UPPER_SEAT_TIME={},\n", par[ P_MIX_FLIP_UPPER_TIME ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tWASH_VALVE_LOWER_SEAT_TIME={},\n",par[ P_MIX_FLIP_LOWER_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_V_OFF_DELAY_TIME={},\n", par[ P_V_OFF_DELAY_TIME ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_V_BOTTOM_ON_DELAY_TIME={},\n", par[ P_V_BOTTOM_OFF_DELAY_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME={},\n", par[ P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_MAIN_CYCLE_WARN_ANSWER_AVG_TIME={},\n", par[ P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_RESTRICTIONS_MODE={},\n", par[ P_RESTRICTIONS_MODE ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_RESTRICTIONS_MANUAL_TIME={},\n", par[ P_RESTRICTIONS_MANUAL_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_AUTO_PAUSE_OPER_ON_DEV_ERR={},\n", par[ P_AUTO_PAUSE_OPER_ON_DEV_ERR ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tCMD={},\n", cmd ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tCMD_ANSWER=\"{}\",\n", cmd_answer ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tVERSION=\"{}\",\n", PRODUCT_VERSION_FULL_STR ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tNODEENABLED = \n\t{{\n\t" ).size;
    unsigned int nc = io_manager::get_instance()->get_nodes_count();
    for ( unsigned int i = 0; i < nc; i++ )
        {
        auto wn = io_manager::get_instance()->get_node( i );
        const char* active_str = wn->is_active ? "1, " : "0, ";
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
            "{}", active_str ).size;
        }
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}},\n" ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tNODEST = \n\t{{\n\t" ).size;
    for ( unsigned int i = 0; i < nc; i++ )
        {
        auto wn = io_manager::get_instance()->get_node( i );
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
            "{}, ", static_cast<int>( wn->get_display_state() ) ).size;
        }
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}},\n" ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_IS_OPC_UA_SERVER_ACTIVE={},\n", par[ P_IS_OPC_UA_SERVER_ACTIVE ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_IS_OPC_UA_SERVER_CONTROL={},\n", par[ P_IS_OPC_UA_SERVER_CONTROL ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_BK_ANSWER_MAX_WAIT_TIME={},\n", par[ P_BK_ANSWER_MAX_WAIT_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tNODES_COMM_ERROR={},\n", nodes_comm_error ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tWATCHDOG_ERROR={},\n", watchdog_error ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tCOMMUN_ERROR={},\n", commun_error ).size;


    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tPARAMS_CHANGE_COUNTER={},\n",
        params_manager::get_instance()->get_params_change_counter() ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tPARAMS_SAVE_COUNTER={},\n",
        params_manager::get_instance()->get_params_save_counter() ).size;


    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\t}}\n" ).size;

    buff[ size ] = '\0';

    return size;
    }
//-----------------------------------------------------------------------------
PAC_info* PAC_info::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new PAC_info();
        }

    return instance;
    }
//-----------------------------------------------------------------------------
void PAC_info::print() const
    {
    printf( "PAC_info\n" );
    }
//-----------------------------------------------------------------------------
int PAC_info::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( strcmp( prop, "CMD" ) == 0 )
        {
        //Команда перезагрузки объекта в виде числа CMD = BASE + номер объекта.
        const int cmd_val = static_cast< int >( val );
        if ( cmd_val >= static_cast< int >( COMMANDS::RELOAD_TECH_OBJECT_BASE ) &&
            cmd_val < static_cast< int >( COMMANDS::RELOAD_TECH_OBJECT_BASE ) + 1000 )
            {
            const u_int obj_n = static_cast< u_int >(
                cmd_val - static_cast< int >( COMMANDS::RELOAD_TECH_OBJECT_BASE ) );
            G_LOG->notice( "Reload tech object [%u] (CMD=%d).", obj_n, cmd_val );
            const int res = G_TECH_OBJECT_MNGR()->reload_object( obj_n );
            auto msg = "Reload object - Ok.";
            if ( res == -1 )
                {
                msg = "Reload object error - object not found.";
                }
            else if ( res == -2 )
                {
                msg = "Reload object error - object is not idle.";
                }
            else if ( res == -3 )
                {
                msg = "Reload object error - Lua reload unavailable.";
                }
            cmd_answer[ 0 ] = 0;
            auto r = fmt::format_to_n( cmd_answer,
                sizeof( cmd_answer ) - 1, "{}", msg );
            *r.out = '\0';
            return res;
            }

        switch ( static_cast<COMMANDS>( static_cast<int>( val ) ) )
            {
            case COMMANDS::CLEAR_RESULT_CMD:
                cmd = 0;
                break;

            case COMMANDS::RELOAD_RESTRICTIONS:
                {
                G_LOG->notice( "Reload restrictions (remote monitor "
                    "client command)." );
                cmd = G_LUA_MANAGER->reload_script( RESTRICTIONS_SCRIPT_N,
                    "restrictions", cmd_answer, sizeof( cmd_answer ) );
                return cmd;
                }

            case COMMANDS::RESET_PARAMS:
                {
                auto prev_val = par[ P_IS_OPC_UA_SERVER_ACTIVE ];
                G_LOG->notice( "Resetting parameters (remote monitor "
                    "client command)." );
                params_manager::get_instance()->reset_CRC_mem();
                params_manager::get_instance()->final_init();

                auto new_val = par[ P_IS_OPC_UA_SERVER_ACTIVE ];
                return proc_OPC( prev_val, new_val, false );
                }

            case COMMANDS::FORCE_SAVE_PARAMS:
                G_LOG->notice( "Force saving parameters (remote monitor "
                    "client command)." );
                return params_manager::get_instance()->save_params();
            }

        return 0;
        }

    if ( strcmp( prop, "WASH_VALVE_SEAT_PERIOD" ) == 0 )
        {
        par.save( P_MIX_FLIP_PERIOD, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "WASH_VALVE_UPPER_SEAT_TIME" ) == 0 )
        {
        par.save( P_MIX_FLIP_UPPER_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "WASH_VALVE_LOWER_SEAT_TIME" ) == 0 )
        {
        par.save( P_MIX_FLIP_LOWER_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_V_OFF_DELAY_TIME" ) == 0 )
        {
        par.save( P_V_OFF_DELAY_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_V_BOTTOM_ON_DELAY_TIME" ) == 0 )
        {
        par.save( P_V_BOTTOM_OFF_DELAY_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME" ) == 0 )
        {
        par.save( P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME, (u_int_4)val );
        return 0;
        }
    if ( strcmp( prop, "P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME" ) == 0 )
        {
        par.save( P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_RESTRICTIONS_MODE" ) == 0 )
        {
        par.save( P_RESTRICTIONS_MODE, (u_int_4)val );

        if ( val == 2 ) //Полуавтоматический режим.
            {
            restrictions_set_to_off_time = get_millisec();
            }
        else
            {
            restrictions_set_to_off_time = 0;
            }

        return 0;
        }
    if ( strcmp( prop, "P_RESTRICTIONS_MANUAL_TIME" ) == 0 )
        {
        par.save( P_RESTRICTIONS_MANUAL_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_AUTO_PAUSE_OPER_ON_DEV_ERR" ) == 0 )
        {
        par.save( P_AUTO_PAUSE_OPER_ON_DEV_ERR, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "NODEENABLED" ) == 0 )
        {
        if ( idx > 0 && idx <= io_manager::get_instance()->get_nodes_count() )
            {
            auto wn = io_manager::get_instance()->get_node( idx - 1 );
            if ( 1 == val )
                {
                if ( !wn->is_active )
                    {
                    wn->is_active = 1;
                    }
                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_SERVICE,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number );
                }
            if ( 0 == val )
                {
                if ( wn->is_active )
                    {
                    io_manager::get_instance()->disconnect( wn );
                    wn->is_active = 0;
                    }
                // Если с узлом ранее не было связи и мы его хотим
                // отключить для обслуживания, то активная ошибка отсутствия
                // связи удаляется без записи в лог сообщения о том, что с
                // узлом появилась связь.
                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_NO_CONNECTION,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number,
                    false );
                wn->is_set_err = false;

                // Если была активна данная ошибка, удаляем её аналогично.
                if ( wn->is_err_mode_alarm_set )
                    {
                    PAC_critical_errors_manager::get_instance()->reset_global_error(
                        PAC_critical_errors_manager::AC_PP_MODE,
                        PAC_critical_errors_manager::AS_IO_COUPLER, wn->number,
                        false );
                    wn->is_err_mode_alarm_set = false;
                    wn->prev_status_register = 0;
                    }
                // Если была активна данная ошибка, удаляем её аналогично.
                if ( wn->is_cfg_bus_error_alarm_set )
                    {
                    PAC_critical_errors_manager::get_instance()->reset_global_error(
                        PAC_critical_errors_manager::AC_CFG_BUS_ERROR,
                        PAC_critical_errors_manager::AS_IO_COUPLER, wn->number,
                        false );
                    wn->is_cfg_bus_error_alarm_set = false;
                    wn->prev_diagnostic_status_register = 0;
                    }

                // Устанавливаем ошибку о переходе узла в сервисный режим.
                PAC_critical_errors_manager::get_instance()->set_global_error(
                    PAC_critical_errors_manager::AC_SERVICE,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number );
                }
            if ( 100 == val ) //Сброс ошибки.
                {
                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_NO_CONNECTION,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number );
                wn->is_set_err = false;

                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_SERVICE,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number );
                }

            return 0;
            }
        }

    if ( strcmp( prop, "P_IS_OPC_UA_SERVER_ACTIVE" ) == 0 )
        {
        cmd_answer[ 0 ] = 0;
        if ( val == 0.0f || val == 1.f )
            {
            auto prev_val = par[ P_IS_OPC_UA_SERVER_ACTIVE ];
            return proc_OPC( prev_val, static_cast<int>( val ), true );
            }
        else
            {
            return 10;
            }
        }

    if ( strcmp( prop, "P_IS_OPC_UA_SERVER_CONTROL" ) == 0 )
        {
        par.save( P_IS_OPC_UA_SERVER_CONTROL, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_BK_ANSWER_MAX_WAIT_TIME" ) == 0 )
        {
        par.save( P_BK_ANSWER_MAX_WAIT_TIME, static_cast<u_int_4>( val ) );
        return 0;
        }

    return 0;
    }

int PAC_info::proc_OPC( int prev_val, int val, bool is_save )
    {
    if ( val == prev_val )
        {
        return 0;
        }

    if ( val == 0 && prev_val == 1 )
        {
        G_OPCUA_SERVER.shutdown();

        if ( is_save ) par.save( P_IS_OPC_UA_SERVER_ACTIVE, 0 );
        return 0;
        }
    else if ( val == 1 && prev_val == 0 )
        {
        if ( auto retval = G_OPCUA_SERVER.init_all_and_start();
            retval != UA_STATUSCODE_GOOD )
            {
            G_LOG->error( "OPC UA server start failed (0x%X). %s",
                retval, UA_StatusCode_name( retval ) );
            auto r = fmt::format_to_n( cmd_answer, sizeof( cmd_answer ) - 1,
                "{}", G_LOG->msg );
            *r.out = '\0';

            G_OPCUA_SERVER.shutdown();
            return 1;
            }

        if ( is_save ) par.save( P_IS_OPC_UA_SERVER_ACTIVE, 1 );
        return 0;
        }

    return 10;
    }

bool PAC_info::is_emulator() const
    {
#ifdef PTUSA_TEST
    return emulator_state;
#else
#ifdef DEBUG_NO_IO_MODULES
    return true;
#else
    if ( G_NO_IO_NODES )
        return true;
    else
        return false;
#endif // DEBUG_NO_IO_MODULES
#endif // PTUSA_TEST
    }

#ifdef PTUSA_TEST
void PAC_info::emulation_on()
    {
    emulator_state = true;
    }

void PAC_info::emulation_off()
    {
    emulator_state = false;
    }

void PAC_info::reset_uptime()
    {
    up_days = 0;
    up_hours = 0;
    up_mins = 0;
    up_secs = 0;
    up_msec = 0;
    last_check_time = get_millisec();
    strcpy( up_time_str, "0 дн. 0:0:0" );
    }
#endif
//-----------------------------------------------------------------------------
void PAC_info::set_cycle_time( uint32_t current_cycle_time )
    {
    cycle_time = current_cycle_time;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_info* G_PAC_INFO()
    {
    return PAC_info::get_instance();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
