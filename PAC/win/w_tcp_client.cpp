#include "w_tcp_client.h"

const char* WSA_Last_Err_Decode();

int win_tcp_client::Communicate( unsigned int bytestosend )
    {
    int res;
    if ( !is_initialized )
        {
        if ( !InitLib() )
            {
            return 0;
            }
        }
    if ( !connectedstate )
        {
        if ( !Connect() )
            {
            return 0;
            }
        }

    if ( send( socket_number, buff, bytestosend, 0 ) == SOCKET_ERROR )
        {
        if ( G_DEBUG )
            {
            printf( "tcp_client_%d Ошибка %d отсылки сообщения.\n", id,
                WSAGetLastError() );
            }
        Disconnect();
        return 0;
        }

    FD_ZERO( &rfds );
    FD_SET( socket_number, &rfds );
    res = select( 0, &rfds, 0, 0, &tv );

    if ( res <= 0 )
        {
        if ( G_DEBUG )
            {
            printf( "tcp_client_%d Ошибка получения ответа\n", id );
            }
        Disconnect();
        return 0;
        }

    res = recv( socket_number, buff, buff_size, 0 );

    if ( 0 == res )
        {
        if ( G_DEBUG )
            {
            printf( "tcp_client_%d Сервер закрыл соединение\n", id );
            }
        Disconnect();
        return 0;
        }

    if ( res < 0 )
        {
        if ( res == -1 )
            {
            if ( G_DEBUG )
                {
                printf( "tcp_client_%d Ошибка %d получения ответа\n", id,
                    WSAGetLastError() );
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                printf(
                    "tcp_client_%d Ошибка получения ответа, истекло время ожидания\n",
                    id );
                }
            }
        Disconnect();
        return 0;
        }

    return res;
    }

win_tcp_client::win_tcp_client( const char* client_ip, unsigned int client_port,
    unsigned int client_id, unsigned char alarm_subclass,
    unsigned int exchange_buf_size /*= 256*/,
    uint32_t send_receive_timeout /*= 100*/ ) :
    tcp_client( client_ip, client_port, client_id, alarm_subclass,
        exchange_buf_size, send_receive_timeout )
    {
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;
    async_startconnnect = get_millisec();
    async_tv.tv_sec = 0;
    async_tv.tv_usec = 100;
    }

int win_tcp_client::InitLib()
    {
    WSAData tmpWSAData;
    if ( !is_initialized )
        {
        if ( WSAStartup( 0x202, &tmpWSAData ) )
            {
            if ( G_DEBUG )
                {
                printf(
                    "tcp_client_%d Ошибка %d инициализации сетевой библиотеки.\n",
                    id, WSAGetLastError() );
                }
            return 0;
            }
        }
    is_initialized = 1;
    return 1;
    }

void win_tcp_client::DeinitLib()
    {
    if ( is_initialized )
        {
        WSACleanup();
        }
    is_initialized = 0;
    }

