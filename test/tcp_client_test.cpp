#include "tcp_client_test.h"

using namespace ::testing;

extern int G_DEBUG;

namespace
    {
    class reconnect_test_client : public tcp_client
        {
        public:
            reconnect_test_client() : tcp_client( "127.0.0.1", 502, 1, 0 ) {}
            int connect_result = ACS_DISCONNECTED;
            int AsyncConnect() override
                {
                connectedstate = connect_result;
                return connect_result;
                }
            void Disconnect() override { connectedstate = ACS_DISCONNECTED; }
            void retry_now()
                {
                Disconnect();
                async_last_connect_try = get_millisec() - reconnectTimeout - 1;
                }
        };
    }

TEST( tcp_client, reconnect_backoff_saturates_and_resets_after_success )
    {
    reconnect_test_client cl;
    cl.reconnectTimeout = 500;
    cl.maxreconnectTimeout = 3000;
    for ( auto expected : { 1000u, 2000u, 3000u, 3000u } )
        {
        cl.retry_now();
        EXPECT_EQ( 0, cl.checkConnection() );
        EXPECT_EQ( expected, cl.reconnectTimeout );
        }

    cl.connect_result = tcp_client::ACS_CONNECTED;
    cl.retry_now();
    EXPECT_EQ( 1, cl.checkConnection() );
    EXPECT_EQ( cl.connectTimeout * RECONNECT_MIN_MULTIPLIER, cl.reconnectTimeout );
    }

TEST( tcp_client, reconnect_backoff_handles_above_cap_and_overflow )
    {
    reconnect_test_client cl;
    cl.maxreconnectTimeout = 3000;
    cl.reconnectTimeout = 4000;
    cl.retry_now();
    EXPECT_EQ( 0, cl.checkConnection() );
    EXPECT_EQ( 3000u, cl.reconnectTimeout );

    cl.maxreconnectTimeout = UINT32_MAX;
    cl.reconnectTimeout = UINT32_MAX / 2 + 1;
    cl.retry_now();
    EXPECT_EQ( 0, cl.checkConnection() );
    EXPECT_EQ( UINT32_MAX, cl.reconnectTimeout );
    }

TEST( tcp_client, Connect )
    {
#ifdef WIN_OS
    win_tcp_client cl( "127.0.0.1", 10000 + 1, 1, 1, 256, 0 );
    cl.InitLib();
#else
    linux_tcp_client cl( "127.0.0.1", 10000 + 1, 1, 1, 256, 0 );
#endif // WIN_OS

    // Should fail - timeout - no G_CMMCTR on such port.
    EXPECT_EQ( 0, cl.Connect() );
    }


TEST( tcp_client, AsyncSend )
    {
#ifdef WIN_OS
    win_tcp_client cl( "127.0.0.1", 10000 + 1, 1, 1, 256, 0 );
    cl.InitLib();
#else
    linux_tcp_client cl( "127.0.0.1", 10000 + 1, 1, 1, 256, 0 );
#endif // WIN_OS

    // Should fail - timeout - no G_CMMCTR on such port.
    EXPECT_EQ( 0, cl.AsyncSend( 100 ) );
    }
