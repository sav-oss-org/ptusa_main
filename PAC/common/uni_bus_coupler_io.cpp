#include <errno.h>

#include "uni_bus_coupler_io.h"
#include "log.h"

#include "fmt/format.h"
#include <cstring>
#include <algorithm>
#include <limits>

#ifdef WIN_OS
const char* WSA_Last_Err_Decode();
#else
extern int errno;
#endif // WIN_OS

namespace
    {
    struct io_phase_scope
        {
        bool& active;
        uni_io_manager::phase_timing& timing;
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        ~io_phase_scope()
            {
            active = false;
            timing.last_us = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start ).count();
            timing.max_us = (std::max)( timing.max_us, timing.last_us );
            timing.total_us += timing.last_us;
            ++timing.cycles;
            }
        };
    }

void uni_io_manager::queue_exchange( io_node* node, int send_size, int expected_size )
    {
    exchange item;
    item.node = node;
    item.send_size = send_size;
    item.expected_size = expected_size;
    for ( size_t i = phase_exchanges.size(); i > 0; --i )
        if ( phase_exchanges[i - 1].node == node ) { item.previous = i - 1; break; }
    std::copy_n( buff, BUFF_SIZE, item.request.begin() );
    phase_exchanges.push_back( item );
    }

void uni_io_manager::prepare_phase( bool writing )
    {
    // Preserve the decoder buffer, including for virtual transport overrides.
    std::array<u_char, BUFF_SIZE> saved;
    std::copy_n( buff, BUFF_SIZE, saved.begin() );
    phase_exchanges.clear();
    for ( unsigned int i = 0; i < nodes_count; ++i )
        {
        auto nd = nodes[i];
        if ( !nd->is_active || ( writing && nd->read_io_error_flag ) ) continue;
        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET )
            {
            if ( writing )
                {
                if ( nd->DO_cnt )
                    {
                    make_wago_do_request( nd );
                    queue_exchange( nd, 13 + ( nd->DO_cnt + 7 ) / 8, 12 );
                    }
                if ( nd->AO_cnt )
                    {
                    make_wago_ao_request( nd );
                    queue_exchange( nd, 13 + nd->AO_size, 12 );
                    }
                }
            else
                {
                if ( nd->DI_cnt )
                    {
                    make_read_request( 0, nd->DI_cnt, 2 );
                    queue_exchange( nd, 12, 9 + ( nd->DI_cnt + 7 ) / 8 );
                    }
                if ( nd->AI_cnt )
                    {
                    make_read_request( 0, nd->AI_size / 2, 4 );
                    queue_exchange( nd, 12, 9 + nd->AI_size );
                    }
                }
            }
        else if ( nd->type == io_node::PHOENIX_BK_ETH )
            {
            unsigned int module_type = 0, module_offset = 0;
            const auto count = writing ? nd->AO_cnt : nd->AI_cnt;
            for ( unsigned int start = 0; start < count; )
                {
                const auto quantity = (std::min)( count - start,
                    static_cast<unsigned int>( MAX_MODBUS_REGISTERS_PER_QUERY ) );
                if ( writing )
                    {
                    make_phoenix_output( nd, start, quantity, module_type, module_offset );
                    make_write_request( PHOENIX_HOLDINGREGISTERS_STARTADDRESS + start, quantity );
                    queue_exchange( nd, 13 + quantity * 2, 12 );
                    }
                else
                    {
                    make_read_request( PHOENIX_INPUTREGISTERS_STARTADDRESS + start, quantity, 4 );
                    queue_exchange( nd, 12, 9 + quantity * 2 );
                    }
                start += quantity;
                }
            if ( !writing )
                {
                make_read_request( PHOENIX_STATUS_REGISTER_ADDRESS, 2, 4 );
                queue_exchange( nd, 12, 13 );
                }
            }
        }
    std::copy( saved.begin(), saved.end(), buff );
    phase_active = true;
    phase_executed = false;
    }

namespace
    {
    bool io_would_retry()
        {
#ifdef WIN_OS
        const auto error = WSAGetLastError();
        return error == WSAEWOULDBLOCK || error == WSAEINTR;
#else
        return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR;
#endif
        }

    void record_io_time( stat_time& stat, std::chrono::steady_clock::duration elapsed,
        const io_manager::io_node& node, const char* direction )
        {
        const auto hour = get_time().tm_hour;
        if ( stat.print_cycle_last_h != hour )
            {
            const auto average = stat.cycles_cnt ? stat.all_time / stat.cycles_cnt : 0;
            G_LOG->debug( "I/O %s '%s' (%s): avg=%u, min=%u, max=%u ms.",
                direction, node.name, node.ip_address, average,
                stat.min_iteration_cycle_time, stat.max_iteration_cycle_time );
            if ( average > G_PAC_INFO()->par[PAC_info::P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME] )
                G_LOG->alert( "I/O %s '%s' (%s): average exchange time %u ms exceeds threshold.",
                    direction, node.name, node.ip_address, average );
            stat.clear();
            stat.print_cycle_last_h = hour;
            }
        const auto ms = static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>( elapsed ).count() );
        stat.all_time += ms;
        ++stat.cycles_cnt;
        stat.min_iteration_cycle_time = (std::min)( stat.min_iteration_cycle_time, ms );
        stat.max_iteration_cycle_time = (std::max)( stat.max_iteration_cycle_time, ms );
        }
    }

