#include "uni_bus_coupler_io_tests.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <thread>

namespace
    {
    using bytes = std::vector<unsigned char>;
    using steady = std::chrono::steady_clock;

    void close_peer( int socket )
        {
#ifdef WIN_OS
        closesocket( socket );
#else
        close( socket );
#endif
        }

    bool receive_exact( int socket, unsigned char* data, int size )
        {
        while ( size > 0 )
            {
            fd_set reads;
            FD_ZERO( &reads );
            FD_SET( socket, &reads );
            timeval timeout{ 1, 0 };
            if ( select( socket + 1, &reads, nullptr, nullptr, &timeout ) <= 0 ) return false;
            const auto n = recv( socket, reinterpret_cast<char*>( data ), size, 0 );
            if ( n <= 0 ) return false;
            data += n;
            size -= n;
            }
        return true;
        }

    void send_bytes( int socket, const bytes& data, size_t first = 0, size_t count = 0 )
        {
        if ( count == 0 ) count = data.size() - first;
        while ( count )
            {
            const auto n = send( socket, reinterpret_cast<const char*>( data.data() + first ),
                static_cast<int>( count ),
#ifdef WIN_OS
                0
#else
                MSG_NOSIGNAL
#endif
            );
            if ( n <= 0 ) return; // The client may intentionally time out.
            first += n;
            count -= n;
            }
        }

    bytes reply_to( const bytes& request )
        {
        if ( request[7] == 0x0F || request[7] == 0x10 )
            {
            bytes reply( request.begin(), request.begin() + 12 );
            reply[4] = 0;
            reply[5] = 6;
            return reply;
            }
        const int quantity = request[10] * 256 + request[11];
        const int count = request[7] == 2 ? ( quantity + 7 ) / 8 : quantity * 2;
        bytes reply( 9 + count, 0x01 );
        std::copy_n( request.begin(), 8, reply.begin() );
        reply[4] = static_cast<unsigned char>( ( count + 3 ) >> 8 );
        reply[5] = static_cast<unsigned char>( count + 3 );
        reply[8] = static_cast<unsigned char>( count );
        return reply;
        }

    // Real loopback sockets, with an ephemeral port and a bounded peer lifetime.
    class io_peer
        {
        uni_io_manager& manager;
        io_manager::io_node& node;
        int server = -1;
        std::thread worker;
        public:
        std::atomic<int> requests{ 0 };
        io_peer( uni_io_manager& manager, io_manager::io_node& node,
            std::function<void( int, const bytes&, int )> respond, int count = 1 ) :
            manager( manager ), node( node )
            {
#ifdef WIN_OS
            WSADATA data;
            if ( WSAStartup( MAKEWORD( 2, 2 ), &data ) ) throw std::runtime_error( "WSAStartup" );
#endif
            const int listener = static_cast<int>( socket( AF_INET, SOCK_STREAM, 0 ) );
            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
            if ( bind( listener, reinterpret_cast<sockaddr*>( &address ), sizeof( address ) ) ||
                listen( listener, 1 ) ) throw std::runtime_error( "listen" );
#ifdef WIN_OS
            int length = sizeof( address );
#else
            socklen_t length = sizeof( address );
#endif
            getsockname( listener, reinterpret_cast<sockaddr*>( &address ), &length );
            node.sock = static_cast<int>( socket( AF_INET, SOCK_STREAM, 0 ) );
            if ( connect( node.sock, reinterpret_cast<sockaddr*>( &address ), sizeof( address ) ) )
                throw std::runtime_error( "connect" );
            server = static_cast<int>( accept( listener, nullptr, nullptr ) );
            close_peer( listener );
#ifdef WIN_OS
            u_long nonblocking = 1;
            ioctlsocket( node.sock, FIONBIO, &nonblocking );
#else
            fcntl( node.sock, F_SETFL, O_NONBLOCK );
#endif
            node.state = io_manager::io_node::ST_OK;
            node.last_poll_time = get_millisec();
            G_PAC_INFO()->par[PAC_info::P_BK_ANSWER_MAX_WAIT_TIME] = 10000;
            worker = std::thread( [this, respond, count]()
                {
                for ( int i = 0; i < count; ++i )
                    {
                    bytes request( 6 );
                    if ( !receive_exact( server, request.data(), 6 ) ) break;
                    const auto size = request[4] * 256 + request[5];
                    if ( size < 6 || size > 256 ) break;
                    request.resize( 6 + size );
                    if ( !receive_exact( server, request.data() + 6, size ) ) break;
                    ++requests;
                    respond( server, request, i );
                    }
                } );
            }
        ~io_peer()
            {
            manager.disconnect( &node );
            if ( worker.joinable() ) worker.join();
            close_peer( server );
#ifdef WIN_OS
            WSACleanup();
#endif
            }
        };

    io_manager::io_node* add_io_node( uni_io_manager& manager, int index,
        int type = io_manager::io_node::WAGO_750_XXX_ETHERNET, int registers = 1 )
        {
        manager.add_node( index, type, index + 1, "127.0.0.1", "Loopback",
            registers * 16, registers * 16, registers, registers * 2, registers, registers * 2 );
        return manager.get_node( index );
        }
    }

