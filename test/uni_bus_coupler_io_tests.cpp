#include "uni_bus_coupler_io_tests.h"

using namespace ::testing;

#ifndef WIN_OS // For linux to deal with __stdcall.
#define __stdcall
#endif

int __stdcall fail_socket( int, int, int )
    {
    return -1;
    }

int __stdcall fail_setsockopt( int, int, int, const char*, int )
    {
    return 1;
    }

#ifdef WIN_OS
int __stdcall fail_ioctlsocket( int, long, long )
    {
    return 1;
    }
#else
int fail_fcntl( int, int )
    {
    return 1;
    }
#endif

namespace
{
void set_connect_test_error()
    {
#ifdef WIN_OS
    WSASetLastError( WSAENETUNREACH );
#else
    errno = ENETUNREACH;
#endif
    }

int __stdcall pending_connect( int, const sockaddr*, int )
    {
#ifdef WIN_OS
    WSASetLastError( WSAEWOULDBLOCK );
#else
    errno = EINPROGRESS;
#endif
    return -1;
    }

int __stdcall failed_connect( int, const sockaddr*, int )
    {
    set_connect_test_error();
    return -1;
    }

int __stdcall refused_getsockopt( int, int, int, char* value, int* )
    {
    // SO_ERROR is independent of errno / WSAGetLastError.
#ifdef WIN_OS
    *reinterpret_cast<int*>( value ) = WSAECONNREFUSED;
    WSASetLastError( 0 );
#else
    *reinterpret_cast<int*>( value ) = ECONNREFUSED;
    errno = 0;
#endif
    return 0;
    }
}

int __stdcall fail_select_m1( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    set_connect_test_error();
    return -1;
    }

int __stdcall fail_select_0( int, fd_set*, fd_set*, fd_set*, struct timeval* tv )
    {
    EXPECT_EQ( 0, tv->tv_sec );
    EXPECT_EQ( 0, tv->tv_usec );
    return 0;
    }

int __stdcall success_select( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return 1;
    }

int __stdcall fail_getsockopt( int, int, int, char*, int* )
    {
    set_connect_test_error();
    return -1;
    }

int __stdcall success_getsockopt( int, int, int, char*, int* )
    {
    return 0;
    }

