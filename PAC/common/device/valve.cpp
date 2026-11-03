#include "fmt/format.h"
#include <algorithm>

#include "valve.h"

#include "manager.h"
#include "bus_coupler_io.h"
#include "PAC_info.h"
#include "g_errors.h"

std::vector<valve*> valve::to_switch_off;
std::vector<valve_DO2_DI2_bistable*> valve::v_bistable;

alfalaval_iol_valve_out_data_swapped valve_iolink_mix_proof::stub_out_info{};
alfalaval_iol_valve_out_data_swapped valve_iolink_shut_off_thinktop::stub_out_info{};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO2::valve_DO2( const char* dev_name ) : valve( dev_name, DT_V, DST_V_DO2 )
    {
    }
//-----------------------------------------------------------------------------
/// @brief Получение состояния клапана без учета обратной связи.
valve::VALVE_STATE valve_DO2::get_valve_state() const
    {
    return (VALVE_STATE)get_state();
    }
//-----------------------------------------------------------------------------
int valve_DO2::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_state();

    int b1 = get_DO( DO_INDEX_1 );
    int b2 = get_DO( DO_INDEX_2 );
    if ( b1 == b2 ) return -1;
    return b2;
    }
//-----------------------------------------------------------------------------
void valve_DO2::direct_on()
    {
    set_fb_state( true, false );
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    set_DO( DO_INDEX_1, 0 );
    set_DO( DO_INDEX_2, 1 );
    }
//-----------------------------------------------------------------------------
void valve_DO2::direct_off()
    {
    set_fb_state( false, true );
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    set_DO( DO_INDEX_1, 1 );
    set_DO( DO_INDEX_2, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve::valve( bool is_on_fb, bool is_off_fb, const char* dev_name,
    device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type ) :
    digital_io_device( dev_name, type, sub_type, ADDITIONAL_PARAMS_COUNT ),
    is_on_fb( is_on_fb ),
    is_off_fb( is_off_fb )
    {
    on_fb = false;
    off_fb = true;
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    set_par_name( P_FB, 0, "P_FB" );
    }
//-----------------------------------------------------------------------------
valve::valve( const char* dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type ) :
    digital_io_device( dev_name, type, sub_type, 0 )
    {
    }
//-----------------------------------------------------------------------------
int valve::save_device_ex( char* buff ) const
    {
    int res = 0;
    if ( is_on_fb )
        {
        res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "FB_ON_ST={}, ",
            get_on_fb_value() ? 1 : 0 ) ).size;
        }
    if ( is_off_fb )
        {
        res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE, "FB_OFF_ST={}, ",
            get_off_fb_value() ? 1 : 0 ) ).size;
        }
    return res;
    }
//-----------------------------------------------------------------------------
int valve::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::get_state();

    switch ( get_valve_state() )
        {
        case V_LOWER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_LOWER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( ( is_off_fb || is_on_fb ) && get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_LOWER_SEAT_FB_OFF;
                }

            return VX_LOWER_SEAT;

        case V_UPPER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_UPPER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( ( is_off_fb || is_on_fb ) && get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_UPPER_SEAT_FB_OFF;
                }

            return VX_UPPER_SEAT;

        case V_ON:
            if ( is_off_fb || is_on_fb ) //Обратная связь есть.
                {
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF ) //Обратная связь отключена.
                    {
                    if ( get_manual_mode() )
                        {
                        return VX_ON_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_ON_FB_OFF;
                        }
                    }
                else //Обратная связь включена.
                    {
                    if ( get_manual_mode() ) //Ручной режим включен.
                        {
                        if ( get_fb_state() == true )
                            {
                            return VX_ON_FB_OK_MANUAL;
                            }
                        else
                            {
                            return VX_ON_FB_ERR_MANUAL;
                            }
                        } // if ( get_manual_mode() )
                    else  //Ручной режим отключен.
                        {
                        if ( get_fb_state() == true )
                            {
                            return VX_ON_FB_OK;
                            }
                        else
                            {
                            return VX_ON_FB_ERR;
                            }
                        }
                    }
                }//if ( is_off_fb || is_on_fb ) //Обратная связь есть.
            else //Обратной связи нет.
                {
                if ( get_manual_mode() )
                    {
                    return VX_ON_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_ON_FB_OK;
                    }
                }
            break;

        case V_OFF:
            if ( is_off_fb || is_on_fb ) //Обратная связь есть.
                {
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF ) //Обратная связь отключена.
                    {
                    if ( get_manual_mode() )
                        {
                        return VX_OFF_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_OFF_FB_OFF;
                        }
                    }
                else //Обратная связь включена.
                    {
                    if ( get_manual_mode() ) //Ручной режим включен.
                        {
                        if ( get_fb_state() == true )
                            {
                            return VX_OFF_FB_OK_MANUAL;
                            }
                        else
                            {
                            return VX_OFF_FB_ERR_MANUAL;
                            }
                        } // if ( get_manual_mode() )
                    else  //Ручной режим отключен.
                        {
                        if ( get_fb_state() == true )
                            {
                            return VX_OFF_FB_OK;
                            }
                        else
                            {
                            return VX_OFF_FB_ERR;
                            }
                        }
                    }
                }//if ( is_off_fb || is_on_fb ) //Обратная связь есть.
            else //Обратной связи нет.
                {
                if ( get_manual_mode() )
                    {
                    return VX_OFF_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_OFF_FB_OK;
                    }
                }
            break;

        case V_STOP:
            return V_STOP;
        }

    return VX_UNKNOWN;
    }
//-----------------------------------------------------------------------------
int valve::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_PAC_INFO()->is_emulator() && prop[ 0 ] == 'F' )
        {
        if ( strcmp( prop, "FB_ON_ST" ) == 0 )
            {
            on_fb = val != .0;
            }
        else
            {
            off_fb = val != .0;
            }
        return 0;
        }

    return device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
bool valve::is_wash_seat_active() const
    {
    return wash_flag;
    }
