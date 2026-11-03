#include "iot_wages_eth.h"
#include "tcp_cmctr.h"
#include "log.h"
#include <cstring>
#include <cstdlib>

iot_wages_eth::iot_wages_eth( unsigned int id, const char* ip,
    unsigned int port, const char* name ) :
    tc( std::unique_ptr<tcp_client>( tcp_client::Create( ip, port, id, 0,
    static_cast<unsigned int> ( CONSTANTS::BUFF_SIZE ),
    static_cast<unsigned long> ( 500 ) ) ) )
    {
    (void)name;
    }

void iot_wages_eth::evaluate()
    {
    if ( tc->AsyncReceive() > 0 )
        {
        convert_value();
        last_correct_recive = get_millisec();
        }
    else if ( get_delta_millisec( last_correct_recive ) > timeout &&
        state != DISCONNECTED )
        {
        state = DISCONNECTED;
        last_correct_recive = get_millisec();
        tc->Disconnect();
        }
    }

int iot_wages_eth::get_wages_state() const
    {
    return state;
    }

float iot_wages_eth::get_wages_value() const
    {
    return value;
    }

void iot_wages_eth::set_wages_value( float new_value )
    {
    if ( new_value >= .0f ) value = new_value;
    }

void iot_wages_eth::set_wages_state( int new_state )
    {
    state = new_state;
    }

void iot_wages_eth::convert_value()
    {
    if ( tc->buff[ 8 ] != 'k' || tc->buff[ 9 ] != 'g' )
        {
        state = INCORRECTDATA;
        }
    else
        {
        state = CORRECTDATA;
        value = static_cast<float>( atof( tc->buff + 1 ) );
        }
    }

void iot_wages_eth::direct_set_tcp_buff( const char* new_value, size_t size,
    int new_status )
    {
    memcpy( tc->buff, new_value, size );
    if ( new_status > 0 )
        {
        convert_value();
        }
    else
        {
        state = DISCONNECTED;
        }
    }