void uni_io_manager::run_exchanges( std::vector<exchange>& exchanges )
    {
    using clock = std::chrono::steady_clock;
    sync_phoenix_transport();
    auto is_udp = [&]( const exchange& item )
        { return phoenix_udp_active && item.node->type == io_node::PHOENIX_BK_ETH; };
    auto finish = [&]( exchange& item, int result, bool close_socket = true )
        {
        item.result = result;
        item.done = true;
        if ( item.started )
            {
            if ( item.sent == item.send_size )
                record_io_time( item.node->recv_stat, clock::now() - item.sent_at, *item.node, "receive" );
            else
                record_io_time( item.node->send_stat, clock::now() - item.started_at, *item.node, "send" );
            }
        if ( result < 0 )
            {
            G_LOG->error( "I/O exchange with '%s' (%s) failed: %s.",
                item.node->name, item.node->ip_address,
                result == -103 ? "invalid Modbus frame" :
                result == -101 ? "send error or deadline exceeded" :
                "receive error or deadline exceeded" );
            if ( close_socket ) disconnect( item.node );
            }
        if ( result == 0 ) item.node->last_poll_time = get_millisec();
        };

    for ( ;; )
        {
        fd_set reads, writes;
        FD_ZERO( &reads );
        FD_ZERO( &writes );
        int max_socket = 0, active = 0;
        auto nearest = (clock::time_point::max)();
        auto admitted = std::count_if( exchanges.begin(), exchanges.end(),
            []( const exchange& item ) { return item.started && !item.done; } );
        for ( size_t i = 0; i < exchanges.size(); ++i )
            {
            auto& item = exchanges[i];
            if ( item.done ) continue;
            bool waiting = false, failed = false;
            // Keep one request outstanding per node, including multi-block I/O.
            if ( item.previous < i )
                {
                const auto& previous = exchanges[item.previous];
                waiting = !previous.done;
                failed = previous.done && ( previous.result != 0 ||
                    ( previous.response[7] & 0x80 ) != 0 );
                }
            if ( failed ) { item.done = true; item.result = 1; continue; }
            if ( waiting ) continue;
            if ( !item.started )
                {
                if ( admitted >= FD_SETSIZE ) continue;
                const auto ready = prepare_node( item.node );
                if ( ready != 0 ) { item.done = true; item.result = ready; continue; }
                if ( item.send_size < 12 || item.send_size > BUFF_SIZE ||
                    item.expected_size < 9 || item.expected_size > BUFF_SIZE ||
                    6 + item.request[4] * 256 + item.request[5] != item.send_size )
                    { finish( item, -103 ); continue; }
#ifndef WIN_OS
                if ( item.node->sock < 0 || item.node->sock >= FD_SETSIZE )
                    { finish( item, -103 ); continue; }
#endif
                // select's Windows fd_set has a fixed capacity. Remaining nodes
                // are admitted after an active slot is released.
                const auto transaction = ++item.node->modbus_transaction_id;
                item.request[0] = static_cast<u_char>( transaction >> 8 );
                item.request[1] = static_cast<u_char>( transaction );
                item.started = true;
                ++admitted;
                item.started_at = clock::now();
                item.deadline = item.started_at + std::chrono::microseconds(
                    io_node::C_RCV_TIMEOUT_SEC * 1000000 + io_node::C_RCV_TIMEOUT_US );
                }
            if ( clock::now() >= item.deadline )
                {
                // Packet loss does not invalidate a UDP socket. Report this
                // exchange as failed, then send fresh data on the next cycle.
                finish( item, item.sent == item.send_size ? -102 : -101, !is_udp( item ) );
                continue;
                }
            if ( item.sent < item.send_size ) FD_SET( item.node->sock, &writes );
            else FD_SET( item.node->sock, &reads );
            max_socket = (std::max)( max_socket, item.node->sock );
            nearest = (std::min)( nearest, item.deadline );
            ++active;
            }
        if ( active == 0 )
            {
            if ( std::all_of( exchanges.begin(), exchanges.end(),
                []( const exchange& item ) { return item.done; } ) ) break;
            continue;
            }
        const auto remaining = (std::max)( int64_t{0},
            std::chrono::duration_cast<std::chrono::microseconds>( nearest - clock::now() ).count() );
        timeval wait{};
        wait.tv_sec = static_cast<long>( remaining / 1000000 );
        wait.tv_usec = static_cast<long>( remaining % 1000000 );
        const auto ready = select( max_socket + 1, &reads, &writes, nullptr, &wait );
        if ( ready < 0 )
            {
            if ( io_would_retry() ) continue;
            for ( auto& item : exchanges )
                if ( item.started && !item.done ) finish( item, -102 );
            continue;
            }
        for ( auto& item : exchanges )
            {
            if ( !item.started || item.done ) continue;
            if ( clock::now() >= item.deadline )
                {
                finish( item, item.sent == item.send_size ? -102 : -101, !is_udp( item ) );
                continue;
                }
            if ( item.sent < item.send_size && FD_ISSET( item.node->sock, &writes ) )
                {
                const int n = send( item.node->sock,
                    reinterpret_cast<const char*>( item.request.data() ) + item.sent,
                    item.send_size - item.sent,
#ifdef WIN_OS
                    0
#else
                    MSG_NOSIGNAL
#endif
                );
                if ( n < 0 && io_would_retry() ) continue;
                if ( n <= 0 || ( phoenix_udp_active &&
                    item.node->type == io_node::PHOENIX_BK_ETH &&
                    n != item.send_size ) )
                    { finish( item, -101 ); continue; }
                item.sent += n;
                if ( item.sent == item.send_size )
                    {
                    item.sent_at = clock::now();
                    if ( phoenix_udp_active &&
                        item.node->type == io_node::PHOENIX_BK_ETH )
                        item.deadline = item.sent_at + std::chrono::milliseconds(
                            G_PAC_INFO()->get_phoenix_modbus_udp_timeout_ms() );
                    record_io_time( item.node->send_stat, item.sent_at - item.started_at, *item.node, "send" );
                    }
                }
            if ( item.sent == item.send_size && FD_ISSET( item.node->sock, &reads ) )
                {
                const bool udp = is_udp( item );
                const int n = recv( item.node->sock,
                    reinterpret_cast<char*>( item.response.data() ) + item.received,
                    udp ? BUFF_SIZE : item.frame_size - item.received,
#ifdef WIN_OS
                    0
#else
                    udp ? MSG_TRUNC : 0
#endif
                    );
                if ( n < 0 && io_would_retry() ) continue;
#ifdef WIN_OS
                // Winsock consumes an oversized datagram and reports this error.
                if ( udp && n < 0 && WSAGetLastError() == WSAEMSGSIZE ) continue;
#endif
                if ( udp && ( n == 0 || n > BUFF_SIZE ) ) continue;
                if ( n <= 0 ) { finish( item, -102 ); continue; }
                if ( udp )
                    {
                    // A connected UDP socket filters foreign peers. Old replies can
                    // still arrive after a timeout, so discard them by transaction ID.
                    if ( n < 2 || item.response[0] != item.request[0] ||
                        item.response[1] != item.request[1] ) continue;
                    if ( n < 9 || item.response[2] != 0 || item.response[3] != 0 ||
                        6 + item.response[4] * 256 + item.response[5] != n )
                        continue;
                    item.received = n;
                    item.frame_size = n;
                    }
                else
                    {
                    item.received += n;
                    if ( item.received < item.frame_size ) continue;
                    if ( item.frame_size == 6 )
                        {
                        item.frame_size = 6 + item.response[4] * 256 + item.response[5];
                        if ( item.frame_size < 9 || item.frame_size > BUFF_SIZE ||
                            item.response[0] != item.request[0] || item.response[1] != item.request[1] ||
                            item.response[2] != 0 || item.response[3] != 0 )
                            finish( item, -103 );
                        continue;
                        }
                    }
                const auto function = item.request[7];
                const bool exception = item.response[7] == ( function | 0x80 );
                bool valid = item.response[6] == item.request[6] &&
                    ( exception ? item.frame_size == 9 :
                    item.response[7] == function && item.frame_size == item.expected_size );
                if ( valid && !exception )
                    {
                    if ( function == 2 || function == 4 )
                        valid = item.response[8] == item.frame_size - 9;
                    else if ( function == 0x0F || function == 0x10 )
                        valid = std::equal( item.request.begin() + 8,
                            item.request.begin() + 12, item.response.begin() + 8 );
                    }
                if ( udp && !valid )
                    {
                    // Discard an invalid datagram without extending the deadline.
                    item.received = 0;
                    item.frame_size = 6;
                    continue;
                    }
                finish( item, valid ? 0 : -103 );
                }
            }
        }
    }