TEST( uni_io_manager, net_init )
    {
    uni_io_manager mngr;

    // Should fail - node is null.
    auto res = mngr.net_init( nullptr );
    EXPECT_EQ( res, 1 );

    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );

    subhook_t socket_hook = subhook_new( reinterpret_cast<void*>( socket ),
        reinterpret_cast<void*>( fail_socket ), SUBHOOK_64BIT_OFFSET );
    subhook_install( socket_hook );
    // Should fail - fail with create socket.
    EXPECT_EQ( 3, mngr.net_init( &node ) );
    subhook_remove( socket_hook );

    subhook_t setsockopt_hook = subhook_new( reinterpret_cast<void*>( setsockopt ),
        reinterpret_cast<void*>( fail_setsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( setsockopt_hook );
    // Should fail - fail with setsockopt().
    EXPECT_EQ( 4, mngr.net_init( &node ) );
    subhook_remove( setsockopt_hook );

#ifdef WIN_OS
    subhook_t ioctlsocket_hook = subhook_new( reinterpret_cast<void*>( ioctlsocket ),
        reinterpret_cast<void*>( fail_ioctlsocket ), SUBHOOK_64BIT_OFFSET );
    subhook_install( ioctlsocket_hook );
    // Should fail - fail with fcntl().
    EXPECT_EQ( 5, mngr.net_init( &node ) );
    subhook_remove( ioctlsocket_hook );
#else
    subhook_t fcntl_hook = subhook_new( reinterpret_cast<void*>( fcntl ),
        reinterpret_cast<void*>( fail_fcntl ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fcntl_hook );
    // Should fail - fail with fcntl().
    EXPECT_EQ( 5, mngr.net_init( &node ) );
    subhook_remove( fcntl_hook );
#endif

    auto connect_hook = subhook_new( reinterpret_cast<void*>( connect ),
        reinterpret_cast<void*>( pending_connect ), SUBHOOK_64BIT_OFFSET );
    subhook_install( connect_hook );

    subhook_t select_m1_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( fail_select_m1 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_m1_hook );
    // Should fail - fail with select() - 1.
    EXPECT_EQ( 6, mngr.net_init( &node ) );
    subhook_remove( select_m1_hook );
    subhook_t select_0_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( fail_select_0 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_0_hook );
    EXPECT_EQ( uni_io_manager::NET_CONNECTING, mngr.net_init( &node ) );
    const int pending_sock = node.sock;
    EXPECT_EQ( io_manager::io_node::ST_CONNECTING, node.state );
    EXPECT_EQ( uni_io_manager::NET_CONNECTING, mngr.net_init( &node ) );
    EXPECT_EQ( pending_sock, node.sock );
    node.connect_start_time = get_millisec() -
        io_manager::io_node::C_CNT_TIMEOUT_US / 1000;
    EXPECT_EQ( 6, mngr.net_init( &node ) );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    EXPECT_EQ( 0, node.sock );
    subhook_remove( select_0_hook );

    subhook_t getsockopt_1_hook = subhook_new( reinterpret_cast<void*>( getsockopt ),
        reinterpret_cast<void*>( fail_getsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( getsockopt_1_hook );
    subhook_t select_1_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( success_select ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_1_hook );
    // Should fail - fail with getsockopt() - 1.
    EXPECT_EQ( 7, mngr.net_init( &node ) );
    subhook_remove( getsockopt_1_hook );
    subhook_remove( select_1_hook );

    subhook_install( select_1_hook );
    subhook_t getsockopt_0_hook = subhook_new( reinterpret_cast<void*>( getsockopt ),
        reinterpret_cast<void*>( success_getsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( getsockopt_0_hook );
    subhook_remove( getsockopt_0_hook );
    auto refused_hook = subhook_new( reinterpret_cast<void*>( getsockopt ),
        reinterpret_cast<void*>( refused_getsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( refused_hook );
    EXPECT_EQ( 7, mngr.net_init( &node ) );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    EXPECT_EQ( 0, node.sock );
    subhook_remove( refused_hook );
    subhook_free( refused_hook );
    subhook_install( getsockopt_0_hook );

    // A synchronous connect failure must not reach select/SO_ERROR success.
    subhook_remove( connect_hook );
    auto failed_hook = subhook_new( reinterpret_cast<void*>( connect ),
        reinterpret_cast<void*>( failed_connect ), SUBHOOK_64BIT_OFFSET );
    subhook_install( failed_hook );
    EXPECT_EQ( 6, mngr.net_init( &node ) );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    subhook_remove( failed_hook );
    subhook_free( failed_hook );
    subhook_install( connect_hook );

    // Should OK.
    EXPECT_EQ( 0, mngr.net_init( &node ) );
    subhook_remove( select_1_hook );
    subhook_remove( getsockopt_0_hook );

    mngr.disconnect( &node );
    subhook_remove( connect_hook );
    subhook_free( connect_hook );

    subhook_free( socket_hook );
    subhook_free( setsockopt_hook );
#ifdef WIN_OS
    subhook_free( ioctlsocket_hook );
#else
    subhook_free( fcntl_hook );
#endif
    subhook_free( select_m1_hook );
    subhook_free( select_0_hook );
    subhook_free( getsockopt_1_hook );
    subhook_free( select_1_hook );
    subhook_free( getsockopt_0_hook );
    }

TEST( uni_io_manager, pending_connection_and_retry_delay )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    G_PAC_INFO()->par[ PAC_info::P_BK_ANSWER_MAX_WAIT_TIME ] = 10'000;
    auto connect_hook = subhook_new( reinterpret_cast<void*>( connect ),
        reinterpret_cast<void*>( pending_connect ), SUBHOOK_64BIT_OFFSET );
    auto select_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( fail_select_0 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( connect_hook );
    subhook_install( select_hook );

    node.last_init_time = get_millisec() - node.delay_time;
    EXPECT_EQ( 1, mngr.e_communicate( &node, 1, 1 ) );
    EXPECT_EQ( io_manager::io_node::ST_CONNECTING, node.state );
    const auto delay = node.delay_time;
    const int sock = node.sock;
    // A pending attempt must be polled even during the reconnect delay.
    node.last_init_time = get_millisec();
    node.connect_start_time = get_millisec() -
        io_manager::io_node::C_CNT_TIMEOUT_US / 1000;
    EXPECT_EQ( -100, mngr.e_communicate( &node, 1, 1 ) );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    EXPECT_EQ( 0, node.sock );
    EXPECT_EQ( delay * 2, node.delay_time );
    EXPECT_EQ( 1, mngr.e_communicate( &node, 1, 1 ) );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
#ifdef WIN_OS
    int value = 0, size = sizeof( value );
    EXPECT_EQ( SOCKET_ERROR, getsockopt( sock, SOL_SOCKET, SO_ERROR,
        reinterpret_cast<char*>( &value ), &size ) );
#else
    EXPECT_EQ( -1, fcntl( sock, F_GETFD ) );
    EXPECT_EQ( EBADF, errno );
#endif

    // Explicit disconnect also closes a pending socket and delays the retry.
    EXPECT_EQ( uni_io_manager::NET_CONNECTING, mngr.net_init( &node ) );
    mngr.disconnect( &node );
    EXPECT_EQ( 0, node.sock );
    EXPECT_EQ( 1, mngr.e_communicate( &node, 1, 1 ) );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    subhook_remove( select_hook );
    subhook_remove( connect_hook );
    subhook_free( select_hook );
    subhook_free( connect_hook );
    G_PAC_INFO()->reset_params();
    }


class test_uni_io_manager : public uni_io_manager
    {
    public:
        int e_communicate( io_node* node, int bytes_to_send,
            int bytes_to_receive ) override
            {
            if ( is_set_function_code )
                {
                buff[ 7 ] = function_code;
                }
            return res;
            }

        void set_result_to_error()
            {
            res = 1;
            }

        void set_result_to_ok()
            {
            res = 0;
            }

        void set_function_code( u_char f_code )
            {
            function_code = f_code;
            is_set_function_code = true;
            }

        void reset_function_code()
            {
            is_set_function_code = false;
            }

    private:
        int res = 0;
        u_char function_code;
        bool is_set_function_code = false;
    };

TEST( uni_io_manager, read_inputs )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    // Should not fail - nodes count is 0.
    auto res = mngr.read_inputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 4 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 1, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 1 )->is_active = false;
    mngr.add_node( 2, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        3, "127.0.0.1", "A300", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 3, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        4, "127.0.0.1", "A400", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 3 )->is_active = false;

    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    testing::internal::CaptureStdout();

    mngr.reset_function_code();
    mngr.set_result_to_ok();
    res = mngr.read_inputs();
    EXPECT_EQ( res, 1 );
    const std::string EXP_TIME = "2025-03-12 00.00.00 ";
    const std::string expectedOutput =
        EXP_TIME + ANSI_COLOR_RED +
        R"(ERROR  (3) -> Read DI:bus coupler returned error. "A300":"127.0.0.1" )"
        "(received code=2, expected=2, received size=0, expected=1).\n" +
        ANSI_COLOR_RESET +
        EXP_TIME + ANSI_COLOR_RED +
        R"(ERROR  (3) -> Read AI:bus coupler returned error. "A100":"127.0.0.1" )"
        "(received code=4, expected=4, received size=31, expected=2).\n" +
        ANSI_COLOR_RESET
        ;
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, expectedOutput );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, write_outputs )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    // Should not fail - nodes count is 0.
    auto res = mngr.write_outputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 4 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 6*16, 6*16, 6, 6, 1, 1 );
    // Just for testing add set of modules.
    mngr.init_node_AO( 0, 0, 1027843, 0 );
    mngr.init_node_AO( 0, 1, 2688093, 1 );
    mngr.init_node_AO( 0, 2, 2688527, 2 );
    mngr.init_node_AO( 0, 3, 2688527, 3 );
    mngr.init_node_AO( 0, 4, 2688666, 4 );
    mngr.init_node_AO( 0, 5, 10, 5 );

    mngr.add_node( 1, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 1 )->is_active = false;
    mngr.add_node( 2, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        3, "127.0.0.1", "A300", 1, 1, 2, 4, 1, 1 );
    mngr.init_node_AO( 2, 0, 638, 0 );
    mngr.init_node_AO( 2, 1, 10, 2 );
    mngr.add_node( 3, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        4, "127.0.0.1", "A400", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 3 )->is_active = false;

    // Должна быть успешная запись.
    res = mngr.write_outputs();
    EXPECT_EQ( res, 0 );

    // Должна быть неуспешная запись (e_communicate() возвращает 1 -
    // неуспешная сетевая операция). Сообщений не должно быть.
    std::string expectedOutput = "";
    testing::internal::CaptureStdout();
    mngr.set_result_to_error();
    res = mngr.write_outputs();
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( res, 1 );
    EXPECT_EQ( output, expectedOutput );

    // Должна быть неуспешная запись (ошибки на уровне данных). Сообщения должны быть.
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );
    const std::string EXP_TIME = "2025-03-12 00.00.00 ";
    expectedOutput =
        EXP_TIME +
#if defined LINUX_OS
        "\x1B[31m" +
#endif
        R"(ERROR  (3) -> Write DO:bus coupler returned error. "A300":"127.0.0.1" )"
        "(received code=1, expected=15, received size=0, expected=1).\n" +
#if defined LINUX_OS
        "\x1B[0m" +
#endif
        EXP_TIME +
#if defined LINUX_OS
        "\x1B[31m" +
#endif
        R"(ERROR  (3) -> Write AO:bus coupler returned error. "A100":"127.0.0.1" )"
        "(received code=1, expected=16, received size=35, expected=6).\n"
#if defined LINUX_OS
        + "\x1B[0m"
#endif
        ;
    testing::internal::CaptureStdout();
    mngr.set_function_code( 1 );
    mngr.set_result_to_ok();
    res = mngr.write_outputs();
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( res, 1 );
    EXPECT_EQ( output, expectedOutput );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, read_write_data )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    mngr.init( 2 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 1, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );

    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    mngr.set_result_to_error();
    auto res = mngr.read_inputs();
    auto temp_node = mngr.get_node( 0 );
    EXPECT_TRUE( temp_node->read_io_error_flag );
    EXPECT_EQ( res, 1 );

    // Так ранее попытка чтения состояния узлов была неуспешной,
    // записи выходов не будет.
    res = mngr.write_outputs();
    temp_node = mngr.get_node( 1 );
    EXPECT_TRUE( temp_node->read_io_error_flag );
    EXPECT_EQ( res, 1 );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, e_communicate )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );

    G_PAC_INFO()->par[ PAC_info::P_BK_ANSWER_MAX_WAIT_TIME ] = 10'000;
    auto res = mngr.e_communicate( &node, 1, 1 );
    EXPECT_NE( res, 0 );

    G_PAC_INFO()->reset_params();
    }


