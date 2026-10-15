#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <cstring>
#include "fmt/format.h"
#include <algorithm>
#include <unordered_map>
#include <inttypes.h>

#include "device.h"
#include "manager.h"

#include "tech_def.h"

#include "g_errors.h"
#include "lua_manager.h"
#include "log.h"
#include "PID.h"

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

circuit_breaker::F_data_out circuit_breaker::stub_out_info{};

power_unit::process_data_out power_unit::stub_p_data_out{};
unsigned int power_unit::WAIT_DATA_TIME = 300;
unsigned int power_unit::WAIT_CMD_TIME = 1000;

analog_valve_iolink::out_data analog_valve_iolink::stub_out_info{};
signal_column_iolink::out_data signal_column_iolink::stub_out_info{};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DO1::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::get_state();

    return current_state;
    }
//-----------------------------------------------------------------------------
void DO1::evaluate_io()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        // Ничего не делаем.
        return;
        }

    current_state = get_DO( DO_INDEX );
    if ( auto node_state = check_output_DO_node_PP_state(); node_state < 0 )
        {
        current_state = -1;
        }
    }
//-----------------------------------------------------------------------------
void DO1::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::direct_on();

    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DO1::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::direct_off();

    set_DO( DO_INDEX, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column_discrete::signal_column_discrete( const char* dev_name,
    int red_lamp_channel, int yellow_lamp_channel,
    int green_lamp_channel, int blue_lamp_channel,
    int siren_channel ) :
    signal_column( dev_name, DST_HLA,
        red_lamp_channel, yellow_lamp_channel, green_lamp_channel,
        blue_lamp_channel, siren_channel )
    {
    }
//-----------------------------------------------------------------------------
void signal_column_discrete::process_DO( u_int n, DO_state state, const char* name )
    {
    set_DO( n - 1, static_cast<char>( state ) );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column_iolink::signal_column_iolink( const char* dev_name ) :
    signal_column( dev_name, DST_HLA_IOLINK )
    {
    }
//-----------------------------------------------------------------------------
void signal_column_iolink::set_string_property( const char* field, const char* value )
    {
    device::set_string_property( field, value );

    if ( strcmp( field, "SIGNALS_SEQUENCE" ) == 0 )
        {
        auto pos = strchr( value, 'R' );
        if ( pos )
            {
            red_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'Y' );
        if ( pos )
            {
            yellow_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'G' );
        if ( pos )
            {
            green_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'B' );
        if ( pos )
            {
            blue_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'A' );
        if ( pos )
            {
            siren_channel = pos - value + 1;
            }
        }
    }
//-----------------------------------------------------------------------------
void signal_column_iolink::process_DO( u_int n, DO_state state, const char* name )
    {
    bool ch_state = state == DO_state::ON;
    switch ( n )
        {
        case 0:
            G_LOG->warning( "%s\t signal_column_iolink::process_DO() - "
                "no \'%s\'.", get_name(), name );
            break;

        case 1:
            out_info->switch_ch1 = ch_state;
            break;
        case 2:
            out_info->switch_ch2 = ch_state;
            break;
        case 3:
            out_info->switch_ch3 = ch_state;
            break;
        case 4:
            out_info->switch_ch4 = ch_state;
            break;
        case 5:
            out_info->switch_ch5 = ch_state;
            break;

        default:
            G_LOG->warning( "%s\t signal_column_iolink::process_DO() - "
                "unknown \'%s\'.", get_name(), name );
            break;
        }
    }
//-----------------------------------------------------------------------------
void signal_column_iolink::evaluate_io()
    {
    signal_column::evaluate_io();

    if ( G_PAC_INFO()->is_emulator() )
        {
        // Ничего не делаем.
        }
    else
        {
        out_info = (out_data*)get_AO_write_data( 0 );
        }
    }
//-----------------------------------------------------------------------------
const char* signal_column_iolink::get_error_description()
    {
    return iol_dev.get_error_description( get_error_id() );
    }
//-----------------------------------------------------------------------------
int signal_column_iolink::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return signal_column::get_state();
        }

    if ( auto st = get_AI_IOLINK_state( 0 ); st != io_device::IOLINKSTATE::OK )
        {
        return -st;
        }

    return signal_column::get_state();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera::camera( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int params_count, bool is_ready ) :
    device( dev_name, DT_CAM, sub_type, params_count ),
    io_device( dev_name ),
    is_cam_ready( is_ready )
    {
    }

void camera::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_set_state( new_state );
        }

    if ( new_state ) direct_on(); else direct_off();
    }

void camera::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_off();

    set_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ), 0 );
    }

void camera::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_on();

    set_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ), 1 );
    }

int camera::get_result( int n ) const
    {
    if ( !G_PAC_INFO()->is_emulator() )
        {
        result = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_RES_1 ) );
        }

    return result;
    }

int camera::save_device_ex( char* buff ) const
    {
    // LCOV_EXCL_START
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "RESULT={}, READY={}, ",
        get_result(), is_cam_ready ) ).size;
    // LCOV_EXCL_STOP
    return res;
    }

int camera::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( strcmp( prop, "RESULT" ) == 0 )
        {
        result = static_cast<int>( val );
        }
    else if ( strcmp( prop, "READY" ) == 0 )
        {
        is_cam_ready = val > 0;
        }
    else device::set_cmd( prop, idx, val );

    return 0;
    }

void camera::set_string_property( const char* field, const char* value )
    {
    device::set_string_property( field, value );

    if ( strcmp( field, "IP" ) == 0 )
        {
        ip = std::string( value );
        }
    }

bool camera::is_ready() const
    {
    return is_cam_ready;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera_DI2::camera_DI2( const char* dev_name, DEVICE_SUB_TYPE sub_type ) :
    camera( dev_name, sub_type, static_cast<int>( PARAMS::PARAMS_CNT ) - 1, false )
    {
    set_par_name( static_cast<u_int>( PARAMS::P_READY_TIME ), 0, "P_READY_TIME" );
    }

void camera_DI2::evaluate_io()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        {
        int o = get_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ) );
        int i = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_READY ) );

        if ( 1 == i )
            {
            start_switch_time = get_millisec();
            camera::direct_set_state( o );
            }
        else if ( get_delta_millisec( start_switch_time ) <
            get_par( static_cast<u_int>( PARAMS::P_READY_TIME ), 0 ) )
            {
            camera::direct_set_state( o );
            }
        else
            {
            camera::direct_set_state( -1 );
            }

        is_cam_ready = i > 0;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera_DI3::camera_DI3( const char* dev_name ) :
    camera_DI2( dev_name, DEVICE_SUB_TYPE::DST_CAM_DO1_DI3 )
    {
    }

void camera_DI3::evaluate_io()
    {
    camera_DI2::evaluate_io();

    if ( !G_PAC_INFO()->is_emulator() )
        {
        result_2 = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_RES_2 ) );
        }
    }

int camera_DI3::get_result( int n ) const
    {
    switch ( n )
        {
        case 1:
            return camera::get_result();

        case 2:
            return result_2;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void i_counter::restart()
    {
    reset();
    start();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool i_DI_device::is_active()
    {
    return get_state() == 0 ? false : true;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void i_DO_device::on()
    {
    if ( !get_manual_mode() )
        {
        direct_on();
        }
    }
//-----------------------------------------------------------------------------
void i_DO_device::instant_off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
void i_DO_device::set_state( int new_state )
    {
    if ( !get_manual_mode() )
        {
        direct_set_state( new_state );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Установка текущего состояния устройства с учетом ручного режима.
void i_AO_device::set_value( float new_value )
    {
    if ( !get_manual_mode() )
        {
        direct_set_value( new_value );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
virtual_valve::virtual_valve( const char* dev_name ) :
    valve( dev_name, DT_V, DST_V_VIRT )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dev_stub::dev_stub() : valve( "STUB", DT_NONE, DST_NONE ),
    camera( "STUB", DST_NONE ),
    signal_column( "STUB", DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
float dev_stub::get_value() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_value( float new_value )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::off()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::on()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::instant_off()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
bool dev_stub::is_active()
    {
    return valve::is_active();
    }
//-----------------------------------------------------------------------------
void dev_stub::set_value( float new_value )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::set_state( int new_state )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE dev_stub::get_valve_state() const
    {
    return VALVE_STATE::V_OFF;
    }
//-----------------------------------------------------------------------------
int dev_stub::get_state() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_on()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_off()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_state( int new_state )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
u_int_4 dev_stub::get_serial_n() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::print() const
    {
    printf( "STUB" );
    }
//-----------------------------------------------------------------------------
void dev_stub::pause()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::start()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::reset()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
u_int dev_stub::get_quantity() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float dev_stub::get_flow() const
    {
    return 0.;
    }
//-----------------------------------------------------------------------------
u_long dev_stub::get_pump_dt() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float dev_stub::get_min_flow() const
    {
    return .0f;
    }
//-----------------------------------------------------------------------------
void dev_stub::abs_reset()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
u_int dev_stub::get_abs_quantity() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::tare()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::pause_daily( DAY_CTR n )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::start_daily( DAY_CTR n )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::process_DO( u_int n, DO_state state, const char* name )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
threshold_regulator::threshold_regulator( const char* name ) :device( name,
    device::DEVICE_TYPE::DT_REGULATOR,
    device::DEVICE_SUB_TYPE::DST_REGULATOR_THLD,
    static_cast<int>( PARAM::PARAMS_COUNT ) - 1 )
    {
    set_par_name( static_cast<int>( PARAM::P_delta ), 0, "P_delta" );
    set_par_name( static_cast<int>( PARAM::P_is_reverse ), 0, "P_is_reverse" );
    }
//-----------------------------------------------------------------------------
int threshold_regulator::get_state() const
    {
    return static_cast<int>( state );
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_off()
    {
    if ( state != STATE::OFF )
        {
        state = STATE::OFF;
        if ( actuator )
            {
            actuator->off();
            }
        }
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_set_state( int new_state )
    {
    switch ( static_cast<STATE>( new_state ) )
        {
        case STATE::OFF:
            direct_off();
            break;

        case STATE::ON:
            direct_on();
            break;
        }
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_on()
    {
    if ( state != STATE::ON )
        {
        state = STATE::ON;
        }
    };
//-----------------------------------------------------------------------------
const char* threshold_regulator::get_name_in_Lua() const
    {
    return get_name();
    };
//-----------------------------------------------------------------------------
float threshold_regulator::get_value() const
    {
    return static_cast<float>( out_state );
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_set_value( float val )
    {
    set_value = val;
    if ( sensor && actuator )
        {
        auto in_value = sensor->get_value();
        if ( sensor->get_type() == DT_FQT )
            {
            in_value = dynamic_cast <i_counter*> ( sensor )->get_flow();
            }

        auto idx = static_cast<int>( PARAM::P_is_reverse );
        auto is_reverse = get_par( idx ) > 0;
        if ( STATE::OFF == state )
            {
            out_state = 0;
            actuator->off();
            }
        else
            {
            auto delta = get_par( static_cast<int>( PARAM::P_delta ) );
            if ( in_value > set_value + delta )
                {
                out_state = is_reverse ? 1 : 0;
                }
            else if ( in_value < set_value - delta )
                {
                out_state = is_reverse ? 0 : 1;
                }
            }

        actuator->set_state( out_state );
        }
    };
//-----------------------------------------------------------------------------
int threshold_regulator::save_device_ex( char* buff ) const
    {
    int res = 0;
    if ( in_value_name[ 0 ] )
        {
        res += static_cast<int>( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
            "IN_VALUE='{}', ", in_value_name ).size );
        }
    if ( out_value_name[ 0 ] )
        {
        res += static_cast<int>( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
            "OUT_VALUE='{}', ", out_value_name ).size );
        }
    return res;
    }
//-----------------------------------------------------------------------------
void threshold_regulator::set_string_property( const char* field, const char* value )
    {
    device::set_string_property( field, value );
    if ( !field ) return;

    switch ( field[ 0 ] )
        {
        //IN_VALUE
        case 'I':
            if ( value )
                {
                snprintf( in_value_name, sizeof( in_value_name ), "%s", value );
                }
            else
                {
                in_value_name[ 0 ] = 0;
                }
            sensor = G_DEVICE_MANAGER()->get_device( value );
            break;

        //OUT_VALUE
        case 'O':
            if ( value )
                {
                snprintf( out_value_name, sizeof( out_value_name ), "%s", value );
                }
            else
                {
                out_value_name[ 0 ] = 0;
                }
            actuator = G_DEVICE_MANAGER()->get_device( value );
            break;

        default:
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
power_unit::power_unit( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type  ) :
    analog_io_device( dev_name, device::DT_G, sub_type, 0 )
    {
    memset( &p_data_in, 0, sizeof( p_data_in ) );

    static_assert( sizeof( process_data_in ) == 18,
        "Struct `process_data_in` must be the 18 bytes size." );
    static_assert( sizeof( process_data_out ) == 7,
        "Struct `process_data_out` must be the 7 bytes size." );
    }
//-----------------------------------------------------------------------------
float power_unit::get_value() const
    {
    return v;
    }
//-----------------------------------------------------------------------------
int power_unit::get_state() const
    {
    return st;
    }
//-----------------------------------------------------------------------------
void power_unit::direct_set_value( float val )
    {
    // Здесь нет возможности управлять состоянием - управляем через отдельные
    // каналы.
    }
//-----------------------------------------------------------------------------
void power_unit::evaluate_io()
    {
    auto data = reinterpret_cast<std::byte*>( get_AI_data( C_AIAO_INDEX ) );

    if ( !data ) return; // Return, if data is nullptr (in debug mode).

    std::copy( data, data + sizeof( p_data_in ),
        reinterpret_cast<std::byte*>( &p_data_in ) );
    v = .1f * static_cast<float>( ( ( p_data_in.sum_currents_2 << 8 ) +
        p_data_in.sum_currents ) );
    err = p_data_in.DC_not_OK;
    st = p_data_in.status_ch1 || p_data_in.status_ch2 ||
        p_data_in.status_ch3 || p_data_in.status_ch4 ||
        p_data_in.status_ch5 || p_data_in.status_ch6 ||
        p_data_in.status_ch7 || p_data_in.status_ch8;

#ifdef DEBUG_IOLINK_POWER_UNIT
    auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "{:b} {:b} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x}",
        data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ],
        data[ 4 ], data[ 5 ], data[ 6 ], data[ 7 ],
        data[ 8 ], data[ 9 ], data[ 10 ], data[ 11 ],
        data[ 12 ], data[ 13 ], data[ 14 ], data[ 15 ],
        data[ 16 ], data[ 17 ] );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );

    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "voltage = {}, out_voltage_2 = {}, DC_not_OK = {}",
        p_data_in.out_voltage, +p_data_in.out_voltage_2, +p_data_in.DC_not_OK );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );
    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "out voltage {}",
        .1f * ( ( p_data_in.out_voltage_2 << 8 ) + p_data_in.out_voltage ) );
    *res.out = 0;
    G_LOG->write_log( i_log::P_INFO );

    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "nominal_current_ch1 = {}, load_current_ch1 = {}, st_ch1 = {}",
        +p_data_in.nominal_current_ch1,
        +p_data_in.load_current_ch1, +p_data_in.status_ch1 );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );
    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "nominal_current_ch4 = {}, load_current_ch4 = {}, st_ch4 = {}\n",
        +p_data_in.nominal_current_ch4,
        +p_data_in.load_current_ch4, +p_data_in.status_ch4 );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );


    char buff[ 500 ] = { 0 };
    save_device( buff );
    fmt::println( "{}", buff );
#endif

    p_data_out = reinterpret_cast<process_data_out*>(
        get_AO_write_data( C_AIAO_INDEX ) );


    if ( get_AI_IOLINK_state( C_AIAO_INDEX ) == io_device::IOLINKSTATE::OK )
        {
        if ( is_processing_cmd )
            {
            // Устанавливаем управляющий бит через некоторое время после
            // записи выходных данных.
            if ( get_delta_millisec( cmd_time ) > WAIT_DATA_TIME )
                {
                p_data_out->valid_flag = true;
                }
            if ( get_delta_millisec( cmd_time ) > WAIT_CMD_TIME )
                {
                is_processing_cmd = false;
                sync_pdout();
                }
            }
        else
            {
            sync_pdout();
            }
        }
    }
//-----------------------------------------------------------------------------
void power_unit::sync_pdout()
    {
    p_data_out->valid_flag = false;
    p_data_out->switch_ch1 = p_data_in.status_ch1;
    p_data_out->switch_ch2 = p_data_in.status_ch2;
    p_data_out->switch_ch3 = p_data_in.status_ch3;
    p_data_out->switch_ch4 = p_data_in.status_ch4;
    p_data_out->switch_ch5 = p_data_in.status_ch5;
    p_data_out->switch_ch6 = p_data_in.status_ch6;
    p_data_out->switch_ch7 = p_data_in.status_ch7;
    p_data_out->switch_ch8 = p_data_in.status_ch8;
    p_data_out->nominal_current_ch1 = p_data_in.nominal_current_ch1;
    p_data_out->nominal_current_ch2 = p_data_in.nominal_current_ch2;
    p_data_out->nominal_current_ch3 = p_data_in.nominal_current_ch3;
    p_data_out->nominal_current_ch4 = p_data_in.nominal_current_ch4;
    p_data_out->nominal_current_ch5 = p_data_in.nominal_current_ch5;
    p_data_out->nominal_current_ch6 = p_data_in.nominal_current_ch6;
    p_data_out->nominal_current_ch7 = p_data_in.nominal_current_ch7;
    p_data_out->nominal_current_ch8 = p_data_in.nominal_current_ch8;
    }
//-----------------------------------------------------------------------------
float power_unit::decode_nominal_current( uint8_t code )
    {
    // Декодирование 3-битного кода номинального тока согласно спецификации IO-Link
    // для блоков питания (например, Phoenix Contact QUINT POWER).
    // code: 0-7 (3 бита)
    // Возвращаемое значение: номинальный ток в амперах.
    switch ( code )
        {
        case 0: return 1.0f;    // 1A
        case 1: return 2.0f;    // 2A
        case 2: return 3.8f;    // 3.8A
        case 3: return 4.0f;    // 4A
        case 4: return 6.0f;    // 6A
        case 5: return 8.0f;    // 8A
        case 6: return 10.0f;   // 10A
        case 7: return 1.0f;    // Зарезервировано (по умолчанию 1A)
        default: return 1.0f;
        }
    }
//-----------------------------------------------------------------------------
int power_unit::save_device_ex( char* buff ) const
    {
    // LCOV_EXCL_START
    auto res = fmt::format_to_n( buff, MAX_COPY_SIZE,
        "NOMINAL_CURRENT_CH={{{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f}}}, ",
        decode_nominal_current( p_data_in.nominal_current_ch1 ),
        decode_nominal_current( p_data_in.nominal_current_ch2 ),
        decode_nominal_current( p_data_in.nominal_current_ch3 ),
        decode_nominal_current( p_data_in.nominal_current_ch4 ),
        decode_nominal_current( p_data_in.nominal_current_ch5 ),
        decode_nominal_current( p_data_in.nominal_current_ch6 ),
        decode_nominal_current( p_data_in.nominal_current_ch7 ),
        decode_nominal_current( p_data_in.nominal_current_ch8 ) );
    auto size = static_cast<int>( res.size );

    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "LOAD_CURRENT_CH={{{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f}}}, ",
        .1f * p_data_in.load_current_ch1, .1f * p_data_in.load_current_ch2,
        .1f * p_data_in.load_current_ch3, .1f * p_data_in.load_current_ch4,
        .1f * p_data_in.load_current_ch5, .1f * p_data_in.load_current_ch6,
        .1f * p_data_in.load_current_ch7, .1f * p_data_in.load_current_ch8 );
    size += res.size;

    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "ST_CH={{{},{},{},{},{},{},{},{}}}, ",
        +p_data_in.status_ch1, +p_data_in.status_ch2,
        +p_data_in.status_ch3, +p_data_in.status_ch4,
        +p_data_in.status_ch5, +p_data_in.status_ch6,
        +p_data_in.status_ch7, +p_data_in.status_ch8 );
    size += res.size;

    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "SUM_CURRENTS={:.1f}, ", v );
    size += res.size;
    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "VOLTAGE={:.1f}, ",
        .1f * static_cast<float>( ( ( p_data_in.out_voltage_2 << 8 ) +
        p_data_in.out_voltage ) ) );
    size += res.size;
    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "OUT_POWER_90={}, ", +p_data_in.out_power_90 );
    size += res.size;
    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE, "ERR={}, ", err );
    // LCOV_EXCL_STOP
    size += res.size;

    *res.out = 0;
    return size;
    }
