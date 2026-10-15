#include <netinet/in.h>
#include <fcntl.h>
#include <inttypes.h>

#include "l_tcp_client.h"
#include "l_tcp_cmctr.h"
#include "PAC_err.h"
#include "log.h"
#include "errno.h"

int linux_tcp_client::Communicate(unsigned int bytestosend)
    {
    if (!connectedstate)
        {
        if (!Connect())
            {
            return 0;
            }
        }

    if (tcp_communicator::sendall(socket_number, (unsigned char*) buff,
        bytestosend, 0, timeout * 1000, ip, "tcp client", 0) < 0)
        {
        Disconnect();
        return 0;
        }

    int res = tcp_communicator::recvtimeout(socket_number,
        (unsigned char*) buff, buff_size, 0, timeout * 1000, ip, "tcp client",
        0);

    if (res < 0)
        {
        Disconnect();
        return 0;
        }

    return res;
    }

linux_tcp_client::linux_tcp_client(const char* client_ip,
    unsigned int client_port, unsigned int client_id,
    unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/,
    uint32_t send_receive_timeout /*= 100*/) :
    tcp_client(client_ip, client_port, client_id, alarm_subclass,
        exchange_buf_size, send_receive_timeout)
    {
    async_startconnnect = get_millisec();
    async_last_connect_try = get_millisec();
    async_tv.tv_sec = 0;
    async_tv.tv_usec = 100;
    }

