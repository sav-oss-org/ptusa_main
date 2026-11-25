#pragma once

#include "dtime.h"

#include <cstdint>

/// Virtual millisecond clock for timing tests.
///
/// While an instance exists, get_millisec() and every timer based
/// on it follow advance() instead of the wall clock, so timer
/// boundaries become deterministic and tests do not sleep.
///
/// Only one instance may be active at a time. ctest runs every
/// test case in its own process, so the process-wide hook is
/// safe; the RAII destructor restores the system clock.
///
/// Do not combine with DeltaMilliSecSubHooker in one test: that
/// helper replaces get_delta_millisec() outright, so advance()
/// does not move timer expiry while its subhook is installed.
class scoped_fake_clock
    {
public:
    explicit scoped_fake_clock( uint32_t start_ms = 1'000'000 )
        : now_ms( start_ms )
        {
        active = this;
        set_millisec_source( [] { return active->now_ms; } );
        }

    ~scoped_fake_clock()
        {
        set_millisec_source( nullptr );
        active = {};
        }

    scoped_fake_clock( const scoped_fake_clock& ) = delete;
    scoped_fake_clock& operator=( const scoped_fake_clock& ) = delete;

    /// Move the virtual clock forward by the given milliseconds.
    void advance( uint32_t delta_ms )
        {
        now_ms += delta_ms;
        }

    uint32_t now() const
        {
        return now_ms;
        }

private:
    static inline scoped_fake_clock* active {};
    uint32_t now_ms {};
    };
