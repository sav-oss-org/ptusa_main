#include "PAC_err.h"

#include "fmt/format.h"

#include <stdio.h>
#include <vector>
#include <cstring>
#include <algorithm>

#include "log.h"

#ifdef WIN_OS
extern const char* WSA_Last_Err_Decode ();
#endif // WINDOWS_OS

auto_smart_ptr < PAC_critical_errors_manager > PAC_critical_errors_manager::instance;
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::~PAC_critical_errors_manager(
    )
    {
    reset_all_error();
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::show_errors() const
    {
    static u_char show_step = 0;
    static uint32_t start_time = get_millisec();

    if ( !errors.empty() )    // Есть ошибки.
        {
        switch ( show_step )
            {
        case 0:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 1;
                get_led()->on( led::L_STATUS, led::C_RED );
                start_time = get_millisec();
                }
            break;

        case 1:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 0;
                get_led()->off( led::L_STATUS );
                start_time = get_millisec();
                }
            break;
            }
        }
    else                        // Нет ошибок.
        {
        switch ( show_step )
            {
        case 0:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 1;
                get_led()->on( led::L_STATUS, led::C_GREEN );
                start_time = get_millisec();
                }
            break;

        case 1:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 0;
                get_led()->off( led::L_STATUS );
                start_time = get_millisec();
                }
            break;
            }
        }
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::set_global_error( ALARM_CLASS eclass,
    ALARM_SUBCLASS p1, unsigned int p2 )
    {
    int b = 0;

    //1.try to find
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        if ( errors[ i ].err_class == eclass &&
            static_cast<unsigned int>( p1 ) == errors[ i ].err_sub_class &&
            p2 == errors[ i ].param )
            {
            b = 1;
            break;
            }
        }

    if ( b == 0 )
        {
        G_LOG->error( "%s",
            get_alarm_descr( eclass, p1, p2, true ) );

        errors.emplace_back( eclass, p1, p2,
            ALARM_CLASS_PRIORITY::P_ERR_CONNECTION );
        errors_id++;
        }
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::reset_all_error()
    {
    errors.clear();
    errors_id = 0;
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::reset_global_error( ALARM_CLASS eclass,
    ALARM_SUBCLASS p1, unsigned int p2, bool is_print_msg /* = true */ )
    {
    int idx = -1;
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        if ( errors[ i ].err_class == eclass &&
            static_cast<unsigned int>( p1 ) == errors[ i ].err_sub_class &&
            p2 == errors[ i ].param )
            {
            idx = i;
            break;
            }
        }

    if ( idx >= 0 )
        {
        if ( is_print_msg )
            {
            G_LOG->info( "%s", get_alarm_descr( eclass, p1, p2, false ) );
            }

        errors.erase( errors.begin() + idx );
        errors_id++;
        }
    }
//-----------------------------------------------------------------------------
int PAC_critical_errors_manager::save_as_Lua_str( char *str, u_int_2 &id )
    {
    int res = 0;

    std::for_each( errors.begin(), errors.end(),
        [ str, &res, this ]( const critical_error& err )
        {
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE, "\t{{\n" ).size;

        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "\tdescription = \"{}\",\n",
            get_alarm_descr( static_cast<ALARM_CLASS>( err.err_class ),
                static_cast<ALARM_SUBCLASS>( err.err_sub_class ),
                err.param, true ) ).size;

        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "\ttype = AT_SPECIAL,\n" ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "\tgroup = '{}',\n", get_alarm_group() ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "\tpriority = {},\n", err.priority ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "\tstate = AS_ALARM,\n" ).size;

        //Для идентификации ошибок.
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "\tid_n = {},\n", err.param ).size;

        res += fmt::format_to_n( str + res, MAX_COPY_SIZE, "\t}},\n" ).size;
        } );

    id = errors_id;
    str[ res ] = '\0';

#ifdef DEBUG_PAC_ERR
    fmt::print( "{}\n", str );
#endif // DEBUG_PAC_ERR

    return res;
    }
