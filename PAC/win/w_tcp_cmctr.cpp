#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "dtime.h"

#include "w_tcp_cmctr.h"
#include "PAC_err.h"

#include "tcp_client.h"
//------------------------------------------------------------------------------
unsigned int max_buffer_use = 0;

//------------------------------------------------------------------------------
const char* WSA_Last_Err_Decode();

//------------------------------------------------------------------------------
tcp_communicator_win::tcp_communicator_win( const char *name_rus, const char *name_eng ):
    tcp_communicator(),
    netOK( 0 )
    {
    // Задаем таймаут.
    tv.tv_sec  = 0;
    tv.tv_usec = 10000; // 0.01 сек.

    sin_len = sizeof( ssin );
    strncpy( host_name_rus, name_rus, TC_MAX_HOST_NAME );
    strncpy( host_name_eng, name_eng, TC_MAX_HOST_NAME );

    net_init();

    if ( G_DEBUG )
        {
        char tmp_host_name[ TC_MAX_HOST_NAME + 1 ] = { 0 };
        gethostname( tmp_host_name, TC_MAX_HOST_NAME );
        printf ( "Host name - \"%s\".\n", tmp_host_name );
        hostent *server = gethostbyname( tmp_host_name );
        if ( server )
            {
            if ( server->h_addr_list[ 0 ] )
                {
                struct in_addr addr;
                addr.s_addr = *( u_long * ) ( server->h_addr_list[ 0 ] );
                printf( "Host socket - \"%s\":%d.\n", inet_ntoa( addr ), port );
                }
            }
        else
            {
            printf( "Ошибка получения адреса сервера: %s\n",
                WSA_Last_Err_Decode() );
            }

        printf ( "PAC name  - \"%s\" (\"%s\").\n", host_name_rus, host_name_eng );
        }

    glob_last_transfer_time = get_sec();
    }
//------------------------------------------------------------------------------
void tcp_communicator_win::killsockets()
    {
    FD_ZERO( &rfds ); /* clear FD set */

    for ( u_int i = 0; i < sst.size(); i++ )
        {
        if ( sst[ i ].active )
            {
            sst[ i ].clID = -1;
            shutdown( sst[ i ].socket, SD_BOTH );
            closesocket( sst[ i ].socket );
            }
        }

    sst.clear();
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int tcp_communicator_win::net_init()
    {
    WSAData tmp_WSA_data;

    if ( WSAStartup( 0x202, &tmp_WSA_data ) )
        {
        printf( "Ошибка инициализации сетевой библиотеки: %s\n",
            WSA_Last_Err_Decode() );
        return -1;
        }

    int type     = SOCK_STREAM;
    int protocol = 0;        /* всегда 0 */
    //-Cоздание мастер-сокета.
    master_socket = socket( PF_INET, type, protocol );

    if ( master_socket < 0 )
        {
        if ( G_DEBUG )
            {
            printf( "tcp_communicator_windows:net_init() - can't create master socket: %s\n",
                WSA_Last_Err_Decode() );
            }
        return -4;
        }

    if ( G_DEBUG )
        {
        printf( "tcp_communicator_windows:net_init() - master socket [ %d ] created.\n",
            master_socket );
        }

    //-Переводим в неблокирующий режим.
    u_long mode = 0;
    int res = ioctlsocket( master_socket, FIONBIO, &mode );
    if ( res == SOCKET_ERROR )
        {
        printf( "tcp_communicator_windows:net_init() - ошибка  вызова  ioctlsocket: %s\n",
            WSA_Last_Err_Decode() );
        closesocket( master_socket );
        return -1;
        }

    //-Адресация мастер-сокета.
    socket_state master_socket_state;
    memset( &master_socket_state.sin, 0, sizeof( master_socket_state.sin ) );
    master_socket_state.sin.sin_family 	    = AF_INET;
    master_socket_state.sin.sin_addr.s_addr = INADDR_ANY;
    master_socket_state.sin.sin_port 		= htons( port );
    master_socket_state.socket              = master_socket;

    master_socket_state.active      = 1; // мастер-сокет всегда активный.
    master_socket_state.is_listener = 1; // сокет является слушателем.
    master_socket_state.evaluated   = 0;

    sst.push_back( master_socket_state );

    //TODO. Для win версии это отключено, чтобы явно возникала ошибка при
    //запуске двух и более экземпляров программы. Для linux надо изучить
    //как данный параметр влияет на работу с сокетом. При необходимости
    //реализовать проверку на максимальное ожидание запросов от сервера.
    //
    //const int on = 1;
    //if ( setsockopt( master_socket, SOL_SOCKET, SO_REUSEADDR,
    //    ( char* ) &on, sizeof( on ) ) )
    //    {
    //    printf( "tcp_communicator_windows:net_init() - ошибка  вызова  setsockopt: %s\n",
    //        WSA_Last_Err_Decode() );
    //    closesocket( master_socket );
    //    return -5;
    //    }

    //-Привязка сокета.
    int err = bind( master_socket, ( struct sockaddr * )
        & master_socket_state.sin, sizeof( master_socket_state.sin ) );
    if ( err < 0 )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_BIND_F,
            0 );                                        //Мастер сокет.

        closesocket( master_socket );
        return -5;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_BIND_F,
            0 );
        }

    err = listen( master_socket, QLEN ); // Делаем мастер-сокет слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        closesocket( master_socket );
        if ( G_DEBUG )
            {
            printf( "tcp_communicator_windows:net_init() - listen: %s\n",
                WSA_Last_Err_Decode() );
            }
        return -6;
        }

    int val = 1;
    setsockopt( master_socket, SOL_SOCKET, SO_REUSEADDR,
        ( char* ) &val, sizeof( val ) );
