#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif
#include <cerrno>

#include "tcp_cmctr.h"
#include "tcp_client.h"
#include "log.h"
#include "PAC_info.h"

#include "cmn_tcp_cmctr.h"

auto_smart_ptr < tcp_communicator > tcp_communicator::instance = 0;
int tcp_communicator::master_socket = 0;
int tcp_communicator::port = 10000;
int tcp_communicator::port_modbus = 10502;
#ifdef PTUSA_TEST
bool tcp_communicator::is_init = false;
#endif //PTUSA_TEST
//------------------------------------------------------------------------------
tcp_communicator::tcp_communicator(): in_buffer_count( 0 ), pidx( 0 ), net_id( 0 )
    {
    max_cycles          = 10;
    glob_cmctr_ok       = 1;
    for ( int i = 0; i < TC_MAX_SERVICE_NUMBER; i++ ) services[ i ] = nullptr;

    clients = new std::map<int, tcp_client*>();
    }
//------------------------------------------------------------------------------
tcp_communicator::srv_ptr tcp_communicator::reg_service( u_char srv_id,
    srv_ptr fk )
    {
    if ( services[ srv_id ] == nullptr)
        {
        services[ srv_id ] = fk;
        return nullptr;
        }
    else
        {
        return services[ srv_id ];
        }
    }
//------------------------------------------------------------------------------
void tcp_communicator::_ErrorAkn( u_char error )
    {
    buf[ 0 ] = net_id;
    buf[ 1 ] = AKN_ERR;
    buf[ 2 ] = pidx;
    buf[ 3 ] = 0;
    buf[ 4 ] = 1;
    buf[ 5 ] = error;
    in_buffer_count = 6;
    }
//------------------------------------------------------------------------------
void tcp_communicator::_AknData( u_long len )
    {
    buf[ 0 ] = net_id;
    buf[ 1 ] = AKN_OK;
    buf[ 2 ] = pidx;
    buf[ 3 ] = ( len >> 8 ) & 0xFF;
    buf[ 4 ] = len & 0xFF;
    in_buffer_count = len + 5;
    }
//------------------------------------------------------------------------------
void tcp_communicator::_AknOK()
    {
    buf[ 0 ] = net_id;
    buf[ 1 ] = AKN_OK;
    buf[ 2 ] = pidx;
    buf[ 3 ] = 0;
    buf[ 4 ] = 0;
    in_buffer_count = 5;
    }
//------------------------------------------------------------------------------
tcp_communicator* tcp_communicator::get_instance()
    {
    return instance;
    }
//------------------------------------------------------------------------------
char* tcp_communicator::get_host_name_rus()
    {
    return host_name_rus;
    }
//------------------------------------------------------------------------------
char* tcp_communicator::get_host_name_eng()
    {
    return host_name_eng;
    }
//------------------------------------------------------------------------------
tcp_communicator::~tcp_communicator()
    {
    delete clients;
    }
//------------------------------------------------------------------------------
void tcp_communicator::set_port( int new_port, int new_port_modbus )
    {
    port = new_port;
    port_modbus = new_port_modbus;
    }
//------------------------------------------------------------------------------
int tcp_communicator::get_port()
    {
    return port;
    }
//------------------------------------------------------------------------------
int tcp_communicator::get_modbus_port()
    {
    return port_modbus;
    }
//------------------------------------------------------------------------------
int tcp_communicator::get_master_socket()
    {
    return master_socket;
    }