//-----------------------------------------------------------------------------
bool PAC_critical_errors_manager::is_any_critical_error() const
    {
    return std::any_of( std::begin( errors ), std::end( errors ),
        []( const critical_error& err ) {
        return err.priority == ALARM_CLASS_PRIORITY::P_ERR_CONNECTION; } );
    }
//-----------------------------------------------------------------------------
PAC_critical_errors_manager * PAC_critical_errors_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new PAC_critical_errors_manager();
        }

    return instance;
    }
//-----------------------------------------------------------------------------
const char* PAC_critical_errors_manager::get_alarm_descr( ALARM_CLASS err_class,
    ALARM_SUBCLASS err_sub_class, unsigned int par, bool is_set )
    {
    const auto BUFF_SIZE = 200;
    static char tmp[ BUFF_SIZE ]{};
    std::memset( tmp, 0, BUFF_SIZE );

    // LCOV_EXCL_START
    auto res = fmt::format_to_n( tmp, BUFF_SIZE, "{}-{}-{} : ",
        static_cast<int>( err_class ), static_cast<int>( err_sub_class ),
        par ).size;
    // LCOV_EXCL_STOP

    if ( err_sub_class == AS_IO_COUPLER &&
        ( err_class == AC_SERVICE || err_class == AC_NO_CONNECTION ||
            err_class == AC_PP_MODE || err_class == AC_CFG_BUS_ERROR ) )
        {
        res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
            "Узел I/O '{}' ('{}', '{}') - ",
            G_IO_MANAGER()->get_node( par - 1 )->name,
            G_IO_MANAGER()->get_node( par - 1 )->ip_address,
            G_CMMCTR->get_host_name_rus() ).size;
        }

    switch ( err_class )
        {
        case AC_SERVICE:
            switch ( err_sub_class )
                {
                case AS_IO_COUPLER:
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}", is_set ?
                        "отключен для обслуживания" : "включен" );
                    break;
                }
            break;

        case AC_PP_MODE:
            switch ( err_sub_class )
                {
                case AS_IO_COUPLER:
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}", is_set ?
                        "активен PP mode (каналы управления заблокированы)" :
                        "обычный режим" );
                    break;
                }
            break;

        case AC_CFG_BUS_ERROR:
            switch ( err_sub_class )
                {
                case AS_IO_COUPLER:
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}", is_set ?
                        "ошибка конфигурации/шины" :
                        "конфигурация и шина в норме" );
                    break;
                }
            break;

        case AC_NO_CONNECTION:
            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}", is_set ?
                "нет связи" : "есть связь" ).size;

            switch ( err_sub_class )
                {
                case AS_IO_COUPLER:
                    // Обработано в начале функции.
                    break;

                case AS_EASYSERVER:
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res, " EasyServer" );
                    break;

                default:
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res, " ?" );
                    break;
                }
            break;

        case AC_NET:
            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}", is_set ?
                " : Ошибка сетевой работы: " : " : Сетевая работа в норме : " ).size;

            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}",
                par == 0 ? "Modbus-устройство 11 №{} : " :
                    par == 1 ? "Мастер {} : " : "? {} : ", par ).size;

            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}",
                err_sub_class == AS_SOCKET_F ? "вызов функции socket(...)" :
                err_sub_class == AS_BIND_F ? "вызов функции bind(...)" :
                err_sub_class == AS_SETSOCKOPT_F ? "вызов функции setsockopt(...)" :
                err_sub_class == AS_LISTEN_F ? "вызов функции listen(...)" :
                "неизвестная ошибка" ).size;

            fmt::format_to_n( tmp + res, BUFF_SIZE - res, "{}", is_set ?
#ifdef LINUX_OS
                strerror( errno )
#endif // LINUX_OS

#ifdef WIN_OS
                WSA_Last_Err_Decode()
#endif // WINDOWS_OS
                : "." );
            break;

        default:
            fmt::format_to_n( tmp + res, BUFF_SIZE - res, "?" );
            break;
        }

    return tmp;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::critical_error::critical_error( int err_class,
    u_int err_sub_class,
    u_int param,
    int priority ) :err_class( err_class ),
    err_sub_class( err_sub_class ), param( param ),
    priority( priority )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
