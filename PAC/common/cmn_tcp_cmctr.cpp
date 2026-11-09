#ifdef WIN_OS
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <time.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#endif

#include "cmn_tcp_cmctr.h"
#include "PAC_err.h"
#include "tcp_client.h"
#include "log.h"

unsigned int max_buffer_use = 0;

//------------------------------------------------------------------------------
#ifdef WIN_OS
const char* WSA_Last_Err_Decode();
#endif // WIN_OS

//------------------------------------------------------------------------------
tcp_communicator_impl::tcp_communicator_impl( const char *name_rus,
    const char *name_eng ) : tcp_communicator(),
    sst(), netOK( 0 )
    {
    // Задаем таймаут.
    tv.tv_sec  = 0;
#ifdef WIN_OS
    tv.tv_usec = 10000; // 0.01 сек.
#else
    tv.tv_usec = 1000;
#endif

    sin_len = sizeof( ssin );
    strncpy( host_name_rus, name_rus, TC_MAX_HOST_NAME );
    strncpy( host_name_eng, name_eng, TC_MAX_HOST_NAME );

    if ( G_DEBUG )
        {
        printf ( "PAC name \"%s\" (\"%s\").\n", host_name_rus, host_name_eng );
        }

    glob_last_transfer_time = get_millisec();
    net_init();
    }
//------------------------------------------------------------------------------
void tcp_communicator_impl::killsockets()
    {
    FD_ZERO( &rfds ); /* clear FD set */

    for ( u_int i = 0; i < sst.size(); i++ )
        {
        if ( sst[ i ].active )
            {
#ifdef WIN_OS
            shutdown( sst[ i ].socket, SD_BOTH );
            closesocket( sst[ i ].socket );
#else
            shutdown( sst[ i ].socket, SHUT_RDWR );
            close( sst[ i ].socket );
#endif
            }
        }
    sst.clear();
    }
//------------------------------------------------------------------------------
int tcp_communicator_impl::net_init()
    {
#ifdef WIN_OS
    WSAData tmp_WSA_data;

    if ( WSAStartup( 0x202, &tmp_WSA_data ) )
        {
        printf( "Ошибка инициализации сетевой библиотеки: %s\n",
            WSA_Last_Err_Decode() );
        return -1;
        }
#else
    errno = 0;
#endif

    int type     = SOCK_STREAM;
    int protocol = 0; /* всегда 0 */

    // Создание мастер-сокета.
    int err = master_socket = socket( PF_INET, type, protocol );

    if ( G_DEBUG )
        {
        printf( "tcp_communicator_impl:net_init() - master socket created."
            " Has number %d:%d.\n", master_socket, port );
        }

    if ( master_socket < 0 )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_SOCKET_F,
            0 );
        return -4;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_SOCKET_F,
            0 );
        }

    // Переводим в неблокирующий режим.
#ifdef WIN_OS
    u_long mode = 0;
    if ( ioctlsocket( master_socket, FIONBIO, &mode ) == SOCKET_ERROR )
        {
        printf( "tcp_communicator_impl:net_init() - ioctlsocket error: %s\n",
            WSA_Last_Err_Decode() );
        closesocket( master_socket );
        return -1;
        }
#else
    fcntl( master_socket, F_SETFL, O_NONBLOCK );
#endif

    // Адресация мастер-сокета.
    socket_state master_socket_state;
    memset( &master_socket_state.sin, 0, sizeof( master_socket_state.sin ) );
    master_socket_state.socket = master_socket;
    master_socket_state.sin.sin_family      = AF_INET;
    master_socket_state.sin.sin_addr.s_addr = INADDR_ANY;
    master_socket_state.sin.sin_port        = htons( port );
    master_socket_state.ismodbus            = 0;

#ifndef WIN_OS
    if ( const int on = 1; setsockopt( master_socket, SOL_SOCKET,
        SO_REUSEADDR, &on, sizeof( on ) ) )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_SETSOCKOPT_F,
            0 );
        close( master_socket );
        return -5;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_SETSOCKOPT_F,
            0 );
        }