void uni_io_manager::sync_phoenix_transport()
    {
    const bool desired = G_PAC_INFO()->is_phoenix_modbus_udp();
    if ( desired == phoenix_udp_active ) return;
    // Recreate only PHOENIX sockets at the next exchange boundary. Other
    // couplers keep their TCP sessions and their current polling state.
    for ( unsigned int i = 0; i < nodes_count; ++i )
        if ( nodes[i]->type == io_node::PHOENIX_BK_ETH )
            {
            disconnect( nodes[i] );
            nodes[i]->last_init_time = get_millisec() - nodes[i]->delay_time;
            }
    phoenix_udp_active = desired;
    }

int uni_io_manager::e_communicate( io_node* node, int bytes_to_send, int bytes_to_receive )
    {
    if ( phase_active )
        {
        if ( !phase_executed )
            {
            run_exchanges( phase_exchanges );
            phase_executed = true;
            }
        for ( auto& item : phase_exchanges )
            if ( item.node == node && !item.consumed && item.send_size == bytes_to_send &&
                item.expected_size == bytes_to_receive &&
                std::equal( buff + 6, buff + 12, item.request.begin() + 6 ) )
                {
                item.consumed = true;
                if ( item.result == 0 ) std::copy( item.response.begin(), item.response.end(), buff );
                return item.result;
                }
        // A phase never starts an unplanned write or retries a failed node.
        return -103;
        }
    std::vector<exchange> single( 1 );
    auto& item = single.front();
    item.node = node;
    item.send_size = bytes_to_send;
    item.expected_size = bytes_to_receive;
    std::copy_n( buff, BUFF_SIZE, item.request.begin() );
    run_exchanges( single );
    if ( item.result == 0 ) std::copy( item.response.begin(), item.response.end(), buff );
    return item.result;
    }

//-----------------------------------------------------------------------------
int uni_io_manager::net_init( io_node* node ) const
    {
    if ( node == nullptr )
        {
        auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
            "Не задан узел." );
        *res.out = '\0';
        G_LOG->write_log( i_log::P_CRIT );

        return 1;
        }

    if ( node->state == io_node::ST_OK ) return 0;

    int sock = node->sock;
    if ( node->state != io_node::ST_CONNECTING )
        {
#ifdef WIN_OS
        WSAData tmp_WSA_data;
        if ( WSAStartup( 0x202, &tmp_WSA_data ) )
            {
            auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                "Ошибка инициализации сетевой библиотеки: {}",
                WSA_Last_Err_Decode() );
            *res.out = '\0';
            G_LOG->write_log( i_log::P_CRIT );

            return 2;
            }
#endif // WIN_OS

        const bool udp = phoenix_udp_active &&
            node->type == io_node::PHOENIX_BK_ETH;
        int type = udp ? SOCK_DGRAM : SOCK_STREAM;
        int protocol = 0; /* всегда 0 */
        int err;
        sock = socket( AF_INET, type, protocol ); // Cоздание сокета.

        if ( sock < 0 )
            {
            auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                "Network communication : can't create I/O node socket : {}",
#ifdef WIN_OS
                WSA_Last_Err_Decode()
#else
                strerror( errno )
#endif // WIN_OS
            );
            *res.out = '\0';
            G_LOG->write_log( i_log::P_CRIT );

            return 3;
            }

#ifndef WIN_OS
        // FD_SET cannot represent descriptors outside this range.
        if ( sock >= FD_SETSIZE )
            {
            close( sock );
            return 3;
            }
#endif

        // Адресация мастер-сокета.
        struct sockaddr_in socket_remote_server;
        const int PORT = 502;
        memset( &socket_remote_server, 0, sizeof( socket_remote_server ) );
        socket_remote_server.sin_family = AF_INET;
        socket_remote_server.sin_addr.s_addr = inet_addr( node->ip_address );
        socket_remote_server.sin_port = htons( PORT );

#ifdef WIN_OS
        unsigned long timeout = io_node::C_CNT_TIMEOUT_US;
        int vlen = sizeof( timeout );
#else
        const int C_ON = 1;
#endif // WIN_OS

        if (
#ifdef WIN_OS
            setsockopt( sock, SOL_SOCKET, SO_REUSEADDR,
                reinterpret_cast<char*>( &timeout ), vlen)
#else
            setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &C_ON, sizeof( C_ON ) )
#endif // WIN_OS
            )
            {
            auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                "Network communication : can't setsockopt I/O node socket : {}",
#ifdef WIN_OS
                WSA_Last_Err_Decode()
#else
                strerror( errno )
#endif // WIN_OS
            );
            *res.out = '\0';
            G_LOG->write_log( i_log::P_CRIT );

#ifdef WIN_OS
            closesocket( sock );