//------------------------------------------------------------------------------
bool tcp_communicator::checkBuff( int s )
    {
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
//------------------------------------------------------------------------------
int tcp_communicator::recvtimeout( int s, u_char* buf,
    int len, long int sec, long int usec, const char* IP, const char* name,
    stat_time* stat )
    {

    //Network performance info.
    if ( stat )
        {
        auto timeInfo_ = get_time();

        //Once per hour writes performance info.
        if ( stat->print_cycle_last_h != timeInfo_.tm_hour )
            {
            u_int t =
                G_PAC_INFO()->par[ PAC_info::P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ];

            stat->print_cycle_last_h = timeInfo_.tm_hour;

            u_long avg_time = stat->all_time / stat->cycles_cnt;
            G_LOG->debug( R"(Network performance : recv : s%d->"%s":"%s" )"
                "avg = %lu, min = %lu, max = %lu, tresh = %u (ms).",
                s, name, IP, avg_time, stat->min_iteration_cycle_time,
                stat->max_iteration_cycle_time, t );

            if ( t < avg_time )
                {
                G_LOG->alert( R"(Network performance : recv : s%d->"%s":"%s" )"
                    "avg %lu > tresh %u (ms).",
                    s, name, IP, avg_time, t );
                }

            stat->clear();
            }
        }

    errno = 0;

    // Настраиваем  file descriptor set.
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( s, &fds );

    // Настраиваем время на таймаут.
    timeval rec_tv;
    rec_tv.tv_sec = sec;
    rec_tv.tv_usec = usec;

    //Network performance info.
    uint32_t start_time = get_millisec();

    auto update_stat_time = [&]()
        {
        if ( !stat ) return;

        auto select_wait_time = get_delta_millisec( start_time );
        stat->cycles_cnt++;
        stat->all_time += select_wait_time;

        if ( select_wait_time > stat->max_iteration_cycle_time )
            {
            stat->max_iteration_cycle_time = select_wait_time;
            }
        if ( select_wait_time < stat->min_iteration_cycle_time )
            {
            stat->min_iteration_cycle_time = select_wait_time;
            }
        };

    // Ждем таймаута или полученных данных.
    int n = select( s + 1, &fds, nullptr, nullptr, &rec_tv );

    if ( 0 == n )
        {
        G_LOG->error(
            R"(Network device : s%d->"%s":"%s")"
            " disconnected on select read try : timeout (%ld ms).",
            s, name, IP, sec * 1000 + usec / 1000 );

        update_stat_time();
        return -2;  // timeout!
        }

    if ( -1 == n )
        {
        G_LOG->error(
            R"(Network device : s%d->"%s":"%s")"
            " disconnected on select read try : %s"
#ifndef WIN_OS
            "."
#endif
            ,
            s, name, IP,
#ifdef WIN_OS
            WSA_Last_Err_Decode()
#else
            strerror( errno )
#endif // WIN_OS
            );

        update_stat_time();
        return -1; // error
        }

    // Данные должны быть здесь, поэтому делаем обычный recv().
    auto res = recv( s, reinterpret_cast<char*>( buf ), len, 0 );

    if ( 0 == res )
        {
        G_LOG->warning(
            R"(Network device : s%d->"%s":"%s")"
            " was closed.",
            s, name, IP );
        }

    if ( res < 0 )
        {
        G_LOG->error(
            R"(Network device : s%d->"%s":"%s")"
            " disconnected on read try : %s"
#ifndef WIN_OS
            "."
#endif
            ,
            s, name, IP,
#ifdef WIN_OS
            WSA_Last_Err_Decode()
#else
            strerror( errno )
#endif // WIN_OS
        );
        }

    update_stat_time();
    return
#ifdef WIN_OS
        res;
#else
        static_cast<int>( res );
#endif
    };
//------------------------------------------------------------------------------
void tcp_communicator::init_instance( const char *name_rus, const char *name_eng )
    {
#ifdef PTUSA_TEST
    if ( !is_init )
       {
       is_init = true;
#endif //PTUSA_TEST
#ifdef WIN_OS
	    instance = new tcp_communicator_impl( name_rus, name_eng );
#endif // WIN_OS

#ifdef LINUX_OS
	    instance = new tcp_communicator_impl( name_rus, name_eng );
#endif
#ifdef PTUSA_TEST
        }
#endif //PTUSA_TEST
    }
//------------------------------------------------------------------------------
#ifdef PTUSA_TEST
void tcp_communicator::clear_instance()
    {
    if ( instance )
        {
        instance.free();
        is_init = false;
        }
    }
#endif //PTUSA_TEST
//------------------------------------------------------------------------------
int tcp_communicator::add_async_client( tcp_client* client )
    {
    clients[0][client->get_socket()] = client;
    client->async_queued = get_millisec();
    return 0;
    }
//------------------------------------------------------------------------------
int tcp_communicator::remove_async_client( tcp_client* client )
    {
    std::map<int, tcp_client*>::iterator it = clients->find(client->get_socket());
    if (it != clients->end())
        {
        clients->erase(it);
        }
    return 0;
    }
//------------------------------------------------------------------------------
int tcp_communicator::sendall( int sockfd, u_char* buf, int len,
    int sec, int usec, const char* IP, const char* name,
    stat_time* stat )
    {
    // Network performance info.
    if ( stat )
        {
        auto timeInfo_ = get_time();

        // Once per hour writes performance info.
        if ( stat->print_cycle_last_h != timeInfo_.tm_hour )
            {
            u_int t =
                G_PAC_INFO()->par[
                    PAC_info::P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ];

            stat->print_cycle_last_h = timeInfo_.tm_hour;

            u_long avg_time = stat->all_time / stat->cycles_cnt;
            G_LOG->debug(
                R"(Network performance : send : s%d->"%s":"%s" )"
                "avg = %lu, min = %lu, max = %lu, tresh = %u (ms).",
                sockfd, name, IP, avg_time,
                stat->min_iteration_cycle_time,
                stat->max_iteration_cycle_time, t );

            if ( t < avg_time )
                {
                G_LOG->alert(
                    R"(Network performance : send : s%d->"%s":"%s" )"
                    "avg %lu > tresh %u (ms).",
                    sockfd, name, IP, avg_time, t );
                }

            stat->clear();
            }
        }

    u_char *p = buf;

    // Настраиваем file descriptor set.
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( sockfd, &fds );

    // Настраиваем время на таймаут.
    timeval rec_tv;
    rec_tv.tv_sec  = sec;
    rec_tv.tv_usec = usec;

    // Network performance info.
    uint32_t st_time = get_millisec();

    int res = 0;

    for ( int i = len; i > 0; )
        {
        // Ждем таймаута или возможности отсылки данных.
        res = select( sockfd + 1, NULL, &fds, NULL, &rec_tv );

        if ( 0 == res )
            {
            G_LOG->error(
                R"(Network device : s%d->"%s":"%s")"
                " disconnected on select write try : timeout (%d ms).",
                sockfd, name, IP, sec * 1000 + usec / 1000 );
            return -2; // timeout!
            }

        if ( -1 == res )
            {
            G_LOG->error(
                R"(Network device : s%d->"%s":"%s")"
                " disconnected on select write try : %s"
#ifndef WIN_OS
                "."
#endif
                ,
                sockfd, name, IP,
#ifdef WIN_OS
                WSA_Last_Err_Decode()
#else
                strerror( errno )
#endif // WIN_OS
                );
            return -1; // error
            }

        int n = static_cast<int>( send( sockfd,
            reinterpret_cast<char*>( p ), i,
#ifdef WIN_OS
            0 ) );
#else
            MSG_NOSIGNAL ) );
#endif

        if ( n < 0 )
            {
            G_LOG->error(
                R"(Network device : s%d->"%s":"%s")"
                " disconnected on write try : %s"
#ifndef WIN_OS
                "."
#endif
                ,
                sockfd, name, IP,
#ifdef WIN_OS
                WSA_Last_Err_Decode()
#else
                strerror( errno )
#endif // WIN_OS
                );
            res = -3; // send error
            break;
            }

#ifndef WIN_OS
        usleep( 1 );
#endif
        i -= n;
        p += n;
        }

    uint32_t select_wait_time = get_delta_millisec( st_time );

    if ( stat )
        {
        stat->cycles_cnt++;
        stat->all_time += select_wait_time;

        if ( select_wait_time > stat->max_iteration_cycle_time )
            {
            stat->max_iteration_cycle_time = select_wait_time;
            }
        if ( select_wait_time < stat->min_iteration_cycle_time )
            {
            stat->min_iteration_cycle_time = select_wait_time;
            }
        }

    return res;
    }
//------------------------------------------------------------------------------