#endif

    // Привязка сокета.
    err = bind( master_socket,
        reinterpret_cast<struct sockaddr*>( &master_socket_state.sin ),
        sizeof( master_socket_state.sin ) );
    if ( err < 0 )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_BIND_F,
            0 );
#ifdef WIN_OS
        closesocket( master_socket );
#else
        close( master_socket );
#endif
        return -5;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_BIND_F,
            0 );
        }

    // Делаем мастер-сокет слушателем.
    err = listen( master_socket, QLEN );
    if ( type == SOCK_STREAM && err < 0 )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_LISTEN_F,
            0 );
#ifdef WIN_OS
        closesocket( master_socket );
#else
        close( master_socket );
#endif
        return -6;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_LISTEN_F,
            0 );
        }

    master_socket_state.active      = 1; // мастер-сокет всегда активный.
    master_socket_state.is_listener = 1; // сокет является слушателем.
    master_socket_state.evaluated   = 0;

    sst.push_back( master_socket_state );

    // Создание серверного сокета modbus_socket.
    err = modbus_socket = socket( PF_INET, type, protocol );

    if ( G_DEBUG )
        {
        printf( "tcp_communicator_impl:net_init() - modbus socket created."
            " Has number %d:%d.\n", modbus_socket, port_modbus );
        }

    if ( modbus_socket < 0 )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_SOCKET_F,
            1 );
        return -4;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_SOCKET_F,
            1 );
        }

    // Адресация modbus_socket сокета.
    socket_state modbus_socket_state;
    memset( &modbus_socket_state.sin, 0, sizeof( modbus_socket_state.sin ) );
    modbus_socket_state.socket              = modbus_socket;
    modbus_socket_state.sin.sin_family      = AF_INET;
    modbus_socket_state.sin.sin_addr.s_addr = 0;
    modbus_socket_state.sin.sin_port        = htons( port_modbus );
    modbus_socket_state.ismodbus            = 1;

    // Привязка сокета.
    err = bind( modbus_socket,
        reinterpret_cast<struct sockaddr*>( &modbus_socket_state.sin ),
        sizeof( modbus_socket_state.sin ) );
    if ( err < 0 )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_BIND_F,
            1 );
#ifdef WIN_OS
        closesocket( master_socket );
#else
        close( master_socket );
#endif
        return -5;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_BIND_F,
            1 );
        }

    // Делаем слушателем.
    err = listen( modbus_socket, QLEN );
    if ( type == SOCK_STREAM && err < 0 )
        {
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_LISTEN_F,
            1 );
#ifdef WIN_OS
        closesocket( master_socket );
#else
        close( master_socket );
#endif
        return -6;
        }
    else
        {
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_NET,
            PAC_critical_errors_manager::AS_LISTEN_F,
            1 );
        }

    modbus_socket_state.active      = 1;
    modbus_socket_state.is_listener = 1;
    modbus_socket_state.evaluated   = 0;

    sst.push_back( modbus_socket_state );

    FD_ZERO( &rfds );

    netOK = 1;
    return 0;
    }
//------------------------------------------------------------------------------
void tcp_communicator_impl::net_terminate()
    {
    killsockets();
    netOK = 0;
    }
//------------------------------------------------------------------------------
tcp_communicator_impl::~tcp_communicator_impl()
    {
    net_terminate();
#ifdef WIN_OS
    WSACleanup();
#endif
    }