#else
            close( sock );
#endif // WIN_OS

            return 4;
            }

        // Переводим в неблокирующий режим.
#ifdef WIN_OS
        u_long mode = 1;
        err = ioctlsocket( sock, FIONBIO, &mode );
#else
        err = fcntl( sock, F_SETFL, O_NONBLOCK );
#endif // WIN_OS

        if ( err != 0 )
            {
            auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                "Network communication : can't fcntl I/O node socket : {}",
#ifdef WIN_OS
                WSA_Last_Err_Decode()
#else
                strerror( errno )
#endif // WIN_OS
            );
            *res.out = '\0';
            G_LOG->write_log( i_log::P_CRIT );

#ifdef WIN_OS
            closesocket( sock );
#else
            close( sock );
#endif // WIN_OS
            return 5;
            }

        // Привязка сокета. Сразу возвращает управление в неблокирующем режиме.
        sockaddr s_address;
        static_assert( sizeof( sockaddr ) == sizeof( sockaddr_in ) );
        std::memcpy( &s_address, &socket_remote_server, sizeof( socket_remote_server ) );
        node->connect_start_time = get_millisec();
        err = connect( sock, &s_address, sizeof( socket_remote_server ) );
#ifdef WIN_OS
        const int connect_error = err == 0 ? 0 : WSAGetLastError();
        const bool pending = connect_error == WSAEWOULDBLOCK;
#else
        const int connect_error = err == 0 ? 0 : errno;
        const bool pending = connect_error == EINPROGRESS;
#endif
        if ( err != 0 && !pending )
            {
            if ( !node->is_set_err )
                {
                auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                    R"(Network device : s{}->"{}":"{}" can't connect : {})",
                    sock, node->name, node->ip_address,
#ifdef WIN_OS
                    WSA_Last_Err_Decode()
#else
                    strerror( connect_error )
#endif
                );
                *res.out = '\0';
                G_LOG->write_log( i_log::P_CRIT );
                }
#ifdef WIN_OS
            closesocket( sock );
#else
            close( sock );
#endif
            return 6;
            }
        node->sock = sock;
        if ( udp && err == 0 )
            {
            node->state = io_node::ST_OK;
            G_LOG->debug( "uni_io_manager:net_init(): UDP socket %d for '%s' (%s).",
                sock, node->name, node->ip_address );
            return 0;
            }
        node->state = io_node::ST_CONNECTING;
        }

    // Poll only: connection establishment must not delay the control cycle.
    fd_set write_events, error_events;
    FD_ZERO( &write_events );
    FD_ZERO( &error_events );
    FD_SET( sock, &write_events );
    FD_SET( sock, &error_events );
    timeval tv{};
    const int ready = select( sock + 1, nullptr, &write_events,
        &error_events, &tv );
    int error = 0;
#ifdef WIN_OS
    int err_len = sizeof( error );
#else
    socklen_t err_len = sizeof( error );
#endif
    if ( ready < 0 )
        {
#ifdef WIN_OS
        error = WSAGetLastError();
        if ( error == WSAEINTR ) return NET_CONNECTING;
#else
        error = errno;
        if ( error == EINTR ) return NET_CONNECTING;
#endif
        }
    else if ( ready == 0 )
        {
        if ( get_delta_millisec( node->connect_start_time ) <
            io_node::C_CNT_TIMEOUT_US / 1000 ) return NET_CONNECTING;
#ifdef WIN_OS
        error = WSAETIMEDOUT;
#else
        error = ETIMEDOUT;
#endif
        }
    else if ( getsockopt( sock, SOL_SOCKET, SO_ERROR,
#ifdef WIN_OS
        reinterpret_cast<char*>( &error ),
#else
        &error,
#endif
        &err_len ) != 0 )
        {
#ifdef WIN_OS
        error = WSAGetLastError();
#else
        error = errno;
#endif
        }

    if ( error != 0 )
        {
        if ( !node->is_set_err )
            {
#ifdef WIN_OS
            WSASetLastError( error );
#endif
            auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                R"(Network device : s{}->"{}":"{}" error during connect : {})",
                sock, node->name, node->ip_address,
#ifdef WIN_OS
                WSA_Last_Err_Decode()
#else
                strerror( error )
#endif
            );
            *res.out = '\0';
            G_LOG->write_log( i_log::P_CRIT );
            }
#ifdef WIN_OS
        closesocket( sock );
#else
        close( sock );
#endif
        node->sock = 0;
        node->state = io_node::ST_NO_CONNECT;
        return ready <= 0 ? 6 : 7;
        }

    const u_long connect_time = get_delta_millisec( node->connect_start_time );
    G_LOG->debug( "uni_io_manager:net_init() : socket %d is successfully "
        R"(connected to "%s":"%s":%d (%lu ms).)",
        sock, node->name, node->ip_address, 502, connect_time );

    node->sock = sock;
    node->state = io_node::ST_OK;

    return 0;
    }
