#include "log_tests.h"

#include <cstdio>
#include <cstring>
#include <string>

using namespace ::testing;

namespace
    {

/// @brief In-memory sink that records both views of a record:
/// the raw console message and the single-line copy read by
/// line-oriented journal sinks.
class recording_log : public i_log
    {
    public:
        void write_log_impl( PRIORITIES priority ) override
            {
            last_priority = priority;
            console = msg;
            line = msg_journal;
            ++writes;
            }

        PRIORITIES last_priority { P_INFO };
        std::string console;
        std::string line;
        int writes { 0 };
    };

    } // namespace

TEST( i_log, crlf_injection_stays_single_line )
    {
    recording_log sink;

    sink.write_log( i_log::P_INFO,
        "cmd ok\r\n2026-01-01 ALERT  (1) -> forged line" );

    EXPECT_EQ( sink.writes, 1 );
    EXPECT_EQ( sink.last_priority, i_log::P_INFO );
    EXPECT_EQ( sink.line,
        "cmd ok\\r\\n2026-01-01 ALERT  (1) -> forged line" );
    EXPECT_EQ( sink.line.find( '\n' ), std::string::npos );
    EXPECT_EQ( sink.line.find( '\r' ), std::string::npos );

    // The console view reaches sinks untouched.
    EXPECT_EQ( sink.console,
        "cmd ok\r\n2026-01-01 ALERT  (1) -> forged line" );
    }

TEST( i_log, direct_msg_write_keeps_console_view_raw )
    {
    recording_log sink;

    std::snprintf( sink.msg, sizeof( sink.msg ),
        "value\tinjected\nforged\r" );
    sink.write_log( i_log::P_WARNING );

    EXPECT_EQ( sink.writes, 1 );
    EXPECT_EQ( sink.console, "value\tinjected\nforged\r" );
    EXPECT_EQ( sink.line, "value\tinjected\\nforged\\r" );
    }

TEST( i_log, line_copy_never_overflows_buffer )
    {
    recording_log sink;

    // Fill the whole buffer so escaping has to truncate the tail.
    for ( size_t i = 0; i < sizeof( sink.msg ) - 1; ++i )
        {
        sink.msg[ i ] = ( i % 2 == 0 ) ? 'a' : '\n';
        }
    sink.msg[ sizeof( sink.msg ) - 1 ] = '\0';

    sink.write_log( i_log::P_ERR );

    EXPECT_LE( sink.line.size(), sizeof( sink.msg ) - 1 );
    EXPECT_EQ( sink.line.find( '\n' ), std::string::npos );

    // @msg survives unchanged for the console rendering.
    EXPECT_EQ( sink.console.size(), sizeof( sink.msg ) - 1 );
    }