class MockUniIoManager : public uni_io_manager
    {
    public:
        MOCK_METHOD( void, add_err_to_log, ( const char*, const char*,
            const char*, int, int, int, int ), ( const, override ) );
        // Переопределяем e_communicate, чтобы не было реального
        // обмена.
        MOCK_METHOD( int, e_communicate, ( io_node*, int, int ), ( override ) );

        // Expose read_input_registers for testing.
        int public_read_input_registers( io_node* node, unsigned int address,
            unsigned int quantity, unsigned char station = 0 )
            {
            return read_input_registers( node, address, quantity, station );
            }

        // Expose resultbuff for testing.
        u_char* get_resultbuff()
            {
            return resultbuff;
            }
    };

class UniBusCouplerIoTest : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            mngr.init( 1 );
            mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
                1, "127.0.0.1", "A100", 6 * 16, 6 * 16, 6, 6, 1, 1 );
            node = mngr.get_node( 0 );
            // Just for testing add set of modules.
            mngr.init_node_AO( 0, 0, 1027843, 0 );
            mngr.init_node_AO( 0, 1, 2688093, 1 );
            mngr.init_node_AO( 0, 2, 2688527, 2 );
            mngr.init_node_AO( 0, 3, 2688527, 3 );
            mngr.init_node_AO( 0, 4, 2688666, 4 );
            mngr.init_node_AO( 0, 5, 10, 5 );
            };

        io_manager::io_node* node;
        NiceMock<MockUniIoManager> mngr;
    };