//-----------------------------------------------------------------------------
void valve::set_seat_wash_state( bool wash_flag )
    {
    this->wash_flag = wash_flag;
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve::get_valve_state() const
    {
    return (VALVE_STATE)digital_io_device::get_state();
    }
//-----------------------------------------------------------------------------
/// @brief Получение состояния обратной связи.
bool valve::get_fb_state() const
    {
    return true;
    }
//-----------------------------------------------------------------------------
bool valve::is_switching_off_finished()
    {
    return !is_switching_off;
    };
//-----------------------------------------------------------------------------
void valve::evaluate()
    {
    if ( v_bistable.empty() == false )
        {
        std::vector< valve_DO2_DI2_bistable* >::iterator iter;
        for ( iter = v_bistable.begin(); iter != v_bistable.end(); iter++ )
            {
            valve_DO2_DI2_bistable* v = *iter;
            v->evaluate();
            }
        }

    if ( to_switch_off.empty() )
        {
        return;
        }

    u_int delay = G_PAC_INFO()->par[ PAC_info::P_V_OFF_DELAY_TIME ];

    for ( auto v : to_switch_off )
        {
        if ( v->is_switching_off &&
            get_delta_millisec( v->start_off_time ) > delay )
            {
            if ( !v->get_manual_mode() )
                {
                v->direct_off();
                }

            v->is_switching_off = false;
            v->was_on_auto = false;
            }
        }

    to_switch_off.erase(
        std::remove_if( to_switch_off.begin(), to_switch_off.end(),
        []( valve* v ) { return v->is_switching_off_finished(); } ),
        to_switch_off.end() );
    }
//-----------------------------------------------------------------------------
bool valve::is_opened() const
    {
    return get_on_fb_value();
    }
//-----------------------------------------------------------------------------
bool valve::is_closed() const
    {
    return get_off_fb_value();
    }
//-----------------------------------------------------------------------------
void valve::set_fb_state( bool new_on_fb, bool new_off_fb )
    {
    on_fb = new_on_fb;
    off_fb = new_off_fb;
    };
//-----------------------------------------------------------------------------
/// @brief Получение значения обратной связи на включенное состояние.
bool valve::get_on_fb_value() const
    {
    return on_fb;
    }
//-----------------------------------------------------------------------------
/// @brief Получение значения обратной связи на выключенное состояние.
bool valve::get_off_fb_value() const
    {
    return off_fb;
    }
//-----------------------------------------------------------------------------
void valve::off()
    {
    if ( false == was_on_auto ||                //Если был включен вручную.
        get_valve_state() == VALVE_STATE::V_UPPER_SEAT ||
        get_valve_state() == VALVE_STATE::V_LOWER_SEAT )
        {
        if ( !get_manual_mode() )
            {
            direct_off();
            }

        return;
        }

    if ( false == is_switching_off )
        {
        is_switching_off = true;
        start_off_time = get_millisec();

        to_switch_off.push_back( this );
        }
    }
//-----------------------------------------------------------------------------
void valve::on()
    {
    was_on_auto = true;
    is_switching_off = false;
    digital_io_device::on();
    }
//-----------------------------------------------------------------------------
void valve::clear_switching_off_queue()
    {
    to_switch_off.clear();
    }
//-----------------------------------------------------------------------------
void valve::clear_v_bistable()
    {
    v_bistable.clear();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1_DI1_off::valve_DO1_DI1_off( const char* dev_name ) :
    valve( false, true, dev_name, DT_V, DST_V_DO1_DI1_FB_OFF )
    {
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_off::direct_on()
    {
    set_fb_state( true, false );
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    if ( auto o = get_DO( DO_INDEX ); 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_off::direct_off()
    {
    set_fb_state( false, true );
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    if ( auto o = get_DO( DO_INDEX ); o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_DO1_DI1_off::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o = get_DO( DO_INDEX );
    return (VALVE_STATE)o;
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI1_off::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( auto o = get_DO( DO_INDEX ), i = get_DI( DI_INDEX ); o != i )
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI1_off::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( DI_INDEX );
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI1_off::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return !get_DI( DI_INDEX );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1_DI1_on::valve_DO1_DI1_on( const char* dev_name ) : valve( true, false,
    dev_name, DT_V, DST_V_DO1_DI1_FB_ON )
    {
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_on::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    if ( auto o = get_DO( DO_INDEX ); 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_on::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    if ( auto o = get_DO( DO_INDEX ); o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_DO1_DI1_on::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o = get_DO( DO_INDEX );
    return (VALVE_STATE)o;
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI1_on::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    int o = get_DO( DO_INDEX );
    if ( auto i = get_DI( DI_INDEX ); o == i )
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI1_on::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( DI_INDEX );
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI1_on::get_off_fb_value() const
    {
    return false;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1_DI2::valve_DO1_DI2( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, DST_V_DO1_DI2 )
    {
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI2::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI2::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_DO1_DI2::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o = get_DO( DO_INDEX );
    return (VALVE_STATE)o;
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI2::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( auto o = get_DO( DO_INDEX ), i1 = get_DI( DI_INDEX_1 ),
        i0 = get_DI( DI_INDEX_2 );
        ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI2::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( DI_INDEX_2 );
    }
//-----------------------------------------------------------------------------
bool valve_DO1_DI2::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( DI_INDEX_1 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO2_DI2::valve_DO2_DI2( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, DST_V_DO2_DI2 )
    {
    }
//-----------------------------------------------------------------------------
void valve_DO2_DI2::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    int o = get_DO( DO_INDEX_1 );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_1, 1 );
        set_DO( DO_INDEX_2, 0 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO2_DI2::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    int o = get_DO( DO_INDEX_2 );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_1, 0 );
        set_DO( DO_INDEX_2, 1 );
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_DO2_DI2::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o1 = get_DO( DO_INDEX_2 );
    return (VALVE_STATE)o1;
    }
//-----------------------------------------------------------------------------
bool valve_DO2_DI2::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( auto o0 = get_DO( DO_INDEX_1 ),
        o1 = get_DO( DO_INDEX_2 ),
        i0 = get_DI( DI_INDEX_1 ),
        i1 = get_DI( DI_INDEX_2 );
        o1 == i1 && o0 == i0 && ( o0 != o1 && i0 != i1 ) )
        {
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_DO2_DI2::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( DI_INDEX_1 );
    }
//-----------------------------------------------------------------------------
bool valve_DO2_DI2::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( DI_INDEX_2 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO2_DI2_bistable::valve_DO2_DI2_bistable( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, V_DO2_DI2_BISTABLE ),
    is_stoped( false ),
    is_opening( false ),
    is_closing( false )
    {
    v_bistable.push_back( this );
    }
//-----------------------------------------------------------------------------
int valve_DO2_DI2_bistable::evaluate()
    {
    if ( G_PAC_INFO()->is_emulator() ) return 0;

    int o = get_DI( DI_INDEX_OPEN );
    int c = get_DI( DI_INDEX_CLOSE );

    unsigned int t = (unsigned int)get_par( valve::P_ON_TIME, 0 );
    if ( get_DO( DO_INDEX_OPEN ) == 1 )
        {
        if ( o == 1 ||
            get_delta_millisec( start_switch_time ) > t )
            {
            set_DO( DO_INDEX_OPEN, 0 );
            }
        }
    if ( get_DO( DO_INDEX_CLOSE ) == 1 )
        {
        if ( c == 1 ||
            get_delta_millisec( start_switch_time ) > t )
            {
            set_DO( DO_INDEX_CLOSE, 0 );
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void valve_DO2_DI2_bistable::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    int o = get_DI( DI_INDEX_OPEN );

    if ( 0 == o && get_DO( DO_INDEX_OPEN ) == 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_OPEN, 1 );
        set_DO( DO_INDEX_CLOSE, 0 );

        is_opening = true;
        is_closing = false;
        }
    }
//-----------------------------------------------------------------------------
void valve_DO2_DI2_bistable::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    int c = get_DI( DI_INDEX_CLOSE );
    if ( 0 == c && get_DO( DO_INDEX_CLOSE ) == 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_OPEN, 0 );
        set_DO( DO_INDEX_CLOSE, 1 );

        is_opening = false;
        is_closing = true;
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_DO2_DI2_bistable::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o = get_DI( DI_INDEX_OPEN );
    int c = get_DI( DI_INDEX_CLOSE );

    int o_do = get_DO( DO_INDEX_OPEN );
    int c_do = get_DO( DO_INDEX_CLOSE );

    if ( o == 1 ) return V_ON;
    if ( c == 1 ) return V_OFF;

    if ( o_do == 1 ) return V_ON;
    if ( c_do == 1 ) return V_OFF;

    if ( is_stoped ) return V_STOP;

    return V_OFF;
    }
//-----------------------------------------------------------------------------
bool valve_DO2_DI2_bistable::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( is_stoped ) return true;

    int i0 = get_DI( DI_INDEX_OPEN );
    int i1 = get_DI( DI_INDEX_CLOSE );
    unsigned int t = (unsigned int)get_par( valve::P_ON_TIME, 0 );
    unsigned int dt = get_delta_millisec( start_switch_time );

    if ( i0 == 1 && i1 == 1 )
        {
        return false;
        }
    //Заклинило в закрытом положении.
    if ( is_opening && dt > t && i1 == 1 ) return false;

    //Заклинило в открытом положении.
    if ( is_closing && dt > t && i0 == 1 ) return false;

    if ( i0 == 1 || i1 == 1 ) return true;

    if ( dt < t ) return true;

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_DO2_DI2_bistable::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( DI_INDEX_CLOSE );
    }
//-----------------------------------------------------------------------------
bool valve_DO2_DI2_bistable::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( DI_INDEX_OPEN );
    }
//-----------------------------------------------------------------------------
void valve_DO2_DI2_bistable::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_set_state( new_state );

    int i0 = get_DI( DI_INDEX_OPEN );
    int i1 = get_DI( DI_INDEX_CLOSE );
    switch ( new_state )
        {
        case V_STOP:
            is_opening = false;
            is_closing = false;

            //Если клапан полностью открыт\закрыт ничего не делаем.
            if ( i0 == 1 || i1 == 1 )
                {
                return;
                }

            is_stoped = true;
            if ( get_DO( DO_INDEX_OPEN ) == 1 )
                {
                set_DO( DO_INDEX_OPEN, 0 );
                }

            if ( get_DO( DO_INDEX_CLOSE ) == 1 )
                {
                set_DO( DO_INDEX_CLOSE, 0 );
                }
            break;

        default:
            is_stoped = false;
            valve::direct_set_state( new_state );
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_mix_proof::valve_mix_proof( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, DST_V_MIXPROOF )
    {
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_upper_seat()
    {
    direct_set_state( (int)VALVE_STATE::V_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_lower_seat()
    {
    direct_set_state( (int)VALVE_STATE::V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_set_state( new_state );

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            {
            direct_off();
            if ( auto u = get_DO( DO_INDEX_U ); u == 0 )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_U, 1 );
                }

            break;
            }

        case V_LOWER_SEAT:
            {
            direct_off();
            if ( auto l = get_DO( DO_INDEX_L ); l == 0 )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_L, 1 );
                }
            break;
            }

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );

    if ( auto o = get_DO( DO_INDEX ); 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    VALVE_STATE st = get_valve_state();
    bool was_seat = st == V_LOWER_SEAT || st == V_UPPER_SEAT;

    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );

    if ( auto o = get_DO( DO_INDEX ); o != 0 || was_seat )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_mix_proof::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o = get_DO( DO_INDEX );

    if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return V_UPPER_SEAT;
    if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return V_LOWER_SEAT;

    return (VALVE_STATE)o;
    }
//-----------------------------------------------------------------------------
bool valve_mix_proof::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    int o = get_DO( DO_INDEX );
    if ( auto i1 = get_DI( DI_INDEX_OPEN ), i0 = get_DI( DI_INDEX_CLOSE );
        ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        return true;
        }

    if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return true;
    if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return true;

    if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_mix_proof::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( DI_INDEX_CLOSE );
    }
//-----------------------------------------------------------------------------
bool valve_mix_proof::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( DI_INDEX_OPEN );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_bottom_mix_proof::valve_bottom_mix_proof( const char* dev_name ) : valve(
    true, true, dev_name, DT_V, DST_V_BOTTOM_MIXPROOF ),
    is_closing_mini( 0 ),
    start_off_time( 0 )
    {
    }
//-----------------------------------------------------------------------------
/// @brief Открыть верхнее седло. Не делаем ничего, так как верхнего
/// седла нет.
void valve_bottom_mix_proof::open_upper_seat()
    {
    }
//-----------------------------------------------------------------------------
/// @brief Открыть нижнее седло.
void valve_bottom_mix_proof::open_lower_seat()
    {
    direct_set_state( V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
void valve_bottom_mix_proof::set_mini_closing_state( bool state )
    {
    is_closing_mini = state;
    start_off_time = get_millisec();
    }
#endif
//-----------------------------------------------------------------------------
void valve_bottom_mix_proof::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_set_state( new_state );

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT: //Открываем микроклапан
            direct_off();
            is_closing_mini = 0;

            if ( 0 == get_DO( DO_INDEX_MINI_V ) )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_MINI_V, 1 );
                }
            break;

        case V_LOWER_SEAT:
            direct_off();

            if ( 0 == get_DO( DO_INDEX_L ) )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_L, 1 );
                }
            break;


        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
/// @brief Определение завершения отключения клапана с задержкой.
bool valve_bottom_mix_proof::is_switching_off_finished()
    {
    //Если сняли флаг закрытия, то удаляем из вектора
    if ( !is_closing_mini )
        {
        return true;
        }

    //Если завершилось время задержки, выключаем мини клапан перед удалением
    //клапана из вектора.
    if ( auto delay = G_PAC_INFO()->par[ PAC_info::P_V_BOTTOM_OFF_DELAY_TIME ];
        get_delta_millisec( start_off_time ) > delay )
        {
        is_closing_mini = 0;
        direct_off();
        return true;
        }

    return false;
    };
//-----------------------------------------------------------------------------
void valve_bottom_mix_proof::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    set_DO( DO_INDEX_L, 0 );

    if ( auto o = get_DO( DO_INDEX ); o == 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        set_DO( DO_INDEX_MINI_V, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_bottom_mix_proof::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    VALVE_STATE st = get_valve_state();

    if ( st == V_LOWER_SEAT )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_L, 0 );
        }

    if ( 1 == get_DO( DO_INDEX ) )
        {
        start_switch_time = get_millisec();
        start_off_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        is_closing_mini = 1;
        }

    if ( 1 == get_DO( DO_INDEX_MINI_V ) && !is_closing_mini )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_MINI_V, 0 );
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_bottom_mix_proof::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o = get_DO( DO_INDEX );

    if ( o == 0 && get_DO( DO_INDEX_MINI_V ) == 1 ) return V_UPPER_SEAT;

    if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return V_LOWER_SEAT;

    return (VALVE_STATE)o;
    }
//-----------------------------------------------------------------------------
bool valve_bottom_mix_proof::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    int o = get_DO( DO_INDEX );

    if ( auto i0 = get_DI( DI_INDEX_CLOSE ), i1 = get_DI( DI_INDEX_OPEN );
        ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        return true;
        }

    if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return true;

    if ( get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_bottom_mix_proof::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( DI_INDEX_CLOSE );
    }
//-----------------------------------------------------------------------------
bool valve_bottom_mix_proof::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( DI_INDEX_OPEN );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char* io_link_valve::get_error_description( int err_id ) const
    {
    switch ( err_id )
        {
        case -116:
            return "не обнаружен магнитный индикатор на штоке клапана (#16)";
        case -117:
            return "конфигурация не соответствует требованиям автоматической "
                "настройки (#17)";
        case -118:
            return "ошибка в пневматических соединениях - проверьте подключение "
                "трубок или соленоидов (#18)";
        case -119:
            return "нет сигнала от датчика верхнего седла (#19)";
        case -120:
            return "клапан не достиг заданного положения в установленное время "
                "(#20)";
        case -121:
            return "обнаружен самопроизвольный ход штока (#21)";
        case -122:
            return "не подключен датчик верхнего седла (#22)";
        case -123:
            return "не обнаружен соленоидный клапан 1 (#23)";
        case -124:
            return "не обнаружен соленоидный клапан 2 (#24)";
        case -125:
            return "не обнаружен соленоидный клапан 3 (#25)";
        case -126:
            return "активировано несколько входных сигналов соленоидных клапанов "
                "(#26)";
        case -127:
            return "обнаружено короткое замыкание на цифровых выходах (#27)";
        case -128:
            return "процесс настройки был прерван (#28)";
        case -129:
            return "постоянное срабатывание кнопки - проверьте кнопки или "
                "замените плату управления (#29)";
        case -130:
            return "потеряна связь с системой управления (#30)";
        case -131:
            return "сработала аварийная остановка - превышен допустимый "
                "ход штока (#31)";

        default:
            static thread_local std::string buf;
            buf = fmt::format( "неизвестная ошибка (#{})", err_id );
            return buf.c_str();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char* alfalaval_iol_valve::get_error_description( int error_id ) const
    {
    if ( error_id == -io_device::IOLINKSTATE::NOTCONNECTED ||
        error_id == -io_device::IOLINKSTATE::DEVICEERROR )
        {
        return iol_dev.get_error_description( error_id );
        }

    if ( error_id >= io_link_valve::ERROR_ID_LAST &&
        error_id <= io_link_valve::ERROR_ID_FIRST )
        {
        return iol_valve.get_error_description( error_id );
        }

    return nullptr;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_mix_proof::valve_iolink_mix_proof( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, V_IOLINK_MIXPROOF )
    {
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::open_upper_seat()
    {
    direct_set_state( (int)VALVE_STATE::V_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::open_lower_seat()
    {
    direct_set_state( (int)VALVE_STATE::V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_mix_proof::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_valve_state();
        }

    if ( in_info.de_en ) return V_OFF;
    if ( in_info.main ) return V_ON;
    if ( in_info.usl ) return V_UPPER_SEAT;
    if ( in_info.lsp ) return V_LOWER_SEAT;

    return V_OFF;
    }

//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::evaluate_io()
    {
    out_info = (alfalaval_iol_valve_out_data_swapped*)get_AO_write_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    if ( extra_offset < 0 )
        {
        out_info = (alfalaval_iol_valve_out_data_swapped*)
            ( (char*)out_info + extra_offset );
        }

    auto data = (const char*)get_AI_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    auto buff = (char*)&in_info;

    if ( !data ) return;

    const int SIZE = 4;
    std::copy( data, data + SIZE, buff );
    //Reverse byte order to get correct int16.
    std::swap( buff[ 0 ], buff[ 1 ] );
    //Reverse byte order to get correct int16.
    std::swap( buff[ 2 ], buff[ 3 ] );

#ifdef DEBUG_IOLINK_MIXPROOF
    char* tmp = (char*)in_info;

    sprintf( G_LOG->msg, "%x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg,
        "de_en %u, main %u, usl %u, lsp %u, pos %.1f\n",
        in_info->de_en, in_info->main, in_info->usl,
        in_info->lsp, 0.1 * in_info->pos );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            extra_offset = (int)value;
            break;

        default:
            valve::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::save_device_ex( char* buff ) const
    {
    int res = valve::save_device_ex( buff );

    bool cs = out_info->sv1 || out_info->sv2 || out_info->sv3;
    int err = in_info.err;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "BLINK={:d}, CS={:d}, ERR={}, V={:.1f}, ",
        blink, cs, err, get_value() ) ).size;

    return res;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_mix_proof::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) ) !=
        io_device::IOLINKSTATE::OK )
        {
        return false;
        }

    if ( in_info.err ) return false;

    if ( get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    if ( out_info->sv1 == false && in_info.de_en && in_info.st ) return true;

    if ( out_info->sv1 == true && in_info.main && in_info.st ) return true;

    if ( out_info->sv2 == true && in_info.usl && in_info.st ) return true;

    if ( out_info->sv3 == true && in_info.lsp && in_info.st ) return true;

    return false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_state();

    auto feed_back_state = static_cast<valve::FB_STATE>(
        get_par( P_FB, 0 ) );
    if ( auto error_id =
        get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
        error_id != io_device::IOLINKSTATE::OK &&
        feed_back_state != FB_IS_AND_OFF )
        {
        return -error_id;
        }

    if ( in_info.err > 0 && feed_back_state != FB_IS_AND_OFF )
        {
        return -( io_link_valve::ERROR_CODE_OFFSET + in_info.err );
        }

    switch ( get_valve_state() )
        {
        case V_LOWER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_LOWER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( feed_back_state == FB_IS_AND_OFF )
                {
                return VX_LOWER_SEAT_FB_OFF;
                }

            if ( get_fb_state() ) return VX_LOWER_SEAT;

            return VX_OFF_FB_ERR;

        case V_UPPER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_UPPER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( feed_back_state == FB_IS_AND_OFF )
                {
                return VX_UPPER_SEAT_FB_OFF;
                }

            if ( get_fb_state() ) return VX_UPPER_SEAT;

            return VX_OFF_FB_ERR;
        default:
            break;
        }

    return valve::get_state();
    }
//-----------------------------------------------------------------------------
float valve_iolink_mix_proof::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_value();

    return 0.1f * in_info.pos;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_mix_proof::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return out_info->sv1 == false && in_info.main && in_info.st;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_mix_proof::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return out_info->sv1 == true && in_info.de_en && in_info.st;
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    if ( false == in_info.main )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = true;
    out_info->sv2 = false;
    out_info->sv3 = false;
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    if ( out_info->sv1 || out_info->sv2 || out_info->sv3 )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = false;
    out_info->sv2 = false;
    out_info->sv3 = false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug(
            "%s\t valve_iolink_mix_proof::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        }

    switch ( prop[ 0 ] )
        {
        case 'B': //BLINK
            {
            val > 0 ? out_info->wink = true : out_info->wink = false;
            blink = out_info->wink;
            break;
            }

        default:
            valve::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_set_state( new_state );

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            {
            direct_off();
            if ( !out_info->sv2 )
                {
                start_switch_time = get_millisec();
                }
            out_info->sv2 = true;
            break;
            }

        case V_LOWER_SEAT:
            {
            direct_off();
            if ( !out_info->sv3 )
                {
                start_switch_time = get_millisec();
                }
            out_info->sv3 = true;
            break;
            }

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
const char* valve_iolink_mix_proof::get_error_description()
    {
    auto error_id = get_error_id();

    if ( auto res = alfalaval_iol_v.get_error_description( error_id ); res )
        {
        return res;
        }

    return valve::get_error_description();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const std::string valve_iolink_shut_off_sorio::SORIO_ARTICLE = "DEF.SORIO-1SV";

valve_iolink_shut_off_sorio::out_data_swapped
valve_iolink_shut_off_sorio::stub_out_info;
//-----------------------------------------------------------------------------
valve_iolink_shut_off_sorio::valve_iolink_shut_off_sorio( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, V_IOLINK_DO1_DI2 )
    {
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_shut_off_sorio::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_valve_state();
        }

    if ( in_info.de_en ) return V_OFF;
    if ( in_info.main ) return V_ON;

    return V_OFF;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::evaluate_io()
    {
    out_info = (out_data_swapped*)get_AO_write_data( 0 );

    auto data = (char*)get_AI_data( 0 );
    auto buff = (char*)&in_info;

    if ( !data ) return;

    const int SIZE = 4;
    std::copy( data, data + SIZE, buff );
    //Reverse byte order to get correct int16.
    std::swap( buff[ 0 ], buff[ 1 ] );
    //Reverse byte order to get correct int16.
    std::swap( buff[ 2 ], buff[ 3 ] );

#ifdef DEBUG_IOLINK_
    char* tmp = (char*)in_info;

    static bool de_en;
    static bool main;
    static uint16_t status;
    static bool sv1;
    static uint16_t led_state;
    static uint16_t pos;

    if ( de_en != in_info->de_en || main != in_info->main ||
        status != in_info->status || sv1 != in_info->sv1 || led_state != in_info->led_state ||
        pos != in_info->pos )
        {
        print_binary( *(int*)tmp );
        printf( "\n" );

        sprintf( G_LOG->msg,
            "de_en %u, main %u, status %u, sv1 %u, led_state %u, pos %u\n",
            in_info->de_en, in_info->main, in_info->status,
            in_info->sv1, in_info->led_state, in_info->pos );
        G_LOG->write_log( i_log::P_NOTICE );

        print_binary( *(int*)out_info );
        printf( "\n\n" );

        de_en = in_info->de_en;
        main = in_info->main;
        status = in_info->status;
        sv1 = in_info->sv1;
        led_state = in_info->led_state;
        pos = in_info->pos;
        }
#endif
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_sorio::save_device_ex( char* buff ) const
    {
    bool cs = out_info->sv1;
    int err = in_info.status;
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE,
        "BLINK={:d}, CS={:d}, ERR={}, V={:.1f}, ",
        blink, cs, err, get_value() ) ).size;
    return res;
    }
//-----------------------------------------------------------------------------
float valve_iolink_shut_off_sorio::get_value() const
    {
    return 0.1f * in_info.pos;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        in_info.pos = (int16_t)( new_value * 10 );
        }
    else
        {
        // Do nothing.
        }
    }
//-----------------------------------------------------------------------------
bool valve_iolink_shut_off_sorio::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( get_AI_IOLINK_state( 0 ) != io_device::IOLINKSTATE::OK )
        {
        return false;
        }

    if ( in_info.status ) return false;

    if ( auto dt = get_delta_millisec( start_switch_time );
        dt < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    if ( !out_info->sv1 && in_info.de_en ) return true;
    if ( out_info->sv1 && in_info.main ) return true;

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_shut_off_sorio::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return !in_info.sv1;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_shut_off_sorio::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return in_info.sv1;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    if ( false == in_info.main )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = true;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    if ( out_info->sv1 )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_sorio::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug(
            "%s\t valve_iolink_mix_proof::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        }

    switch ( prop[ 0 ] )
        {
        case 'B': //BLINK
            {
            val > 0 ? out_info->wink = true : out_info->wink = false;
            blink = out_info->wink;
            break;
            }

        default:
            valve::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_set_state( new_state );

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_gea_tvis_a15::valve_iolink_gea_tvis_a15( const char* dev_name,
    device::DEVICE_SUB_TYPE device_sub_type ) :
    valve( true, true, dev_name, DT_V, device_sub_type ), out_info( nullptr )
    {
    }
//-----------------------------------------------------------------------------
int valve_iolink_gea_tvis_a15::save_device_ex( char* buff ) const
    {
    int cs = in_info.pv_y1_on || in_info.pv_y2_on || in_info.pv_y3_on;
    int err = in_info.error_on;
    int sup = in_info.SUP;

    int res = valve::save_device_ex( buff );
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "CS={}, SUP={}, ERR={}, V={:.1f}, ", cs, sup, err, get_value() ).size;
    return res;
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15::evaluate_io()
    {
    auto data = get_AO_write_data( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    if ( !data ) return;

    out_info = (out_data_swapped*)data;
    if ( extra_offset < 0 )
        {
        out_info = (out_data_swapped*)( (char*)data + extra_offset );
        }

    auto input_data = (char*)get_AI_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    auto buffer = (char*)&in_info;

    const int IN_SIZE = 4;
    std::copy( input_data, input_data + IN_SIZE, buffer );
    std::swap( buffer[ 2 ], buffer[ 3 ] );

#ifdef DEBUG_IOLINK_
    char* tmp = (char*)in_info;

    sprintf( G_LOG->msg, "%x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg,
        "pv_y1_on %u, pv_y2_on %u, pv_y3_on %u, s1 %u, s2 %u, s3 %u, s4 %u, SUP %u, error_on %u, pos %.1f\n",
        in_info.pv_y1_on, in_info.pv_y2_on, in_info.pv_y3_on, in_info.s1, in_info.s2, in_info.s3, in_info.s4,
        in_info.SUP, in_info.error_on, 0.1 * in_info->pos );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
}
//-----------------------------------------------------------------------------
float valve_iolink_gea_tvis_a15::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_value();
        }

    return 0.1f * in_info.pos;
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::direct_set_value( new_value );
        }
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::direct_off();
        }

    if ( out_info->pv_y1 || out_info->pv_y2 || out_info->pv_y3 )
        start_switch_time = get_millisec();

    out_info->pv_y1 = false;
    out_info->pv_y2 = false;
    out_info->pv_y3 = false;
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::direct_on();
        }

    if ( !in_info.pv_y1_on || !in_info.pv_y2_on || !in_info.pv_y3_on )
        start_switch_time = get_millisec();

    out_info->pv_y1 = true;
    out_info->pv_y2 = false;
    out_info->pv_y3 = false;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_gea_tvis_a15::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_fb_state();
        }

    if ( get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) ) !=
        io_device::IOLINKSTATE::OK )
        return false;

    if ( in_info.error_on ) return false;

    if ( in_info.SUP ) return false;

    if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
        return true;

    if ( auto active_solenoid_count = static_cast<int>( in_info.pv_y1_on ) +
        static_cast<int>( in_info.pv_y2_on ) +
        static_cast<int>( in_info.pv_y3_on );
        active_solenoid_count > 1 )
        {
        return false; // Активно более одного соленоида.
        }

    if ( !in_info.pv_y1_on && !in_info.pv_y2_on && !in_info.pv_y3_on &&
        in_info.s1 && in_info.s4 ) return true;         // Неактивное состояние.
    if ( in_info.pv_y1_on &&
        in_info.s2 ) return true;                       // Активное состяние.
    if ( in_info.pv_y2_on ) return true;                // Нижнее седло.
    if ( in_info.pv_y3_on &&
        in_info.s1 && in_info.s4 ) return true;         // Верхнее седло.

    return false;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_gea_tvis_a15::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_off_fb_value();
        }

    return !in_info.pv_y1_on && !in_info.pv_y2_on && !in_info.pv_y3_on &&
        in_info.s1 && in_info.s4;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_gea_tvis_a15::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_on_fb_value();
        }

    if ( auto active_solenoid_count = static_cast<int>( in_info.pv_y1_on ) +
        static_cast<int>( in_info.pv_y2_on ) +
        static_cast<int>( in_info.pv_y3_on );
        active_solenoid_count > 1 )
        {
        return false; // Активно более одного соленоида.
        }

    return in_info.pv_y1_on && in_info.s2;
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15::set_rt_par( u_int idx, float value )
    {
    if ( 1u == idx )
        extra_offset = static_cast<int>( value );
    else valve::set_rt_par( idx, value );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const std::string valve_iolink_gea_tvis_a15_ss::GEA_TVIS_A15_SINGLE_SEAT_ARTICLE = "GEA.TA15P8IAJ";
//-----------------------------------------------------------------------------
valve_iolink_gea_tvis_a15_ss::valve_iolink_gea_tvis_a15_ss( const char* dev_name )
    : valve_iolink_gea_tvis_a15( dev_name, device::V_IOLINK_DO1_DI2 )
    {
    this->out_info = nullptr;
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_gea_tvis_a15_ss::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_valve_state();
        }

    if ( in_info.pv_y1_on ) return V_ON;
    else return V_OFF;
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15_ss::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::direct_set_state( new_state );
        }

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const std::string valve_iolink_gea_tvis_a15_ds::GEA_TVIS_A15_DOUBLE_SEAT_ARTICLE = "GEA.TA15L8IAJ";

valve_iolink_gea_tvis_a15_ds::valve_iolink_gea_tvis_a15_ds( const char* dev_name )
    : valve_iolink_gea_tvis_a15( dev_name, device::V_IOLINK_MIXPROOF )
    {
    this->out_info = nullptr;
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15_ds::open_upper_seat()
    {
    direct_set_state( (int)VALVE_STATE::V_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15_ds::open_lower_seat()
    {
    direct_set_state( (int)VALVE_STATE::V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_gea_tvis_a15_ds::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_valve_state();
        }

    if (  in_info.pv_y1_on && !in_info.pv_y2_on && !in_info.pv_y3_on ) return V_ON;
    if ( !in_info.pv_y1_on && !in_info.pv_y2_on &&  in_info.pv_y3_on ) return V_UPPER_SEAT;
    if ( !in_info.pv_y1_on &&  in_info.pv_y2_on && !in_info.pv_y3_on ) return V_LOWER_SEAT;
    else return V_OFF;
    }
//-----------------------------------------------------------------------------
void valve_iolink_gea_tvis_a15_ds::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::direct_set_state( new_state );
        }

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            direct_off();
            out_info->pv_y3 = true;
            break;

        case V_LOWER_SEAT:
            direct_off();
            out_info->pv_y2 = true;
            break;

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_shut_off_thinktop::valve_iolink_shut_off_thinktop( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, V_IOLINK_DO1_DI2 )
    {
    in_info.err = 0;
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_shut_off_thinktop::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::get_valve_state();
        }

    if ( in_info.de_en ) return V_OFF;
    if ( in_info.main ) return V_ON;

    return V_OFF;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::evaluate_io()
    {
    out_info = (alfalaval_iol_valve_out_data_swapped*)get_AO_write_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    if ( extra_offset < 0 )
        {
        out_info = (alfalaval_iol_valve_out_data_swapped*)
            ( (char*)out_info + extra_offset );
        }

    auto data = (char*)get_AI_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    auto buff = (char*)&in_info;

    if ( !data ) return;

    const int SIZE = 4;
    std::copy( data, data + SIZE, buff );
    //Reverse byte order to get correct int16.
    std::swap( buff[ 0 ], buff[ 1 ] );
    //Reverse byte order to get correct int16.
    std::swap( buff[ 2 ], buff[ 3 ] );

#ifdef DEBUG_IOLINK_
    char* tmp = (char*)&in_info;

    sprintf( G_LOG->msg, "%x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg,
        "de_en %u, main %u, usl %u, lsp %u, pos %.1f\n",
        in_info.de_en, in_info.main, in_info.usl,
        in_info.lsp, 0.1 * in_info.pos );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            extra_offset = (int)value;
            break;

        default:
            valve::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_thinktop::save_device_ex( char* buff ) const
    {
    bool cs = out_info->sv1;
    int err = in_info.err;
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE,
        "BLINK={:d}, CS={:d}, ERR={}, V={:.1f}, ",
        blink, cs, err, get_value() ) ).size;
    return res;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_shut_off_thinktop::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) ) !=
        io_device::IOLINKSTATE::OK )
        {
        return false;
        }

    if ( in_info.err ) return false;

    if ( get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    if ( !out_info->sv1 && in_info.de_en && in_info.st ) return true;
    if ( out_info->sv1 && in_info.main && in_info.st ) return true;

    return false;
    }
//-----------------------------------------------------------------------------
float valve_iolink_shut_off_thinktop::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_value();

    return 0.1f * in_info.pos;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_shut_off_thinktop::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return !out_info->sv1 && in_info.de_en && in_info.st;
    }
//-----------------------------------------------------------------------------
bool valve_iolink_shut_off_thinktop::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return out_info->sv1 && in_info.main && in_info.st;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    if ( false == in_info.main )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = true;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    if ( out_info->sv1 )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_thinktop::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug(
            "%s\t valve_iolink_mix_proof::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        }

    switch ( prop[ 0 ] )
        {
        case 'B': //BLINK
            {
            val > 0 ? out_info->wink = true : out_info->wink = false;
            blink = out_info->wink;
            break;
            }

        default:
            valve::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_set_state( new_state );

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
const char* valve_iolink_shut_off_thinktop::get_error_description()
    {
    auto error_id = get_error_id();

    if ( auto res = alfalaval_iol_v.get_error_description( error_id ); res )
        {
        return res;
        }

    return valve::get_error_description();
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_thinktop::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_state();

    auto feed_back_state = static_cast<valve::FB_STATE>(
        get_par( P_FB, 0 ) );
    if ( auto error_id =
        get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
        error_id != io_device::IOLINKSTATE::OK &&
        feed_back_state != FB_IS_AND_OFF )
        {
        return -error_id;
        }

    if ( in_info.err > 0 && feed_back_state != FB_IS_AND_OFF )
        {
        return -( io_link_valve::ERROR_CODE_OFFSET + in_info.err );
        }

    return valve::get_state();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iol_terminal::valve_iol_terminal( bool is_on_fb, bool is_off_fb,
    const char* dev_name, device::DEVICE_SUB_TYPE sub_type, u_int terminal_size ) :
    valve( is_on_fb, is_off_fb, dev_name, DT_V, sub_type )
    {
    terminal_id.resize( terminal_size );
    }

valve_iol_terminal::valve_iol_terminal( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type, u_int terminal_size ) :
    valve( dev_name, DT_V, sub_type )
    {
    terminal_id.resize( terminal_size );
    }

void valve_iol_terminal::set_rt_par( u_int idx, float value )
    {
    idx -= 1;
    if ( idx < terminal_id.size() )
        {
        terminal_id[ idx ] = static_cast<u_int>( value );
        }
    else
        {
        valve::set_rt_par( idx, value );
        }
    }

bool valve_iol_terminal::check_config() const
    {
    auto idx = 0;
    return std::all_of( std::begin( terminal_id ), std::end( terminal_id ),
        [&]( const unsigned int& id )
        {
        if ( const auto data = get_AO_write_data( static_cast<u_int>
            ( IO_CONSTANT::AO_INDEX_1 ) + idx++ );
            !data )
            {
            return false;
            }

        return id > 0;
        } );
    }

/// @brief Установка данных состояния устройства.
void valve_iol_terminal::set_state_bit( char* data,
    unsigned int n ) const
    {
    auto offset = ( n - 1 ) / 8;
    data[ offset ] |= 1 << ( ( n - 1 ) % 8 );
    }

void valve_iol_terminal::reset_state_bit( char* data,
    unsigned int n ) const
    {
    auto offset = ( n - 1 ) / 8;
    data[ offset ] &= ~( 1 << ( ( n - 1 ) % 8 ) );
    }

unsigned int valve_iol_terminal::get_terminal_id(
    valve_iol_terminal::TERMINAL_OUTPUT n ) const
    {
    return terminal_id[ static_cast<unsigned int>( n ) - 1 ];
    }

int valve_iol_terminal::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return get_valve_state();

    IOLINKSTATE res = get_AO_IOLINK_state(
        static_cast<u_int>( IO_CONSTANT::AO_INDEX_1 ) );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -static_cast<int>( res );
        }
    else
        {
        return valve::get_state();
        }
    }

void valve_iol_terminal::direct_set_state( int new_state )
    {
    if ( new_state )
        {
        direct_on();
        }
    else direct_off();
    };

valve::VALVE_STATE valve_iol_terminal::get_valve_state() const
    {
    return state;
    }

void valve_iol_terminal::direct_on()
    {
    if ( !check_config() ) return;

    if ( auto st = get_valve_state();
        valve::VALVE_STATE::V_ON != st )
        {
        start_switch_time = get_millisec();
        }

    auto data = (char*)( get_AO_write_data(
        static_cast<u_int> ( IO_CONSTANT::AO_INDEX_1 ) ) );
    set_state_bit( data, get_terminal_id( TERMINAL_OUTPUT::ON ) );
    for ( u_int i = 1; i < static_cast<u_int>( terminal_id.size() ); i++ )
        {
        data = (char*)( get_AO_write_data(
            static_cast<u_int> ( IO_CONSTANT::AO_INDEX_1 ) + i ) );
        reset_state_bit( data, get_terminal_id( static_cast<TERMINAL_OUTPUT>(
            static_cast<size_t>( TERMINAL_OUTPUT::ON ) + i ) ) );
        }
    state = valve::VALVE_STATE::V_ON;
    }

void valve_iol_terminal::direct_off()
    {
    if ( !check_config() ) return;

    if ( auto st = get_valve_state();
        valve::VALVE_STATE::V_OFF != st )
        {
        start_switch_time = get_millisec();
        }

    for ( size_t i = 0; i < terminal_id.size(); i++ )
        {
        auto data = (char*)( get_AO_write_data(
            static_cast<u_int> ( IO_CONSTANT::AO_INDEX_1 ) + i ) );
        reset_state_bit( data, get_terminal_id( static_cast<TERMINAL_OUTPUT>(
            static_cast<size_t>( TERMINAL_OUTPUT::ON ) + i ) ) );
        }
    state = valve::VALVE_STATE::V_OFF;
    }
//-----------------------------------------------------------------------------
void valve_iol_terminal::set_st( VALVE_STATE new_state )
    {
    state = new_state;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iol_terminal_DO1::valve_iol_terminal_DO1( const char* dev_name ) :
    valve_iol_terminal( dev_name, device::DEVICE_SUB_TYPE::V_IOLINK_VTUG_DO1 )
    {
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iol_terminal_DO1_DI1_on::valve_iol_terminal_DO1_DI1_on( const char* dev_name ) :
    valve_iol_terminal( true, false, dev_name, V_IOLINK_VTUG_DO1_FB_ON )
    {
    }

bool valve_iol_terminal_DO1_DI1_on::get_fb_state() const
    {
    if ( !check_config() ) return false;

    int i = get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );

    if ( auto o = get_valve_state(); o == i )
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) <
        static_cast<u_long>( get_par( valve::P_ON_TIME, 0 ) ) )
        {
        return true;
        }

    return false;
    }

bool valve_iol_terminal_DO1_DI1_on::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );
    }

