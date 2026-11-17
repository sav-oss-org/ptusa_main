# Experimental Tracy profiler integration. Disabled by default; configure
# with -DPTUSA_TRACY=ON and connect the Tracy GUI to port 8086/tcp by
# default.
option(PTUSA_TRACY "Instrument executables with the Tracy profiler client" OFF)

if(NOT PTUSA_TRACY)
    return()
endif()

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)
set(TRACY_STATIC ON)

add_subdirectory(tracy EXCLUDE_FROM_ALL)