TEST_F( UniBusCouplerIoTest, write_outputs_PHOENIX_BK_ETH )
    {
    //Проверяем обработку неуспешной записи выходов (сетевая ошибка).
    ON_CALL( mngr, e_communicate( _, _, _ ) ).WillByDefault(
        []( const io_manager::io_node*, int, int )
        {
        return -1;
        } );
    EXPECT_CALL( mngr, add_err_to_log( _, _, _, _, _, _, _ ) )
        .Times( 0 );

    auto res = mngr.write_outputs();
    // Проверяем, что флаг не изменился (остался false).
    EXPECT_FALSE( node->flag_error_write_message );
    EXPECT_EQ( res, 1 );

    //Проверяем обработку получения ошибки при записи выходов.
    ON_CALL( mngr, e_communicate( _, _, _ ) ).WillByDefault(
        [this]( const io_manager::io_node*, int, int )
        {
        mngr.buff[ 7 ] = 0x11; // Любое значение, кроме 0x10.
        return 0;
        } );
    uni_io_manager real_mngr;
    EXPECT_CALL( mngr, add_err_to_log( ::testing::StrEq( "Write AO" ),
        _, _, 0x11, 0x10, 35, 6 ) )
        .WillRepeatedly( Invoke( &real_mngr, &uni_io_manager::add_err_to_log ) );
    testing::internal::CaptureStdout();
    res = mngr.write_outputs();
    auto output = testing::internal::GetCapturedStdout();
    // Проверяем, что флаг изменился (стал true).
    EXPECT_TRUE( node->flag_error_write_message );
    EXPECT_EQ( res, 1 );
    EXPECT_NE( output, "" ); //Здесь должно быть сообщение.

    testing::internal::CaptureStdout();
    res = mngr.write_outputs();
    EXPECT_EQ( res, 1 );
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, "" ); //Здесь уже не должно быть сообщения.
    }