inline bool valve_iol_terminal_DO1_DI1_on::get_off_fb_value() const
    {
    return false;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iol_terminal_DO1_DI1_off::valve_iol_terminal_DO1_DI1_off( const char* dev_name ) :
    valve_iol_terminal( false, true, dev_name, V_IOLINK_VTUG_DO1_FB_OFF )
    {
    }

bool valve_iol_terminal_DO1_DI1_off::get_fb_state() const
    {
    if ( !check_config() ) return false;

    int i = get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );

    if ( auto o = get_valve_state(); o != i )
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) <
        static_cast<u_long>( get_par( valve::P_ON_TIME, 0 ) ) )
        {
        return true;
        }

    return false;
    }

bool valve_iol_terminal_DO1_DI1_off::get_on_fb_value() const
    {
    return false;
    }

inline bool valve_iol_terminal_DO1_DI1_off::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iol_terminal_mixproof_DO3::valve_iol_terminal_mixproof_DO3( const char* dev_name,
    bool is_on_fb, bool is_off_fb, device::DEVICE_SUB_TYPE sub_type ) :
    valve_iol_terminal( is_on_fb, is_off_fb, dev_name, sub_type, 3 )
    {
    }

valve_iol_terminal_mixproof_DO3::valve_iol_terminal_mixproof_DO3( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type ) : valve_iol_terminal( dev_name, sub_type, 3 )
    {
    }