#ifdef MODBUS
    // Создание серверного сокета modbus_socket.
    err = modbus_socket = socket ( PF_INET, type, protocol );

    if ( G_DEBUG )
        {
        printf( "tcp_communicator_windows:net_init() - modbus socket created. Has number %d\n\r",
            modbus_socket );
        }

    if ( modbus_socket < 0 )
        {
        if ( G_DEBUG )
            {
            perror( "tcp_communicator_windows:net_init() - can't create modbus socket" );
            }

        return -4;
        }
    // Адресация modbus_socket сокета.
    socket_state modbus_socket_state;
    memset( &modbus_socket_state.sin, 0, sizeof ( modbus_socket_state.sin ) );
    modbus_socket_state.sin.sin_family 	    = AF_INET;
    modbus_socket_state.sin.sin_addr.s_addr = 0;
    modbus_socket_state.sin.sin_port 		= htons ( port_modbus ); // Порт.
    modbus_socket_state.socket = modbus_socket;

    modbus_socket_state.active      = 1;
    modbus_socket_state.is_listener = 1;
    modbus_socket_state.evaluated   = 0;

    sst.push_back( modbus_socket_state );

    err = bind( modbus_socket, ( struct sockaddr * ) & modbus_socket_state.sin,
        sizeof ( modbus_socket_state.sin ) );	   // Привязка сокета.
    if ( err < 0 )
        {
        if ( G_DEBUG )
            {
            printf( "tcp_communicator_windows:net_init() - can't bind modbus socket to port %d : %s\n",
                10502, WSA_Last_Err_Decode() );
            }

        closesocket( modbus_socket );
        return -5;
        }
    err = listen( modbus_socket, QLEN ); // Делаем слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        closesocket( modbus_socket );
        if ( G_DEBUG )
            {
            perror( "tcp_communicator_windows:net_init() - listen" );
            }
        return -6;
        }
#endif //MODBUS

    netOK = 1;
    return 0;
    }
//------------------------------------------------------------------------------
void tcp_communicator_win::net_terminate()
    {
    killsockets();
    netOK = 0;
    }
//------------------------------------------------------------------------------
tcp_communicator_win::~tcp_communicator_win()
    {
    net_terminate();
    WSACleanup();
    }
