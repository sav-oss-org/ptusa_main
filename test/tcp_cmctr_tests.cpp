#include "tcp_cmctr_tests.h"
#include "PAC_info.h"
#include <memory>

using namespace ::testing;

namespace
    {
    class frame_scheduler_test : public tcp_communicator
        {
        public:
            int evaluate() override { return 0; }
            bool ready( bool debug, int service, int available )
                {
                debugger_cycle = debug;
                buf[ 0 ] = 's';
                buf[ 1 ] = static_cast<u_char>( service );
                buf[ 2 ] = 1;
                buf[ 3 ] = 1;
                buf[ 4 ] = 0;
                buf[ 5 ] = 4;
                return buffered_frame_ready( available );
                }
            int frame_size() const { return incoming_frame_size; }
        };
    }

TEST( tcp_communicator, separates_debugger_cycles_and_waits_for_complete_frames )
    {
    auto scheduler = std::make_unique<frame_scheduler_test>();
    EXPECT_FALSE( scheduler->ready( false, 2, 10 ) );
    EXPECT_TRUE( scheduler->ready( true, 2, 10 ) );
    EXPECT_FALSE( scheduler->ready( true, 1, 10 ) );
    EXPECT_TRUE( scheduler->ready( false, 1, 10 ) );
    EXPECT_FALSE( scheduler->ready( true, 2, 5 ) );
    EXPECT_FALSE( scheduler->ready( true, 2, 9 ) );
    EXPECT_TRUE( scheduler->ready( true, 2, 20 ) );
    EXPECT_EQ( 10, scheduler->frame_size() );
    }

#ifndef WIN_OS // For linux to deal with __stdcall.
#define __stdcall
#endif

TEST( tcp_communicator, evaluate )
    {
    tcp_communicator::init_instance( "Тест", "Test" );

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

#ifdef LINUX_OS
    linux_tcp_client cl( "127.0.0.1", G_CMMCTR->get_port(), 1, 1 );
#else
    win_tcp_client cl( "127.0.0.1", G_CMMCTR->get_port(), 1, 1 );
    cl.InitLib();
#endif

    EXPECT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_DISCONNECTED,
        cl.get_connected_state() );
    cl.reconnectTimeout = 0;
    auto cnt = 0;
    auto r = cl.AsyncConnect();
    while ( r != tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED && cnt < 1000 )
        {
        sleep_ms( 1 );
        r = cl.AsyncConnect();
        cnt++;
        }
    ASSERT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED,
        cl.get_connected_state() );

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    //Должен быть получен ответ на подключение.
    auto size = cl.AsyncReceive();
    cnt = 0;
    while ( size <= 0 && cnt < 1000 )
        {
        EXPECT_EQ( 0, G_CMMCTR->evaluate() );
        sleep_ms( 1 );
        size = cl.AsyncReceive();
        cnt++;
        }
    ASSERT_GT( size, 0 );
    cl.buff[ size ] = '\0';
    EXPECT_STREQ( cl.buff, "PAC accept" );

    // Queue a complete frame, then disconnect before evaluate can handle the
    // request. A stale asynchronous client must not tear down the server.
    const char request[] = { 's', 1, 1, 1, 0, 0 };
    memcpy( cl.buff, request, sizeof( request ) );
    cl.AsyncSend( sizeof( request ) );
    cl.Disconnect();
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );


#ifdef LINUX_OS
    linux_tcp_client modbus_cl( "127.0.0.1", G_CMMCTR->get_modbus_port(), 1, 1 );
#else
    win_tcp_client modbus_cl( "127.0.0.1", G_CMMCTR->get_modbus_port(), 1, 1 );
    modbus_cl.InitLib();
#endif

    ASSERT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_DISCONNECTED,
        modbus_cl.get_connected_state() );
    modbus_cl.reconnectTimeout = 0;
    cnt = 0;
    r = modbus_cl.AsyncConnect();;
    while ( r != tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED && cnt < 1000 )
        {
        sleep_ms( 1 );
        r = modbus_cl.AsyncConnect();
        cnt++;
        }
    EXPECT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED,
        modbus_cl.get_connected_state() );

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    sleep_ms( 1 );
    //Для модбас клиента ответа на подключение не будет.
    EXPECT_FALSE( tcp_communicator::checkBuff( modbus_cl.get_socket() ) );

    // Queue a receive without sending data: no socket event should be needed
    // to expire the request. Keep the peer open and silent.
    modbus_cl.tcp_client::AsyncReceive();
    ASSERT_EQ( tcp_client::ACS_CONNECTED, modbus_cl.get_connected_state() );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    ASSERT_EQ( tcp_client::ACS_CONNECTED, modbus_cl.get_connected_state() );
    EXPECT_EQ( tcp_client::AR_BUSY, modbus_cl.get_async_result() );
    modbus_cl.async_queued = get_millisec() - modbus_cl.async_timeout - 1;
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    EXPECT_EQ( tcp_client::AR_TIMEOUT, modbus_cl.get_async_result() );
    EXPECT_EQ( tcp_client::ACS_DISCONNECTED, modbus_cl.get_connected_state() );

    // The completed request must no longer be processed by the communicator.
    modbus_cl.set_async_result( tcp_client::AR_FREE );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    EXPECT_EQ( tcp_client::AR_FREE, modbus_cl.get_async_result() );

    modbus_cl.Disconnect();
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

    tcp_communicator::clear_instance();
    }

