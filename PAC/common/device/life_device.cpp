#include "fmt/format.h"

#include "life_device.h"

//-----------------------------------------------------------------------------
watchdog::watchdog( const char* name, device::DEVICE_SUB_TYPE sub_type ) :
    device( name, device::DEVICE_TYPE::DT_WATCHDOG, sub_type,
        static_cast<u_int>( PARAM::PARAMS_COUNT ) - 1 )
    {
    set_par_name( static_cast<u_int>( PARAM::P_T_GEN ), 0, "P_T_GEN" );
    set_par_name( static_cast<u_int>( PARAM::P_T_ERR ), 0, "P_T_ERR" );
    }
//-----------------------------------------------------------------------------
void watchdog::evaluate_io()
    {
    if ( !DI_dev && !AI_dev )
        {
        device::set_state( 0 );
        }

    if ( DI_dev )
        {
        if ( auto st = DI_dev->get_state(); prev_dev_state != st )
            {
            prev_dev_state = st;
            device::set_state( 1 );
            start_in_check_time = get_millisec();
            }
        }
    if ( AI_dev )
        {
        if ( auto v = AI_dev->get_value(); prev_dev_value != v )
            {
            prev_dev_value = v;
            device::set_state( 1 );
            start_in_check_time = get_millisec();
            }
        }

    if ( auto set_dt = static_cast<unsigned long>(
             get_par( static_cast<u_int>( PARAM::P_T_ERR ) ) );
        set_dt > 0 && get_delta_millisec( start_in_check_time ) > set_dt )
        {
        start_in_check_time = get_millisec();
        device::set_state( -1 );
        }

    if ( auto set_dt = static_cast<unsigned long>(
             get_par( static_cast<u_int>( PARAM::P_T_GEN ) ) );
        get_delta_millisec( start_out_check_time ) > set_dt )
        {
        start_out_check_time = get_millisec();

        if ( DO_dev ) DO_dev->set_state( !DO_dev->is_active() );
        if ( AO_dev )
            {
            AO_dev->set_value( AO_dev->get_value() + 1 );
            if ( AO_dev->get_value() > MAX_OUT_VALUE ) AO_dev->set_value( 0 );
            }
        }
    }
//-----------------------------------------------------------------------------
void watchdog::set_string_property( const char* field, const char* value )
    {
    device::set_string_property( field, value );

    if ( !field ) return;

    if ( strcmp( field, "DI_dev" ) == 0 )
        {
        DI_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else if ( strcmp( field, "AI_dev" ) == 0 )
        {
        AI_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else if ( strcmp( field, "DO_dev" ) == 0 )
        {
        DO_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else if ( strcmp( field, "AO_dev" ) == 0 )
        {
        AO_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else
        {
        G_LOG->alert(
            "%s\t watchdog::set_string_property() - "
            "Unknown field \"%s\"",
            get_name(), field );
        }
    }
//-----------------------------------------------------------------------------
void watchdog::set_property( const char* field, device* value )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug(
            "%s\t watchdog::set_property() - "
            "field = \"%s\", val = \"%s\"",
            get_name(), field, value ? value->get_name() : "nullptr" );
        }

    if ( !field ) return;

    if ( strcmp( field, "DI_dev" ) == 0 )
        {
        DI_dev = value;
        }
    else if ( strcmp( field, "AI_dev" ) == 0 )
        {
        AI_dev = value;
        }
    else if ( strcmp( field, "DO_dev" ) == 0 )
        {
        DO_dev = value;
        }
    else if ( strcmp( field, "AO_dev" ) == 0 )
        {
        AO_dev = value;
        }
    else
        {
        G_LOG->alert(
            "%s\t watchdog::set_property() - "
            "Unknown field \"%s\"",
            get_name(), field );
        }
    }
//-----------------------------------------------------------------------------
const char* watchdog::get_name_in_Lua() const
    {
    return get_name();
    }
//-----------------------------------------------------------------------------
void watchdog::set_descr( const char* new_description )
    {
    device::set_descr( new_description );
    error_description = fmt::format( "ошибка связи - '{}'", get_description() );
    }
//-----------------------------------------------------------------------------
const char* watchdog::get_error_description()
    {
    if ( auto err_id = get_error_id(); err_id < 0 )
        {
        return error_description.c_str();
        }

    return "нет ошибок";
    }