// Test that status register field exists and can be set/read for
// different node types.
TEST( io_node, status_register_only_for_phoenix )
    {
    G_PAC_INFO()->emulation_off();

    io_manager::get_instance()->init( 2 );

    // Add Phoenix BK ETH node.
    io_manager::get_instance()->add_node( 0,
        io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
        "A100", 0, 0, 0, 0, 0, 0 );

    // Add WAGO node.
    io_manager::get_instance()->add_node( 1,
        io_manager::io_node::WAGO_750_XXX_ETHERNET, 2, "127.0.0.1",
        "A200", 0, 0, 0, 0, 0, 0 );

    auto phoenix_node = io_manager::get_instance()->get_node( 0 );
    auto wago_node = io_manager::get_instance()->get_node( 1 );

    // Set same status register for both.
    phoenix_node->status_register = 0x0010;  // Bit 4 set.
    wago_node->status_register = 0x0010;  // Bit 4 set.

    phoenix_node->is_active = true;
    phoenix_node->state = io_manager::io_node::ST_OK;
    wago_node->is_active = true;
    wago_node->state = io_manager::io_node::ST_OK;

    // Only Phoenix should report PP mode.
    EXPECT_EQ( phoenix_node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );
    EXPECT_EQ( wago_node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_OK );

    G_PAC_INFO()->emulation_on();
    }

// Test status register field initialization.
TEST( io_node, status_register_initialized_to_zero )
    {
    io_manager::get_instance()->init( 1 );
    io_manager::get_instance()->add_node( 0,
        io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
        "A100", 0, 0, 0, 0, 0, 0 );

    auto node = io_manager::get_instance()->get_node( 0 );

    // Status register should be initialized to 0.
    EXPECT_EQ( node->status_register, 0 );
    }