//------------------------------------------------------------------------------
int tcp_communicator_win::evaluate()
    {
    // Проверка связи с сервером.
    if ( get_sec() - glob_last_transfer_time > 5 )
        {
        if ( glob_cmctr_ok )
            {
            glob_cmctr_ok = 0;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_EASYSERVER,
                PAC_critical_errors_manager::AS_EASYSERVER );
            }
        }
    else
        {
        if ( glob_cmctr_ok == 0 )
            {
            glob_cmctr_ok = 1;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_EASYSERVER,
                PAC_critical_errors_manager::AS_EASYSERVER );
            }
        }
    // Проверка связи с сервером.-!>

    // Инициализация сети, при необходимости.
    if ( !netOK )
        {
        net_init();
        if ( !netOK ) return -100;
        }
    // Инициализация сети, при необходимости.-!>

    debugger_cycle = !debugger_cycle;
    int count_cycles = 0;
    while ( count_cycles < max_cycles )
        {
        /* service loop */
        count_cycles++;

        FD_ZERO( &rfds );
        for ( u_int i = 0; i < sst.size(); i++ )
            {
            if ( sst[ i ].active &&
                sst[ i ].is_listener &&
                !sst[ i ].evaluated )
                {
                //- Re-join active sockets.
                FD_SET( sst[ i ].socket, &rfds );
                }
            }

        //Добавляем асинхронные сокеты в список прослушки
        for ( auto it = clients->begin(); it != clients->end(); )
            {
            // A client can disconnect while its asynchronous request is queued.
            // Remove stale requests before passing their sockets to select().
            if ( it->second->get_connected_state() != tcp_client::ACS_CONNECTED ||
                it->first != it->second->get_socket() )
                {
                it = clients->erase( it );
                continue;
                }
            FD_SET( it->second->get_socket(), &rfds);
            ++it;
            }

        //-Ждём события в одном из сокетов.
        if ( rfds.fd_count == 0 ) break;
        timeval ready_timeout{};
        rc = select( 0/*Не учитывается*/, &rfds, NULL, NULL, &ready_timeout );

        if ( rc < 0 )
            {
            if ( G_DEBUG )
                {
                printf( "Ошибка select: %s\n", WSA_Last_Err_Decode() );
                }

            net_terminate();
            WSACleanup();
            return -1;
            }

        for ( u_int i = 0; i < sst.size(); i++ )  /* scan all possible sockets */
            {
            // Поступил новый запрос на соединение.
            if ( FD_ISSET ( sst[ i ].socket, &rfds ) )
                {
#ifndef MODBUS
                if ( sst[ i ].socket == master_socket )
#else
                if ( sst[ i ].socket == master_socket ||
                    sst[ i ].socket == modbus_socket )
#endif
                    {
                    /* master socket */
                    memset( &ssin, 0, sizeof ( ssin ) );
                    int slave_socket = accept ( sst[ i ].socket,
                        ( struct sockaddr * ) &ssin, &sin_len );

                    if ( slave_socket <= 0 )    // Ошибка.
                        {
                        if ( G_DEBUG )
                            {
                            printf( "Ошибка accept(): %s\n",
                                WSA_Last_Err_Decode() );
                            }
                        continue;
                        }
                    const int no_delay = 1;
                    setsockopt( slave_socket, IPPROTO_TCP, TCP_NODELAY,
                        reinterpret_cast<const char*>( &no_delay ), sizeof( no_delay ) );
                    // Установка режима сокета.
                    u_long mode = 0;
                    if ( ioctlsocket( slave_socket, FIONBIO, &mode ) == SOCKET_ERROR )
                        {
                        printf( "Ошибка перевода клиентского сокета в неблокирующий режим: %s\n",
                            WSA_Last_Err_Decode() );
                        // Ошибка, разрушаем сокет.
                        shutdown( slave_socket, SD_BOTH );
                        closesocket( slave_socket );
                        continue;
                        }
                    if ( G_DEBUG )
                        {
#ifndef MODBUS
                        // Определение имени клиента.
                        hostent *client = gethostbyaddr( ( char* ) &ssin.sin_addr, 4, AF_INET );

                        if ( client )
                            {
                            printf( "Accepted connection on %d socket from %s [ %s ].\n",
                                slave_socket, inet_ntoa( ssin.sin_addr ), client->h_name  );
                            }

                        else
                            {
                            printf( "Error getting client name. " );
                            printf( "Accepted connection on %d socket from %s.\n",
                                slave_socket, inet_ntoa( ssin.sin_addr ) );
                            }
#else
                        printf( "Accepted connection on %d socket from %s.\n",
                            slave_socket, inet_ntoa( ssin.sin_addr ) );
#endif //  MODBUS
                        }

#ifdef MODBUS
                    if ( sst[ i ].socket != modbus_socket )
                        {
#endif
                        char Message1[] = "PAC accept";
                        send( slave_socket, Message1, strlen ( Message1 ), 0 );
#ifdef MODBUS
                        }
#endif
                    socket_state slave_socket_state;
                    slave_socket_state.active = 1;
                    slave_socket_state.init   = 1;
                    slave_socket_state.is_listener = 1;
                    slave_socket_state.evaluated = 0;
                    slave_socket_state.socket = slave_socket;

                    memcpy( &slave_socket_state.sin, &ssin, sin_len );

                    sst.push_back( slave_socket_state );
                    }
                else         /* slave socket */
                    {
                    if ( !frame_ready( sst[ i ].socket ) )
                        {
                        sst[ i ].evaluated = 1;
                        continue;
                        }
                    do_echo ( i );
                    if ( !debugger_cycle ) glob_last_transfer_time = get_sec();
                    }
                }
            }
        //проверка асинхронных сокетов на предмет поступления данных
        for (std::map<int, tcp_client*>::iterator it = clients->begin(); it != clients->end();)
            {
            int is_removed = 0;
            if (FD_ISSET(it->second->get_socket(), &rfds)) //если есть событие на сокете
                {
                if ( int err = in_buffer_count = recvtimeout(
                    it->second->get_socket(),
                    reinterpret_cast<unsigned char*>( it->second->buff ),
                    it->second->buff_size, 1, 0 );
                    err <= 0) //Ошибка чтения
                    {
                    it->second->Disconnect();
                    it->second->set_async_result(it->second->AR_SOCKETERROR);
                    }
                else //Получены данные
                    {
                    it->second->set_async_result(in_buffer_count);
                    }
                is_removed = 1;
                }
            else //проверяем на таймаут
                {
                if (get_delta_millisec(it->second->async_queued) > it->second->async_timeout)
                    {
                    it->second->Disconnect();
                    it->second->set_async_result(it->second->AR_TIMEOUT);
                    is_removed = 1;
                    }
                }

            if (is_removed)
                {
                clients->erase(it++);
                }
            else
                {
                it++;
                }
            }

        // Even without socket events, pending requests must expire.
        if ( 0 == rc ) break;
        }  /* service loop */

    for ( u_int i = 0; i < sst.size(); i++ )
        {
        sst[ i ].evaluated = 0;
        }

    return 0;
    }