//-----------------------------------------------------------------------------
void power_unit::direct_on()
    {
    p_data_out->switch_ch1 = true;
    p_data_out->switch_ch2 = true;
    p_data_out->switch_ch3 = true;
    p_data_out->switch_ch4 = true;
    p_data_out->switch_ch5 = true;
    p_data_out->switch_ch6 = true;
    p_data_out->switch_ch7 = true;
    p_data_out->switch_ch8 = true;

    if ( G_PAC_INFO()->is_emulator() )
        {
        p_data_in.status_ch1 = true;
        p_data_in.status_ch2 = true;
        p_data_in.status_ch3 = true;
        p_data_in.status_ch4 = true;
        p_data_in.status_ch5 = true;
        p_data_in.status_ch6 = true;
        p_data_in.status_ch7 = true;
        p_data_in.status_ch8 = true;
        st = 1;
        }

    is_processing_cmd = true;
    cmd_time = get_millisec();
    }
//-----------------------------------------------------------------------------
void power_unit::direct_off()
    {
    p_data_out->switch_ch1 = false;
    p_data_out->switch_ch2 = false;
    p_data_out->switch_ch3 = false;
    p_data_out->switch_ch4 = false;
    p_data_out->switch_ch5 = false;
    p_data_out->switch_ch6 = false;
    p_data_out->switch_ch7 = false;
    p_data_out->switch_ch8 = false;

    if ( G_PAC_INFO()->is_emulator() )
        {
        p_data_in.status_ch1 = false;
        p_data_in.status_ch2 = false;
        p_data_in.status_ch3 = false;
        p_data_in.status_ch4 = false;
        p_data_in.status_ch5 = false;
        p_data_in.status_ch6 = false;
        p_data_in.status_ch7 = false;
        p_data_in.status_ch8 = false;
        st = 0;
        }

    is_processing_cmd = true;
    cmd_time = get_millisec();
    }
//-----------------------------------------------------------------------------
int power_unit::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
            "{}\t power_unit::set_cmd() - prop = {}, idx = {}, val = {}",
            get_name(), prop, idx, val );
        *res.out = '\0';
        G_LOG->write_log( i_log::P_DEBUG );
        }

    if ( strcmp( prop, "ST" ) == 0 )
        {
        if ( auto new_val = static_cast<int>( val ); new_val )
            {
            on();
            }
        else
            {
            off();
            }

        return 0;
        }
    auto new_st = static_cast<bool>( val );
    auto status = static_cast<int8_t>( val );

    if ( strcmp( prop, "ST_CH" ) == 0 )
        {
        switch ( idx )
            {
            case 1:
                p_data_out->switch_ch1 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch1 = status;
                break;
            case 2:
                p_data_out->switch_ch2 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch2 = status;
                break;
            case 3:
                p_data_out->switch_ch3 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch3 = status;
                break;
            case 4:
                p_data_out->switch_ch4 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch4 = status;
                break;
            case 5:
                p_data_out->switch_ch5 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch5 = status;
                break;
            case 6:
                p_data_out->switch_ch6 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch6 = status;
                break;
            case 7:
                p_data_out->switch_ch7 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch7 = status;
                break;
            case 8:
                p_data_out->switch_ch8 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch8 = status;
                break;

            default:
                // Do nothing.
                break;
            }
        is_processing_cmd = true;
        cmd_time = get_millisec();
        if ( G_PAC_INFO()->is_emulator() )
            st = p_data_in.status_ch1 || p_data_in.status_ch2 ||
                p_data_in.status_ch3 || p_data_in.status_ch4 ||
                p_data_in.status_ch5 || p_data_in.status_ch6 ||
                p_data_in.status_ch7 || p_data_in.status_ch8;

        return 0;
        }

    auto nom_curr = static_cast<int8_t>( val );
    if ( strcmp( prop, "NOMINAL_CURRENT_CH" ) == 0 )
        {
        switch ( idx )
            {
            case 1:
                p_data_out->nominal_current_ch1 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch1 = nom_curr;
                    }
                break;
            case 2:
                p_data_out->nominal_current_ch2 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch2 = nom_curr;
                    }
                break;
            case 3:
                p_data_out->nominal_current_ch3 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch3 = nom_curr;
                    }
                break;
            case 4:
                p_data_out->nominal_current_ch4 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch4 = nom_curr;
                    }
                break;
            case 5:
                p_data_out->nominal_current_ch5 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch5 = nom_curr;
                    }
                break;
            case 6:
                p_data_out->nominal_current_ch6 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch6 = nom_curr;
                    }
                break;
            case 7:
                p_data_out->nominal_current_ch7 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch7 = nom_curr;
                    }
                break;
            case 8:
                p_data_out->nominal_current_ch8 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch8 = nom_curr;
                    }
                break;

            default:
                // Do nothing.
                break;
            }
        is_processing_cmd = true;
        cmd_time = get_millisec();
        return 0;
        }

    return analog_io_device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
base_counter::base_counter( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int extra_par_cnt ) :
    device( dev_name, DT_FQT, sub_type, extra_par_cnt ),
    io_device( dev_name )
    {
    device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
    }
//-----------------------------------------------------------------------------
int base_counter::get_state() const
    {
    return device::get_state();
    }
//-----------------------------------------------------------------------------
void base_counter::direct_on()
    {
    start();
    }
//-----------------------------------------------------------------------------
void base_counter::direct_off()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void base_counter::direct_set_state( int new_state )
    {
    switch ( new_state )
        {
        case 0:
            direct_off();
            break;

        case static_cast<int>( STATES::S_WORK ):
            direct_on();
            break;

        case static_cast<int>( STATES::S_PAUSE ):
            pause();
            break;
        }

    if ( G_PAC_INFO()->is_emulator() )
        {
        device::direct_set_state( new_state );
        }
    }
//-----------------------------------------------------------------------------
void base_counter::direct_set_value( float new_value )
    {
    value = new_value;
    }
//-----------------------------------------------------------------------------
void base_counter::set_abs_value( float new_value )
    {
    abs_value = new_value;
    };
//-----------------------------------------------------------------------------
float base_counter::calculate_quantity( float& val, float& last_read_val,
    bool& is_first ) const
    {
    float current = get_raw_value();
    float delta = 0;

    if ( is_first )
        {
        if ( current != 0 )
            {
            last_read_val = current;
            is_first = false;
            }
        }
    else
        {

        if ( current < last_read_val )
            {
            delta = get_max_raw_value() - last_read_val + current;
            if ( delta > MAX_OVERFLOW && current < delta )
                {
                delta = current;
                }
            }
        else
            {
            delta = current - last_read_val;
            }

        last_read_val = current;
        if ( delta > 0 )
            {
            val += delta;
            }
        }

    return delta;
    }
//-----------------------------------------------------------------------------
float base_counter::get_value() const
    {
    return value;
    }
//-----------------------------------------------------------------------------
u_int base_counter::get_quantity() const
    {
    return static_cast<u_int>( value );
    }
//-----------------------------------------------------------------------------
u_int base_counter::get_abs_quantity() const
    {
    return static_cast<u_int>( abs_value );
    }
//-----------------------------------------------------------------------------
void base_counter::set_property( const char* field, device* dev )
    {
    if ( field && field[ 0 ] == 'M' ) //Связанные насосы.
        {
        motors.push_back( dev );
        }
    }
//-----------------------------------------------------------------------------
void base_counter::pause()
    {
    get_quantity(); // Пересчитываем значение счетчика.

    device::direct_set_state( static_cast<int>( STATES::S_PAUSE ) );
    }
//-----------------------------------------------------------------------------
void base_counter::start()
    {
    if ( static_cast<int>( STATES::S_PAUSE ) == device::get_state() )
        {
        last_read_value = get_raw_value();
        non_working_pump_flag = false;
        min_flow_detected_flag = false;
        start_pump_working_time = get_millisec();
        start_pump_working_time_self_flow = get_millisec();
        device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
        }
    else if ( device::get_state() < 0 ) // Есть какая-либо ошибка.
        {
        non_working_pump_flag = false;
        min_flow_detected_flag = false;
        start_pump_working_time = get_millisec();
        start_pump_working_time_self_flow = get_millisec();
        device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
        }
    }