int linux_tcp_client::Connect()
    {
    int res;
    if (connectedstate)
        {
        return 1;
        }
    socket_number = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_number < 0)
        {
        G_LOG->error( "Network device : s%d->\"%s\" error creating socket : "
            "timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
        return 0;
        }

    if ( const int C_ON = 1; setsockopt(socket_number, SOL_SOCKET, SO_REUSEADDR, &C_ON,
        sizeof(C_ON)))
        {
        G_LOG->error( "Network device : s%d->\"%s\" error setting socket "
            "params : timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
        close(socket_number);
        return 0;
        }

    //Переводим сокет в неблокирующий режим.
    int flags = fcntl(socket_number, F_GETFL);
    res = fcntl(socket_number, F_SETFL, flags | O_NONBLOCK);
    if (res != 0)
        {
        G_LOG->error( "Network device : s%d->\"%s\" error setting nonblock "
            "mode : timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
        close(socket_number);
        socket_number = 0;
        return 0;
        }

    struct sockaddr_in sock_address;
    memset(&sock_address, 0, sizeof(sockaddr_in));
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons((u_short) port);
    sock_address.sin_addr.s_addr = inet_addr(ip);

    res = connect(socket_number, (struct sockaddr*) &sock_address,
        sizeof(sock_address));

    fd_set rdevents;
    struct timeval tv;
    FD_ZERO(&rdevents);
    FD_SET(socket_number, &rdevents);

    tv.tv_sec = connectTimeout / 1000;
    tv.tv_usec = (connectTimeout % 1000) * 1000;

    res = select(socket_number + 1, 0, &rdevents, 0, &tv);

    if (res <= 0)
        {
        G_LOG->error( "Network device : s%d->\"%s\" error in connect : "
            "timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
        close(socket_number);
        socket_number = 0;
        return 0;
        }

    if (FD_ISSET(socket_number, &rdevents))
        {
        socklen_t err_len;
        int error = 0;
        err_len = sizeof(error);
        if (getsockopt(socket_number, SOL_SOCKET, SO_ERROR, &error, &err_len)
            < 0 || error != 0)
            {
            G_LOG->error( "Network device : s%d->\"%s\" error in connect : "
                "timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
            close(socket_number);
            socket_number = 0;
            return 0;
            }
        }

    connectedstate = 1;
    return 1;
    }

int linux_tcp_client::AsyncConnect()
{
    int res;
    if (connectedstate == ACS_CONNECTED)
        {
        return 1;
        }

    else
        {
        if (connectedstate == ACS_DISCONNECTED)
            {
                socket_number = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
                if (socket_number < 0)
                    {
                    G_LOG->error( "Network device : s%d->\"%s\" error creating "
                        "socket : timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
                    return 0;
                    }

                if ( const int C_ON = 1; setsockopt(socket_number, SOL_SOCKET, SO_REUSEADDR, &C_ON,
                    sizeof(C_ON)))
                    {
                    G_LOG->error( "Network device : s%d->\"%s\" error setting "
                        "socket params : timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
                    close(socket_number);
                    socket_number = 0;
                    return 0;
                    }

                //Переводим сокет в неблокирующий режим.
                int flags = fcntl(socket_number, F_GETFL);
                res = fcntl(socket_number, F_SETFL, flags | O_NONBLOCK);
                if (res != 0)
                    {
                    G_LOG->error( "Network device : s%d->\"%s\" error setting "
                        "nonblock mode : timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
                    close(socket_number);
                    socket_number = 0;
                    return 0;
                    }

                struct sockaddr_in sock_address;
                memset(&sock_address, 0, sizeof(sockaddr_in));
                sock_address.sin_family = AF_INET;
                sock_address.sin_port = htons((u_short) port);
                sock_address.sin_addr.s_addr = inet_addr(ip);

                int connectres = connect(socket_number, (struct sockaddr*) &sock_address, sizeof(sock_address));
                if (connectres == -1 && errno != EINPROGRESS)
                    {
                    Disconnect();
                    }
                else
                    {
                    async_startconnnect = get_millisec();
                    connectedstate = ACS_CONNECTING;
                    }
            }

        if (connectedstate == ACS_CONNECTING)
        {
            fd_set rdevents;
            FD_ZERO(&rdevents);
            FD_SET(socket_number, &rdevents);
            res = select(socket_number + 1, 0, &rdevents, 0, &async_tv);
            if (res == 0)
            {
                if (get_delta_millisec(async_startconnnect) > timeout)
                {
                    G_LOG->error( "Network device : s%d->\"%s\" disconnected "
                        "on timeout : timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
                    shutdown(socket_number, SHUT_RDWR);
                    close(socket_number);
                    socket_number = 0;
                    connectedstate = ACS_DISCONNECTED;
                    return ACS_DISCONNECTED;
                }
                else
                {
                    connectedstate = ACS_CONNECTING;
                    return ACS_CONNECTING;
                }
            }

            if (res < 0)
            {
                G_LOG->error( "Network device : s%d->\"%s\" connect error : "
                    "timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
                shutdown(socket_number, SHUT_RDWR);
                close(socket_number);
                socket_number = 0;
                connectedstate = ACS_DISCONNECTED;
                return ACS_DISCONNECTED;
            }


            if (FD_ISSET(socket_number, &rdevents))
                 {
                 socklen_t err_len;
                 int error = 0;
                 err_len = sizeof(error);
                 if (getsockopt(socket_number, SOL_SOCKET, SO_ERROR, &error, &err_len)
                     < 0 || error != 0)
                     {
                     G_LOG->error( "Network device : s%d->\"%s\" error in "
                         "connect(select) : timeout (%" PRIu32 " ms).", id, ip, connectTimeout );
                     shutdown(socket_number, SHUT_RDWR);
                     close(socket_number);
                     socket_number = 0;
                     connectedstate = ACS_DISCONNECTED;
                     return ACS_DISCONNECTED;
                     }
                 }

        }
        connectedstate = ACS_CONNECTED;
        return ACS_CONNECTED;
    }
}


void linux_tcp_client::Disconnect()
    {
    shutdown(socket_number, SHUT_RDWR);
    close(socket_number);
    socket_number = 0;
    connectedstate = 0;
    }

int linux_tcp_client::AsyncSend(unsigned int bytestosend)
    {
    async_result = AR_BUSY;
    async_bytes_to_send = bytestosend;

    if ( !checkConnection() ) return 0;

    int res = tcp_communicator::sendall( socket_number, ( unsigned char* ) buff, bytestosend, 0, timeout * 10, ip, "async tcp client", 0 );

    if (res < 0)
        {
        async_result = AR_SOCKETERROR;
        Disconnect();
        G_LOG->error( "Network device : s%d->\"%s\" error on send - "
            "disconnect : timeout (%" PRIu32 " ms).", socket_number, ip, connectTimeout );
        return 0;
        }
    else
        {
        return tcp_client::AsyncSend(bytestosend);
        }
    }

int linux_tcp_client::AsyncReceive()
    {
    async_result = AR_BUSY;

    if ( !checkConnection() ) return 0;

    if ( tcp_communicator::checkBuff( socket_number ) && !newDataIsAvailable )
        {
        asyncReceiveTime = get_millisec();
        newDataIsAvailable = true;
        }

    int res = 0;

    if ( get_delta_millisec( asyncReceiveTime ) >= async_timeout && newDataIsAvailable )
        {
        newDataIsAvailable = false;
        res = tcp_communicator::recvtimeout( socket_number, reinterpret_cast<unsigned char*>( buff ),
            buff_size, 0, 0, ip, "tcp client", nullptr );
        }

    if ( res < 0 )
       {
       async_result = AR_SOCKETERROR;
       Disconnect();
       return 0;
       }
    return res;
    }

int linux_tcp_client::get_async_result() {
    /// В процессе соединения циклично вызываем функцию для реализации асинхронного соединения.
    if ( connectedstate == ACS_CONNECTING )
    {
        AsyncSend(async_bytes_to_send);
    }
    return async_result;
}

linux_tcp_client::~linux_tcp_client()
    {
    if (connectedstate)
        {
        Disconnect();
        }
}

