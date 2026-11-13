#include "dtime.h"

#include <chrono>
#include <cstdint>

namespace
    {
    const auto START_POINT = std::chrono::steady_clock::now();
    }

#ifdef PTUSA_TEST
tm get_time_next_hour()
    {
    thread_local static struct tm timeInfo_;
#ifdef LINUX_OS
    auto t_ = time( nullptr );
    localtime_r( &t_, &timeInfo_ );
#else
    static time_t t_ = time( nullptr );
    localtime_s( &timeInfo_, &t_ );
#endif // LINUX_OS
    timeInfo_.tm_hour++;

    return timeInfo_;
    }

tm get_fixed_time()
    {
    thread_local static struct tm timeInfo_;
    static const time_t FIXED_TIMESTAMP = 1741737600;  // 2025-03-12 00.00.00
#ifdef LINUX_OS
    gmtime_r( &FIXED_TIMESTAMP, &timeInfo_ );
#else
    gmtime_s( &timeInfo_, &FIXED_TIMESTAMP );
#endif
    return timeInfo_;
    }
#endif
//-----------------------------------------------------------------------------
inline auto get_duration()
    {
    auto now = std::chrono::steady_clock::now();
    auto duration = now - START_POINT;
    return duration;
    }
//-----------------------------------------------------------------------------
uint32_t get_sec()
    {
    auto s = std::chrono::duration_cast<std::chrono::seconds>( get_duration() )
                 .count();
    return static_cast<uint32_t>( s );
    }
//-----------------------------------------------------------------------------
uint32_t get_millisec()
    {
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>( get_duration() )
            .count();

    return static_cast<uint32_t>( ms );
    }
//-----------------------------------------------------------------------------
uint32_t get_delta_millisec( uint32_t time1 )
    {
    auto now = get_millisec();
    // Unsigned integer subtraction in C++ handles wraparound correctly via
    // modular arithmetic.
    return now - time1;
    }