void valve_iol_terminal_mixproof_DO3::open_upper_seat()
    {
    if ( !check_config() ) return;

    auto idx = static_cast<u_int> ( IO_CONSTANT::AO_INDEX_1 );
    auto data = (char*)get_AO_write_data( idx );
    reset_state_bit( data, get_terminal_id( TERMINAL_OUTPUT::ON ) );
    idx = static_cast<u_int> ( IO_CONSTANT::AO_INDEX_2 );
    data = (char*)get_AO_write_data( idx );
    set_state_bit( data, get_terminal_id( TERMINAL_OUTPUT::UPPER_SEAT ) );
    idx = static_cast<u_int> ( IO_CONSTANT::AO_INDEX_3 );
    data = (char*)get_AO_write_data( idx );
    reset_state_bit( data, get_terminal_id( TERMINAL_OUTPUT::LOWER_SEAT ) );

    set_st( V_UPPER_SEAT );
    }

void valve_iol_terminal_mixproof_DO3::open_lower_seat()
    {
    if ( !check_config() ) return;

    auto idx = static_cast<u_int> ( IO_CONSTANT::AO_INDEX_1 );
    auto data = (char*)get_AO_write_data( idx );
    reset_state_bit( data, get_terminal_id( TERMINAL_OUTPUT::ON ) );
    idx = static_cast<u_int> ( IO_CONSTANT::AO_INDEX_2 );
    data = (char*)get_AO_write_data( idx );
    reset_state_bit( data, get_terminal_id( TERMINAL_OUTPUT::UPPER_SEAT ) );
    idx = static_cast<u_int> ( IO_CONSTANT::AO_INDEX_3 );
    data = (char*)get_AO_write_data( idx );
    set_state_bit( data, get_terminal_id( TERMINAL_OUTPUT::LOWER_SEAT ) );

    set_st( V_LOWER_SEAT );
    }

