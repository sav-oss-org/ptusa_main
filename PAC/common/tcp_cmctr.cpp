#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif
#include <cerrno>

#include "tcp_cmctr.h"
#include "tcp_client.h"
#include "log.h"
#include "PAC_info.h"

#ifdef WIN_OS
#include "w_tcp_cmctr.h"
#endif // WIN_OS

#ifdef LINUX_OS
#include "l_tcp_cmctr.h"
#endif

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
    // A descriptor at or above FD_SETSIZE does not fit into the fd_set
    // bitmap, so reject it and report the same result as a failed select().
    if ( s < 0 || s >= FD_SETSIZE )
        {
        G_LOG->warning(
            "Network device : s%d is out of select() range"
            " ( FD_SETSIZE = %d ).",
            s, FD_SETSIZE );
        return false;
        }

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
	    instance = new tcp_communicator_win( name_rus, name_eng );
#endif // WIN_OS

#ifdef LINUX_OS
	    instance = new tcp_communicator_linux( name_rus, name_eng );
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