//-----------------------------------------------------------------------------
void base_counter::reset()
    {
    value = .0f;
    }
//-----------------------------------------------------------------------------
void base_counter::abs_reset()
    {
    abs_value = .0f;
    }
//-----------------------------------------------------------------------------
void base_counter::restart()
    {
    reset();
    start();
    }
//-----------------------------------------------------------------------------
void base_counter::check_self_flow()
    {
    auto min_flow = get_min_flow();

    if ( min_flow == .0f )
        {
        return; // Если минимальный поток 0 - дальше не проверяем.
        }

    if ( get_flow() <= min_flow )
        {
        // Расход ниже минимального.
        min_flow_detected_flag = true;
        return;
        }

    // Расход выше минимального.
    if ( min_flow_detected_flag )
        {
        // Фиксируем время и счетчик появления расхода, превышающего
        // минимальный.
        start_pump_working_time_self_flow = get_millisec();
        counter_prev_value_self_flow = get_abs_quantity();
        min_flow_detected_flag = false;
        }
    else
        {
        // Проверяем счетчик на ошибку - он должен изменить свои показания.
        if ( get_abs_quantity() != counter_prev_value_self_flow )
            {
            start_pump_working_time_self_flow = get_millisec();
            counter_prev_value_self_flow = get_abs_quantity();
            }
        else
            {
            if ( auto dt = get_pump_dt(); dt > 0 &&
                get_delta_millisec( start_pump_working_time_self_flow ) > dt )
                {
                device::direct_set_state( static_cast<int>(
                    STATES::S_FLOW_ERROR ) );
                }
            }
        }
    }
//-----------------------------------------------------------------------------
void base_counter::check_connected_pumps()
    {
    if ( motors.empty() )
        {
        return;
        }

    auto is_pump_working = false;
    for ( auto m : motors )
        {
        if ( m->get_state() == 1 )
            {
            is_pump_working = true;
            }
        }

    if ( !is_pump_working )
        {
        // Насос не работает.
        non_working_pump_flag = true;
        return;
        }

    if ( auto min_flow = get_min_flow(); get_flow() < min_flow )
        {
        // Расход ниже минимального.
        return;
        }

    // Насос работает.
    if ( non_working_pump_flag )
        {
        // Фиксируем время и показания момента включения насоса.
        start_pump_working_time = get_millisec();
        counter_prev_value = get_abs_quantity();
        non_working_pump_flag = false;
        return;
        }

    // Насос работает уже некоторое время, проверяем счетчик на ошибку - он
    // должен изменить свои показания.
    if ( get_abs_quantity() != counter_prev_value )
        {
        // Показания счетчика меняются - состояние устанавливаем в РАБОТА.
        start_pump_working_time = get_millisec();
        counter_prev_value = get_abs_quantity();
        }
    else
        {
        if ( auto dt = get_pump_dt(); dt > 0 &&
            get_delta_millisec( start_pump_working_time ) > dt )
            {
            // Показания счетчика не изменяются - состояние устанавливаем в
            // ОШИБКА_НАСОСА.
            device::direct_set_state( static_cast<int>( STATES::S_PUMP_ERROR ) );
            }
        }
    }
//-----------------------------------------------------------------------------
void base_counter::evaluate_io()
    {
    if ( STATES::S_WORK == static_cast<STATES>( get_state() ) )
        {
        calculate_quantity( value, last_read_value, is_first_read );
        }

    auto delta = calculate_quantity( abs_value, abs_last_read_value,
        abs_is_first_read );

    if ( STATES::S_WORK == day_t1_state )
        {
        day_t1_value += delta;
        }
    if ( STATES::S_WORK == day_t2_state )
        {
        day_t2_value += delta;
        }

    if ( c_day != get_time().tm_yday ) // Наступил новый день.
        {
        prev_day_t1_value = day_t1_value;
        day_t1_value = 0;
        prev_day_t2_value = day_t2_value;
        day_t2_value = 0;

        c_day = get_time().tm_yday;
        }


    // В паузе счетчика или при наличии ошибок - возвращаем данное состояние.
    if ( device::get_state() == static_cast<int>( STATES::S_PAUSE ) ||
        device::get_state() < 0 )
        {
        non_working_pump_flag = false;
        min_flow_detected_flag = false;
        return;                     // Не изменяем данное состояние.
        }

    check_self_flow();          // Проверка на самотёк.
    check_connected_pumps();    // Проверяем на работу связанных насосов.
    }
//-----------------------------------------------------------------------------
void base_counter::print() const
    {
    device::print();
    }
//-----------------------------------------------------------------------------
int base_counter::set_cmd( const char* prop, u_int idx, double val )
    {
    auto f_value = static_cast<float>( val );

    switch ( prop[ 0 ] )
        {
        case 'A': //ABS_V
            set_abs_value( f_value );
            break;

        case 'D': //DAY_T1, DAY_T2
            if ( prop[ 5 ] == '1' ) day_t1_value = f_value;
            else day_t2_value = f_value;
            break;

        default:
            if ( strcmp( prop, "PREV_DAY_T1" ) == 0 )
                prev_day_t1_value = f_value;
            else if ( strcmp( prop, "PREV_DAY_T2" ) == 0 )
                prev_day_t2_value = f_value;
            else return device::set_cmd( prop, idx, val );
        }

    return 0;
    };
//-----------------------------------------------------------------------------
int base_counter::save_device_ex( char* buff ) const
    {
    // LCOV_EXCL_START
    return fmt::format_to_n( buff, MAX_COPY_SIZE,
        "ABS_V={}, DAY_T1={}, PREV_DAY_T1={}, DAY_T2={}, PREV_DAY_T2={}, ",
        get_abs_quantity(), day_t1_value, prev_day_t1_value,
        day_t2_value, prev_day_t2_value ).size;
    // LCOV_EXCL_STOP
    }
//-----------------------------------------------------------------------------
const char* base_counter::get_error_description()
    {
    if ( auto err_id = get_error_id(); err_id < 0 )
        {
        switch ( err_id )
            {
            case static_cast<int>( STATES::S_PUMP_ERROR ):
                return "счет импульсов";

            case static_cast<int>( STATES::S_FLOW_ERROR ):
                return "самотёк";

            case static_cast<int>( STATES::S_LOW_ERR ):
                return "канал потока (нижний предел)";

            default:
                return "неизвестная ошибка";
            }
        }

    return "нет ошибок";
    }
//-----------------------------------------------------------------------------
void base_counter::pause_daily( DAY_CTR n )
    {
    if ( n == DAY_CTR::DAY_T1 )
        day_t1_state = STATES::S_PAUSE;
    else if ( n == DAY_CTR::DAY_T2 )
        day_t2_state = STATES::S_PAUSE;
    }
//-----------------------------------------------------------------------------
void base_counter::start_daily( DAY_CTR n )
    {
    if ( n == DAY_CTR::DAY_T1 )
        day_t1_state = STATES::S_WORK;
    else if ( n == DAY_CTR::DAY_T2 )
        day_t2_state = STATES::S_WORK;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter::counter( const char *dev_name, DEVICE_SUB_TYPE sub_type,
                     int extra_par_cnt ):
    base_counter( dev_name, sub_type, extra_par_cnt )
    {
    }
//-----------------------------------------------------------------------------
float counter::get_raw_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return 0;
        }

    return static_cast<float>( *( (u_int_2*)get_AI_data( AI_Q_INDEX ) ) );
    }
//-----------------------------------------------------------------------------
float counter::get_max_raw_value() const
    {
    return USHRT_MAX;
    }
//-----------------------------------------------------------------------------
float counter::get_flow() const
    {
    return .0f;
    }
//-----------------------------------------------------------------------------
u_long counter::get_pump_dt() const
    {
    return 0;
    };
//-----------------------------------------------------------------------------
float counter::get_min_flow() const
    {
    return .0f;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter_f::counter_f( const char *dev_name ) :
    counter( dev_name, DST_FQT_F, LAST_PARAM_IDX - 1 )
    {
    set_par_name( P_MIN_FLOW, 0, "P_MIN_FLOW" );
    set_par_name( P_MAX_FLOW, 0, "P_MAX_FLOW" );
    set_par_name( P_CZ, 0, "P_CZ" );
    set_par_name( P_DT, 0, "P_DT" );
    set_par_name( P_ERR_MIN_FLOW, 0, "P_ERR_MIN_FLOW" );
    }
//-----------------------------------------------------------------------------
int counter_f::get_state() const
    {
    if ( get_flow() == -1. )
        {
        return (int) STATES::S_LOW_ERR;
        }

    return base_counter::get_state();
    }
//-----------------------------------------------------------------------------
float counter_f::get_flow() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return get_par( P_CZ, 0 ) + flow_value;
        }
    else
        {
        return get_par( P_CZ, 0 ) +
            get_AI( AI_FLOW_INDEX, get_par( P_MIN_FLOW, 0 ),
            get_par( P_MAX_FLOW, 0 ) );
        }
    }
//-----------------------------------------------------------------------------
int counter_f::save_device_ex( char *buff ) const
    {
    int res = counter::save_device_ex( buff );
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "F={:.2f}, ",
        get_flow() ).size;

    return res;
    }
//-----------------------------------------------------------------------------
u_long counter_f::get_pump_dt() const
    {
    return static_cast<u_long>( get_par( P_DT, 0 ) );
    }
//-----------------------------------------------------------------------------
float counter_f::get_min_flow() const
    {
    return get_par( P_ERR_MIN_FLOW, 0 );
    }
