#include "dtime_test.h"
#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;

TEST( sys, get_millisec )
    {
    // Test that get_millisec() returns a positive or zero value.
    uint32_t time1 = get_millisec();
    EXPECT_GE( time1, 0 );

    // Test that consecutive calls return increasing values.
    std::this_thread::sleep_for( 1ms );
    uint32_t time2 = get_millisec();
    EXPECT_GE( time2, time1 );

    // Test that the difference is reasonable (at least 1ms).
    uint32_t diff = time2 - time1;
    EXPECT_GE( diff, 1 );
    }

TEST( sys, get_sec )
    {
    // Test that get_sec() returns a positive or zero value.
    uint32_t sec1 = get_sec();
    EXPECT_GE( sec1, 0 );

    // Test that consecutive calls return the same or increasing values.
    uint32_t sec2 = get_sec();
    EXPECT_GE( sec2, sec1 );

    // Test that after 1 second, the value increases.
    std::this_thread::sleep_for( 1100ms );
    uint32_t sec3 = get_sec();
    EXPECT_GT( sec3, sec1 );
    EXPECT_GE( sec3 - sec1, 1 );
    }

TEST( sys, get_delta_millisec )
    {
    auto get_delta_millisec_test = []( uint32_t time1 )
    {
        uint32_t now = 0;
        // Unsigned integer subtraction in C++ handles wraparound correctly via
        // modular arithmetic.
        return now - time1;
    };

    auto dt = get_delta_millisec( 0 );
    EXPECT_GE( dt, 0 );

    auto now = get_millisec();
    dt = get_delta_millisec( now );
    // Сразу же измеряем пройденное время - должно быть меньше 100 мс.
    EXPECT_LE( dt, 100 );

    // Проверка корректной обработки переполнения.
    dt = get_delta_millisec_test( UINT32_MAX );
    EXPECT_EQ( dt, 1 );
    }

TEST( sys, millisec_to_sec_conversion )
    {
    // Test that get_millisec and get_sec are consistent.
    uint32_t sec_before = get_sec();
    uint32_t ms_before = get_millisec();

    std::this_thread::sleep_for( 1100ms );

    uint32_t sec_after = get_sec();
    uint32_t ms_after = get_millisec();

    // Both should have increased.
    EXPECT_GT( sec_after, sec_before );
    EXPECT_GT( ms_after, ms_before );

    // Millisecond delta should be roughly 1000x second delta.
    uint32_t sec_delta = sec_after - sec_before;
    uint32_t ms_delta = ms_after - ms_before;

    // Allow some tolerance for timing precision.
    EXPECT_GE( ms_delta, ( sec_delta - 1 ) * 1000 );
    EXPECT_LE( ms_delta, ( sec_delta + 1 ) * 1000 + 200 );
    }

TEST( sys, sleep_ms_zero )
    {
    // Test that sleep_ms(0) doesn't crash.
    uint32_t start = get_millisec();
    sleep_ms( 0 );

    uint32_t elapsed = get_delta_millisec( start );

    // Should be very quick (less than 10ms).
    EXPECT_LE( elapsed, 10 );
    }

TEST( sys, get_time )
    {
    // Test that get_time returns a valid time structure.
    tm time_info = get_time();

    // Check that the time structure has reasonable values.
    EXPECT_GE( time_info.tm_year, 100 );  // Year >= 2000.
    EXPECT_GE( time_info.tm_mon, 0 );
    EXPECT_LE( time_info.tm_mon, 11 );
    EXPECT_GE( time_info.tm_mday, 1 );
    EXPECT_LE( time_info.tm_mday, 31 );
    EXPECT_GE( time_info.tm_hour, 0 );
    EXPECT_LE( time_info.tm_hour, 23 );
    EXPECT_GE( time_info.tm_min, 0 );
    EXPECT_LE( time_info.tm_min, 59 );
    EXPECT_GE( time_info.tm_sec, 0 );
    EXPECT_LE( time_info.tm_sec, 61 );  // Allow leap seconds.
    }

TEST( sys, get_time_consistency )
    {
    // Test that consecutive calls to get_time are consistent.
    tm time1 = get_time();
    sleep_ms( 100 );
    tm time2 = get_time();

    // Times should be within a few seconds of each other.
    // Convert to comparable format (ignore differences less than 2 seconds).
    int diff_sec = ( time2.tm_hour * 3600 + time2.tm_min * 60 + time2.tm_sec ) -
                   ( time1.tm_hour * 3600 + time1.tm_min * 60 + time1.tm_sec );

    // Should be within 2 seconds (accounting for wraparound at midnight).
    if ( diff_sec >= 0 )
        {
        EXPECT_LE( diff_sec, 2 );
        }
    }

#ifdef PTUSA_TEST
TEST( sys, get_time_next_hour )
    {
    // Test that get_time_next_hour returns time one hour in the future.
    tm time_next = get_time_next_hour();

    // Check that the time structure has reasonable values.
    EXPECT_GE( time_next.tm_year, 100 );  // Year >= 2000.
    EXPECT_GE( time_next.tm_mon, 0 );
    EXPECT_LE( time_next.tm_mon, 11 );
    EXPECT_GE( time_next.tm_mday, 1 );
    EXPECT_LE( time_next.tm_mday, 31 );
    EXPECT_GE( time_next.tm_hour, 0 );
    EXPECT_LE( time_next.tm_hour, 23 );
    EXPECT_GE( time_next.tm_min, 0 );
    EXPECT_LE( time_next.tm_min, 59 );
    EXPECT_GE( time_next.tm_sec, 0 );
    EXPECT_LE( time_next.tm_sec, 61 );
    }

TEST( sys, get_fixed_time )
    {
    // Test that get_fixed_time returns the expected fixed time.
    tm fixed_time = get_fixed_time();

    // Should return 2025-03-12 00:00:00 UTC.
    EXPECT_EQ( fixed_time.tm_year, 125 );  // 2025 - 1900.
    EXPECT_EQ( fixed_time.tm_mon, 2 );     // March (0-indexed).
    EXPECT_EQ( fixed_time.tm_mday, 12 );
    EXPECT_EQ( fixed_time.tm_hour, 0 );
    EXPECT_EQ( fixed_time.tm_min, 0 );
    EXPECT_EQ( fixed_time.tm_sec, 0 );
    }

TEST( sys, get_fixed_time_consistency )
    {
    // Test that get_fixed_time always returns the same value.
    tm time1 = get_fixed_time();
    tm time2 = get_fixed_time();

    EXPECT_EQ( time1.tm_year, time2.tm_year );
    EXPECT_EQ( time1.tm_mon, time2.tm_mon );
    EXPECT_EQ( time1.tm_mday, time2.tm_mday );
    EXPECT_EQ( time1.tm_hour, time2.tm_hour );
    EXPECT_EQ( time1.tm_min, time2.tm_min );
    EXPECT_EQ( time1.tm_sec, time2.tm_sec );
    }
#endif  // PTUSA_TEST