//------------------------------------------------------------------------------
int tcp_communicator_impl::evaluate()
    {
    // Проверка связи с сервером.
    if ( get_delta_millisec( glob_last_transfer_time ) > 5000L )
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

    int count_cycles = 0;
    while ( count_cycles < max_cycles )
        {
        /* service loop */
        count_cycles++;
#ifndef WIN_OS
        sleep_ms( 1 );
        int max_sock_number = 0;
#endif

        FD_ZERO( &rfds );
        for ( u_int i = 0; i < sst.size(); i++ )
            {
            if ( sst[ i ].active &&
                sst[ i ].is_listener &&
                !sst[ i ].evaluated )
                {
                /* re-join active sockets */
                FD_SET( sst[ i ].socket, &rfds );
#ifndef WIN_OS
                if ( sst[ i ].socket > max_sock_number )
                    {
                    max_sock_number = sst[ i ].socket;
                    }
#endif
                }
            }

        // Добавляем асинхронные сокеты в список прослушки.
        for ( std::map<int, tcp_client*>::iterator it = clients->begin();
            it != clients->end(); ++it )
            {
            FD_SET( it->second->get_socket(), &rfds );
#ifndef WIN_OS
            if ( it->second->get_socket() > max_sock_number )
                {
                max_sock_number = it->second->get_socket();
                }
#endif
            }

        // Ждём события в одном из сокетов.
#ifdef WIN_OS
        rc = select( 0 /* Не учитывается */, &rfds, NULL, NULL, &tv );
#else
        rc = select( max_sock_number + 1, &rfds, NULL, NULL, &tv );
#endif

        if ( 0 == rc ) break; // Ничего не произошло.

        if ( rc < 0 )
            {
            sprintf( G_LOG->msg,
                "Network communication : select socket : %s.",
#ifdef WIN_OS
                WSA_Last_Err_Decode()
#else
                strerror( errno )
#endif
                );
            G_LOG->write_log( i_log::P_ERR );
            continue;
            }

        for ( u_int i = 0; i < sst.size(); i++ )
            {
            // Поступил новый запрос на соединение.
            if ( FD_ISSET( sst[ i ].socket, &rfds ) )
                {
                if ( sst[ i ].socket == master_socket ||
                    sst[ i ].socket == modbus_socket )
                    {
                    /* master socket */
                    memset( &ssin, 0, sizeof( ssin ) );
                    slave_socket = accept( sst[ i ].socket,
                        reinterpret_cast<struct sockaddr*>( &ssin ),
                        &sin_len );

                    if ( slave_socket <= 0 ) // Ошибка.
                        {
                        sprintf( G_LOG->msg,
                            "Network communication : accept socket : %s.",
#ifdef WIN_OS
                            WSA_Last_Err_Decode()
#else
                            strerror( errno )
#endif
                            );
                        G_LOG->write_log( i_log::P_ERR );
                        continue;
                        }

                    if ( sst[ i ].socket != modbus_socket )
                        {
                        char Message1[] = "PAC accept";
                        send( slave_socket, Message1,
                            static_cast<int>( strlen( Message1 ) ),
#ifdef WIN_OS
                            0 );
#else
                            MSG_NOSIGNAL );
#endif
                        }

                    // Установка сокета в неблокирующий режим.
#ifdef WIN_OS
                    u_long nb_mode = 0;
                    if ( ioctlsocket( slave_socket, FIONBIO, &nb_mode )
                        == SOCKET_ERROR )
                        {
                        printf( "Ошибка перевода клиентского сокета в"
                            " неблокирующий режим: %s\n",
                            WSA_Last_Err_Decode() );
                        shutdown( slave_socket, SD_BOTH );
                        closesocket( slave_socket );
                        continue;
                        }
#else
                    if ( fcntl( slave_socket, F_SETFL, O_NONBLOCK ) < 0 )
                        {
                        // Ошибка, разрушаем сокет.
                        shutdown( slave_socket, 0 );
                        close( slave_socket );
                        sprintf( G_LOG->msg,
                            "Network communication : fcntl socket : %s.",
                            strerror( errno ) );
                        G_LOG->write_log( i_log::P_ERR );
                        continue;
                        }
#endif

                    const char *DESCR = "modbus";
                    if ( sst[ i ].socket != modbus_socket )
                        {
                        DESCR = "server";
                        }

                    // Определение имени клиента.
                    if ( hostent* client = gethostbyaddr(
#ifdef WIN_OS
                        reinterpret_cast<char*>( &ssin.sin_addr ),
#else
                        &ssin.sin_addr,
#endif
                        4, AF_INET ); client )
                        {
                        sprintf( G_LOG->msg,
                            "Network communication : accepted %s"
                            " connection : s%d->\"%s\":\"%s\".",
                            DESCR, slave_socket, client->h_name,
                            inet_ntoa( ssin.sin_addr ) );
                        G_LOG->write_log( i_log::P_INFO );
                        }
                    else
                        {
                        sprintf( G_LOG->msg,
                            "Network communication : accepted %s"
                            " connection : s%d->\"?\":\"%s\".",
                            DESCR, slave_socket,
                            inet_ntoa( ssin.sin_addr ) );
                        G_LOG->write_log( i_log::P_INFO );
                        }

                    FD_SET( slave_socket, &rfds );
                    socket_state slave_socket_state;
                    slave_socket_state.socket      = slave_socket;
                    slave_socket_state.active      = 1;
                    slave_socket_state.init        = 1;
                    slave_socket_state.is_listener = 1;
                    slave_socket_state.evaluated   = 0;
                    slave_socket_state.ismodbus    =
                        ( sst[ i ].socket == modbus_socket ) ? 1 : 0;
                    memcpy( &slave_socket_state.sin, &ssin, sin_len );

                    sst.push_back( slave_socket_state );
                    }
                else  /* slave socket */
                    {
                    do_echo( i );
                    glob_last_transfer_time = get_millisec();
                    }
                }
            }

        // Проверка асинхронных сокетов на предмет поступления данных.
        for ( std::map<int, tcp_client*>::iterator it = clients->begin();
            it != clients->end(); )
            {
            int is_removed = 0;
            if ( FD_ISSET( it->second->get_socket(), &rfds ) )
                {
                // Если есть событие на сокете.
                if ( int err = recvtimeout( it->second->get_socket(),
                    reinterpret_cast<unsigned char*>( it->second->buff ),
                    it->second->buff_size,
                    1, 0, it->second->ip, "async client", 0 );
                    err <= 0 ) // Ошибка чтения.
                    {
                    it->second->Disconnect();
                    it->second->set_async_result(
                        it->second->AR_SOCKETERROR );
                    }
                else // Получены данные.
                    {
                    in_buffer_count = err;
                    it->second->set_async_result( in_buffer_count );
                    }
                is_removed = 1;
                }
            else // Проверяем на таймаут.
                {
                if ( get_delta_millisec( it->second->async_queued ) >
                    it->second->async_timeout )
                    {
                    it->second->Disconnect();
                    it->second->set_async_result( it->second->AR_TIMEOUT );
                    is_removed = 1;
                    }
                }

            if ( is_removed )
                {
                clients->erase( it++ );
                }
            else
                {
                it++;
                }
            }

        }  /* service loop */

    for ( u_int i = 0; i < sst.size(); i++ )
        {
        sst[ i ].evaluated = 0;
        }

    return 0;
    }