//-----------------------------------------------------------------------------
int uni_io_manager::write_outputs()
    {
    if ( 0 == nodes_count )
        {
        return 0;
        }

    io_phase_scope phase{ phase_active, write_timing };
    prepare_phase( true );

    int res = 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];
        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET )
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->read_io_error_flag )
                {
                res = 1;
                continue;
                }

            if ( nd->DO_cnt > 0 )
                {
                u_int bytes_cnt = nd->DO_cnt / 8 + ( nd->DO_cnt % 8 > 0 ? 1 : 0 );

                make_wago_do_request( nd );

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x0F )
                        {
                        memcpy( nd->DO, nd->DO_, nd->DO_cnt );
                        nd->flag_error_write_message = false;
                        }
                    else
                        {
                        if ( !nd->flag_error_write_message )
                            {
                            // Есть какая-то ошибка на прикладном уровне.
                            add_err_to_log( "Write DO", nd->name, nd->ip_address,
                                static_cast<int>( buff[ 7 ] ), 0x0F,
                                static_cast<int>( buff[ 8 ] ), bytes_cnt );
                            res = 1;
                            nd->flag_error_write_message = true;
                            }
                        continue;
                        }
                    }
                else
                    {
                    // Была какая-то сетевая ошибка.
                    res = 1;
                    continue;
                    }

                }// if ( nd->DO_cnt > 0 )

            if ( nd->AO_cnt > 0 )
                {
                u_int bytes_cnt = nd->AO_size;

                make_wago_ao_request( nd );

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x10 )
                        {
                        memcpy( nd->AO, nd->AO_, sizeof( nd->AO ) );
                        nd->flag_error_write_message = false;
                        }
                    else
                        {
                        if ( !nd->flag_error_write_message )
                            {
                            add_err_to_log( "Write AO", nd->name, nd->ip_address,
                                static_cast<int>( buff[ 7 ] ), 0x10,
                                static_cast<int>( buff[ 8 ] ), bytes_cnt );
                            }
                        nd->flag_error_write_message = true;
                        res = 1;
                        continue;
                        }
                    }
                else
                    {
                    res = 1;
                    continue;
                    }
                }// if ( nd->AO_cnt > 0 )
            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];
        u_int ao_module_type = 0;
        u_int ao_module_offset = 0;

        if ( nd->type == io_node::PHOENIX_BK_ETH )
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->read_io_error_flag )
                {
                res = 1;
                continue;
                }

            if ( nd->AO_cnt > 0 )
                {
                unsigned int start_register = 0;
                unsigned int start_write_address = PHOENIX_HOLDINGREGISTERS_STARTADDRESS;
                unsigned int registers_count;

                if (nd->AO_cnt > MAX_MODBUS_REGISTERS_PER_QUERY)
                    {
                    registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                    }
                else
                    {
                    registers_count = nd->AO_cnt;
                    }


                do
                    {
                    make_phoenix_output( nd, start_register, registers_count,
                        ao_module_type, ao_module_offset );

                    if (write_holding_registers(nd, start_write_address + start_register, registers_count) >= 0)
                        {
                        if (buff[7] == 0x10)
                            {
                            memcpy(&(nd->AO[start_register]), &(nd->AO_[start_register]), registers_count * 2);
                            memcpy(&(nd->DO[start_register * 16]), &(nd->DO_[start_register * 16]), registers_count * 16);
                            nd->flag_error_write_message = false;
                            }
                        else
                            {
                            if (!nd->flag_error_write_message)
                                {
                                add_err_to_log( "Write AO", nd->name, nd->ip_address,
                                    static_cast<int>( buff[ 7 ] ), 0x10,
                                    static_cast<int>( buff[ 8 ] ), registers_count );

                                nd->flag_error_write_message = true;
                                }
                            res = 1;
                            }
                        }
                    else
                        {
                        res = 1;
                        }

                    start_register += registers_count;
                    registers_count = nd->AO_cnt - start_register;
                    if (registers_count > MAX_MODBUS_REGISTERS_PER_QUERY)
                        {
                        registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                        }

                    } while (start_register < nd->AO_cnt);


                }// if ( nd->AO_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return res;
    }
//-----------------------------------------------------------------------------
int uni_io_manager::prepare_node( io_node* node )
    {
    // Проверка связи с узлом I/O.
    if ( get_delta_millisec( node->last_poll_time ) >=
        G_PAC_INFO()->par[ PAC_info::P_BK_ANSWER_MAX_WAIT_TIME ] )
        {
        // Если связь была, но сейчас пропала, то выставляем ошибку связи.
        if ( false == node->is_set_err )
            {
            node->is_set_err = true;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }

        // Reset PP mode alarm on communication loss.
        if ( node->is_err_mode_alarm_set )
            {
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::AC_PP_MODE,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number,
                false );

            // Reset PP-mode tracking state so a new transition is detected
            // after reconnect.
            node->prev_status_register = 0;
            node->is_err_mode_alarm_set = false;
            }

        // Reset CFG-bus error alarm on communication loss.
        if ( node->is_cfg_bus_error_alarm_set )
            {
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::AC_CFG_BUS_ERROR,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number,
                false );
            // Reset CFG-bus error tracking state so a new transition is detected
            // after reconnect.
            node->prev_diagnostic_status_register = 0;
            node->is_cfg_bus_error_alarm_set = false;
            }
        }
    else
        {
        if ( node->is_set_err )
            {
            node->is_set_err = false;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }
        }
    // Проверка связи с узлом I/O.-!>

    // Инициализация сетевого соединения, при необходимости.
    if ( node->state != io_node::ST_OK )
        {
        if ( node->state != io_node::ST_CONNECTING &&
            get_delta_millisec( node->last_init_time ) < node->delay_time )
            {
            return 1;
            }

        if ( net_init( node ) == NET_CONNECTING ) return 1;
        if ( node->state != io_node::ST_OK )
            {
            node->last_init_time = get_millisec();
            if ( node->delay_time < io_node::C_MAX_DELAY )
                {
                node->delay_time += node->delay_time;
                }
            return -100;
            }
        }
    // Инициализация сетевого соединения, при необходимости.-!>

    node->delay_time = io_node::C_INITIAL_RECONNECT_DELAY;

    return 0;
    }

int uni_io_manager::read_input_registers(io_node* node, unsigned int address,
    unsigned int quantity, unsigned char station /*= 0*/)
    {
    make_read_request( address, quantity, 0x04, station );
    unsigned int bytes_cnt = quantity * 2;
    if (e_communicate(node, 12, bytes_cnt + 9) == 0)
        {
        if (buff[7] == 0x04 && buff[8] == bytes_cnt)
            {
            resultbuff = &buff[9];
            return 1;
            }
        else
            {
            return 0;
            }
        }
    return -1;
    }

int uni_io_manager::write_holding_registers(io_node* node,
    unsigned int address, unsigned int quantity, unsigned char station)
    {
    unsigned int bytes_cnt = quantity * 2;
    make_write_request( address, quantity, station );
    if (e_communicate(node, bytes_cnt + 13, 12) == 0)
        {
        if (buff[7] == 0x10)
            {
            return 1;
            }
        else
            {
            return 0;
            }
        }
    return -1;
    }

void uni_io_manager::add_err_to_log( const char* cmd,
    const char* node_name, const char* node_ip_address,
    int exp_fun_code, int rec_fun_code, int exp_size, int rec_size ) const
    {
    auto result = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        R"({}:bus coupler returned error. "{}":"{}" )"
        "(received code={}, expected={}, received size={}, expected={}).",
        cmd, node_name, node_ip_address, exp_fun_code, rec_fun_code, exp_size,
        rec_size );
    *result.out = '\0';
    G_LOG->write_log( i_log::P_ERR );
    };