TEST( uni_io_exchange, fragmented_response_is_complete_before_publication )
    {
    uni_io_manager manager;
    manager.init( 1 );
    auto node = add_io_node( manager, 0 );
    io_peer peer( manager, *node, [&]( int socket, const bytes& request, int )
        {
        const auto response = reply_to( request );
        send_bytes( socket, response, 0, 6 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        send_bytes( socket, response, 6 );
        }, 2 );
    ASSERT_EQ( 0, manager.read_inputs() );
    EXPECT_EQ( 0x0101, node->AI[0] );
    EXPECT_EQ( 1, node->DI[0] );
    EXPECT_FALSE( node->read_io_error_flag );
    EXPECT_EQ( 2, peer.requests );
    }

TEST( uni_io_exchange, fragment_waits_share_one_deadline )
    {
    uni_io_manager manager;
    manager.init( 1 );
    auto node = add_io_node( manager, 0 );
    io_peer peer( manager, *node, []( int socket, const bytes& request, int )
        {
        const auto response = reply_to( request );
        std::this_thread::sleep_for( std::chrono::milliseconds( 160 ) );
        send_bytes( socket, response, 0, 6 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 160 ) );
        send_bytes( socket, response, 6 );
        } );
    node->AI[0] = 1234;
    EXPECT_EQ( 1, manager.read_inputs() );
    EXPECT_TRUE( node->read_io_error_flag );
    EXPECT_EQ( 1234, node->AI[0] );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node->state );
    EXPECT_EQ( 1, peer.requests ); // No second request after a failed read.
    }

TEST( uni_io_exchange, independent_nodes_overlap_but_each_phase_waits_for_acknowledgements )
    {
    uni_io_manager manager;
    manager.init( 2 );
    auto first = add_io_node( manager, 0 );
    auto second = add_io_node( manager, 1 );
    std::array<std::atomic<int>, 4> arrivals{};
    auto respond = [&]( int socket, const bytes& request, int index )
        {
        EXPECT_EQ( index + 1, request[0] * 256 + request[1] );
        ++arrivals[index];
        const auto deadline = steady::now() + std::chrono::milliseconds( 180 );
        while ( arrivals[index] != 2 && steady::now() < deadline )
            std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        EXPECT_EQ( 2, arrivals[index] ) << "Independent node was not sent a request";
        send_bytes( socket, reply_to( request ) );
        };
    io_peer one( manager, *first, respond, 4 );
    io_peer two( manager, *second, respond, 4 );
    ASSERT_EQ( 0, manager.read_inputs() );
    EXPECT_EQ( 0x0101, first->AI[0] );
    EXPECT_EQ( 0x0101, second->AI[0] );
    first->DO_[0] = 1;
    second->AO_[0] = 42;
    ASSERT_EQ( 0, manager.write_outputs() );
    EXPECT_EQ( 1, first->DO[0] );
    EXPECT_EQ( 42, second->AO[0] );
    EXPECT_EQ( 4, one.requests );
    EXPECT_EQ( 4, two.requests );
    EXPECT_EQ( 1u, manager.get_read_timing().cycles );
    EXPECT_EQ( 1u, manager.get_write_timing().cycles );
    EXPECT_GT( manager.get_read_timing().last_us, 0u );
    }