void valve_iol_terminal_mixproof_DO3::direct_set_state( int new_state )
    {
    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            {
            open_upper_seat();
            break;
            }

        case V_LOWER_SEAT:
            {
            open_lower_seat();
            break;
            }

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_valve_ey::analog_valve_ey( const char* dev_name ) :device( dev_name,
    device::DEVICE_TYPE::DT_VC,
    device::DEVICE_SUB_TYPE::DST_VC_EY, 0 )
    {};
//-----------------------------------------------------------------------------
void analog_valve_ey::set_property( const char* field, device* dev )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t analog_valve_ey::set_property() - "
            "field = \"%s\", val = \"%s\"",
            get_name(), field, dev ? dev->get_name() : "nullptr" );
        }

    if ( strcmp( field, "TERMINAL" ) == 0 )
        {
        conv = dynamic_cast<converter_iolink_ao*>( dev );
        if ( conv )
            {
            conv->evaluate_io();
            conv->direct_off();
            }
        }
    else
        {
        G_LOG->alert( "%s\t analog_valve_ey::set_property() - "
            "Unknown field \"%s\"", get_name(), field );
        }
    };
//-----------------------------------------------------------------------------
void analog_valve_ey::set_string_property( const char* field,
    const char* new_value )
    {
    if ( !field ) return;

    device::set_string_property( field, new_value );
    switch ( field[ 0 ] )
        {
        //TERMINAL
        case 'T':
            conv = dynamic_cast<converter_iolink_ao*>(
                G_DEVICE_MANAGER()->get_device( new_value ) );
            if ( conv )
                {
                conv->evaluate_io();
                conv->direct_off();
                }
            break;

        default:
            break;
        }
    }
