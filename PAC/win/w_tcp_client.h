#ifndef w_tcp_client_h__
#define w_tcp_client_h__
#include "tcp_client.h"
#include <winsock2.h>

class win_tcp_client : public tcp_client
    {
    private:
        timeval tv;
        fd_set rfds;
        uint32_t async_startconnnect;
        timeval async_tv;
        sockaddr_in async_sock_address;
        unsigned int async_bytes_to_send;
        uint32_t asyncReceiveTime;
        bool isNewData;

    public:
        int InitLib();
        void DeinitLib();
        int Connect();
        int AsyncConnect() override;
        virtual void Disconnect();
        virtual int Communicate( unsigned int bytestosend ) override;
        bool checkBuff( int s );

        virtual int AsyncSend( unsigned int bytestosend );

        int AsyncReceive() override;
        virtual int get_async_result() override;
        win_tcp_client( const char* client_ip, unsigned int client_port,
            unsigned int client_id, unsigned char alarm_subclass,
            unsigned int exchange_buf_size = 256,
            uint32_t send_receive_timeout = 100 );

        ~win_tcp_client();
    };


#endif // w_tcp_client_h__