TEST( uni_io_exchange, silent_node_does_not_stop_healthy_node_or_publish_failed_outputs )
    {
    uni_io_manager manager;
    manager.init( 2 );
    auto silent = add_io_node( manager, 0 );
    auto healthy = add_io_node( manager, 1 );
    silent->AI[0] = 55;
    silent->AO_[0] = 99;
    io_peer one( manager, *silent, []( int, const bytes&, int ) {}, 1 );
    io_peer two( manager, *healthy, []( int socket, const bytes& request, int )
        { send_bytes( socket, reply_to( request ) ); }, 4 );
    EXPECT_EQ( 1, manager.read_inputs() );
    EXPECT_TRUE( silent->read_io_error_flag );
    EXPECT_FALSE( healthy->read_io_error_flag );
    EXPECT_EQ( 55, silent->AI[0] );
    EXPECT_EQ( 0x0101, healthy->AI[0] );
    EXPECT_EQ( 1, manager.write_outputs() );
    EXPECT_EQ( 0, silent->AO[0] );
    EXPECT_EQ( 1, one.requests );
    EXPECT_EQ( 4, two.requests );
    }

TEST( uni_io_exchange, wrong_write_acknowledgement_stops_remaining_writes )
    {
    uni_io_manager manager;
    manager.init( 1 );
    auto node = add_io_node( manager, 0 );
    node->DO_[0] = 1;
    node->AO_[0] = 42;
    io_peer peer( manager, *node, []( int socket, const bytes& request, int )
        {
        auto response = reply_to( request );
        response[9] ^= 1;
        send_bytes( socket, response );
        } );
    EXPECT_EQ( 1, manager.write_outputs() );
    EXPECT_EQ( 0, node->DO[0] );
    EXPECT_EQ( 0, node->AO[0] );
    EXPECT_EQ( 1, peer.requests );
    }

TEST( uni_io_exchange, invalid_frames_do_not_publish_inputs )
    {
    for ( int corruption = 0; corruption < 6; ++corruption )
        {
        SCOPED_TRACE( corruption );
        uni_io_manager manager;
        manager.init( 1 );
        auto node = add_io_node( manager, 0 );
        node->DI[0] = 7;
        io_peer peer( manager, *node, [=]( int socket, const bytes& request, int )
            {
            auto response = reply_to( request );
            switch ( corruption )
                {
                case 0: response[1] ^= 1; break; // transaction id
                case 1: response[2] = 1; break; // protocol id
                case 2: response[4] = 255; break; // excessive length
                case 3: response[6] ^= 1; break; // unit id
                case 4: response[7] = 4; break; // function
                case 5: response[8] += 1; break; // byte count
                }
            send_bytes( socket, response );
            } );
        EXPECT_EQ( 1, manager.read_inputs() );
        EXPECT_EQ( 7, node->DI[0] );
        EXPECT_TRUE( node->read_io_error_flag );
        }
    }

TEST( uni_io_exchange, modbus_exception_finishes_without_waiting_for_normal_length )
    {
    uni_io_manager manager;
    manager.init( 1 );
    auto node = add_io_node( manager, 0 );
    io_peer peer( manager, *node, []( int socket, const bytes& request, int )
        {
        bytes response( request.begin(), request.begin() + 9 );
        response[5] = 3;
        response[7] |= 0x80;
        response[8] = 2;
        send_bytes( socket, response );
        } );
    manager.make_read_request( 0, 1, 4 );
    EXPECT_EQ( 0, manager.e_communicate( node, 12, 11 ) );
    EXPECT_EQ( 0x84, manager.buff[7] );
    EXPECT_EQ( io_manager::io_node::ST_OK, node->state );
    }

TEST( uni_io_exchange, phoenix_blocks_remain_ordered_and_status_is_read_last )
    {
    uni_io_manager manager;
    manager.init( 1 );
    auto node = add_io_node( manager, 0, io_manager::io_node::PHOENIX_BK_ETH, 124 );
    io_peer peer( manager, *node, []( int socket, const bytes& request, int index )
        {
        const int expected[] = { 8000, 8123, 7996, 9000, 9123 };
        EXPECT_EQ( expected[index], request[8] * 256 + request[9] );
        auto response = reply_to( request );
        if ( index == 2 ) std::fill( response.begin() + 9, response.end(), 0 );
        send_bytes( socket, response );
        }, 5 );
    ASSERT_EQ( 0, manager.read_inputs() );
    EXPECT_EQ( 0x0101, node->AI[123] );
    ASSERT_EQ( 0, manager.write_outputs() );
    EXPECT_EQ( 5, peer.requests );
    }

