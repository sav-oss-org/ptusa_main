#include "main_cycle_test.h"
#include "lua_manager.h"
#include "tcp_cmctr.h"
#include "PAC_info.h"
#include "prj_mngr.h"
#include <chrono>

#include "dtime.h"

using namespace ::testing;

extern bool G_NO_IO_NODES;
extern bool G_READ_ONLY_IO_NODES;

TEST( main_cycle, main_cycle )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );
    tcp_communicator::init_instance( "Тест", "Test" );

    G_NO_IO_NODES = false;
    G_READ_ONLY_IO_NODES = false;
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] = 1;

    main_cycle();
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_time_next_hour ), SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );
    main_cycle();

    // Минимальное время применяется при каждом вызове, без статического кэша.
    const auto savedSleep = G_PROJECT_MANAGER->sleep_time_ms;
    const auto savedMinimum = G_PROJECT_MANAGER->min_cycle_time;
    G_PROJECT_MANAGER->sleep_time_ms = 0.1;
    G_PROJECT_MANAGER->min_cycle_time = 20;
    const auto started = std::chrono::steady_clock::now();
    main_cycle();
    EXPECT_GE( std::chrono::steady_clock::now() - started,
        std::chrono::milliseconds( 20 ) );
    G_PROJECT_MANAGER->sleep_time_ms = savedSleep;
    G_PROJECT_MANAGER->min_cycle_time = savedMinimum;

    subhook_remove( get_time_hook );
    G_LUA_MANAGER->free_Lua();
    tcp_communicator::clear_instance();
    }