//-----------------------------------------------------------------------------
void analog_valve_ey::direct_on()
    {
    direct_set_value( static_cast<float>( CONSTANTS::FULL_OPENED ) );
    }
//-----------------------------------------------------------------------------
void analog_valve_ey::direct_off()
    {
    direct_set_value( static_cast<float>( CONSTANTS::FULL_CLOSED ) );
    }
//-----------------------------------------------------------------------------
void analog_valve_ey::direct_set_value( float new_value )
    {
    if ( !conv ) return;

    conv->set_channel_value( ey_number, new_value );
    }
//-----------------------------------------------------------------------------
float analog_valve_ey::get_value() const
    {
    if ( !conv ) return 0.0f;

    return conv->get_channel_value( ey_number );
    }
//-----------------------------------------------------------------------------
int analog_valve_ey::get_state() const
    {
    if ( !conv ) return -200;
    else return conv->get_state();
    }
//-----------------------------------------------------------------------------
void analog_valve_ey::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            if ( value < 1.0f || value > 2.0f )
                {
                G_LOG->error( "%s\t analog_valve_ey::set_rt_par() - "
                    "ey_number = %f", get_name(), value );
                }
            else
                {
                ey_number = static_cast<int>( value );
                }
            break;

        default:
            device::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_valve_iolink::analog_valve_iolink( const char* dev_name ) : AO1(
    dev_name, DT_VC, DST_VC_IOLINK,
    static_cast<int>( PAR_CONSTANTS::ADDITIONAL_PARAMS_COUNT ) - 1 )
    {
    in_info.closed = true;
    in_info.opened = false;

    set_par_name( static_cast<int>( PAR_CONSTANTS::P_FB ), 0, "P_FB" );
    }