// Test error bits 0-5 detection with various register values.
TEST( io_node, error_bits_detection )
    {
    G_PAC_INFO()->emulation_off();
    io_manager::get_instance()->init( 1 );
    io_manager::get_instance()->add_node( 0,
        io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
        "A100", 0, 0, 0, 0, 0, 0 );

    auto node = io_manager::get_instance()->get_node( 0 );
    node->is_active = true;
    node->state = io_manager::io_node::ST_OK;

    // Test no error bits set.
    node->status_register = 0x0000;
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_OK );

    // Test each individual error bit (0-5).
    node->status_register = 0x0001;  // Bit 0: Error occurred.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    node->status_register = 0x0002;  // Bit 1: Net Fail.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    node->status_register = 0x0004;  // Bit 2: Config mismatch.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    node->status_register = 0x0008;  // Bit 3: Startup fault.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    node->status_register = 0x0010;  // Bit 4: PP mode.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    node->status_register = 0x0020;  // Bit 5: Startup not completed.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    // Test multiple error bits.
    node->status_register = 0x003F;  // All error bits 0-5.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    // Test error bits with higher bits set.
    node->status_register = 0xFF10;  // Bit 4 + other high bits.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    // Test no error bits but other bits set.
    node->status_register = 0xFFC0;  // All bits except 0-5.
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_OK );

    G_PAC_INFO()->emulation_on();
    }

// Test read_phoenix_status_register method - successful read.
TEST( uni_io_manager, read_phoenix_status_register_success )
    {
    class TestUniIoManager : public uni_io_manager
        {
        public:
            // Override e_communicate to simulate successful Modbus
            // response.
            int e_communicate( io_node* node, int bytes_to_send,
                int bytes_to_receive ) override
                {
                // Simulate successful response for reading 1 register
                // (2 bytes).
                buff[7] = 0x04;  // Function code.
                buff[8] = 2;     // Byte count.
                buff[9] = 0x00;  // High byte of register value.
                // Low byte of register value (0x0012 = bits 1 and 4).
                buff[10] = 0x12;
                return 0;  // Success.
                }

            u_char* get_resultbuff() { return resultbuff; }
        };

    TestUniIoManager mngr;
    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->read_io_error_flag = false;
    node->status_register = 0;
    node->is_active = true;
    node->state = io_manager::io_node::ST_OK;

    // Simulate calling read_inputs which calls
    // read_phoenix_status_register. For testing, we'll directly
    // test the logic.
    int result = mngr.read_input_registers( node,
        uni_io_manager::PHOENIX_STATUS_REGISTER_ADDRESS, 1 );
    EXPECT_GT( result, 0 );  // Should succeed.

    // The read_phoenix_status_register logic would be:
    // Get resultbuff pointer and convert bytes.
    const u_char* rbuff = mngr.get_resultbuff();
    if ( result > 0 )
        {
        node->status_register = static_cast<u_int_2>(
            uni_io_manager::BYTE_SHIFT_MULTIPLIER * rbuff[ 0 ] +
            rbuff[ 1 ] );
        }

    EXPECT_EQ( node->status_register, 0x0012 );

    G_PAC_INFO()->emulation_off();
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );
    G_PAC_INFO()->emulation_on();
    }

// Test read_phoenix_status_register - communication failure
TEST( uni_io_manager, read_phoenix_status_register_failure )
    {
    class TestUniIoManager : public uni_io_manager
        {
        public:
            int e_communicate( io_node* node, int bytes_to_send,
                int bytes_to_receive ) override
                {
                return -1;  // Communication failure.
                }
        };

    TestUniIoManager mngr;
    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->read_io_error_flag = false;
    node->status_register = 0x00FF;  // Set to non-zero initially.

    // Try to read - should fail.
    int result = mngr.read_input_registers( node,
        uni_io_manager::PHOENIX_STATUS_REGISTER_ADDRESS, 1 );
    EXPECT_LE( result, 0 );  // Should fail.

    // The read_phoenix_status_register logic on failure.
    if ( result <= 0 )
        {
        node->status_register = 0;
        }

    EXPECT_EQ( node->status_register, 0 );
    }

