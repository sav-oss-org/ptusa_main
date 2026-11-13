#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "console.h"

//-----------------------------------------------------------------------------
void sleep_ms( unsigned int ms )
    {
    usleep( 1000 * ms );
    }
//-----------------------------------------------------------------------------
tm get_time()
    {
    static time_t t_;
    struct tm* timeInfo_;
    t_ = time( 0 );
    timeInfo_ = localtime( &t_ );

    return *timeInfo_;
    }
//-----------------------------------------------------------------------------