//-----------------------------------------------------------------------------
void analog_valve_iolink::evaluate_io()
    {
    out_info = (out_data*)get_AO_write_data( AO_INDEX );

    auto data = (const char*)get_AI_data( AO_INDEX );
    auto buff = (char*)&in_info;

    if ( !data ) return;

    const int SIZE = 10;
    std::copy( data, data + SIZE, buff );

#ifdef DEBUG_VC_IOLINK
    //Variants of float representation.
    int a1 = ( buff[ 0 ] << 24 ) + ( buff[ 1 ] << 16 ) + ( buff[ 2 ] << 8 ) + buff[ 3 ];
    int a2 = ( buff[ 1 ] << 24 ) + ( buff[ 0 ] << 16 ) + ( buff[ 3 ] << 8 ) + buff[ 2 ];
    int a3 = ( buff[ 2 ] << 24 ) + ( buff[ 3 ] << 16 ) + ( buff[ 1 ] << 8 ) + buff[ 0 ];
    int a4 = ( buff[ 3 ] << 24 ) + ( buff[ 2 ] << 16 ) + ( buff[ 0 ] << 8 ) + buff[ 1 ];
    float* f1 = (float*)&a1;
    float* f2 = (float*)&a2;
    float* f3 = (float*)&a3;
    float* f4 = (float*)&a4;
    sprintf( G_LOG->msg, "WARNING %s %f %f %f %f", get_name(),
        *f1, *f2, *f3, *f4 );
    G_LOG->write_log( i_log::P_WARNING );
#endif

    //Reverse byte order to get correct float.
    std::swap( buff[ 3 ], buff[ 0 ] );
    std::swap( buff[ 1 ], buff[ 2 ] );
    //Reverse byte order to get correct float.
    std::swap( buff[ 7 ], buff[ 4 ] );
    std::swap( buff[ 5 ], buff[ 6 ] );

#ifdef DEBUG_VC_IOLINK
    sprintf( G_LOG->msg,
        "closed %u, opened %u, status %u, NAMUR state %u, used setpoint %.3f, valve position %.3f\n",
        in_info->closed, in_info->opened, in_info->status, in_info->namur_state,
        in_info->setpoint, in_info->position );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
    }
//-----------------------------------------------------------------------------
float analog_valve_iolink::get_min_value() const
    {
    return static_cast<float>( CONSTANTS::FULL_CLOSED );
    }
//-----------------------------------------------------------------------------
float analog_valve_iolink::get_max_value() const
    {
    return static_cast<float>( CONSTANTS::FULL_OPENED );
    }
//-----------------------------------------------------------------------------
int analog_valve_iolink::save_device_ex( char* buff ) const
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE,
        "NAMUR_ST={}, OPENED={:d}, CLOSED={:d}, BLINK={:d}, ",
        +in_info.namur_state, +in_info.opened, +in_info.closed, blink ) ).size;
    return res;
    }
//-----------------------------------------------------------------------------
void analog_valve_iolink::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return AO1::direct_on();

    direct_set_value( static_cast<float>( CONSTANTS::FULL_OPENED ) );
    }
//-----------------------------------------------------------------------------
void analog_valve_iolink::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return AO1::direct_off();

    direct_set_value( static_cast<float>( CONSTANTS::FULL_CLOSED ) );
    }
//-----------------------------------------------------------------------------
void analog_valve_iolink::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() ) return AO1::direct_set_value( new_value );

    out_info->position = new_value;

    char* buff = (char*)&out_info->position;
    //Reverse byte order to get correct float.
    std::swap( buff[ 3 ], buff[ 0 ] );
    std::swap( buff[ 1 ], buff[ 2 ] );
    }
//-----------------------------------------------------------------------------
float analog_valve_iolink::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return AO1::get_value();

    return in_info.position;
    }
//-----------------------------------------------------------------------------
int analog_valve_iolink::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    if ( auto error_id =
        get_AI_IOLINK_state( static_cast<u_int>( AO1::AO_INDEX ) );
        error_id != io_device::IOLINKSTATE::OK )
        {
        // Проверяем параметр P_FB для возможности отключения ошибок устройства.
        if ( get_par( static_cast<int>(
            analog_valve_iolink::PAR_CONSTANTS::P_FB ), 0 ) == 0 )
            {
            return 1; // Обратная связь отключена - нет ошибок.
            }

        return -error_id;
        }

    return in_info.status;
    }
//-----------------------------------------------------------------------------
const char* analog_valve_iolink::get_error_description()
    {
    return iol_device.get_error_description( get_error_id() );
    }
//-----------------------------------------------------------------------------
inline int analog_valve_iolink::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug(
            "%s\t analog_valve_iolink::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        }

    switch ( prop[ 0 ] )
        {
        case 'B': //BLINK
            {
            val > 0 ? out_info->wink = true : out_info->wink = false;
            blink = out_info->wink;
            break;
            }

        default:
            AO1::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iol_terminal_DO1_DI2::valve_iol_terminal_DO1_DI2( const char* dev_name ) :
    valve_iol_terminal( true, true, dev_name, V_IOLINK_VTUG_DO1_DI2 )
    {
    }

bool valve_iol_terminal_DO1_DI2::get_fb_state() const
    {
    auto o = get_valve_state();

    int i1 = get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );
    int i2 = get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_2 ) );
    if ( VALVE_STATE::V_ON == o && i1 == 1 && i2 == 0 ) //Открыт.
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( VALVE_STATE::V_OFF == o && i1 == 0 && i2 == 1 ) //Закрыт.
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) <
        static_cast<u_long>( get_par( valve::P_ON_TIME, 0 ) ) )
        {
        return true;
        }

    return false;
    }

bool valve_iol_terminal_DO1_DI2::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );
    }

inline bool valve_iol_terminal_DO1_DI2::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_2 ) );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iol_terminal_mixproof_DO3_DI2::
valve_iol_terminal_mixproof_DO3_DI2( const char* dev_name ) :
    valve_iol_terminal_mixproof_DO3( dev_name, true, true, V_IOL_TERMINAL_MIXPROOF_DO3_DI2 )
    {
    }

/// @brief Получение состояния обратной связи.
bool valve_iol_terminal_mixproof_DO3_DI2::get_fb_state() const
    {
    auto o = get_valve_state();

    int i1 = get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );
    int i2 = get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_2 ) );
    if ( VALVE_STATE::V_ON == o && i1 == 1 && i2 == 0 ) //Открыт.
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( VALVE_STATE::V_OFF == o && i1 == 0 && i2 == 1 ) //Закрыт.
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( VALVE_STATE::V_LOWER_SEAT == o ||
        VALVE_STATE::V_UPPER_SEAT == o ) return true;

    if ( get_delta_millisec( start_switch_time ) <
        static_cast<u_long>( get_par( valve::P_ON_TIME, 0 ) ) )
        {
        return true;
        }

    return false;
    }

bool valve_iol_terminal_mixproof_DO3_DI2::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_1 ) );
    }

bool valve_iol_terminal_mixproof_DO3_DI2::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( static_cast<u_int> ( IO_CONSTANT::DI_INDEX_2 ) );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_valve::analog_valve( const char* dev_name ) :
    AO1( dev_name, DT_VC, DST_VC, 0 )
    {
    }
//-----------------------------------------------------------------------------
float analog_valve::get_min_value() const
    {
    return C_MIN;
    }
//-----------------------------------------------------------------------------
float analog_valve::get_max_value() const
    {
    return C_MAX;
    }
//-----------------------------------------------------------------------------
void analog_valve::direct_on()
    {
    device::direct_on();
    direct_set_value( static_cast<float>( C_MAX ) );
    }
//-----------------------------------------------------------------------------
void analog_valve::direct_off()
    {
    device::direct_off();
    direct_set_value( static_cast<float>( C_MIN ) );
    }
//-----------------------------------------------------------------------------
void analog_valve::direct_set_state( int new_state )
    {
    if ( new_state == 0 )
        {
        direct_off();
        }
    else
        {
        direct_on();
        }
    }