//-----------------------------------------------------------------------------
int counter_f::set_cmd( const char* prop, u_int idx, double val )
    {
    switch ( prop[ 0 ] )
        {
        case 'F':
            flow_value = static_cast<float>( val );
            break;

        default:
            return counter::set_cmd( prop, idx, val );
        }

    return 0;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter_iolink::counter_iolink( const char* dev_name ) :base_counter( dev_name,
    device::DST_FQT_IOLINK,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<u_int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    set_par_name( static_cast<u_int>( CONSTANTS::P_DT ), 0, "P_DT" );
    set_par_name( static_cast<u_int>( CONSTANTS::P_ERR_MIN_FLOW ), 0,
        "P_ERR_MIN_FLOW" );
    };
//-----------------------------------------------------------------------------
void counter_iolink::evaluate_io()
    {
    if ( auto data = reinterpret_cast<std::byte*>( get_AI_data( 0 ) );
        !G_PAC_INFO()->is_emulator() && data )
        {
        if ( n_article == ARTICLE::IFM_SMFx20 )
            {
            auto buff = reinterpret_cast<std::byte*>( &smfx20_in_info );
            const int SIZE = sizeof( smfx20_in_info );
            std::copy( data, data + SIZE, buff );

            //Reverse byte order to get correct float.
            std::swap( buff[ 3 ], buff[ 0 ] );
            std::swap( buff[ 1 ], buff[ 2 ] );
            //Reverse byte order to get correct int16.
            std::swap( buff[ 4 ], buff[ 5 ] );
            //Reverse byte order to get correct int16.
            std::swap( buff[ 6 ], buff[ 7 ] );
            //Reverse byte order to get correct uint16.
            std::swap( buff[ 8 ], buff[ 9 ] );
            }
        else
            {
            auto buff = reinterpret_cast<std::byte*>( &in_info );
            const int SIZE = 8;
            std::copy( data, data + SIZE, buff );

            //Reverse byte order to get correct float.
            std::swap( buff[ 3 ], buff[ 0 ] );
            std::swap( buff[ 1 ], buff[ 2 ] );
            //Reverse byte order to get correct int16.
            std::swap( buff[ 4 ], buff[ 5 ] );
            //Reverse byte order to get correct int16.
            std::swap( buff[ 6 ], buff[ 7 ] );

#ifdef DEBUG_FQT_IOLINK
            sprintf( G_LOG->msg,
                "Totalizer %.2f, flow %d, temperature %d,"
                " status2 %d, status1 %d",
                in_info.totalizer, in_info.flow, in_info.temperature,
                in_info.out2, in_info.out1 );
            G_LOG->write_log( i_log::P_NOTICE );
            sprintf( G_LOG->msg,
                "get_quantity() %d, get_flow() %f, get_temperature() %f",
                get_quantity(), get_flow(), get_temperature() );
            G_LOG->write_log( i_log::P_NOTICE );
#endif
            }
        }

    base_counter::evaluate_io();
    };
//-----------------------------------------------------------------------------
float counter_iolink::get_temperature() const
    {
    if ( n_article == ARTICLE::IFM_SMFx20 )
        {
        return TE_GRADIENT * smfx20_in_info.temperature;
        }
    return TE_GRADIENT * in_info.temperature;
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_conductivity() const
    {
    return CONDUCTIVITY_GRADIENT * smfx20_in_info.conductivity;
    }
//-----------------------------------------------------------------------------
int counter_iolink::get_state() const
    {
    if ( !G_PAC_INFO()->is_emulator() )
        {
        IOLINKSTATE res = get_AI_IOLINK_state(
            static_cast<u_int>( CONSTANTS::AI_INDEX ) );
        if ( res != io_device::IOLINKSTATE::OK )
            {
            return -static_cast<int>( res );
            }
        }

    return base_counter::get_state();
    }
//-----------------------------------------------------------------------------
u_long counter_iolink::get_pump_dt() const
    {
    return static_cast<u_long>(
        get_par( static_cast<u_int>( CONSTANTS::P_DT ), 0 ) );
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_min_flow() const
    {
    return get_par( static_cast<u_int>( CONSTANTS::P_ERR_MIN_FLOW ), 0 );
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_raw_value() const
    {
    if ( n_article == ARTICLE::IFM_SMFx20 )
        {
        return smfx20_in_info.totalizer;
        }
    return in_info.totalizer;
    };
//-----------------------------------------------------------------------------
float counter_iolink::get_max_raw_value() const
    {
    return 499999999.99f; ///< Максимальное значение счетчика.
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_flow() const
    {
    if ( n_article == ARTICLE::IFM_SMFx20 )
        {
        return get_par( static_cast<u_int>( CONSTANTS::P_CZ ), 0 )
            + smfx20_in_info.flow * get_flow_gradient();
        }
    return get_par( static_cast<u_int>( CONSTANTS::P_CZ ), 0 )
        + in_info.flow * get_flow_gradient();
    }
//-----------------------------------------------------------------------------
int counter_iolink::save_device_ex( char* buff ) const
    {
    int res = base_counter::save_device_ex( buff );
    if ( n_article == ARTICLE::IFM_SMFx20 )
        {
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE,
            "F={:.2f}, T={:.1f}, C={:.0f}, ",
            get_flow(), get_temperature(), get_conductivity() ).size;
        }
    else
        {
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "F={:.2f}, T={:.1f}, ",
            get_flow(), get_temperature() ).size;
        }

    return res;
    }
//-----------------------------------------------------------------------------
int counter_iolink::set_cmd( const char* prop, u_int idx, double val )
    {
    switch ( prop[ 0 ] )
        {
        case 'A': // Свойство `ABS_V`.
        case 'V': // Свойство `V`.
            // Учитываем коэффициент, который переводит в мл.
            return base_counter::set_cmd( prop, idx, val / mL_in_L );

        case 'C':
            smfx20_in_info.conductivity = static_cast<uint16_t>(
                round( val / CONDUCTIVITY_GRADIENT ) );
            break;

        case 'F':
            if ( n_article == ARTICLE::IFM_SMFx20 )
                {
                smfx20_in_info.flow = static_cast<int16_t>(
                    round( val / get_flow_gradient() ) );
                }
            else
                {
                in_info.flow = static_cast<int16_t>(
                    round( val / get_flow_gradient() ) );
                }
            break;

        case 'T':
            if ( n_article == ARTICLE::IFM_SMFx20 )
                {
                smfx20_in_info.temperature = static_cast<int16_t>(
                    round( val / TE_GRADIENT ) );
                }
            else
                {
                in_info.temperature = static_cast<int16_t>(
                    round( val / TE_GRADIENT ) );
                }
            break;

        default:
            return base_counter::set_cmd( prop, idx, val );
        }

    return 0;
    };
//-----------------------------------------------------------------------------
u_int counter_iolink::get_quantity() const
    {
    return static_cast<u_int>( get_value() );
    }
//-----------------------------------------------------------------------------
u_int counter_iolink::get_abs_quantity() const
    {
    return static_cast<u_int>( mL_in_L * get_abs_value() );
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_value() const
    {
    return base_counter::get_value() * mL_in_L;
    }
//-----------------------------------------------------------------------------
const char* counter_iolink::get_error_description()
    {
    switch ( auto error_id = get_error_id() )
        {
        case -static_cast<int>( io_device::IOLINKSTATE::NOTCONNECTED ) :
        case -static_cast<int>( io_device::IOLINKSTATE::DEVICEERROR ) :
            return iol_dev.get_error_description( error_id );

        default:
            return base_counter::get_error_description();
        }
    }
//-----------------------------------------------------------------------------
void counter_iolink::set_article( const char* new_article )
    {
    device::set_article( new_article );

    if ( strcmp( new_article, "IFM.SM6100" ) == 0 )
        {
        n_article = ARTICLE::IFM_SM6100;
        return;
        }
    if ( strcmp( new_article, "IFM.SM4000" ) == 0 )
        {
        n_article = ARTICLE::IFM_SM4000;
        return;
        }
    if ( strcmp( new_article, "IFM.SMF420" ) == 0 ||
         strcmp( new_article, "IFM.SMF320" ) == 0 )
        {
        n_article = ARTICLE::IFM_SMFx20;
        return;
        }

    G_LOG->warning( "%s unknown article \"%s\"",
        get_name(), new_article );
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_flow_gradient() const
    {
    switch ( n_article )
        {
        case ARTICLE::IFM_SM6100:
            return 0.01f;

        case ARTICLE::IFM_SM4000:
            return 0.001f;

        case ARTICLE::IFM_SMFx20:
            return 0.1f;

        case ARTICLE::DEFAULT:
        default:
            return 0.01f; // Default to SM6100 gradient.
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DI1::DI1( const char* dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt,
    int current_state_init_val ) :
    digital_io_device( dev_name, type, sub_type,
    ADDITIONAL_PARAMS_COUNT + par_cnt ),
    current_state( current_state_init_val )
    {
    set_par_name( P_DT, 0, "P_DT" );
    }
//-----------------------------------------------------------------------------
void DI1::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::direct_on();
    }
//-----------------------------------------------------------------------------
void DI1::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::direct_off();
    }
//-----------------------------------------------------------------------------
void DI1::evaluate_io()
    {
    if ( auto dt = static_cast<u_int_4>( get_par( P_DT, 0 ) ); dt > 0 )
        {
        if ( current_state != get_DI( DI_INDEX ) )
            {
            if ( get_delta_millisec( state_change_time ) > dt )
                {
                current_state = get_DI( DI_INDEX );
                state_change_time = get_millisec();
                }
            }
        else
            {
            state_change_time = get_millisec();
            }
        }
    else current_state = get_DI( DI_INDEX );
    }
//-----------------------------------------------------------------------------
int DI1::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::get_state();

    return current_state;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e::temperature_e( const char* dev_name ) : AI1( dev_name, DT_TE, DST_TE,
    ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_ERR, start_param_idx, "P_ERR" );
    param_emulator( 20, 2 );    //Average room temperature.
    }
//-----------------------------------------------------------------------------
float temperature_e::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        float v = AI1::get_value();
        return -1000 == v ? get_par( P_ERR, start_param_idx ) : v;
        }
    else
        {
        float v = get_AI( C_AI_INDEX, 0, 0 );
        return -1000 == v ? get_par( P_ERR, start_param_idx ) :
            get_par( P_ZERO_ADJUST_COEFF, 0 ) + v;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e_analog::temperature_e_analog( const char* dev_name ) :
    AI1( dev_name, DT_TE, DST_TE_ANALOG, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_ERR, start_param_idx, "P_ERR" );
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    param_emulator( 20, 2 );    //Average room temperature.
    }
//-----------------------------------------------------------------------------
float temperature_e_analog::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return analog_io_device::get_value();
        }

    float v = get_AI( C_AI_INDEX, 0, 0 );
    if ( v < 0 )
        {
        return get_par( P_ERR, start_param_idx );
        }
    else
        {
        auto min = get_par( P_MIN_V, start_param_idx );
        auto max = get_par( P_MAX_V, start_param_idx );
        v = get_AI( C_AI_INDEX, min, max );
        return get_par( P_ZERO_ADJUST_COEFF ) + v;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e_iolink::temperature_e_iolink( const char* dev_name ) :
    AI1( dev_name, DT_TE, DST_TE_IOLINK, ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( static_cast<u_int>( CONSTANTS::P_ERR ),
        start_param_idx, "P_ERR" );
    }
//-----------------------------------------------------------------------------
float temperature_e_iolink::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_value();

    if ( get_AI_IOLINK_state( C_AI_INDEX ) != io_device::IOLINKSTATE::OK )
        {
        return get_par( static_cast<u_int>( CONSTANTS::P_ERR ),
            start_param_idx );
        }
    else
        {
        return get_par( P_ZERO_ADJUST_COEFF, 0 ) + 0.1f * info.v;
        }
    }
//-----------------------------------------------------------------------------
int temperature_e_iolink::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    if ( auto st = get_AI_IOLINK_state( C_AI_INDEX );
        st != io_device::IOLINKSTATE::OK )
        {
        return -st;
        }

    return 1;
    }
//-----------------------------------------------------------------------------
void temperature_e_iolink::evaluate_io()
    {
    auto data = reinterpret_cast<std::byte*>( get_AI_data( C_AI_INDEX ) );
    if ( !data ) return;

    std::swap( data[ 0 ], data[ 1 ] );
    memcpy( &info, data, 2 );
    }
//-----------------------------------------------------------------------------
const char* temperature_e_iolink::get_error_description()
    {
    return iol_dev.get_error_description( get_error_id() );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e_iolink_tm311::temperature_e_iolink_tm311( const char* dev_name ) :
    AI1( dev_name, DT_TE, DST_TE_IOLINK, static_cast<u_int>( CONSTANTS::ADDITIONAL_PARAM_COUNT ) )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( static_cast<u_int>( CONSTANTS::P_ERR ),
        start_param_idx, "P_ERR" );
    }
//-----------------------------------------------------------------------------
float temperature_e_iolink_tm311::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_value();

    if ( get_AI_IOLINK_state( C_AI_INDEX ) != io_device::IOLINKSTATE::OK )
        {
        return get_par( static_cast<u_int>( CONSTANTS::P_ERR ),
            start_param_idx );
        }
    else
        {
        return get_par( P_ZERO_ADJUST_COEFF, 0 ) + 0.1f * info.temperature;
        }
    }
//-----------------------------------------------------------------------------
int temperature_e_iolink_tm311::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    if ( auto st = get_AI_IOLINK_state( C_AI_INDEX );
        st != io_device::IOLINKSTATE::OK )
        {
        return -st;
        }

    // Extract measured value status.
    if ( info.status1 == 3 && info.status2 == 0 )
        {
        return 1;
        }
    else
        {
        return -10 - info.status1 * 10 - info.status2;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void temperature_e_iolink_tm311::evaluate_io()
    {
    auto data = reinterpret_cast<std::byte*>( get_AI_data( C_AI_INDEX ) );
    if ( !data ) return;

    std::memcpy( &info, data, sizeof( TM311_data ) );
    auto bytes = reinterpret_cast<std::byte*>( &info );
    std::swap( bytes[ 0 ], bytes[ 1 ] );
    std::swap( bytes[ 2 ], bytes[ 3 ] );
    }
//-----------------------------------------------------------------------------
const char* temperature_e_iolink_tm311::get_error_description()
    {
    auto err_id = get_error_id();

    if ( err_id < 0 &&
        err_id > -static_cast<int>( io_device::IOLINKSTATE::NOTCONNECTED ) )
        {
        static thread_local std::string msg;
        msg.clear();

        if ( err_id <= -10 && err_id >= -19)
            msg =  "\'Bad\' - значение не может быть использовано";
        else if ( err_id <= -20 && err_id >= -29 )
            {
            msg = "\'Uncertain\' "
                "— значение может использоваться только ограниченно "
                "(например, температура устройства вне диапазона)";
            }
        else if ( err_id <= -30 && err_id >= -39 )
            {
            msg = "\'Manual/Fixed\' "
                "— значение может использоваться только ограниченно "
                "(например, симуляция)";
            }

        if ( err_id % 10 == -1 )
            msg += "; \'Low limited\' - значение нарушило нижний предел";
        else if ( err_id % 10 == -2 )
            {
            msg += "; \'High limited\' - значение нарушило верхний предел";
            }
        else if ( err_id % 10 == -3 )
            {
            msg += "; \'Constant\' - "
                "значение установлено как постоянное (например, симуляция)";
            }

        if ( !msg.empty() ) return msg.c_str();
        }

    return iol_dev.get_error_description( err_id );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void virtual_wages::direct_off()
    {
    state = 0;
    value = 0;
    }

void virtual_wages::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_wages::get_value() const
    {
    return value;
    }

void virtual_wages::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_wages::direct_on()
    {
    state = 1;
    }

int virtual_wages::get_state() const
    {
    return state;
    }

void virtual_wages::tare()
    {
    // Ничего не делаем.
    }

virtual_wages::virtual_wages( const char* dev_name ) :
    device( dev_name, device::DT_WT, device::DST_WT_VIRT, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wages_RS232::wages_RS232( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_WT, device::DST_WT_RS232,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    }

float wages_RS232::get_value_from_wages()
    {
    //Получение массива данных (1). Если данные пустые, вернуть старое
    //значение (2). Если 1 - буфер не пустой, переключиться в режим считывания
    //данных, вернуть старое значение (3). После переключения в режим
    //считывания данных получаем данные и обрабатываем (4)
    //Если данные корректные, то в 4м байте символ "+" (ASCII - 43), иначе -
    //ошибка (5).

    auto data = (char*)get_AI_data(
        static_cast<int>( CONSTANTS::C_AIAO_INDEX ) );                     //1

    if ( !data )
        {
        state = -1;
        value = 0.0f;
        return value;
        }

    if ( data[ 0 ] == 0 && data[ 1 ] == 0 ) return value;                  //2
    else if ( data[ 0 ] == 1 && data[ 1 ] == 0 )                           //3
        {
        set_command( static_cast<int>( STATES::TOGGLE_COMMAND ) );
        return value;
        }

    set_command( static_cast<int>( STATES::BUFFER_MOD ) );                 //4

    if ( data[ 2 ] != '+' )                                                //5
        {
        state = -1;
        value = 0.0f;
        return value;
        }

    std::swap( data[ 6 ], data[ 7 ] );
    std::swap( data[ 8 ], data[ 9 ] );
    std::swap( data[ 10 ], data[ 11 ] );
    state = 1;
    value = static_cast<float>( atof( data + 6 ) );
    return value;
    }

float wages_RS232::get_value() const
    {
    return value + get_par( static_cast<u_int>( CONSTANTS::P_CZ ) );
    }

void wages_RS232::set_command( int new_state )
    {
    //Установить состояние чтения состояния буфера (1).
    //Установить состояние чтения данных (2).
    auto out =
        (int*)get_AO_write_data( static_cast<int>( CONSTANTS::C_AIAO_INDEX ) );
    if ( !out ) return;

    auto new_st = static_cast<STATES>( new_state );
    if ( new_st == STATES::BUFFER_MOD )                                    //1
        {
        *out = 0;
        }
    else if ( new_st == STATES::TOGGLE_COMMAND )                           //2
        {
        *out = static_cast<u_int>( STATES::READ_CHARACTER );
        }
    }

int wages_RS232::get_state() const
    {
    //Здесь мы должны получать состояние весов. Например: идёт взвешивание,
    //взвешивание успешно завершилось, ошибка взвешивания (отрицательные
    //значения), ожидание взвешивания и т.п.
    return state;
    }

void wages_RS232::evaluate_io()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        value = analog_io_device::get_value();
        }
    else
        {
        value = get_value_from_wages();
        }
    }

void wages_RS232::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        return analog_io_device::direct_set_value( new_value );
    }

void wages_RS232::tare()
    {
    //Этот метод нужен для тарировки весов (когда текущий вес устанавливается
    //в качестве нулевого).
    }
//-----------------------------------------------------------------------------
wages_eth::wages_eth( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_WT, device::DST_WT_ETH,
        static_cast< int >( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    }

float wages_eth::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::get_value() +
            get_par( static_cast<u_int>( CONSTANTS::P_CZ ) );
        }

    if ( weth ) return weth->get_wages_value() +
        get_par( static_cast<u_int>( CONSTANTS::P_CZ ) );

    return 0.0f;
    }

int wages_eth::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::get_state();
        }

    if ( weth ) return weth->get_wages_state();
    return 0;
    }