//------------------------------------------------------------------------------
int tcp_communicator_win::do_echo( int idx )
    {
    socket_state &sock_state = sst[ idx ];

    FD_CLR( sock_state.socket, &rfds );

    int err = 0, res;

    if ( sock_state.init )         /* socket is just initiated */
        {
        sock_state.init = 0;

#ifdef  MODBUS
        return err;
#endif // MODBUS
        }

    sock_state.evaluated = 1;
    memset( buf, 0, BUFSIZE );

    // Полный пакет уже проверен frame_ready; чтение без ожидания.
    err = in_buffer_count = recvtimeout( sock_state.socket, buf, incoming_frame_size, 0, 0 );

    if ( err <= 0 )               /* read error */
        {
        if ( G_DEBUG )
            {
            switch ( err )
                {
                case 0:
                    printf( "Socket %d->\"%s\" was closed.\n",
                        sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ) );
                    break;

                case -1:
                    printf( "Socket %d->\"%s\" disconnected on read try : %s\n",
                        sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ),
                        WSA_Last_Err_Decode() );
                    break;

                case -2:
                    printf( "Socket %d->\"%s\" disconnected on read try - timeout.\n",
                        sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ) );
                    break;

                default:
                    printf( "Socket %d->\"%s\" disconnected on read try : %s\n",
                        sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ),
                        WSA_Last_Err_Decode() );
                    break;
                }
            }

        shutdown( sock_state.socket, SD_BOTH );
        closesocket( sock_state.socket );
        sst.erase( sst.begin() + idx, sst.begin() + idx + 1 );
        return err;
        }

    if ( G_DEBUG )
        {
        if ( in_buffer_count > max_buffer_use )
            {
            max_buffer_use = in_buffer_count + in_buffer_count / 10;
            printf( "Max buffer use %u\n", max_buffer_use );
            }
        }

    //Структура полученных данных.
    //buff[0] = 's';
    //buff[1] = Service_ID;        // C_SERVICE_N = 1
    //buff[2] = 1;                 // FRAME_SINGLE.
    //buff[3] = ++pidx;            // Идентификатор пакета.
    //buff[4] = ( char ) ( length >> 8 );
    //buff[5] = length & 0xFF;
    //memcpy(buff+6,data,length);

    net_id = buf[ 0 ];
    pidx = buf[ 3 ];

    if ( net_id == 's' && buf[ 1 ] < TC_MAX_SERVICE_NUMBER && services[ buf[ 1 ] ] != NULL &&
        ( buf[ 2 ] + buf[ 3 ] != 0 ) )
        {
        switch ( buf[ 2 ] )
            {
            case FRAME_SINGLE:
                res = services[ buf[ 1 ] ] (
                    ( u_int ) ( buf[ 4 ] * 256 + buf[ 5 ] ), buf + 6, buf + 5 );

                if ( res == 0 )
                    {
                    _AknOK();
                    }
                else
                    {
                    _AknData( res );
                    if ( G_DEBUG )
                        {
                        if ( ( unsigned int ) res > max_buffer_use )
                            {
                            max_buffer_use = res + res / 10;
                            printf( "Max buffer use %u\n", res );
                            }
                        }
                    }
                break;

            default:
                _ErrorAkn( ERR_WRONG_CMD );
                if ( G_DEBUG )
                    {
                    printf( "Wrong command received on socket %d->\"%s\"\n",
                        sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ) );
                    }
                break;
            }
        }
    else
        {
        if ( ( services[ 15 ] != NULL ) && ( 0 == buf[ 2 ] + buf[ 3 ] ) ) //MODBUS
            {
            res = services[ 15 ] ( ( u_int ) ( buf[ 4 ] * 256 + buf[ 5 ] ),
                buf + 6,
                buf + 6 );
            if ( res > 0 )
                {
                buf[ 4 ] = ( res >> 8 ) & 0xFF;
                buf[ 5 ] = res & 0xFF;
                in_buffer_count = res + 6;
                }
            sock_state.evaluated = 0;
            }
        else
            {
            _ErrorAkn( ERR_WRONG_SERVICE );
            if ( G_DEBUG )
                {
                printf( "No such service %d at socket %d->\"%s\"\n",
                    buf[ 1 ], sock_state.socket,
                    inet_ntoa( sock_state.sin.sin_addr ) );
                }
            }
        }

    err = send( sock_state.socket, ( char* ) buf, in_buffer_count, 0 );

    if ( err <= 0 )               /* write error */
        {
        if ( G_DEBUG )
            {
            printf( "Socket %d->\"%s\" disconnected on write try : %s\n",
                sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ),
                WSA_Last_Err_Decode() );
            }

        shutdown( sock_state.socket, 0 );
        closesocket( sock_state.socket );
        sst.erase( sst.begin() + idx, sst.begin() + idx + 1 );
        return err;
        }

    return err;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