TEST( uni_io_exchange, phoenix_udp_recovers_after_loss_and_discards_invalid_datagrams )
    {
#ifdef WIN_OS
    WSADATA winsock;
    ASSERT_EQ( 0, WSAStartup( MAKEWORD( 2, 2 ), &winsock ) );
#endif
    uni_io_manager manager;
    manager.init( 1 );
    auto* node = add_io_node( manager, 0, io_manager::io_node::PHOENIX_BK_ETH );
    const int server = static_cast<int>( socket( AF_INET, SOCK_DGRAM, 0 ) );
    ASSERT_GE( server, 0 );
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
    ASSERT_EQ( 0, bind( server, reinterpret_cast<sockaddr*>( &address ), sizeof( address ) ) );
#ifdef WIN_OS
    int address_length = sizeof( address );
#else
    socklen_t address_length = sizeof( address );
#endif
    ASSERT_EQ( 0, getsockname( server, reinterpret_cast<sockaddr*>( &address ),
        &address_length ) );
    node->sock = static_cast<int>( socket( AF_INET, SOCK_DGRAM, 0 ) );
    ASSERT_GE( node->sock, 0 );
    ASSERT_EQ( 0, connect( node->sock, reinterpret_cast<sockaddr*>( &address ),
        sizeof( address ) ) );
#ifdef WIN_OS
    u_long nonblocking = 1;
    ASSERT_EQ( 0, ioctlsocket( node->sock, FIONBIO, &nonblocking ) );
#else
    ASSERT_EQ( 0, fcntl( node->sock, F_SETFL, O_NONBLOCK ) );
#endif
    node->state = io_manager::io_node::ST_OK;
    node->last_poll_time = get_millisec();
    G_PAC_INFO()->par[PAC_info::P_BK_ANSWER_MAX_WAIT_TIME] = 10000;
    G_PAC_INFO()->set_phoenix_modbus_udp( true );
    manager.phoenix_udp_active = true;
    node->modbus_transaction_id = 65534;
    const int original_socket = node->sock;
    // Leave the first request unanswered. The next cycle must poll immediately
    // on the same socket, without the TCP reconnect delay.
    EXPECT_NE( 0, manager.read_inputs() );
    EXPECT_EQ( io_manager::io_node::ST_OK, node->state );
    EXPECT_EQ( original_socket, node->sock );
    EXPECT_TRUE( node->read_io_error_flag );
    std::atomic<int> requests{ 0 };
    std::thread peer( [&]()
        {
        uint16_t previous_transaction = 0;
        for ( int index = 0; index < 9; ++index )
            {
            fd_set reads;
            FD_ZERO( &reads );
            FD_SET( server, &reads );
            timeval timeout{ 1, 0 };
            if ( select( server + 1, &reads, nullptr, nullptr, &timeout ) <= 0 ) break;
            bytes request( 262 );
            sockaddr_in client{};
#ifdef WIN_OS
            int client_length = sizeof( client );
#else
            socklen_t client_length = sizeof( client );
#endif
            const int n = recvfrom( server,
                reinterpret_cast<char*>( request.data() ),
                static_cast<int>( request.size() ), 0,
                reinterpret_cast<sockaddr*>( &client ), &client_length );
            if ( n < 12 ) break;
            request.resize( n );
            const auto transaction = static_cast<uint16_t>(
                request[0] * 256 + request[1] );
            if ( index == 0 ) EXPECT_EQ( 65535, transaction );
            if ( index > 0 ) EXPECT_EQ(
                static_cast<uint16_t>( previous_transaction + 1 ), transaction );
            previous_transaction = transaction;
            ++requests;
            if ( index == 6 ) continue; // Lose only the status reply.
            auto response = reply_to( request );
            if ( index > 0 && index % 2 == 0 )
                std::fill( response.begin() + 9, response.end(), 0 );
            if ( index == 1 )
                {
                auto send_datagram = [&]( const bytes& data )
                    {
                    sendto( server, reinterpret_cast<const char*>( data.data() ),
                        static_cast<int>( data.size() ), 0,
                        reinterpret_cast<sockaddr*>( &client ), client_length );
                    };
                auto stale = response;
                stale[1] ^= 1;
                send_datagram( stale );
                sendto( server, "", 0, 0,
                    reinterpret_cast<sockaddr*>( &client ), client_length );
                auto invalid = response;
                invalid[2] = 1; // Incorrect protocol with a matching transaction.
                send_datagram( invalid );
                invalid = response;
                invalid[6] ^= 1; // Wrong Unit ID.
                send_datagram( invalid );
                invalid = response;
                invalid[8] = 0; // Wrong byte count.
                send_datagram( invalid );
                invalid = response;
                invalid.resize( 2048 ); // Oversized datagram (WSAEMSGSIZE on Windows).
                send_datagram( invalid );
                }
            sendto( server, reinterpret_cast<const char*>( response.data() ),
                static_cast<int>( response.size() ), 0,
                reinterpret_cast<sockaddr*>( &client ), client_length );
            }
        } );
    EXPECT_EQ( 0, manager.read_inputs() );
    EXPECT_EQ( 0x0101, node->AI[0] );
    EXPECT_EQ( std::chrono::milliseconds( 25 ),
        manager.phase_exchanges[0].deadline -
        manager.phase_exchanges[0].sent_at );
    EXPECT_EQ( 0, G_PAC_INFO()->set_phoenix_modbus_udp_timeout_ms( 40 ) );
    EXPECT_EQ( 0, manager.read_inputs() );
    EXPECT_EQ( std::chrono::milliseconds( 40 ),
        manager.phase_exchanges[0].deadline -
        manager.phase_exchanges[0].sent_at );
    EXPECT_FALSE( node->read_io_error_flag );
    EXPECT_EQ( 5, requests );
    EXPECT_NE( 0, manager.read_inputs() );
    EXPECT_TRUE( node->read_io_error_flag );
    EXPECT_EQ( 1, manager.write_outputs() ); // No output after incomplete read.
    EXPECT_EQ( 7, requests );
    EXPECT_EQ( original_socket, node->sock );
    EXPECT_EQ( 0, manager.read_inputs() );
    EXPECT_FALSE( node->read_io_error_flag );
    EXPECT_EQ( 9, requests );
    manager.disconnect( node );
    peer.join();
    close_peer( server );
    G_PAC_INFO()->set_phoenix_modbus_udp( false );
    G_PAC_INFO()->set_phoenix_modbus_udp_timeout_ms( 25 );
#ifdef WIN_OS
    WSACleanup();
#endif
    }

