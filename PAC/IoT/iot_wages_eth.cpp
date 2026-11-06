#include "iot_wages_eth.h"
#include <cstring>
#include <cstdlib>

#include "fmt/format.h"

iot_wages_eth::iot_wages_eth( unsigned int id, const char* ip,
    unsigned int port, const char* wages_name ) :
    tc( std::unique_ptr<tcp_client>( tcp_client::Create( ip, port, id, 0,
    static_cast<unsigned int> ( CONSTANTS::BUFF_SIZE ),
    static_cast<unsigned long> ( CONSTANTS::SEND_RECEIVE_TIMEOUT ) ) ) )
    {
    fmt::format_to_n( name.data(), name.size() - 1, "{}",
        wages_name ? wages_name : fmt::to_string( id ) );
    }

void iot_wages_eth::evaluate()
    {
    status = tc->AsyncReceive();
    if ( status > 0 )
    {
        convert_value();
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
    if ( tc->buff[ 8 ] != 'k' || tc->buff[ 9 ] != 'g' || !status )
        {
        state = 0;
        return;
        }
    else
        {
        state = 1;
        value = static_cast<float>( atof( tc->buff + 1 ) );
        }
    }

void iot_wages_eth::direct_set_tcp_buff( const char* new_value, size_t size,
    int new_status )
    {
    memcpy( tc->buff, new_value, size );
    status = new_status;
    convert_value();
    }