void wages_eth::evaluate_io()
    {
    if ( G_PAC_INFO()->is_emulator() ) return;

    weth->evaluate();
    }

void wages_eth::tare()
    {
    //Этот метод нужен для тарировки весов (когда текущий вес устанавливается
    //в качестве нулевого).
    }

void wages_eth::set_string_property( const char* field, const char* value )
    {
    device::set_string_property( field, value );

    if ( !weth && strcmp( field, "IP" ) == 0 )
        {
        int port = 1001;
        int id = 0;
        weth = new iot_wages_eth( id, value, port, get_name() );
        }
    }

void wages_eth::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        if ( new_value >= .0f )
            {
            device::direct_set_value( new_value );
            }
        return;
        }

    if ( weth ) weth->set_wages_value( new_value );
    }

void wages_eth::direct_set_state( int state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_set_state( state );
        }

    if ( weth ) weth->set_wages_state( state );
    }

void wages_eth::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_off();
        }

    if ( weth ) weth->set_wages_state( 0 );
    }

void wages_eth::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_on();
        }

    if ( weth ) weth->set_wages_state( 1 );
    }

void wages_eth::direct_set_tcp_buff( const char* new_value, size_t size,
    int new_status )
    {
    weth->direct_set_tcp_buff( new_value, size, new_status );
    }
//-----------------------------------------------------------------------------
wages_pxc_axl::wages_pxc_axl( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_WT, device::DST_WT_PXC_AXL,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_DT ), 0, "P_DT" );
    set_par_name( static_cast<int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    set_par_name( static_cast<int>( CONSTANTS::P_K ), 0, "P_K" );
    }

void wages_pxc_axl::evaluate_io()
    {
    auto idx = static_cast<u_int>( CONSTANTS::C_AIAO_INDEX );
    auto data = reinterpret_cast<std::byte*>( get_AI_data( idx ) );

    if ( !data ) return;

    //Reverse byte order to get correct int32.
    std::swap( data[ 0 ], data[ 2 ] );
    std::swap( data[ 1 ], data[ 3 ] );
    int weigth = 0;
    std::memcpy( &weigth, data, sizeof( weigth ) );
    w = 0.001f * static_cast<float>( weigth ) *
        get_par( static_cast<int>( CONSTANTS::P_K ) );

    switch ( static_cast<ERR_VALUES>( weigth ) )
        {
        case ERR_VALUES::ERR_OVERRANGE:
            st = static_cast<int>( ERR_STATES::ERR_OVERRANGE );
            break;

        case ERR_VALUES::ERR_WIRE_BREAK:
            st = static_cast<int>( ERR_STATES::ERR_WIRE_BREAK );
            break;

        case ERR_VALUES::ERR_SHORT_CIRCUIT:
            st = static_cast<int>( ERR_STATES::ERR_SHORT_CIRCUIT );
            break;

        case ERR_VALUES::ERR_INVALID_VALUE:
            st = static_cast<int>( ERR_STATES::ERR_INVALID_VALUE );
            break;

        case ERR_VALUES::ERR_FAULTY_SUPPLY_VOLTAGE:
            st = static_cast<int>( ERR_STATES::ERR_FAULTY_SUPPLY_VOLTAGE );
            break;

        case ERR_VALUES::ERR_FAULTY_DEVICE:
            st = static_cast<int>( ERR_STATES::ERR_FAULTY_DEVICE );
            break;

        case ERR_VALUES::ERR_UNDERRANGE:
            st = static_cast<int>( ERR_STATES::ERR_UNDERRANGE );
            break;

        default:
            st = 0;
            break;
        }
    }

void wages_pxc_axl::tare()
    {
    set_par( static_cast<int>( CONSTANTS::P_CZ ), 0, -w );
    }

void wages_pxc_axl::reset_tare()
    {
    set_par( static_cast<int>( CONSTANTS::P_CZ ), 0, 0 );
    }

float wages_pxc_axl::get_value() const
    {
    return w + get_par( static_cast<int>( CONSTANTS::P_CZ ) );
    }

int wages_pxc_axl::get_state() const
    {
    return st;
    }

void wages_pxc_axl::direct_set_state( int new_state )
    {
    switch ( static_cast<CMDS>( new_state ) )
        {
        case CMDS::TARE:
            tare();
            break;

        case CMDS::RESET_TARE:
            reset_tare();
            break;
        }
    }

void wages_pxc_axl::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        w = new_value;
        }
    }
//-----------------------------------------------------------------------------
wages::wages( const char *dev_name ) : analog_io_device(
    dev_name, DT_WT, DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    set_par_name( P_NOMINAL_W,  0, "P_NOMINAL_W" );
    set_par_name( P_RKP, 0, "P_RKP");
    set_par_name( P_CZ, 0, "P_CZ" );
    set_par_name( P_DT, 0, "P_DT");
    }
//-----------------------------------------------------------------------------
void wages::tare()
    {
    set_par(P_CZ, 0, -weight);
    return;
    }
//-----------------------------------------------------------------------------
float wages::get_weight() const
    {
    if (get_delta_millisec(filter_time) > 500)
        {
        filter_time = get_millisec();
        float rkp = get_par(P_RKP, 0);
        if (0 == rkp) return -1001;
        float uref = get_AI(C_AI_Uref);
        if (0 == uref) return -1002;
        float filterval = get_par(P_DT, 0);
        if ( auto now_weight =
            get_AI( C_AI_Ud ) / rkp / uref * get_par( P_NOMINAL_W, 0 );
            fabs( now_weight - weight ) > filterval )
            {
            weight = now_weight;
            }
        if (filterval >= 1)
            {
            weight = ceilf(weight);
            }
        }
    return weight + get_par(P_CZ, 0);
    }
//-----------------------------------------------------------------------------
float wages::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return weight + get_par(P_CZ, 0);

    return get_weight();
    }
//-----------------------------------------------------------------------------
void wages::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() ) weight = new_value;
    }
//-----------------------------------------------------------------------------
void wages::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        switch ( new_state )
            {
            case S_TARE:
                tare();
                break;
            }
        }
    }
//-----------------------------------------------------------------------------
int wages::get_state() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int wages::save_device_ex( char* buff ) const
    {
    return static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "W={:.3f}, ",
        get_value() ).size );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e::level_e( const char* dev_name ) : level(
    dev_name, DST_LT, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_cyl::level_e_cyl( const char* dev_name ) : level(
    dev_name, DST_LT_CYL, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = level::get_params_count();
    set_par_name( P_MAX_P, start_param_idx, "P_MAX_P" );
    set_par_name( P_R, start_param_idx, "P_R" );
    }
//-----------------------------------------------------------------------------
int level_e_cyl::calc_volume() const
    {
    float v = get_par( P_R, start_param_idx );
    v = (float)M_PI * v * v *  get_value() *
        get_par( P_MAX_P, start_param_idx ) / 9.81f;

    int v_kg = 10 * (int)( v * 100 + 0.5f ); //Переводим в килограммы.

    return v_kg;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_cone::level_e_cone( const char* dev_name ) : level(
    dev_name, DST_LT_CONE, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = level::get_params_count();
    set_par_name( P_MAX_P, start_param_idx, "P_MAX_P" );
    set_par_name( P_R, start_param_idx, "P_R" );
    set_par_name( P_H_CONE, start_param_idx, "P_H_CONE" );
    }
//-----------------------------------------------------------------------------
int level_e_cone::calc_volume() const
    {
    float r = get_par( P_R, start_param_idx );
    float tg_a = 0;
    if ( get_par( P_H_CONE, start_param_idx ) > 0 )
        {
        tg_a = r / get_par( P_H_CONE, start_param_idx );
        }

    float h_cone = get_par( P_H_CONE, start_param_idx );
    float h_curr = get_par( P_MAX_P, start_param_idx ) * get_value() / 9.81f;

    float v = 0;
    if ( h_curr <= h_cone )
        {
        v = (float)M_PI * h_curr * tg_a * h_curr * tg_a * h_curr / 3;
        }
    else
        {
        v = (float)M_PI * r * r * ( h_curr - h_cone * 2 / 3 );
        }
    int v_kg = 10 * (int)( v * 100 + 0.5f ); //Переводим в килограммы.

    return v_kg;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_cyl_hor::level_e_cyl_hor( const char* dev_name ) : level(
    dev_name, DST_LT_CYL_HOR,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    start_param_idx = level::get_params_count();
    set_par_name( static_cast<int>( CONSTANTS::P_C0 ),
        start_param_idx, "P_C0" );
    set_par_name( static_cast<int>( CONSTANTS::P_MAX_P ),
        start_param_idx, "P_MAX_P" );
    set_par_name( static_cast<int>( CONSTANTS::P_R ),
        start_param_idx, "P_R" );
    set_par_name( static_cast<int>( CONSTANTS::P_H ),
        start_param_idx, "P_H" );
    set_par_name( static_cast<int>( CONSTANTS::P_ANGLE ),
        start_param_idx, "P_ANGLE" );
    set_par_name( static_cast<int>( CONSTANTS::P_DENSITY ),
        start_param_idx, "P_DENSITY" );
    }
//-----------------------------------------------------------------------------
int level_e_cyl_hor::calc_volume() const
    {
    const float R = get_par( static_cast<int>( CONSTANTS::P_R ),
        start_param_idx );
    const float L = get_par( static_cast<int>( CONSTANTS::P_H ),
        start_param_idx );
    const float rho = get_par( static_cast<int>( CONSTANTS::P_DENSITY ),
        start_param_idx );

    if ( R <= 0.0f || L <= 0.0f || rho <= 0.0f ) return 0;

    // Pressure in bar derived from percentage reading.
    float p_bar = get_value() / 100.0f *
        get_par( static_cast<int>( CONSTANTS::P_MAX_P ), start_param_idx ) +
        get_par( static_cast<int>( CONSTANTS::P_C0 ), start_param_idx );
    if ( p_bar < 0.0f ) p_bar = 0.0f;

    // Height at the sensor position in meters.
    const float h = p_bar * 100000.0f / ( rho * 9.81f );

    // Inclination angle in radians.
    const float theta_rad = get_par( static_cast<int>( CONSTANTS::P_ANGLE ),
        start_param_idx ) * static_cast<float>( M_PI ) / 180.0f;
    const float sin_theta = sinf( theta_rad );

    // Numerical integration along the cylinder axis.
    float V = 0.0f;
    const int NUM_SLICES = 10;
    const float dx = L / NUM_SLICES;
    for ( int i = 0; i < NUM_SLICES; i++ )
        {
        float x = ( static_cast<float>( i ) + 0.5f ) * dx;
        float H_x = h - x * sin_theta;

        // Clamp to physical bounds.
        if ( H_x < 0.0f ) H_x = 0.0f;
        if ( H_x > 2.0f * R ) H_x = 2.0f * R;

        if ( H_x >= 2.0f * R )
            {
            V += dx * (float)M_PI * R * R;
            }
        else if ( H_x > 0.0f )
            {
            // Guard argument of acos and sqrt against floating-point
            // imprecision near the boundary.
            float cos_arg = ( R - H_x ) / R;
            if ( cos_arg < -1.0f ) cos_arg = -1.0f;
            if ( cos_arg > 1.0f ) cos_arg = 1.0f;
            float sqrt_arg = 2.0f * R * H_x - H_x * H_x;
            if ( sqrt_arg < 0.0f ) sqrt_arg = 0.0f;

            float term1 = R * R * acosf( cos_arg );
            float term2 = ( R - H_x ) * sqrtf( sqrt_arg );
            V += dx * ( term1 - term2 );
            }
        }

    // Mass in kg; round to nearest 10 kg.
    float M = rho * V;

    int v_kg = 10 * (int)( M / 10.0f + 0.5f );

    return v_kg;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1::valve_DO1( const char* dev_name ) : valve( dev_name, DT_V, DST_V_DO1 )
    {
    }
//-----------------------------------------------------------------------------
void valve_DO1::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void valve_DO1::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    set_DO( DO_INDEX, 0 );
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_DO1::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return (VALVE_STATE)digital_io_device::get_state();
        }

    return (VALVE_STATE)get_DO( DO_INDEX );
    };
//-----------------------------------------------------------------------------
bool valve_DO1::get_fb_state() const
    {
    return true;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void virtual_motor::direct_off()
    {
    state = 0;
    value = 0;
    }

void virtual_motor::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_motor::get_value() const
    {
    return value;
    }

void virtual_motor::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_motor::direct_on()
    {
    state = 1;
    }

int virtual_motor::get_state() const
    {
    return state;
    }

virtual_motor::virtual_motor( const char* dev_name ):
    i_motor( dev_name, device::DST_M_VIRT, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
motor::motor( const char* dev_name, device::DEVICE_SUB_TYPE sub_type ) :
    i_motor( dev_name, sub_type, ADDITIONAL_PARAM_COUNT ),
    io_device( dev_name )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    }
//-----------------------------------------------------------------------------
float motor::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_value();

    if ( auto sub_type = get_sub_type();
        sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        return get_AO( AO_INDEX, C_MIN_VALUE, C_MAX_VALUE );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void motor::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        return device::direct_set_value( new_value );

    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        set_AO( AO_INDEX, new_value, C_MIN_VALUE, C_MAX_VALUE );
        }
    }
//-----------------------------------------------------------------------------
void motor::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        return device::direct_set_state( new_state );

    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
        {
        if ( new_state == 2 )
            {
            // Включение прямого пуска.
            int o = get_DO( DO_INDEX );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX, 1 );
                }

            // Включение реверса.
            o = get_DO( DO_INDEX_REVERSE );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_REVERSE, 1 );
                }

            return;
            }
        }

    if ( sub_type == device::DST_M_REV_2 ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( new_state == 2 )
            {
            // Выключение прямого пуска.
            int o = get_DO( DO_INDEX );
            if ( 1 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX, 0 );
                }

            // Включение реверса.
            o = get_DO( DO_INDEX_REVERSE );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_REVERSE, 1 );
                }

            return;
            }
        }

    if ( new_state )
        {
        direct_on();
        }
    else
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int motor::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    int o = get_DO( DO_INDEX );

    if ( auto sub_type = get_sub_type();
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( int err = get_DI(DI_INDEX_ERROR); 1 == err )
            {
            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                {
                return -1;
                }
            }
        else
            {
            start_switch_time = get_millisec();
            }

        if ( auto ro = get_DO( DO_INDEX_REVERSE ); 1 == ro )
            {
            return 2;
            }

        if ( 1 == o )
            {
            return 1;
            }

        return 0;
        }

    int i = get_DI( DI_INDEX );

    if ( auto sub_type = get_sub_type();
        sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 )
        {
        int ro = get_DO( DO_INDEX_REVERSE );

        if ( 0 == ro && 0 == o && 0 == i )
            {
            start_switch_time = get_millisec();
            return 0;
            }

        if ( 0 == ro && 1 == o && 1 == i )
            {
            start_switch_time = get_millisec();
            return 1;
            }

        if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
            {
            if ( 1 == ro && 1 == o && 1 == i )
                {
                start_switch_time = get_millisec();
                return 2;
                }
            }

        if ( sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 )
            {
            if ( 1 == ro && 0 == o && 1 == i )
                {
                start_switch_time = get_millisec();
                return 2;
                }
            }
        }
    else
        {
        if ( o == i )
            {
            start_switch_time = get_millisec();
            return i;
            }
        }

    if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
        {
        return -1;
        }
    else
        {
        return i;
        }
    }