TEST( tcp_communicator, checkBuff )
    {
    int s = socket( AF_INET, SOCK_STREAM, 0 );

    auto res = tcp_communicator::checkBuff( s );
#ifdef LINUX_OS
    EXPECT_TRUE( res );
#else
    EXPECT_FALSE( res );
#endif

    res = tcp_communicator::checkBuff( -10 );
    EXPECT_FALSE( res );
    }

int __stdcall fail_select_r1( int, const fd_set*, const fd_set*,
    const fd_set*, const struct timeval* )
    {
    return -1;
    }

int __stdcall fail_select_r0( int, const fd_set*, const fd_set*,
    const fd_set*, const struct timeval* )
    {
    return 0;
    }

int __stdcall good_select( int, const fd_set*, const fd_set*,
    const fd_set*, const struct timeval* )
    {
    return 1;
    }

int __stdcall good_recv(
#ifdef WIN_OS
    SOCKET,
#else
    int,
#endif
    const char*, int len, int flags )
    {
    return 1;
    }

int __stdcall fail_recv_0(
#ifdef WIN_OS
    SOCKET,
#else
    int,
#endif
    const char*, int len, int flags )
    {
    return 0;
    }

TEST( tcp_communicator, recvtimeout )
    {
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_time_next_hour ), SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    const auto SIZE = 100;
    u_char buff[ SIZE ] = { 0 };
    auto stat = stat_time();

    tcp_communicator::init_instance( "Тест", "Test" );
    G_CMMCTR->evaluate();

    const auto TIME_SEC = 1;
    const auto TIME_USEC = 10'000;

    auto res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, TIME_SEC,
        TIME_USEC, "", "", &stat );
    EXPECT_EQ( res, -2 );
    // Проверяем, что время ожидания истекло - время цикла в миллисекундах
    // должно быть не меньше чем `TIME_SEC * 1000` миллисекунд.
    EXPECT_GE( stat.all_time, TIME_SEC * 1000 );
    EXPECT_GE( stat.cycles_cnt, 1 );
    EXPECT_GE( stat.max_iteration_cycle_time, TIME_SEC * 1000 );
    EXPECT_GE( stat.min_iteration_cycle_time, TIME_SEC * 1000 );

    stat.print_cycle_last_h--;
    // Устанавливаем в 0 время ожидания ответа, при превышении которого
    // будет уведомление.
    G_PAC_INFO()->par[ PAC_info::P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ] = 0;
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, TIME_SEC,
        0, "", "", &stat );
    EXPECT_EQ( res, -2 );

    // Should fail - fail with select() returns -1.
    subhook_t select_m1_hook = subhook_new( reinterpret_cast<void*>( &select ),
        reinterpret_cast<void*>( &fail_select_r1 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_m1_hook );
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, TIME_SEC,
        0, "", "", &stat );
    EXPECT_EQ( res, -1 );
    subhook_remove( select_m1_hook );
    subhook_free( select_m1_hook );

    // Should fail - fail with select() returns 0.
    subhook_t select_0_hook = subhook_new( reinterpret_cast<void*>( &select ),
        reinterpret_cast<void*>( &fail_select_r0 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_0_hook );
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, TIME_SEC,
        0, "", "", &stat );
    EXPECT_EQ( res, -2 );
    subhook_remove( select_0_hook );
    subhook_free( select_0_hook );

    // Should fail - fail with recv() returns -1.
    subhook_t select_good_hook = subhook_new( reinterpret_cast<void*>( &select ),
        reinterpret_cast<void*>( &good_select ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_good_hook );
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, TIME_SEC,
        0, "", "", &stat );
    EXPECT_EQ( res, -1 );

    // Should fail - fail with recv() returns 0.
    subhook_t recv_0_hook = subhook_new( reinterpret_cast<void*>( &recv ),
        reinterpret_cast<void*>( &fail_recv_0 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( recv_0_hook );
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, TIME_SEC,
        0, "", "", &stat );
    EXPECT_EQ( res, 0 );
    subhook_remove( recv_0_hook );
    subhook_free( recv_0_hook );

    // Should not fail.
    subhook_t recv_good_hook = subhook_new( reinterpret_cast<void*>( &recv ),
        reinterpret_cast<void*>( &good_recv ), SUBHOOK_64BIT_OFFSET );
    subhook_install( recv_good_hook );
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, TIME_SEC,
        0, "", "", &stat );
    EXPECT_EQ( res, 1 );
    subhook_remove( recv_good_hook );
    subhook_free( recv_good_hook );
    subhook_remove( select_good_hook );
    subhook_free( select_good_hook );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    tcp_communicator::clear_instance();
    }
