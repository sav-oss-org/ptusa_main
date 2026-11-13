#ifndef l_tcp_client_h__
#define l_tcp_client_h__
#include "tcp_client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include "l_tcp_cmctr.h"

class linux_tcp_client : public tcp_client
    {
    private:
        uint32_t async_startconnnect;
        timeval async_tv;
        sockaddr_in async_sock_address;
        unsigned int async_bytes_to_send;
        uint32_t asyncReceiveTime;
        bool newDataIsAvailable;

    public:
        int Connect();
        int AsyncConnect() override;
        void Disconnect();
        virtual int Communicate( unsigned int bytestosend );
        virtual int AsyncSend( unsigned int bytestosend );
        int AsyncReceive() override;
        virtual int get_async_result() override;
        linux_tcp_client( const char* client_ip, unsigned int client_port,
            unsigned int client_id, unsigned char alarm_subclass,
            unsigned int exchange_buf_size = 256,
            uint32_t send_receive_timeout = 100 );
        ~linux_tcp_client();
    };


#endif // l_tcp_client_h__