TEST( uni_io_exchange, phoenix_udp_net_init_creates_datagram_socket )
    {
    uni_io_manager manager;
    manager.init( 1 );
    auto* node = add_io_node( manager, 0, io_manager::io_node::PHOENIX_BK_ETH );
    G_PAC_INFO()->set_phoenix_modbus_udp( true );
    manager.sync_phoenix_transport();
    EXPECT_EQ( 0, manager.net_init( node ) );
    EXPECT_EQ( io_manager::io_node::ST_OK, node->state );
    int type = 0;
#ifdef WIN_OS
    int length = sizeof( type );
#else
    socklen_t length = sizeof( type );
#endif
    EXPECT_EQ( 0, getsockopt( node->sock, SOL_SOCKET, SO_TYPE,
        reinterpret_cast<char*>( &type ), &length ) );
    EXPECT_EQ( SOCK_DGRAM, type );
    manager.disconnect( node );
    G_PAC_INFO()->set_phoenix_modbus_udp( false );
#ifdef WIN_OS
    WSACleanup();
#endif
    }

TEST( uni_io_exchange, switching_phoenix_transport_reopens_only_phoenix_sockets )
    {
#ifdef WIN_OS
    WSADATA winsock;
    ASSERT_EQ( 0, WSAStartup( MAKEWORD( 2, 2 ), &winsock ) );
#endif
    uni_io_manager manager;
    manager.init( 2 );
    auto* phoenix = add_io_node( manager, 0, io_manager::io_node::PHOENIX_BK_ETH );
    auto* wago = add_io_node( manager, 1 );
    phoenix->sock = static_cast<int>( socket( AF_INET, SOCK_STREAM, 0 ) );
    wago->sock = static_cast<int>( socket( AF_INET, SOCK_STREAM, 0 ) );
    ASSERT_GE( phoenix->sock, 0 );
    ASSERT_GE( wago->sock, 0 );
    phoenix->state = io_manager::io_node::ST_OK;
    wago->state = io_manager::io_node::ST_OK;
    const int wago_socket = wago->sock;
    G_PAC_INFO()->set_phoenix_modbus_udp( true );
    manager.sync_phoenix_transport();
    EXPECT_TRUE( manager.phoenix_udp_active );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, phoenix->state );
    EXPECT_EQ( io_manager::io_node::ST_OK, wago->state );
    EXPECT_EQ( wago_socket, wago->sock );
    manager.disconnect( wago );
    G_PAC_INFO()->set_phoenix_modbus_udp( false );
#ifdef WIN_OS
    WSACleanup();
#endif
    }