int win_tcp_client::Connect()
    {
    int res;
    if ( connectedstate )
        {
        return 1;
        }
    socket_number = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
    if ( socket_number == INVALID_SOCKET )
        {
        if ( G_DEBUG )
            {
            printf( "tcp_client_%d: Ошибка создания сокета %d!\n", id,
                WSAGetLastError() );
            }

        return 0;
        }

    int vlen = sizeof( timeout );

    if ( setsockopt( socket_number, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout,
             vlen ) == SOCKET_ERROR ||
         setsockopt( socket_number, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
             vlen ) == SOCKET_ERROR )
        {
        if ( G_DEBUG )
            {
            printf(
                "tcp_client_%d: Ошибка установления параметров сокета %d!\n",
                id, WSAGetLastError() );
            }
        return 0;
        }

    //Переводим сокет в неблокирующий режим.
    u_long mode = 1;
    res = ioctlsocket( socket_number, FIONBIO, &mode );
    if ( res == SOCKET_ERROR )
        {
        if ( G_DEBUG )
            {
            printf(
                "tcp_client_%d: Ошибка перевода сокета в неблокирующий режим %d!\n",
                id, WSAGetLastError() );
            }


        closesocket( socket_number );
        socket_number = 0;
        return 0;
        }

    sockaddr_in sock_address;
    memset( &sock_address, 0, sizeof( sockaddr_in ) );
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons( (u_short)port );
    sock_address.sin_addr.s_addr = inet_addr( ip );

    connect( socket_number, (SOCKADDR*)&sock_address, sizeof( sockaddr_in ) );

    FD_ZERO( &rfds );
    FD_SET( socket_number, &rfds );
    res = select( 0, 0, &rfds, 0, &tv );

    if ( res <= 0 )
        {
        if ( 0 == res )
            {
            if ( G_DEBUG )
                {
                printf( "tcp_client_%d: Ошибка соединения. Таймаут\n", id );
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                printf( "tcp_client_%d: Ошибка соединения %d!\n", id,
                    WSAGetLastError() );
                }
            }
        closesocket( socket_number );
        socket_number = 0;
        return 0;
        }

    int sock_error;
    int sock_err_len = sizeof( sock_error );

    if ( FD_ISSET( socket_number, &rfds ) )
        {
        res = getsockopt( socket_number, SOL_SOCKET, SO_ERROR,
            (char*)&sock_error, &sock_err_len );
        if ( res < 0 || sock_error != 0 )
            {
            if ( G_DEBUG )
                {
                printf( "tcp_client_%d: Ошибка соединения(select) %d!\n", id,
                    sock_error );
                }
            closesocket( socket_number );
            socket_number = 0;
            return 0;
            }
        }


    connectedstate = 1;
    return 1;
    }