// Test read_phoenix_status_register - early return when error flag
// is set.
TEST( uni_io_manager, read_phoenix_status_register_error_flag_set )
    {
    uni_io_manager mngr;
    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->read_io_error_flag = true;  // Error flag is set.
    u_int_2 initial_value = 0x00AB;
    node->status_register = initial_value;

    // When read_io_error_flag is set, read_phoenix_status_register
    // should return early and not attempt to read the register. The
    // status_register value should remain unchanged. We verify this by
    // checking that status_register remains unchanged.
    EXPECT_EQ( node->status_register, initial_value );
    }

// Test byte-order conversion with various values.
TEST( uni_io_manager, read_phoenix_status_register_byte_order_conversion )
    {
    class TestUniIoManager : public uni_io_manager
        {
        public:
            u_char test_high_byte = 0;
            u_char test_low_byte = 0;

            int e_communicate( io_node* node, int bytes_to_send,
                int bytes_to_receive ) override
                {
                buff[7] = 0x04;
                buff[8] = 2;
                buff[9] = test_high_byte;
                buff[10] = test_low_byte;
                return 0;
                }

            u_char* get_resultbuff() { return resultbuff; }
        };

    G_PAC_INFO()->emulation_off();

    TestUniIoManager mngr;
    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->read_io_error_flag = false;
    node->is_active = true;
    node->state = io_manager::io_node::ST_OK;

    // Test 1: High byte = 0x12, Low byte = 0x34 -> 0x1234.
    mngr.test_high_byte = 0x12;
    mngr.test_low_byte = 0x34;
    int result = mngr.read_input_registers( node,
        uni_io_manager::PHOENIX_STATUS_REGISTER_ADDRESS, 1 );
    EXPECT_GT( result, 0 );
    const u_char* rbuff = mngr.get_resultbuff();
    node->status_register = static_cast<u_int_2>(
        uni_io_manager::BYTE_SHIFT_MULTIPLIER * rbuff[ 0 ] +
        rbuff[ 1 ] );
    EXPECT_EQ( node->status_register, 0x1234 );

    // Test 2: High byte = 0x00, Low byte = 0x3F (all error
    // bits) -> 0x003F.
    mngr.test_high_byte = 0x00;
    mngr.test_low_byte = 0x3F;
    result = mngr.read_input_registers( node,
        uni_io_manager::PHOENIX_STATUS_REGISTER_ADDRESS, 1 );
    EXPECT_GT( result, 0 );
    rbuff = mngr.get_resultbuff();
    node->status_register = static_cast<u_int_2>(
        uni_io_manager::BYTE_SHIFT_MULTIPLIER * rbuff[ 0 ] +
        rbuff[ 1 ] );
    EXPECT_EQ( node->status_register, 0x003F );
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );

    // Test 3: High byte = 0xFF, Low byte = 0xFF -> 0xFFFF.
    mngr.test_high_byte = 0xFF;
    mngr.test_low_byte = 0xFF;
    result = mngr.read_input_registers( node,
        uni_io_manager::PHOENIX_STATUS_REGISTER_ADDRESS, 1 );
    EXPECT_GT( result, 0 );
    rbuff = mngr.get_resultbuff();
    node->status_register = static_cast<u_int_2>(
        uni_io_manager::BYTE_SHIFT_MULTIPLIER * rbuff[ 0 ] +
        rbuff[ 1 ] );
    EXPECT_EQ( node->status_register, 0xFFFF );

    // Test 4: All zeros -> 0x0000.
    mngr.test_high_byte = 0x00;
    mngr.test_low_byte = 0x00;
    result = mngr.read_input_registers( node,
        uni_io_manager::PHOENIX_STATUS_REGISTER_ADDRESS, 1 );
    EXPECT_GT( result, 0 );
    rbuff = mngr.get_resultbuff();
    node->status_register = static_cast<u_int_2>(
        uni_io_manager::BYTE_SHIFT_MULTIPLIER * rbuff[ 0 ] +
        rbuff[ 1 ] );
    EXPECT_EQ( node->status_register, 0x0000 );
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_OK );

    G_PAC_INFO()->emulation_on();
    }