//-----------------------------------------------------------------------------
int uni_io_manager::read_inputs()
    {
    if ( 0 == nodes_count )
        {
        return 0;
        }

    io_phase_scope phase{ phase_active, read_timing };
    prepare_phase( false );

    auto res = 0;
    for (u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];

        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET ) // Ethernet I/O nodes.
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->DI_cnt > 0 )
                {
                const auto bytes_cnt = ( nd->DI_cnt + 7 ) / 8;
                make_read_request( 0, nd->DI_cnt, 2 );

                if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x02 && buff[ 8 ] == bytes_cnt )
                        {
                        for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                            {
                            for ( int k = 0; k < 8; k++ )
                                {
                                if ( idx < nd->DI_cnt )
                                    {
                                    nd->DI[ idx ] = ( buff[ j + 9 ] >> k ) & 1;
#ifdef DEBUG_KBUS
                                    printf( "%d -> %d, ", idx, nd->DI[ idx ] );
#endif // DEBUG_KBUS
                                    idx++;
                                    }
                                }
                            }
#ifdef DEBUG_KBUS
                        printf( "\n" );
#endif // DEBUG_KBUS
                        nd->read_io_error_flag = false;
                        nd->flag_error_read_message = false;
                        }
                    else
                        {
                        if ( !nd->flag_error_read_message )
                            {
                            add_err_to_log( "Read DI", nd->name, nd->ip_address,
                                static_cast<int>( buff[ 7 ] ), 0x02,
                                static_cast<int>( buff[ 8 ] ), bytes_cnt );
                            nd->flag_error_read_message = true;
                            }
                        nd->read_io_error_flag = true;
                        res = 1;
                        continue;
                        }
                    } // if ( buff[ 7 ] == 0x02 && buff[ 8 ] == bytes_cnt )
                else
                    {
                    nd->read_io_error_flag = true;
                    res = 1;
                    continue;
                    }
                }// if ( nd->DI_cnt > 0 )

            if ( nd->AI_cnt > 0 )
                {
                const auto bytes_cnt = nd->AI_size;
                make_read_request( 0, nd->AI_size / 2, 4 );

                if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x04 && buff[ 8 ] == bytes_cnt )
                        {
                        int idx = 0;
                        for ( unsigned int l = 0; l < nd->AI_cnt; l++ )
                            {
                            switch ( nd->AI_types[ l ] )
                                {
                                case 638:
                                    nd->AI[ l ] = 256 * buff[ 9 + idx + 2 ] +
                                        buff[ 9 + idx + 3 ];
                                    idx += 4;
                                    break;

                                default:
                                    nd->AI[ l ] = 256 * buff[ 9 + idx ] +
                                        buff[ 9 + idx + 1 ];
                                    idx += 2;
                                    break;
                                }
                            }
                        nd->read_io_error_flag = false;
                        nd->flag_error_read_message = false;
                        } // if ( buff[ 7 ] == 0x04 && buff[ 8 ] == bytes_cnt )
                    else
                        {
                        if ( !nd->flag_error_read_message )
                            {
                            add_err_to_log( "Read AI", nd->name, nd->ip_address,
                                static_cast<int>( buff[ 7 ] ), 0x04,
                                static_cast<int>( buff[ 8 ] ), bytes_cnt );
                            nd->flag_error_read_message = true;
                            }
                        nd->read_io_error_flag = true;
                        res = 1;
                        continue;
                        }
                    }
                else
                    {
                    nd->read_io_error_flag = true;
                    res = 1;
                    continue;
                    } // if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                }// if ( nd->AI_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];

        if ( nd->type == io_node::PHOENIX_BK_ETH ) // Ethernet I/O nodes.
            {

            if ( !nd->is_active )
                {
                continue;
                }

            if (nd->AI_cnt > 0)
                {
                unsigned int start_register = 0;
                unsigned int start_read_address = PHOENIX_INPUTREGISTERS_STARTADDRESS;
                unsigned int registers_count;

                if (nd->AI_cnt > MAX_MODBUS_REGISTERS_PER_QUERY)
                    {
                    registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                    }
                else
                    {
                    registers_count = nd->AI_cnt;
                    }

                unsigned int analog_dest = 0;
                unsigned int bit_dest = 0;

                do
                    {
#ifdef DEBUG_BK_MIN
                    G_LOG->warning("Read %d node registers from %d", registers_count, start_read_address + start_register);
#endif // DEBUG_BK_MIN
                    int result = read_input_registers(nd, start_read_address + start_register, registers_count);

#ifdef TEST_NODE_IO
                    printf("\n\r");
                    for (int ideb = 0; ideb < registers_count; ideb++)
                        {
                        printf("%d = %d, ", start_read_address + start_register + ideb, 256 * resultbuff[ideb * 2] + resultbuff[ideb * 2 + 1]);
                        }
#endif

                    if (result >= 0)
                        {
                        if (result)
                            {
                            for (int index_source = 0; analog_dest < start_register + registers_count; analog_dest++)
                                {
                                switch (nd->AI_types[analog_dest])
                                    {
                                    case 1027843:           //AXL F IOL8
                                    case 1088132:           //AXL SE IOL4
                                        memcpy(&nd->AI[analog_dest], resultbuff + index_source, 2);
                                        index_source += 2;
                                        break;

                                    default:
                                        nd->AI[analog_dest] = 256 * resultbuff[index_source] + resultbuff[index_source + 1];
                                        index_source += 2;
                                        break;
                                    }
#ifdef DEBUG_BK
                                G_LOG->warning("%d %u", analog_dest, nd->AI[analog_dest]);
#endif // DEBUG_BK
                                }

                            for (int index_source = 0; bit_dest < (start_register + registers_count) * 2 * 8; index_source++)
                                {
                                for (int k = 0; k < 8; k++)
                                    {
                                    nd->DI[bit_dest] = (resultbuff[index_source] >> k) & 1;
#ifdef DEBUG_BK
                                    G_LOG->notice("%d %d", bit_dest, (resultbuff[index_source] >> k) & 1);
#endif // DEBUG_BK
                                    bit_dest++;
                                    }
                                }
                            }
                        else
                            {
                            if ( !nd->flag_error_read_message )
                                {
                                add_err_to_log( "Read AI", nd->name, nd->ip_address,
                                    static_cast<int>( buff[ 7 ] ), 0x04,
                                    static_cast<int>( buff[ 8 ] ), registers_count * 2 );
                                nd->flag_error_read_message = true;
                                }
                            nd->read_io_error_flag = true;
                            res = 1;
                            break;
                            }
                        }
                    else
                        {
                        nd->read_io_error_flag = true;
                        res = 1;
                        break;
                        }
                    start_register += registers_count;
                    registers_count = nd->AI_cnt - start_register;
                    if (registers_count > MAX_MODBUS_REGISTERS_PER_QUERY)
                        {
                        registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                        }
                    nd->read_io_error_flag = false;
                    nd->flag_error_read_message = false;
                    } while (start_register < nd->AI_cnt);
                } // if (nd->AI_cnt > 0)

            // Read Status Register (7996) for PP mode detection.
            if ( !read_phoenix_status_register( nd ) )
                {
                nd->read_io_error_flag = true;
                res = 1;
                }
            else if ( nd->AI_cnt == 0 ) nd->read_io_error_flag = false;
            }// nd->type == io_node::PHOENIX_BK_ETH
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return res;
    }