//-----------------------------------------------------------------------------
void motor::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_on();

    if ( auto sub_type = get_sub_type();
        sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        // Выключение реверса.
        int o = get_DO( DO_INDEX_REVERSE );
        if ( 0 != o )
            {
            start_switch_time = get_millisec();
            set_DO( DO_INDEX_REVERSE, 0 );
            }
        }

    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void motor::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_off();

    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        // Отключение реверса.
        o = get_DO( DO_INDEX_REVERSE );
        if ( o != 0 )
            {
            start_switch_time = get_millisec();
            set_DO( DO_INDEX_REVERSE, 0 );
            }
        }

    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
int motor::save_device_ex( char *buff ) const
    {
    int res = 0;
    if ( G_PAC_INFO()->is_emulator() )
        return static_cast<int>(
        fmt::format_to_n( buff, MAX_COPY_SIZE, "R=0, ERRT=0, " ).size );

    if ( auto sub_type = get_sub_type();
        sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( sub_type == device::M_REV_2_ERROR ||
            sub_type == device::DST_M_REV_FREQ_2_ERROR )
            {
            res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "R={}, ERRT={}, ",
                get_DO( DO_INDEX_REVERSE ), get_DI( DI_INDEX_ERROR ) ).size );
            }
        else
            {
            res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "R={}, ",
                get_DO( DO_INDEX_REVERSE ) ).size );
            }
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool level_s::is_active()
    {
    switch ( get_sub_type() )
        {
        case DST_LS_MIN:
            return get_state() == 0 ? false : true;

        case DST_LS_MAX:
            return get_state() == 0 ? true : false;

        default:
            return get_state() == 0 ? false : true;
        }
    }
//-----------------------------------------------------------------------------
level_s::level_s( const char *dev_name, device::DEVICE_SUB_TYPE sub_type ):
    DI1( dev_name, DT_LS, sub_type, 0,
        sub_type == DST_LS_MAX ? 1 : 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_s_iolink::level_s_iolink( const char *dev_name,
    device::DEVICE_SUB_TYPE sub_type ):
    analog_io_device( dev_name, DT_LS, sub_type, LAST_PARAM_IDX - 1 ),
    current_state( sub_type == device::LS_IOLINK_MAX ? 1 : 0 )
    {
    set_par_name( P_DT, 0, "P_DT" );
    set_par_name( P_ERR, 0, "P_ERR" );
    }

void level_s_iolink::evaluate_io()
    {
    auto data = (char*)get_AI_data( C_AI_INDEX );

    if ( !data ) return;

    if ( auto devstate = get_AI_IOLINK_state( C_AI_INDEX );
        devstate != io_device::IOLINKSTATE::OK )
        {
        v = get_par( P_ERR, 0 );
        current_state = -devstate;
        time = get_millisec();

        return;
        }

    int st = 0;
    switch ( n_article )
        {
        case ARTICLE::IFM_LMT100:   //IFM.LMT100
        case ARTICLE::IFM_LMT102:   //IFM.LMT102
        case ARTICLE::IFM_LMT104:   //IFM.LMT104
        case ARTICLE::IFM_LMT105:   //IFM.LMT105
        case ARTICLE::IFM_LMT121:   //IFM.LMT121
        case ARTICLE::IFM_LMT202:   //IFM.LMT202
            {
            LS_data info{};
            std::reverse_copy( data, data + sizeof( info ), (char*)&info );
            v = (float) info.v;
            st = info.st1;
            break;
            }

        case ARTICLE::EH_FTL33:     //E&H.FTL33-GR7N2ABW5J
        case ARTICLE::EH_FTW33:     //E&H.FTW33
            {
            rev_LS_data info{};
            std::reverse_copy( data, data + sizeof( info ),
                reinterpret_cast<char*>(&info) );
            v = 0.1f * info.v;
            st = info.st1;
            break;
            }


        case ARTICLE::DEFAULT:
            v = get_par( P_ERR, 0 );
            st = 0;
            break;
        }

    if ( auto dt = static_cast<u_int_4>( get_par( P_DT, 0 ) ); dt > 0 )
        {
        if ( current_state != st )
            {
            if ( get_delta_millisec( time ) > dt )
                {
                current_state = st;
                time = get_millisec();
                }
            }
        else
            {
            time = get_millisec();
            }
        }
    else current_state = st;
    }

void level_s_iolink::set_article( const char* new_article )
    {
    direct_set_state( current_state );

    device::set_article( new_article );

    auto article = get_article();
    if ( article[ 0 ] == 'I' )
        {
        if ( strcmp( article, "IFM.LMT100" ) == 0 )
            {
            n_article = ARTICLE::IFM_LMT100;
            return;
            }
        if ( strcmp( article, "IFM.LMT102" ) == 0 )
            {
            n_article = ARTICLE::IFM_LMT102;
            return;
            }
        if ( strcmp( article, "IFM.LMT104" ) == 0 )
            {
            n_article = ARTICLE::IFM_LMT104;
            return;
            }
        if ( strcmp( article, "IFM.LMT105" ) == 0 )
            {
            n_article = ARTICLE::IFM_LMT105;
            return;
            }
        if ( strcmp( article, "IFM.LMT121" ) == 0 )
            {
            n_article = ARTICLE::IFM_LMT121;
            return;
            }
        if ( strcmp( article, "IFM.LMT202" ) == 0 )
            {
            n_article = ARTICLE::IFM_LMT202;
            return;
            }
        }

    if ( article[ 0 ] == 'E' )
        {
        if ( strstr( article, "E&H.FTL33" ) != nullptr )
            {
            n_article = ARTICLE::EH_FTL33;
            return;
            }

        if ( strstr( article, "E&H.FTW33" ) != nullptr )
            {
            n_article = ARTICLE::EH_FTW33;
            return;
            }
        }

    if ( G_DEBUG )
        {
        G_LOG->warning( "%s unknown article \"%s\"",
            get_name(), new_article );
        }
    n_article = ARTICLE::DEFAULT;
    }

#ifdef PTUSA_TEST
level_s_iolink::ARTICLE level_s_iolink::get_article_n() const
    {
    return n_article;
    }
#endif

float level_s_iolink::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_value();

    return v;
    }

int level_s_iolink::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    return current_state;
    }

bool level_s_iolink::is_active()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    return current_state;
    }

