#include "tcp_client.h"
#include "PAC_err.h"
#include "tcp_cmctr.h"

#ifdef WIN_OS
#include "w_tcp_client.h"
#endif //WIN_OS

#ifdef LINUX_OS
#include "l_tcp_client.h"
#endif


tcp_client::tcp_client( const char* client_ip, unsigned int client_port, unsigned int client_id, unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/, uint32_t send_receive_timeout /*= 100*/ )
    {
    ip = new char[16];
    strcpy(ip, client_ip);
    port = client_port;
    id = client_id;
    subclass = alarm_subclass;
    buff = new char[exchange_buf_size];
    buff_size = exchange_buf_size;
    timeout = send_receive_timeout;
    async_timeout = send_receive_timeout * 2;
    connectTimeout = send_receive_timeout;
    reconnectTimeout = send_receive_timeout * RECONNECT_MIN_MULTIPLIER;
    maxreconnectTimeout = send_receive_timeout * RECONNECT_MAX_MULTIPLIER;
    socket_number = 0;
    connectedstate = 0;
    is_initialized = 0;
    async_result = 0;
    async_queued = 0;
    async_last_connect_try = 0;
    }

tcp_client::~tcp_client()
    {
    delete [] ip;
    ip = nullptr;
    delete [] buff;
    buff = nullptr;
    }

tcp_client* tcp_client::Create( const char* ip, unsigned int port, unsigned int id, unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/, uint32_t send_receive_timeout /*= 100*/ )
    {
#if defined WIN_OS
    return new win_tcp_client(ip, port, id, alarm_subclass, exchange_buf_size, send_receive_timeout);

#elif defined LINUX_OS
    return new linux_tcp_client(ip, port, id, alarm_subclass, exchange_buf_size, send_receive_timeout);

#else
    return 0;
#endif
    }

int tcp_client::Communicate( unsigned int bytestosend )
    {
    return 0;
    }

int tcp_client::get_id()
    {
    return id;
    }

int tcp_client::checkConnection()
    {
    if ( connectedstate != ACS_CONNECTED )
        {
        if ( get_delta_millisec( async_last_connect_try ) > reconnectTimeout || connectedstate == ACS_CONNECTING )
            {
            if ( connectedstate == ACS_DISCONNECTED )
                {
                async_last_connect_try = get_millisec();
                }


            int connectres = AsyncConnect();

            if ( connectres == ACS_DISCONNECTED )
                {
                async_result = AR_SOCKETERROR;
                // Saturate before doubling to avoid exceeding the cap or overflowing.
                reconnectTimeout = reconnectTimeout > maxreconnectTimeout / 2
                    ? maxreconnectTimeout : reconnectTimeout * 2;
                return 0;
                }

            if ( connectres == ACS_CONNECTING )
                {
                connectedstate = ACS_CONNECTING;
                return 0;
                }

            if ( connectres == ACS_CONNECTED )
                {
                reconnectTimeout = connectTimeout * RECONNECT_MIN_MULTIPLIER;
                }
            }
        else
            {
            async_result = AR_SOCKETERROR;
            return 0;
            }
        }

    return 1;
    }

int tcp_client::AsyncConnect()
    {
    return 0;
    }

int tcp_client::AsyncSend( unsigned int bytestosend )
    {
    async_result = AR_BUSY;
    tcp_communicator::get_instance()->add_async_client( this );

    return 0;
    }

int tcp_client::AsyncReceive()
    {
    tcp_communicator::get_instance()->add_async_client( this );

    async_result = AR_BUSY;
    return 0;
    }

int tcp_client::get_async_result()
    {
    return async_result;
    }

int tcp_client::get_connected_state()
	{
	return connectedstate;
	}

int tcp_client::get_socket()
    {
    return socket_number;
    }

int tcp_client::set_async_result( int ar )
    {
    async_result = ar;
    return ar;
    }