//-----------------------------------------------------------------------------
bool uni_io_manager::read_phoenix_status_register( io_node* nd )
    {
    if ( auto result = read_input_registers( nd, PHOENIX_STATUS_REGISTER_ADDRESS,
        2 ); result <= 0 )
        {
#ifdef DEBUG_BK
        G_LOG->debug( "Failed to read status registers (%d, %d) "
            "for node \"%s\".",
            PHOENIX_STATUS_REGISTER_ADDRESS,
            PHOENIX_DIAGNOSTIC_STATUS_REGISTER_ADDRESS, nd->name );
#endif // DEBUG_BK
        return false;
        }

    nd->status_register = static_cast<u_int_2>(
        BYTE_SHIFT_MULTIPLIER * resultbuff[ 0 ] + resultbuff[ 1 ] );
    nd->diagnostic_status_register = static_cast<u_int_2>(
        BYTE_SHIFT_MULTIPLIER * resultbuff[ 2 ] + resultbuff[ 3 ] );

    // Check for PP mode state changes.
    // PP mode has become active.
    if ( const auto is_err_mode_active =
        ( nd->status_register & io_node::STATUS_REG_PP_MODE_MASK ) != 0,
        was_err_mode_active =
        ( nd->prev_status_register & io_node::STATUS_REG_PP_MODE_MASK ) != 0;
        is_err_mode_active && !was_err_mode_active )
        {
        if ( !nd->is_err_mode_alarm_set )
            {
            nd->is_err_mode_alarm_set = true;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_PP_MODE,
                PAC_critical_errors_manager::AS_IO_COUPLER, nd->number );
            }
        }
    // PP mode has become inactive.
    else if ( !is_err_mode_active && was_err_mode_active &&
        nd->is_err_mode_alarm_set )
        {
        nd->is_err_mode_alarm_set = false;
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_PP_MODE,
            PAC_critical_errors_manager::AS_IO_COUPLER, nd->number );
        }


    if ( const auto is_cfg_bus_error_active =
        ( nd->diagnostic_status_register &
            io_node::DIAG_STATUS_REG_CFG_BUS_ERROR_MASK ) != 0,
        was_cfg_bus_error_active =
        ( nd->prev_diagnostic_status_register &
            io_node::DIAG_STATUS_REG_CFG_BUS_ERROR_MASK ) != 0;
            is_cfg_bus_error_active && !was_cfg_bus_error_active &&
        !nd->is_cfg_bus_error_alarm_set )
        {
        nd->is_cfg_bus_error_alarm_set = true;
        PAC_critical_errors_manager::get_instance()->set_global_error(
            PAC_critical_errors_manager::AC_CFG_BUS_ERROR,
            PAC_critical_errors_manager::AS_IO_COUPLER, nd->number );
        }
    else if ( !is_cfg_bus_error_active && was_cfg_bus_error_active &&
        nd->is_cfg_bus_error_alarm_set )
        {
        nd->is_cfg_bus_error_alarm_set = false;
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_CFG_BUS_ERROR,
            PAC_critical_errors_manager::AS_IO_COUPLER, nd->number );
        }

    nd->prev_status_register = nd->status_register;
    nd->prev_diagnostic_status_register = nd->diagnostic_status_register;
    return true;
    }