int win_tcp_client::AsyncConnect()
    {
    int res;
    if ( connectedstate == ACS_CONNECTED )
        {
        return 1;
        }
    else

        {
        if ( connectedstate == ACS_DISCONNECTED )
            {
            socket_number = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
            if ( socket_number == INVALID_SOCKET )
                {
                if ( G_DEBUG )
                    {
                    printf( "tcp_client_%d: Ошибка создания сокета : %s.\n", id,
                        WSA_Last_Err_Decode() );
                    }

                return 0;
                }

            int vlen = sizeof( timeout );

            if ( setsockopt( socket_number, SOL_SOCKET, SO_SNDTIMEO,
                     (char*)&timeout, vlen ) == SOCKET_ERROR ||
                 setsockopt( socket_number, SOL_SOCKET, SO_RCVTIMEO,
                     (char*)&timeout, vlen ) == SOCKET_ERROR )
                {
                if ( G_DEBUG )
                    {
                    printf(
                        "tcp_client_%d: Ошибка установления параметров сокета %d!\n",
                        id, WSAGetLastError() );
                    }
                return 0;
                }

            //Переводим сокет в неблокирующий режим.
            u_long mode = 1;
            res = ioctlsocket( socket_number, FIONBIO, &mode );
            if ( res == SOCKET_ERROR )
                {
                if ( G_DEBUG )
                    {
                    printf(
                        "tcp_client_%d: Ошибка перевода сокета в неблокирующий режим %d!\n",
                        id, WSAGetLastError() );
                    }


                closesocket( socket_number );
                socket_number = 0;
                return 0;
                }

            sockaddr_in async_sock_address;
            memset( &async_sock_address, 0, sizeof( sockaddr_in ) );
            async_sock_address.sin_family = AF_INET;
            async_sock_address.sin_port = htons( (u_short)port );
            async_sock_address.sin_addr.s_addr = inet_addr( ip );

            connect( socket_number, (SOCKADDR*)&async_sock_address,
                sizeof( sockaddr_in ) );
            async_startconnnect = get_millisec();

            connectedstate = ACS_CONNECTING;
            }

        if ( connectedstate == ACS_CONNECTING )
            {
            FD_ZERO( &rfds );
            FD_SET( socket_number, &rfds );
            res = select( 0, 0, &rfds, 0, &async_tv );

            if ( res == 0 )
                {
                if ( get_delta_millisec( async_startconnnect ) > timeout )
                    {
                    if ( G_DEBUG )
                        {
                        printf(
                            "tcp_client_%d: Ошибка соединения. Таймаут\n", id );
                        }
                    closesocket( socket_number );
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

            if ( res < 0 )
                {
                if ( G_DEBUG )
                    {
                    printf( "tcp_client_%d: Ошибка соединения %d!\n", id,
                        WSAGetLastError() );
                    }
                closesocket( socket_number );
                socket_number = 0;
                connectedstate = ACS_DISCONNECTED;
                return ACS_DISCONNECTED;
                }

            int sock_error;
            int sock_err_len = sizeof( sock_error );

            if ( FD_ISSET( socket_number, &rfds ) )
                {
                res = getsockopt( socket_number, SOL_SOCKET, SO_ERROR,
                    (char*)&sock_error, &sock_err_len );
                if ( res < 0 || sock_error != 0 )
                    {
                    if ( G_DEBUG )
                        {
                        printf(
                            "tcp_client_%d: Ошибка соединения(select) %d!\n",
                            id, sock_error );
                        }
                    closesocket( socket_number );
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

void win_tcp_client::Disconnect()
    {
    //tcp_communicator::get_instance()->remove_async_client(this);
    shutdown( socket_number, SD_BOTH );
    closesocket( socket_number );
    socket_number = 0;
    connectedstate = 0;
    }

win_tcp_client::~win_tcp_client()
    {
    if ( connectedstate )
        {
        Disconnect();
        }
    }

bool win_tcp_client::checkBuff( int s )

    {
    errno = 0;

    // Настраиваем  file descriptor set.
    fd_set fds;
    FD_ZERO( &fds );

    FD_SET( s, &fds );


    // Настраиваем время на таймаут.
    timeval rec_tv;
    rec_tv.tv_sec = 0;
    rec_tv.tv_usec = 0;

    // Ждем таймаута или полученных данных.
    int n = select( s + 1, &fds, nullptr, nullptr, &rec_tv );


    return n >= 1;
    }

int win_tcp_client::AsyncSend( unsigned int bytestosend )
    {
    async_result = AR_BUSY;
    async_bytes_to_send = bytestosend;

    if ( !checkConnection() ) return 0;

    int res = send( socket_number, buff, bytestosend, 0 );
    if ( res == SOCKET_ERROR )
        {
        if ( G_DEBUG )
            {
            printf( "tcp_client_%d Ошибка %d отсылки сообщения.\n", id,
                WSAGetLastError() );
            }
        async_result = AR_SOCKETERROR;
        Disconnect();
        return 0;
        }
    else
        {
        return tcp_client::AsyncSend( bytestosend );
        }
    }

int win_tcp_client::AsyncReceive()
    {
    async_result = AR_BUSY;

    if ( !checkConnection() ) return 0;

    if ( checkBuff( socket_number ) && !isNewData )
        {
        asyncReceiveTime = get_millisec();
        isNewData = true;
        }

    int res = 0;

    if ( get_delta_millisec( asyncReceiveTime ) >= async_timeout && isNewData )
        {
        res = recv( socket_number, buff, buff_size, 0 );
        isNewData = false;
        }

    if ( res < 0 )
        {
        async_result = AR_SOCKETERROR;
        Disconnect();
        return 0;
        }
    return res;
    }


int win_tcp_client::get_async_result()
    {
    /// В процессе соединения циклично вызываем функцию для реализации асинхронного соединения.
    if ( connectedstate == ACS_CONNECTING )
        {
        AsyncSend( async_bytes_to_send );
        }
    return async_result;
    }
