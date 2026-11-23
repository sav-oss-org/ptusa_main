#include "log_tests.h"

#include <cstdio>
#include <cstring>
#include <string>

using namespace ::testing;

namespace
    {

/// @brief In-memory journal sink that records sanitized records.
class recording_log : public i_log
    {
    public:
        void write_log_impl( PRIORITIES priority ) override
            {
            last_priority = priority;
            record = msg;
            ++writes;
            }

        PRIORITIES last_priority { P_INFO };
        std::string record;
        int writes { 0 };
    };

    } // namespace

TEST( i_log, crlf_injection_stays_single_line )
    {
    recording_log journal;

    journal.write_log( i_log::P_INFO,
        "cmd ok\r\n2026-01-01 ALERT  (1) -> forged line" );

    EXPECT_EQ( journal.writes, 1 );
    EXPECT_EQ( journal.last_priority, i_log::P_INFO );
    EXPECT_EQ( journal.record,
        "cmd ok\\r\\n2026-01-01 ALERT  (1) -> forged line" );
    EXPECT_EQ( journal.record.find( '\n' ), std::string::npos );
    EXPECT_EQ( journal.record.find( '\r' ), std::string::npos );
    }

TEST( i_log, direct_msg_write_is_escaped )
    {
    recording_log journal;

    std::snprintf( journal.msg, sizeof( journal.msg ),
        "value\tinjected\nforged\r" );
    journal.write_log( i_log::P_WARNING );

    EXPECT_EQ( journal.writes, 1 );
    EXPECT_EQ( journal.record, "value\\tinjected\\nforged\\r" );
    }

TEST( i_log, escaped_record_never_overflows_buffer )
    {
    recording_log journal;

    // Fill the whole buffer so escaping has to truncate the tail.
    for ( size_t i = 0; i < sizeof( journal.msg ) - 1; ++i )
        {
        journal.msg[ i ] = ( i % 2 == 0 ) ? 'a' : '\n';
        }
    journal.msg[ sizeof( journal.msg ) - 1 ] = '\0';

    journal.write_log( i_log::P_ERR );

    EXPECT_LE( journal.record.size(), sizeof( journal.msg ) - 1 );
    EXPECT_EQ( journal.record.find( '\n' ), std::string::npos );
    EXPECT_EQ( std::strlen( journal.msg ), journal.record.size() );
    }