//------------------------------------------------------------------------------
int tcp_communicator_impl::do_echo( int idx )
    {
    socket_state &sock_state = sst[ idx ];
    FD_CLR( sock_state.socket, &rfds );

    static const char* const SERVER     = "easyserver";
    static const char* const MODBUS_DEV = "modbus device";

    const char *dev_name = SERVER;
    if ( sock_state.ismodbus )
        {
        dev_name = MODBUS_DEV;
        }

    int err = 0, res = 0;

    sock_state.evaluated = 1;
    memset( buf, 0, BUFSIZE );

    if ( sock_state.init )
        {
        sock_state.init = 0;
        if ( sock_state.ismodbus )
            {
            // Ожидаем данные с таймаутом 50 мсек.
            err = in_buffer_count = recvtimeout( sock_state.socket, buf,
                BUFSIZE, 0, 50000L,
                inet_ntoa( sock_state.sin.sin_addr ), dev_name,
                &sock_state.recv_stat );
            if ( err == -2 )
                {
                // Если при подключении на модбас-сокет первый раз данные
                // не пришли, то игнорируем (панель Weintek).
                return 0;
                }
            }
        else
            {
            // Ожидаем данные с таймаутом 300 мсек.
            err = in_buffer_count = recvtimeout( sock_state.socket, buf,
                BUFSIZE, 0, 300000L,
                inet_ntoa( sock_state.sin.sin_addr ), dev_name,
                &sock_state.recv_stat );
            }
        }
    else
        {
        // Ожидаем данные с таймаутом 300 мсек.
        err = in_buffer_count = recvtimeout( sock_state.socket, buf,
            BUFSIZE, 0, 300000L,
            inet_ntoa( sock_state.sin.sin_addr ), dev_name,
            &sock_state.recv_stat );
        }

    if ( err <= 0 ) /* read error */
        {
#ifdef WIN_OS
        shutdown( sock_state.socket, SD_BOTH );
        closesocket( sock_state.socket );
#else
        shutdown( sock_state.socket, 0 );
        close( sock_state.socket );
#endif
        sst.erase( sst.begin() + idx, sst.begin() + idx + 1 );
        return err;
        }

    if ( in_buffer_count > max_buffer_use )
        {
        sprintf( G_LOG->msg,
            "Network performance : (in) buffer use max = %u,"
            " tresh = %u (b).",
            in_buffer_count, BUFSIZE );
        G_LOG->write_log( i_log::P_DEBUG );

        max_buffer_use =
            static_cast<unsigned int>( in_buffer_count + 0.1 * in_buffer_count );
        }

    net_id = buf[ 0 ];
    pidx   = buf[ 3 ];

    if ( net_id == 's' && buf[ 1 ] < TC_MAX_SERVICE_NUMBER &&
        services[ buf[ 1 ] ] != NULL && buf[ 2 ] + buf[ 3 ] != 0 )
        {
        switch ( buf[ 2 ] )
            {
            case FRAME_SINGLE:
                res = services[ buf[ 1 ] ] (
                    static_cast<u_int>( buf[ 4 ] * 256 + buf[ 5 ] ),
                    buf + 6, buf + 5 );

                if ( static_cast<unsigned int>( res ) > max_buffer_use )
                    {
                    sprintf( G_LOG->msg,
                        "Network performance : (out) buffer use max = %u,"
                        " tresh = %u (b).",
                        res, BUFSIZE );
                    G_LOG->write_log( i_log::P_DEBUG );

                    max_buffer_use = static_cast<unsigned int>(
                        res + 0.1 * res );
                    }

                if ( res == 0 )
                    {
                    _AknOK();
                    }
                else
                    {
                    _AknData( res );
                    }
                break;

            default:
                _ErrorAkn( ERR_WRONG_CMD );
                sprintf( G_LOG->msg,
                    "tcp_communicator_impl::do_echo wrong command"
                    " received on socket %d->\"%s\".",
                    sock_state.socket,
                    inet_ntoa( sock_state.sin.sin_addr ) );
                G_LOG->write_log( i_log::P_WARNING );
                break;
            }
        }
    else
        {
        if ( services[ 15 ] != NULL &&
            0 == buf[ 2 ] + buf[ 3 ] ) // MODBUS
            {
            res = services[ 15 ] (
                static_cast<u_int>( buf[ 4 ] * 256 + buf[ 5 ] ),
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
            sprintf( G_LOG->msg,
                "No such service %d at socket %d->\"%s\".",
                buf[ 1 ], sock_state.socket,
                inet_ntoa( sock_state.sin.sin_addr ) );
            G_LOG->write_log( i_log::P_WARNING );
            }
        }

    err = tcp_communicator::sendall( sock_state.socket, buf,
        in_buffer_count, 0, 300000,
        inet_ntoa( sock_state.sin.sin_addr ), dev_name,
        &sock_state.send_stat );

    if ( err <= 0 ) /* write error */
        {
#ifdef WIN_OS
        shutdown( sock_state.socket, SD_BOTH );
        closesocket( sock_state.socket );
#else
        shutdown( sock_state.socket, 0 );
        close( sock_state.socket );
#endif
        sst.erase( sst.begin() + idx, sst.begin() + idx + 1 );
        return err;
        }

    return err;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