//-----------------------------------------------------------------------------
void uni_io_manager::disconnect( io_node* node )
    {
    if ( node->sock || node->state != io_node::ST_NO_CONNECT )
        {
        shutdown( node->sock,
#ifdef WIN_OS
            SD_BOTH
#else
            SHUT_RDWR
#endif // WIN_OS
        );

#ifdef WIN_OS
        closesocket( node->sock );
#else
        close( node->sock );
#endif // WIN_OS

        node->sock = 0;
        }
    node->state = io_node::ST_NO_CONNECT;
    node->last_init_time = get_millisec();

    // Reset PP mode alarm on disconnect.
    if ( node->is_err_mode_alarm_set )
        {
        node->is_err_mode_alarm_set = false;
        node->prev_status_register = 0;
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_PP_MODE,
            PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
        }

    if ( node->is_cfg_bus_error_alarm_set )
        {
        node->is_cfg_bus_error_alarm_set = false;
        PAC_critical_errors_manager::get_instance()->reset_global_error(
            PAC_critical_errors_manager::AC_CFG_BUS_ERROR,
            PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
        }
    }
//-----------------------------------------------------------------------------
uni_io_manager::uni_io_manager()
    {
    writebuff = &buff[ 13 ];
    resultbuff = &buff[ 9 ];
    }
//-----------------------------------------------------------------------------

void uni_io_manager::make_wago_do_request( io_node* nd )
    {
    const auto bytes_cnt = nd->DO_cnt / 8 + ( nd->DO_cnt % 8 != 0 );
    if ( bytes_cnt > BUFF_SIZE - 13 ) return;
    buff[ 0 ] = 's';
    buff[ 1 ] = 's';
    buff[ 2 ] = 0;
    buff[ 3 ] = 0;
    buff[ 4 ] = 0;
    buff[ 4 ] = static_cast<unsigned char>( ( 7u + bytes_cnt ) >> 8 );
    buff[ 5 ] = static_cast<unsigned char>( 7u + bytes_cnt );
    buff[ 6 ] = 0; //nodes[ i ]->number;
    buff[ 7 ] = 0x0F;
    buff[ 8 ] = 0;
    buff[ 9 ] = 0;
    buff[ 10 ] = static_cast<unsigned char>( nd->DO_cnt >> 8 );
    buff[ 11 ] = (unsigned char)nd->DO_cnt & 0xFF;
    buff[ 12 ] = static_cast <unsigned char>( bytes_cnt );

    for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
        {
        u_char b = 0;
        for ( u_int k = 0; k < 8; k++ )
            {
            if ( idx < nd->DO_cnt )
                {
                b = b | static_cast <unsigned char>( ( nd->DO_[ idx ] & 1 ) << k );
                idx++;
                }
            }
        buff[ j + 13 ] = b;
        }

    }

void uni_io_manager::make_wago_ao_request( io_node* nd )
    {
    const auto bytes_cnt = nd->AO_size;
    if ( bytes_cnt > BUFF_SIZE - 13 ) return;
    buff[ 0 ] = 's';
    buff[ 1 ] = 's';
    buff[ 2 ] = 0;
    buff[ 3 ] = 0;
    buff[ 4 ] = 0;
    buff[ 4 ] = static_cast<unsigned char>( ( 7u + bytes_cnt ) >> 8 );
    buff[ 5 ] = static_cast<unsigned char>( 7u + bytes_cnt );
    buff[ 6 ] = 0; //nodes[ i ]->number;
    buff[ 7 ] = 0x10;
    buff[ 8 ] = 0;
    buff[ 9 ] = 0;
    buff[ 10 ] = static_cast <unsigned char>( bytes_cnt / 2 >> 8 );
    buff[ 11 ] = bytes_cnt / 2 & 0xFF;
    buff[ 12 ] = static_cast <unsigned char>( bytes_cnt );

    for ( unsigned int idx = 0, l = 0; idx < nd->AO_cnt; idx++ )
        {
        switch ( nd->AO_types[ idx ] )
            {
            case 638:
                buff[ 13 + l ] = 0;
                buff[ 13 + l + 1 ] = 0;
                buff[ 13 + l + 2 ] = 0;
                buff[ 13 + l + 3 ] = 0;
                l += 4;
                break;

            default:
                buff[ 13 + l ] = (u_char)( ( nd->AO_[ idx ] >> 8 ) & 0xFF );
                buff[ 13 + l + 1 ] = (u_char)( nd->AO_[ idx ] & 0xFF );
                l += 2;
                break;
            }
        }

    }

void uni_io_manager::make_phoenix_output( io_node* nd, unsigned int start_register,
    unsigned int registers_count, unsigned int& ao_module_type, unsigned int& ao_module_offset )
    {
    auto bit_src = start_register * 16;
    for (u_int j = 0; j < registers_count * 2; j++)
        {
        u_char b = 0;
        for (u_int k = 0; k < 8; k++)
            {
            b = b | static_cast <unsigned char>( (nd->DO_[bit_src] & 1) << k );
            bit_src++;
            }
        writebuff[j] = b;
        }

    for (unsigned int idx = start_register, l = 0; idx < start_register + registers_count; idx++)
        {
        if (nd->AO_types[idx] != ao_module_type)
            {
            ao_module_type = nd->AO_types[idx];
            ao_module_offset = 0;
            }
        else
            {
            ao_module_offset++;
            }

        switch (ao_module_type)
            {
            case 1027843:           //AXL F IOL8
            case 1088132:           //AXL SE IOL4
                ao_module_offset %= 32;	   //if there are same modules one after other on bus
                if (ao_module_offset > 2)  //first 3 words (bytes 0-5) are reserved, 2nd byte is used for trigger discrete outputs.
                    {
                    memcpy(&writebuff[l], &nd->AO_[idx], 2);
                    }
                l += 2;
                break;

            case 2688093:			//CNT2 INC2
                ao_module_offset %= 14;	   //if there are same modules one after other on bus
                if (0 == ao_module_offset) //assign start command and positive increment for both counters
                    {
                    writebuff[l] = 0x5;
                    writebuff[l + 1] = 0x5;
                    }
                else
                    {
                    writebuff[l] = 0;
                    writebuff[l + 1] = 0;
                    }
                l += 2;
                break;

            case 2688527:       //AXL F AO4 1H
            case 2702072:       //AXL F AI2 AO2 1H
            case 1088123:       //AXL SE AO4 I 4-20
            case 2688666:       //AXL F RS UNI XC
                writebuff[l] = (u_char)((nd->AO_[idx] >> 8) & 0xFF);
                writebuff[l + 1] = (u_char)(nd->AO_[idx] & 0xFF);
                l += 2;
                break;

            default:
                l += 2;
                break;
            }
        }

    }

void uni_io_manager::make_read_request( unsigned int address, unsigned int quantity,
    unsigned char function, unsigned char station )
    {
    buff[0] = 's';
    buff[1] = 's';
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[5] = 6;
    buff[6] = station;
    buff[7] = function;
    buff[8] = (u_int_2)address >> 8;
    buff[9] = (u_int_2)address & 0xFF;
    buff[10] = (u_int_2)quantity >> 8;
    buff[11] = (u_int_2)quantity & 0xFF;
    }

void uni_io_manager::make_write_request( unsigned int address, unsigned int quantity,
    unsigned char station )
    {
    const auto bytes_cnt = quantity * 2;
    buff[0] = 's';
    buff[1] = 's';
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[4] = static_cast<unsigned char>( ( 7 + bytes_cnt ) >> 8 );
    buff[5] = static_cast<unsigned char>( 7 + bytes_cnt );
    buff[6] = station;
    buff[7] = 0x10;
    buff[8] = (u_int_2)address >> 8;
    buff[9] = (u_int_2)address & 0xFF;
    buff[10] = (u_int_2)quantity >> 8;
    buff[11] = (u_int_2)quantity & 0xFF;
    buff[12] = static_cast <unsigned char>( bytes_cnt );
    }