const char* level_s_iolink::get_error_description()
    {
    return iol_dev.get_error_description( get_error_id() );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
flow_s::flow_s( const char* dev_name ) : DI1( dev_name, DT_FS, DST_NONE,
    0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DI_signal::DI_signal( const char* dev_name ) : DI1( dev_name, DT_DI, DST_NONE, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
button::button( const char* dev_name ) : DI1( dev_name, DT_SB, DST_NONE, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DO_signal::DO_signal( const char* dev_name ) : DO1( dev_name, DT_DO, DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
siren::siren( const char* dev_name ) : DO1( dev_name, DT_HA, DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
lamp::lamp( const char* dev_name ) : DO1( dev_name, DT_HL, DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_iolink::level_e_iolink( const char *dev_name ) :
    level( dev_name, DST_LT_IOLINK, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = level::get_params_count();
    set_par_name( P_MAX_P, start_param_idx, "P_MAX_P" );
    set_par_name( P_R, start_param_idx, "P_R" );
    set_par_name( P_H_CONE, start_param_idx, "P_H_CONE" );
    }
//-----------------------------------------------------------------------------
int level_e_iolink::calc_volume() const
    {
    int v_kg = 0;
    float v = 0.0;
    if ( G_PAC_INFO()->is_emulator() )
        {
        v = get_value();
        v = v / 100 * get_par( P_MAX_P, start_param_idx );
        }
    else
        {
        v = this->v;
        }

    if (PT_extra)
        {
        v -= PT_extra->get_value();
        }

    if ( get_par( P_H_CONE, start_param_idx ) <= 0 )
        {
        float val = get_par( P_R, start_param_idx );
        val = (float)M_PI * val * val * v * 100 / 9.81f;

        v_kg = 10 * (int)( val * 100 + 0.5f ); //Переводим в килограммы.
        }
    else
        {
        float r = get_par( P_R, start_param_idx );
        float tg_a = 0;
        if ( get_par( P_H_CONE, start_param_idx ) > 0 )
            {
            tg_a = r / get_par( P_H_CONE, start_param_idx );
            }

        float h_cone = get_par( P_H_CONE, start_param_idx );
        float h_curr = 100 * v / 9.81f;

        float val = 0;
        if ( h_curr <= h_cone )
            {
            val = (float)M_PI * h_curr * tg_a * h_curr * tg_a * h_curr / 3;
            }
        else
            {
            val = (float)M_PI * r * r * ( h_curr - h_cone * 2 / 3 );
            }
        v_kg = 10 * (int)( val * 100 + 0.5f ); //Переводим в килограммы.
        }

    return v_kg;
    }
//-----------------------------------------------------------------------------
float level_e_iolink::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_value();

    if ( get_AI_IOLINK_state( C_AI_INDEX ) != io_device::IOLINKSTATE::OK )
        {
        return static_cast<float>( get_err_volume() );
        }
    else
        {
        return v / get_par( P_MAX_P, start_param_idx ) * 100;
        }
    }
//-----------------------------------------------------------------------------
int level_e_iolink::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state( C_AI_INDEX );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -static_cast<int>( res );
        }
    else
        {
        return st;
        }
    }
//-----------------------------------------------------------------------------
void level_e_iolink::set_article( const char* new_article )
    {
    device::set_article( new_article );
    pressure_e_iolink::read_article( new_article, n_article, this );
    alfa = pressure_e_iolink::get_alfa( n_article );
    }
//-----------------------------------------------------------------------------
void level_e_iolink::evaluate_io()
    {
    auto data = (char*)get_AI_data( C_AI_INDEX );

    if ( !data ) return;

    pressure_e_iolink::evaluate_io( get_name(), data, n_article, v, st, alfa );
    }
//-----------------------------------------------------------------------------
void level_e_iolink::set_string_property( const char* field, const char* value )
    {
    device::set_string_property( field, value );

    if ( strcmp( field, "PT" ) == 0 )
        {
        PT_extra = PT( value );
        }
    }
//-----------------------------------------------------------------------------
const char* level_e_iolink::get_error_description()
    {
    return iol_dev.get_error_description( get_error_id() );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
pressure_e::pressure_e( const char* dev_name ) : AI1( dev_name, DT_PT, DST_NONE,
    ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float pressure_e::get_max_val() const
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float pressure_e::get_min_val() const
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
pressure_e_iolink::pressure_e_iolink( const char* dev_name ) :
    analog_io_device( dev_name, DT_PT, DST_PT_IOLINK, LAST_PARAM_IDX - 1 )
    {
    set_par_name( P_ERR, 0, "P_ERR" );
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::set_article( const char* new_article )
    {
    device::set_article( new_article );
    read_article( new_article, n_article, this );
    alfa = get_alfa( n_article );
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::read_article( const char* article,
    ARTICLE& n_article, const device* dev )
    {
    if ( strcmp( article, "IFM.PI2715" ) == 0 )
        {
        n_article = ARTICLE::IFM_PI2715;
        return;
        }
    if ( strcmp( article, "IFM.PI2794" ) == 0 )
        {
        n_article = ARTICLE::IFM_PI2794;
        return;
        }
    if ( strcmp( article, "IFM.PI2797" ) == 0 )
        {
        n_article = ARTICLE::IFM_PI2797;
        return;
        }

    if ( strcmp( article, "IFM.PM1704" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1704;
        return;
        }
    if ( strcmp( article, "IFM.PM1705" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1705;
        return;
        }
    if ( strcmp( article, "IFM.PM1706" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1706;
        return;
        }
    if ( strcmp( article, "IFM.PM1707" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1707;
        return;
        }
    if ( strcmp( article, "IFM.PM1708" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1708;
        return;
        }
    if ( strcmp( article, "IFM.PM1709" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1709;
        return;
        }
    if ( strcmp( article, "IFM.PM1715" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1715;
        return;
        }
    if ( strcmp( article, "IFM.PM1717" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1717;
        return;
        }

    if ( strcmp( article, "FES.8001446" ) == 0 )
        {
        n_article = ARTICLE::FES_8001446;
        return;
        }

    if ( strcmp( article, "E&H.PMP23-CA7N2KB3CJ" ) == 0 ||
        strcmp( article, "E&H.PMP23-CA7N1KB3CJ" ) == 0 )
        {
        n_article = ARTICLE::EH_PMP23;
        return;
        }

    if ( G_DEBUG )
        {
        G_LOG->warning( "%s unknown article \"%s\"",
            dev->get_name(), article );
        }
    }
//-----------------------------------------------------------------------------
const pressure_e_iolink::article_info& pressure_e_iolink::get_article_info( ARTICLE n_article )
    {
    static const std::unordered_map<ARTICLE, article_info> article_data = {
        { ARTICLE::IFM_PM1708, { 0.00001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PM1706, { 0.0001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PM1707, { 0.0001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PM1709, { 0.0001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PM1717, { 0.0001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PI2715, { 0.001f, PT_DATA_TYPE } },
        { ARTICLE::IFM_PI2797, { 0.001f, PT_DATA_TYPE } },
        { ARTICLE::IFM_PM1704, { 0.001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PM1705, { 0.001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PM1715, { 0.001f, EX_PT_DATA_TYPE } },
        { ARTICLE::IFM_PI2794, { 0.01f, PT_DATA_TYPE } },
        { ARTICLE::FES_8001446, { 0.000610388818f, PT_DATA_TYPE } },
        { ARTICLE::EH_PMP23, { 0.0001f, EH_PT_DATA_TYPE } }
    };

    auto it = article_data.find( n_article );
    if ( it != article_data.end() )
        {
        return it->second;
        }

    static const article_info default_info = { 1.0f, PT_DATA_TYPE };
    return default_info;
    }
//-----------------------------------------------------------------------------
float pressure_e_iolink::get_alfa( ARTICLE n_article )
    {
    return get_article_info( n_article ).scaling_factor;
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::evaluate_io( const char *name, char* data, ARTICLE n_article,
    float& v, int& st, float alfa )
    {
    if ( !data ) return;

    // Special handling for DEFAULT article
    if ( n_article == ARTICLE::DEFAULT )
        {
        v = 0;
        st = 0;
        return;
        }

    if ( const auto& info = get_article_info( n_article );
        info.processing_type == PT_DATA_TYPE )
        {
        PT_data pt_info{};
        std::reverse_copy( data, data + sizeof( pt_info ), (char*)&pt_info );
        v = pt_info.v;
        st = 0;
        }
    else if ( info.processing_type == EX_PT_DATA_TYPE )
        {
        ex_PT_data ex_info{};
        auto data_ptr = ( (char*)&ex_info );
        std::copy( data, data + sizeof( ex_info ), (char*)&ex_info );
        std::swap( data_ptr[ 0 ], data_ptr[ 1 ] );
        std::swap( data_ptr[ 2 ], data_ptr[ 3 ] );
        v = ex_info.v;
        st = ex_info.status;
        }
    else if ( info.processing_type == EH_PT_DATA_TYPE )
        {
        EH_PT_data EH_info{};
        std::memcpy( &EH_info, data, sizeof( EH_info ) );

        auto bytes = reinterpret_cast<std::byte*>( &EH_info );
        std::swap( bytes[ 0 ], bytes[ 1 ] );
        std::swap( bytes[ 2 ], bytes[ 3 ] );
        v = static_cast<float>( EH_info.v );
        st = 1;
        }

    v = alfa * v;
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::evaluate_io()
    {
    pressure_e_iolink::evaluate_io(
        get_name(), (char*)get_AI_data( C_AI_INDEX ), n_article, v, st, alfa );
    }
//-----------------------------------------------------------------------------
const char* pressure_e_iolink::get_error_description()
    {
    return iol_dev.get_error_description( get_error_id() );
    }
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
pressure_e_iolink::ARTICLE pressure_e_iolink::get_article_n() const
    {
    return n_article;
    }
#endif
//-----------------------------------------------------------------------------
float pressure_e_iolink::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    if (get_AI_IOLINK_state(C_AI_INDEX) != io_device::IOLINKSTATE::OK)
        {
        return get_par( P_ERR, 0 );
        }
    else
        {
        return v;
        }
    }
//-----------------------------------------------------------------------------
int pressure_e_iolink::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state( C_AI_INDEX );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -(int)res;
        }
    else
        {
        return st;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
circuit_breaker::circuit_breaker( const char* dev_name ):analog_io_device(
    dev_name, DT_F, DST_F, 0)
    {
    }
//-----------------------------------------------------------------------------
int circuit_breaker::save_device_ex( char *buff ) const
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "ERR={}, M={}, ",
        err, m ) ).size;

    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "NOMINAL_CURRENT_CH={{{},{},{},{}}}, ",
        +in_info.nominal_current_ch1, +in_info.nominal_current_ch2,
        +in_info.nominal_current_ch3, +in_info.nominal_current_ch4 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "LOAD_CURRENT_CH={{{:.1f},{:.1f},{:.1f},{:.1f}}}, ",
        .1f * in_info.load_current_ch1, .1f * in_info.load_current_ch2,
        .1f * in_info.load_current_ch3, .1f * in_info.load_current_ch4 ) ).size;

    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "ST_CH={{{},{},{},{}}}, ",
        +in_info.st_ch1, +in_info.st_ch2,
        +in_info.st_ch3, +in_info.st_ch4 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "ERR_CH={{{},{},{},{}}}, ",
        +in_info.err_ch1, +in_info.err_ch2,
        +in_info.err_ch3, +in_info.err_ch4 ) ).size;

    return res;
    }
//-----------------------------------------------------------------------------
int circuit_breaker::set_cmd( const char *prop, u_int idx, double val )
    {
    if (G_DEBUG)
        {
        G_LOG->debug(
            "%s\t circuit_breaker::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val);
        }

    if ( strcmp( prop, "ST" ) == 0 )
        {
        if ( auto new_val = static_cast<int>( val ); new_val )
            {
            on();
            }
        else
            {
            off();
            }

        return 0;
        }


    if ( strcmp( prop, "ST_CH" ) == 0 )
        {
        switch ( idx )
            {
            case 1:
                out_info->switch_ch1 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch1 = val;
                return 0;
            case 2:
                out_info->switch_ch2 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch2 = val;
                return 0;
            case 3:
                out_info->switch_ch3 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch3 = val;
                return 0;
            case 4:
                out_info->switch_ch4 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch4 = val;
                return 0;
            }
        }

    return analog_io_device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
void circuit_breaker::direct_set_value( float v )
    {
    if ( v )
        {
        on();
        }
    else
        {
        off();
        }
    }
//-----------------------------------------------------------------------------
void circuit_breaker::direct_on()
    {
    out_info->valid_flag = true;
    out_info->switch_ch1 = true;
    out_info->switch_ch2 = true;
    out_info->switch_ch3 = true;
    out_info->switch_ch4 = true;

    if ( G_PAC_INFO()->is_emulator() )
        {
        in_info.st_ch1 = true;
        in_info.st_ch2 = true;
        in_info.st_ch3 = true;
        in_info.st_ch4 = true;

        analog_io_device::direct_on();
        }
    }
//-----------------------------------------------------------------------------
void circuit_breaker::direct_off()
    {
    out_info->valid_flag = true;
    out_info->switch_ch1 = false;
    out_info->switch_ch2 = false;
    out_info->switch_ch3 = false;
    out_info->switch_ch4 = false;

    if ( G_PAC_INFO()->is_emulator() )
        {
        in_info.st_ch1 = false;
        in_info.st_ch2 = false;
        in_info.st_ch3 = false;
        in_info.st_ch4 = false;

        analog_io_device::direct_off();
        }
    }
//-----------------------------------------------------------------------------
float circuit_breaker::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    return v;
    }
//-----------------------------------------------------------------------------
int circuit_breaker::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state( C_AI_INDEX );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -(int)res;
        }
    else
        {
        return st;
        }
    }
//-----------------------------------------------------------------------------
void circuit_breaker::evaluate_io()
    {
    auto res = get_AO_write_data( 0 );
    if ( !res ) return;

    out_info = (F_data_out*)res;

    if ( get_AI_IOLINK_state(C_AI_INDEX) == io_device::IOLINKSTATE::OK )
        {
        if ( !is_read_OK )
            {
            out_info->switch_ch1 = in_info.st_ch1;
            out_info->switch_ch2 = in_info.st_ch2;
            out_info->switch_ch3 = in_info.st_ch3;
            out_info->switch_ch4 = in_info.st_ch4;
            out_info->nominal_current_ch1 = in_info.nominal_current_ch1;
            out_info->nominal_current_ch2 = in_info.nominal_current_ch2;
            out_info->nominal_current_ch3 = in_info.nominal_current_ch3;
            out_info->nominal_current_ch4 = in_info.nominal_current_ch4;

            is_read_OK = true;
            }
        }
    else
        {
        is_read_OK = false;
        }

    auto data = (char*)get_AI_data(C_AI_INDEX);
    std::copy(data, data + sizeof(in_info), (char*)&in_info);

#ifdef DEBUG_IOLINK_F
    char* tmp = (char*)data;
    sprintf(G_LOG->msg, "%x %x %x %x %x %x %x %x\n",
        tmp[0], tmp[1], tmp[2], tmp[3],
        tmp[4], tmp[5], tmp[6], tmp[7] );
    G_LOG->write_log(i_log::P_WARNING);

    sprintf(G_LOG->msg, "nominal_current_ch1 %u, load_current_ch1 %u, "
        "st_ch1 %x, err_ch1 %x, %.1f\n", in_info.nominal_current_ch1,
        in_info.load_current_ch1, in_info.st_ch1, in_info.err_ch1,
        .1f * in_info.v );
    G_LOG->write_log(i_log::P_WARNING);
    sprintf(G_LOG->msg, "nominal_current_ch2 %u, load_current_ch2 %u, "
        "st_ch2 %x, err_ch2 %x\n", in_info.nominal_current_ch2,
        in_info.load_current_ch2, in_info.st_ch2, in_info.err_ch2 );
    G_LOG->write_log(i_log::P_WARNING);
#endif
    v = .1f * in_info.v;
    st = in_info.st_ch1 || in_info.st_ch2 ||
        in_info.st_ch3 || in_info.st_ch4 ? 1 : 0;
    err = in_info.err_ch1 || in_info.err_ch2 ||
        in_info.err_ch3 || in_info.err_ch4 ? 1 : 0;
    }
//-----------------------------------------------------------------------------
circuit_breaker::F_data_out::F_data_out(): switch_ch1( false ), switch_ch2( false ),
    switch_ch3( false ), switch_ch4( false ),
    reserved( 0 ), valid_flag( false ),
    nominal_current_ch1( 0 ), nominal_current_ch2( 0 ),
    nominal_current_ch3( 0 ), nominal_current_ch4( 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
concentration_e::concentration_e( const char* dev_name,
    DEVICE_SUB_TYPE sub_type ) :
    AI1( dev_name, DT_QT, sub_type, ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float concentration_e::get_max_val() const
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float concentration_e::get_min_val() const
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
concentration_e_ok::concentration_e_ok( const char* dev_name ) : concentration_e( dev_name,
    DST_QT_OK )
    {
    }

int concentration_e_ok::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return concentration_e::get_state();

    int i = get_DI( DI_INDEX );
    return i == 1 ? concentration_e::get_state() : -1;
    }

int concentration_e_ok::save_device_ex( char* buff ) const
    {
    int res = 0;
    if ( G_PAC_INFO()->is_emulator() )
        {
        res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE,
            "OK=1, " ).size );
        }
    else
        {
        res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "OK={}, ",
            get_DI( DI_INDEX ) ).size );
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
concentration_e_iolink::concentration_e_iolink( const char* dev_name ) :
    analog_io_device( dev_name,
    DT_QT, DST_QT_IOLINK, static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_ERR ), 0, "P_ERR" );
    set_par_name( static_cast<int>( CONSTANTS::P_MAX_V ), 0, "P_MAX_V" );
    };
//-----------------------------------------------------------------------------
concentration_e_iolink::~concentration_e_iolink()
    {
    delete info;
    info = nullptr;
    }
//-----------------------------------------------------------------------------
int concentration_e_iolink::save_device_ex( char *buff ) const
    {
    // LCOV_EXCL_START
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "T={:.1f}, ",
        get_temperature() ) ).size;
    // LCOV_EXCL_STOP

    return res;
    }
//-----------------------------------------------------------------------------
float concentration_e_iolink::get_temperature() const
    {
    return .1f * info->temperature;
    }
//-----------------------------------------------------------------------------
float concentration_e_iolink::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    if ( get_AI_IOLINK_state( static_cast<int>( CONSTANTS::C_AI_INDEX ) ) !=
        io_device::IOLINKSTATE::OK )
        {
        return get_par( static_cast<int>( CONSTANTS::P_ERR ), 0 );
        }
    else
        {
        auto v = 0.001f * static_cast<float>( info->conductivity );
        if ( auto max_v = get_par( static_cast<int>( CONSTANTS::P_MAX_V ), 0 );
            max_v > 0 && v > max_v )
            {
            return max_v;
            }
        return v;
        }
    }
//-----------------------------------------------------------------------------
int concentration_e_iolink::get_state() const
	{
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state(
        static_cast<int>( CONSTANTS::C_AI_INDEX ) );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -static_cast<int>( res );
        }
    else
        {
        return info->status;
        }
	}
//-----------------------------------------------------------------------------
void concentration_e_iolink::evaluate_io()
    {
    auto data = (char*)get_AI_data(0);
    if ( !data ) return;

    const int SIZE = 12;
    std::reverse_copy (data, data + SIZE, (char*) info);

#ifdef DEBUG_IOLINK_QT
    char *tmp = (char*) info;
    sprintf( G_LOG->msg, "%x %x %x %x %x %x %x %x %x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ],
        tmp[ 4 ], tmp[ 5 ], tmp[ 6 ], tmp[ 7 ],
        tmp[ 8 ], tmp[ 9 ], tmp[ 10 ], tmp[ 11 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg, "conductivity %u, temperature %u, "
            "status %x\n", info->conductivity,
            info->temperature, info->status);
    G_LOG->write_log(i_log::P_NOTICE);

    sprintf( G_LOG->msg,
            "conductivity %.3f, temperature %.1f, status %x\n",
            get_value(), get_temperature(), get_state());
    G_LOG->write_log(i_log::P_NOTICE);
#endif
    }
//-----------------------------------------------------------------------------
const char* concentration_e_iolink::get_error_description()
    {
    return iol_dev.get_error_description( get_error_id() );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_input::analog_input( const char* dev_name ) : AI1( dev_name, DT_AI,
    DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float analog_input::get_max_val() const
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float analog_input::get_min_val() const
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_io_device::analog_io_device( const char* dev_name,
    device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) : device( dev_name, type, sub_type, par_cnt ),
    io_device( dev_name )
    {
    }
//-----------------------------------------------------------------------------
void analog_io_device::print() const
    {
    device::print();
    //io_device::print();
    }
//-----------------------------------------------------------------------------
int analog_io_device::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t analog_io_device::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        }

    auto& emulator = get_emulator();
    if ( strcmp( prop, "M_EXP" ) == 0 )
        {
        emulator.param( static_cast<float>( val ), emulator.get_st_deviation() );
        }
    else if ( strcmp( prop, "S_DEV" ) == 0 )
        {
        emulator.param( emulator.get_m_expec(), static_cast<float>( val ) );
        }
    else if ( prop[ 0 ] == 'E' )
        {
        set_emulation( val != 0 );
        }
    else
        {
        return device::set_cmd( prop, idx, val );
        }

    return 0;
    }

int analog_io_device::save_device_ex( char* buff ) const
    {
    // LCOV_EXCL_START
    auto res = fmt::format_to_n(
        buff, MAX_COPY_SIZE,
        "E={}, M_EXP={:.1f}, S_DEV={:.1f}, ",
        is_emulation() ? 1 : 0, get_emulator().get_m_expec(),
        get_emulator().get_st_deviation() );
    // LCOV_EXCL_STOP
    return static_cast<int>( res.size );
    }
//-----------------------------------------------------------------------------
float analog_io_device::get_value() const
    {
    if ( is_emulation() ) return get_emulator().get_value();

    return device::get_value();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int timer::save( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int timer::load( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int timer::get_saved_size() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
timer::timer()
    {
    }
//-----------------------------------------------------------------------------
void timer::start()
    {
    if ( STATE::S_STOP == state )
        {
        work_time = 0;
        }

    if ( STATE::S_PAUSE == state || STATE::S_STOP == state )
        {
        state = STATE::S_WORK;
        last_time = get_millisec();
        }
    }
//-----------------------------------------------------------------------------
void timer::reset()
    {
    state = STATE::S_STOP;
    work_time = 0;
    }
//-----------------------------------------------------------------------------
void timer::pause()
    {
    if ( STATE::S_WORK == state )
        {
        work_time += get_delta_millisec( last_time );
        }
    state = STATE::S_PAUSE;
    }
//-----------------------------------------------------------------------------
bool timer::is_time_up() const
    {
    if ( STATE::S_WORK == state )
        {
        u_int time = work_time + get_delta_millisec( last_time );
        if (  time <= countdown_time )
            {
            return 0;
            }
        else
            {
            return 1;
            }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
uint32_t timer::get_work_time() const
    {
    if ( STATE::S_WORK == state )
        {
        return work_time + get_delta_millisec( last_time );
        }
    else
        {
        return work_time;
        }
    }
//-----------------------------------------------------------------------------
void timer::set_countdown_time( uint32_t new_countdown_time )
    {
    if ( G_DEBUG )
        {
        if ( 0 == new_countdown_time )
            {
            printf( "Error in timer::set_countdown_time: "
                "invalid time value = %" PRIu32 "!\n", new_countdown_time );
            }
        }

    countdown_time = new_countdown_time;
    }
//-----------------------------------------------------------------------------
uint32_t timer::get_countdown_time() const
    {
    return countdown_time;
    }
//-----------------------------------------------------------------------------
timer::STATE timer::get_state() const
    {
    return state;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
timer_manager::timer_manager( u_int timers_count ) : timers_cnt( timers_count )
    {
    if ( timers_cnt )
        {
        timers = new timer[ timers_cnt ];
        }
    }
//-----------------------------------------------------------------------------
timer_manager::~timer_manager()
    {
    if ( timers )
        {
        delete [] timers;
        timers     = nullptr;
        timers_cnt = 0;
        }
    }
//-----------------------------------------------------------------------------
timer* timer_manager::operator[]( unsigned int index )
    {
    index -= 1;

    if ( index < timers_cnt )
        {
        return &timers[ index ];
        }
    else
        {
        if ( G_DEBUG )
            {
            printf( "timer_manager[] - error: index[ %u ] > count [ %u ]\n",
                index, timers_cnt );
            }
        }

    return &stub;
    }
//-----------------------------------------------------------------------------
int timer_manager::get_count() const
    {
    return timers_cnt;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void virtual_device::direct_off()
    {
    state = 0;
    value = 0;
    }

void virtual_device::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_device::get_value() const
    {
    return value;
    }

void virtual_device::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_device::direct_on()
    {
    state = 1;
    }

int virtual_device::get_state() const
    {
    return state;
    }

bool virtual_device::is_active()
    {
    bool ret = ((state == 0) ? false : true);
    return (level_logic_invert ? !ret : ret);
    }

void virtual_device::set_rt_par(unsigned int idx, float value)
    {
    switch (idx)
        {
        case 1:
            level_logic_invert = value != 0;
            break;
        }
    }

virtual_device::virtual_device( const char *dev_name,
    device::DEVICE_TYPE dev_type,
    device::DEVICE_SUB_TYPE dev_sub_type ) : device (dev_name, dev_type, dev_sub_type, 0)
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_output::analog_output( const char* dev_name ) :
    AO1( dev_name, DT_AO, DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    set_par_name( P_MIN_VALUE, 0, "P_MIN_V" );
    set_par_name( P_MAX_VALUE, 0, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float analog_output::get_min_value() const
    {
    return get_par( P_MIN_VALUE, 0 );
    }
//-----------------------------------------------------------------------------
float analog_output::get_max_value() const
    {
    return get_par( P_MAX_VALUE, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
motor_altivar::motor_altivar( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    i_motor( dev_name, sub_type,
        par_cnt + static_cast<int>( CONSTANTS::ADDITIONAL_PARAM_COUNT ) - 1 ),
        io_device( dev_name )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_ON_TIME ), 0, "P_ON_TIME" );
    }
//-----------------------------------------------------------------------------
int motor_altivar::save_device_ex(char * buff) const
    {
    int res = 0;
    if ( G_PAC_INFO()->is_emulator() )
        {
        res = fmt::format_to_n( buff, MAX_COPY_SIZE,
            "R={}, FRQ={:.1f}, RPM={}, EST={}, AMP={:.1f}, MAX_FRQ=0.0, ",
            reverse, freq, rpm, est, amperage ).size;
        }
    else
        {
        res = fmt::format_to_n( buff, MAX_COPY_SIZE,
            "R={}, FRQ={:.1f}, RPM={}, EST={}, AMP={:.1f}, MAX_FRQ={:.1f}, ",
            atv->reverse, atv->frq_value, atv->rpm_value, atv->remote_state,
            atv->amperage, atv->frq_max ).size;
        }

    return res;
    }
//-----------------------------------------------------------------------------
float motor_altivar::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return freq;

    return atv->get_output_in_percent();
    }
//-----------------------------------------------------------------------------
void motor_altivar::direct_set_value(float value)
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        freq = value;
        return;
        }

    atv->set_output_in_percent( value );
    }
//-----------------------------------------------------------------------------
void motor_altivar::direct_set_state(int new_state)
    {
    if ( G_PAC_INFO()->is_emulator() )
        return device::direct_set_state( new_state );

    if (new_state == 2)
        {
        atv->cmd = 2;
        atv->reverse = 1;
        return;
        }

    if (new_state == 100)
        {
        atv->Disable();
        return;
        }

    if (new_state == 101)
        {
        atv->Enable();
        return;
        }

    if (new_state)
        {
        direct_on();
        }
    else
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int motor_altivar::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    return atv->state;
    }
//-----------------------------------------------------------------------------
void motor_altivar::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_on();

    atv->cmd = 1;
    atv->reverse = 0;
    }
//-----------------------------------------------------------------------------
void motor_altivar::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_off();

    if (atv->state < 0)
        {
            atv->cmd = 4; //2 bit - fault reset
        }
    else
        {
        atv->cmd = 0;
        }
    atv->reverse = 0;
    }
//-----------------------------------------------------------------------------
void motor_altivar::set_string_property(const char * field, const char * value)
    {
    device::set_string_property( field, value );

    if (strcmp(field, "IP") == 0)
        {
        int port = 502;
        int timeout = 300;
        auto nodeip = std::string(value);
        nodeip.append(":");
        nodeip.append(std::to_string(port));
        nodeip.append(" ");
        nodeip.append(std::to_string(timeout));
        if (!atv)
            {
            atv = G_ALTIVAR_MANAGER()->get_node(nodeip.c_str());
            if (!atv)
                {
                G_ALTIVAR_MANAGER()->add_node(value, port, timeout,
                    get_article(), get_name() );
                atv = G_ALTIVAR_MANAGER()->get_node(nodeip.c_str());
                }
            }
        }
    }
//-----------------------------------------------------------------------------
void motor_altivar::print() const
    {
    device::print();
    }
//-----------------------------------------------------------------------------
int motor_altivar::get_params_count() const
    {
    return static_cast<int>( CONSTANTS::ADDITIONAL_PARAM_COUNT ) - 1;
    }
//-----------------------------------------------------------------------------
float motor_altivar::get_amperage() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return amperage;

    return atv->amperage;
    }
//-----------------------------------------------------------------------------
int motor_altivar::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        printf( "motor_altivar::set_cmd() - prop = %s, idx = %d, val = %f\n",
            prop, idx, val );

        if ( strcmp( prop, "R" ) == 0 )
            {
            reverse = static_cast<int>( val );
            return 0;
            }
        else if ( strcmp( prop, "FRQ" ) == 0 )
            {
            freq = static_cast<float>( val );
            return 0;
            }
        else if ( strcmp( prop, "RPM" ) == 0 )
            {
            rpm = static_cast<int>( val );
            return 0;
            }
        else if ( strcmp( prop, "EST" ) == 0 )
            {
            est = static_cast<int>( val );
            return 0;
            }
        else if ( strcmp( prop, "AMP" ) == 0 )
            {
            amperage = static_cast<float>( val );
            return 0;
            }
        }

    return device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float motor_altivar_linear::get_linear_speed() const
    {
    float d = get_par( static_cast<int>( CONSTANTS::P_SHAFT_DIAMETER ),
        start_param_idx );
    float n = get_par( static_cast<int>( CONSTANTS::P_TRANSFER_RATIO ),
        start_param_idx );
    float v = .0f;

    if ( 0 != d && 0 != n )
        {
        if ( G_PAC_INFO()->is_emulator() )
            {
            v = ( get_rpm() * (float)M_PI * d ) / ( n * SEC_IN_MIN );
            }
        else
            {
            v = ( get_atv()->rpm_value * (float)M_PI * d ) / ( n * SEC_IN_MIN );
            }
        }

    return v;
    }
//-----------------------------------------------------------------------------
motor_altivar_linear::motor_altivar_linear( const char* dev_name ) :
    motor_altivar( dev_name, device::M_ATV_LINEAR,
        static_cast<int>( CONSTANTS::ADDITIONAL_PARAM_COUNT ) - 1 )
    {
    start_param_idx = motor_altivar::get_params_count();
    set_par_name( static_cast<int>( CONSTANTS::P_SHAFT_DIAMETER ),
        start_param_idx, "P_SHAFT_DIAMETER" );
    set_par_name( static_cast<int>( CONSTANTS::P_TRANSFER_RATIO ),
        start_param_idx, "P_TRANSFER_RATIO" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
converter_iolink_ao::converter_iolink_ao( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_EY, device::DST_CONV_AO2, 0 )
    {
#ifdef PTUSA_TEST
    stub_p_data_out = {};
#endif

    static_assert( sizeof( process_data_in ) == PROCESS_DATA_IN_SIZE,
        "Struct `process_data_in` must be the 1 byte size." );
    static_assert( sizeof( process_data_out ) == PROCESS_DATA_OUT_SIZE,
        "Struct `process_data_out` must be the 4 bytes size." );
    }

void converter_iolink_ao::direct_on()
    {
    set_channel_value( 1, 100 );
    set_channel_value( 2, 100 );
    }

void converter_iolink_ao::direct_off()
    {
    set_channel_value( 1, 0 );
    set_channel_value( 2, 0 );
    }

float converter_iolink_ao::get_channel_value( u_int ch ) const
    {
    switch ( ch )
        {
        case 1: return v1;
        case 2: return v2;
        default: return 0.0;
        }
    }

int converter_iolink_ao::get_state() const
    {
    if ( err )
        {
        return -err;
        }

    return analog_io_device::get_state();
    }

uint16_t converter_iolink_ao::calc_setpoint( float &val ) const
    {
    if ( val < 0 ) val = 0.0f;
    else if ( val > 100 ) val = 100.0f;

    // Конвертируем: диапазон 0% - 4'000, 100% - 20'000.
    auto setpoint = static_cast<uint16_t>( 4000 + val * 16000.0f / 100.0f );
    auto tmp = (char*)&setpoint;
    std::swap( tmp[ 0 ], tmp[ 1 ] );
    return setpoint;
    }

void converter_iolink_ao::set_channel_value( u_int ch, float val )
    {
    auto new_value = calc_setpoint( val );
    switch ( ch )
        {
        case 1:
            p_data_out->setpoint_ch1 = new_value;
            v1 = val;
            break;

        case 2:
            p_data_out->setpoint_ch2 = new_value;
            v2 = val;
            break;
        }

    calculate_state();
    }

void converter_iolink_ao::calculate_state()
    {
    if ( v1 > 0.0f || v2 > 0.0f ) analog_io_device::direct_on();
    else if ( v1 == 0.0f && v2 == 0.0f ) analog_io_device::direct_off();
    }

void converter_iolink_ao::evaluate_io()
    {
    if ( G_PAC_INFO()->is_emulator() ) return;

    auto data = reinterpret_cast<std::byte*>( get_AI_data( C_AIAO_INDEX ) );

    if ( !data ) return; // Return, if data is nullptr (in debug mode).

    std::copy( data, data + PROCESS_DATA_IN_SIZE,
        reinterpret_cast<std::byte*>( &p_data_in ) );

    p_data_out = reinterpret_cast<process_data_out*>(
        get_AO_write_data( C_AIAO_INDEX ) );

    if ( auto iol_st = get_AI_IOLINK_state( C_AIAO_INDEX );
        iol_st == io_device::IOLINKSTATE::OK )
        {
        // Проверка статуса устройства (0 = OK согласно IODD).
        if ( p_data_in.device_status == 0 )
            {
            err = 0;
            }
        else
            {
            err = p_data_in.device_status;
            }
        }
    else
        {
        err = iol_st;
        }
    }

int converter_iolink_ao::save_device_ex( char* buff ) const
    {
    auto l = analog_io_device::save_device_ex( buff );
    auto res = fmt::format_to_n( buff + l, MAX_COPY_SIZE,
        "CH={{{:.2f},{:.2f}}}, ", v1, v2 );

    return res.size + l;
    }

int converter_iolink_ao::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( strcmp( prop, "CH" ) == 0 )
        {
        set_channel_value( idx, static_cast<float>( val ) );
        return 0;
        }

    return analog_io_device::set_cmd( prop, idx, val );
    }

const char* converter_iolink_ao::get_error_description()
    {
    auto err_id = get_error_id();
    switch ( err_id )
        {
        case -1:
            return "требуется обслуживание";

        case -2:
            return "не соответствует спецификации";

        case -3:
            return "функциональная проверка";

        case -4:
            return "отказ";

        default:
            return iol_dev.get_error_description( err_id );
        }
    }

#ifdef WIN_OS
#pragma warning(pop)
#endif // WIN_OS