//-----------------------------------------------------------------------------
void analog_valve::direct_set_value( float new_value )
    {
    AO1::direct_set_value( new_value );

    if ( new_value > 0.0f ) device::direct_on();
    else device::direct_off();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS::valve_AS( const char* dev_name, DEVICE_SUB_TYPE sub_type ) :
    valve( true, true, dev_name, DT_V, sub_type ),
    AS_number( 0 )
    {
    }
//-----------------------------------------------------------------------------
void valve_AS::print() const
    {
    printf( "%s [%u]\t", get_name(), AS_number );
    }
//-----------------------------------------------------------------------------
void valve_AS::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            AS_number = (u_int)value;
            break;

        default:
            valve::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
/// @brief Получение данных состояния устройства.
char valve_AS::get_state_data( const char* data ) const
    {
    if ( data == nullptr )
        {
        return 0;
        }

    char state = 0;
    u_int offset = 0;

    if ( AS_number < 32 )		 //Eсли номер < 32).
        {
        offset = MAILBOX_OFFSET + AS_number / 2;
        state = data[ offset ];
        if ( AS_number % 2 == 0 )//Четный номер - старшие четыре бита.
            {
            state >>= 4;
            }
        }
    else
        {
        u_int new_n = AS_number - 32;
        offset = MAILBOX_OFFSET + ( 32 / 2 ) + new_n / 2 + new_n % 2;
        state = data[ offset ];
        if ( AS_number % 2 == 1 )//Нечетный номер - старшие четыре бита.
            {
            state >>= 4;
            }
        }

    return state;
    }
//-----------------------------------------------------------------------------
char* valve_AS::get_data_with_offset( char* data )
    {
    if ( data == nullptr )
        {
        return nullptr;
        }

    char* state;
    u_int offset = 0;

    if ( AS_number < 32 )		 //Eсли номер < 32).
        {
        offset = MAILBOX_OFFSET + AS_number / 2;
        }
    else
        {
        u_int new_n = AS_number - 32;
        offset = MAILBOX_OFFSET + ( 32 / 2 ) + new_n / 2 + new_n % 2;
        }

    state = data + offset;
    return state;
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_AS::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    auto data = (const char*)get_AO_read_data( AO_INDEX );
    auto state = get_state_data( data );

    auto o = ( state & C_OPEN_S1 ) > 0 ? 1 : 0;

    if ( auto u = state & get_upper_seat_offset() ? 1 : 0; !o && u == 1 )
        return V_UPPER_SEAT;
    if ( auto l = state & get_lower_seat_offset() ? 1 : 0; !o && l == 1 )
        return V_LOWER_SEAT;

    return (VALVE_STATE)o;
    }
//-----------------------------------------------------------------------------
bool valve_AS::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    auto AO_data = (const char*)get_AO_read_data( AO_INDEX );
    auto AO_state = get_state_data( AO_data );

    auto o = ( AO_state & C_OPEN_S1 ) > 0 ? 1 : 0;
    auto l = ( AO_state & get_lower_seat_offset() ) > 0 ? 1 : 0;
    auto u = ( AO_state & get_upper_seat_offset() ) > 0 ? 1 : 0;

    auto AI_data = (const char*)get_AI_data( AI_INDEX );
    auto AI_state = get_state_data( AI_data );

    if ( auto i0 = ( AI_state & S_CLOSED ) > 0 ? 1 : 0,
        i1 = ( AI_state & S_OPENED ) > 0 ? 1 : 0;
        ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        return true;
        }

    if ( o == 0 && l == 1 ) return true;
    if ( o == 0 && u == 1 ) return true;

    if ( get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
int valve_AS::get_lower_seat_offset() const
    {
    if ( reverse_seat_connection )
        {
        return C_OPEN_S3;
        }
    else
        {
        return C_OPEN_S2;
        }
    }
//-----------------------------------------------------------------------------
int valve_AS::get_upper_seat_offset() const
    {
    if ( reverse_seat_connection )
        {
        return C_OPEN_S2;
        }
    else
        {
        return C_OPEN_S3;
        }
    }
//-----------------------------------------------------------------------------
bool valve_AS::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    auto AI_data = (const char*)get_AI_data( AI_INDEX );
    auto AI_state = get_state_data( AI_data );

    int i0 = AI_state & S_CLOSED;

    return i0 > 0 ? 1 : 0;
    }
//-----------------------------------------------------------------------------
bool valve_AS::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    auto AI_data = (const char*)get_AI_data( AI_INDEX );
    auto AI_state = get_state_data( AI_data );

    int i1 = AI_state & S_OPENED;

    return i1 > 0 ? 1 : 0;
    }
//-----------------------------------------------------------------------------
void valve_AS::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    auto data = (char*)get_AO_write_data( AO_INDEX );
    auto write_state = get_data_with_offset( data );
    auto read_state = get_state_data( data );

    if ( auto o = ( read_state & C_OPEN_S1 ) > 0 ? 1 : 0; o == 1 )
        {
        start_switch_time = get_millisec();
        }

    auto offset = 0;
    //Для первого 31-го устройства четный номер - старшие четыре
    //бита (1), для остальных устройств нечетный номер - старшие четыре
    //бита (2).
    if ( ( AS_number < 32 && AS_number % 2 == 0 ) ||    		//1
        ( AS_number >= 32 && AS_number % 2 == 1 ) )				//2
        {
        offset = 4;
        }

    //Сбрасываем в ноль все четыре нужные бита.
    *write_state &= 0xF0 >> offset;
    }
//-----------------------------------------------------------------------------
void valve_AS::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    auto data = (char*)get_AO_write_data( AO_INDEX );
    auto write_state = get_data_with_offset( data );
    auto read_state = get_state_data( data );

    if ( auto o = ( read_state & C_OPEN_S1 ) > 0 ? 1 : 0; 0 == o )
        {
        start_switch_time = get_millisec();
        }

    auto offset = 0;
    //Для первого 31-го устройства четный номер - старшие четыре
    //бита (1), для остальных устройств нечетный номер - старшие четыре
    //бита (2).
    if ( ( AS_number < 32 && AS_number % 2 == 0 ) ||    		//1
        ( AS_number >= 32 && AS_number % 2 == 1 ) )				//2
        {
        offset = 4;
        }
    *write_state |= C_OPEN_S1 << offset;
    *write_state &= ~( get_upper_seat_offset() << offset );
    *write_state &= ~( get_lower_seat_offset() << offset );
    }
//-----------------------------------------------------------------------------
void valve_AS::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve::direct_set_state( new_state );
        }

    auto offset = 0;
    // Для первого 31-го устройства четный номер - старшие четыре бита (1),
    // для остальных устройств нечетный номер - старшие четыре бита (2).
    if ( ( AS_number < 32 && AS_number % 2 == 0 ) ||    		//1
        ( AS_number >= 32 && AS_number % 2 == 1 ) )				//2
        {
        offset = 4;
        }

    auto data = (char*)get_AO_write_data( AO_INDEX );
    auto write_state = get_data_with_offset( data );
    auto read_state = get_state_data( data );

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            {
            direct_off();
            if ( auto u = read_state & get_upper_seat_offset() ? 1 : 0; !u )
                {
                start_switch_time = get_millisec();
                }
            *write_state |= get_upper_seat_offset() << offset;
            break;
            }

        case V_LOWER_SEAT:
            {
            direct_off();
            if ( auto l = read_state & get_lower_seat_offset() ? 1 : 0; !l )
                {
                start_switch_time = get_millisec();
                }
            *write_state |= get_lower_seat_offset() << offset;
            break;
            }

        default:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::unordered_set<std::string> valve_AS::V70_ARTICLES = { "AL.9615-4002-12" };

valve_AS_mix_proof::valve_AS_mix_proof( const char* dev_name ) :
    valve_AS( dev_name, DST_V_AS_MIXPROOF )
    {
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::open_upper_seat()
    {
    direct_set_state( V_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::open_lower_seat()
    {
    direct_set_state( V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS_DO1_DI2::valve_AS_DO1_DI2( const char* dev_name ) :
    valve_AS( dev_name, DST_V_AS_DO1_DI2 ),
    start_err_time( get_millisec() )
    {
    }
//---------------------------------------------------------------------------
void valve_AS_DO1_DI2::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return valve_AS::direct_set_state( new_state );
        }

    switch ( new_state )
        {
        case (int)VALVE_STATE::V_UPPER_SEAT:
        case (int)VALVE_STATE::V_LOWER_SEAT:
            //Ничего не делаем, так как нет седел.
            break;

        default:
            valve_AS::direct_set_state( new_state );
            break;
        }
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_AS_DO1_DI2::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    auto data = (const char*)get_AO_read_data( AO_INDEX );
    auto state = get_state_data( data );

    int o = ( state & C_OPEN_S1 ) > 0 ? 1 : 0;

    return (VALVE_STATE)o;
    }
//-----------------------------------------------------------------------------
bool valve_AS_DO1_DI2::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    auto AO_data = (const char*)get_AO_read_data( AO_INDEX );
    auto AO_state = get_state_data( AO_data );

    auto AI_data = (const char*)get_AI_data( AI_INDEX );
    auto AI_state = get_state_data( AI_data );

    if ( auto o = ( AO_state & C_OPEN_S1 ) > 0 ? 1 : 0,
        i0 = ( AI_state & S_CLOSED ) > 0 ? 1 : 0,
        i1 = ( AI_state & S_OPENED ) > 0 ? 1 : 0;
        ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        start_err_time = get_millisec();
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    if ( get_delta_millisec( start_err_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_mini_flushing::valve_mini_flushing( const char* dev_name ) : valve(
    true, true, dev_name, DT_V, DST_V_MINI_FLUSHING )
    {
    }

void valve_mini_flushing::open_upper_seat()
    {
    //Не делаем ничего, так как верхнего седла нет.
    }

void valve_mini_flushing::open_lower_seat()
    {
    direct_set_state( V_LOWER_SEAT );
    }

void valve_mini_flushing::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_set_state( new_state );

    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            direct_off();
            break;

        case V_LOWER_SEAT:      //Открываем миниклапан.
            direct_off();

            if ( 0 == get_DO( DO_INDEX_MINI_V ) )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_MINI_V, 1 );
                }
            break;

        default:
            direct_on();
            break;
        }
    }

void valve_mini_flushing::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    int o = get_DO( DO_INDEX );

    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        set_DO( DO_INDEX_MINI_V, 0 );
        }
    }

void valve_mini_flushing::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    if ( get_DO( DO_INDEX_MINI_V ) == 1 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_MINI_V, 0 );
        }

    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }

valve::VALVE_STATE valve_mini_flushing::get_valve_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_valve_state();

    int o = get_DO( DO_INDEX );

    if ( o == 0 && get_DO( DO_INDEX_MINI_V ) == 1 ) return V_LOWER_SEAT;

    return (VALVE_STATE)o;
    }

bool valve_mini_flushing::get_fb_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_fb_state();

    if ( auto o = get_DO( DO_INDEX ),
        i0 = get_DI( DI_INDEX_CLOSE ),
        i1 = get_DI( DI_INDEX_OPEN );
        ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }

bool valve_mini_flushing::get_off_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_off_fb_value();

    return get_DI( DI_INDEX_CLOSE );
    }

bool valve_mini_flushing::get_on_fb_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::get_on_fb_value();

    return get_DI( DI_INDEX_OPEN );
    }
