#include "PAC_dev_lua_tests.h"
#include "g_errors.h"

using namespace ::testing;

TEST( toLuapp, tolua_PAC_dev_open )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    EXPECT_EQ( 0,
        luaL_dostring( L, "o1=tech_object( \'O1\', 1, 1, \'O1\', 1, 1, 10, 10, 10, 10 )" ) );
    EXPECT_EQ( 0,
        luaL_dostring( L, "o1:get_modes_manager():add_operation(\'Test operation\')" ) );

    // Test "u_int operation_state::steps_count() const".
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ operation.RUN ]:steps_count()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 0, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    EXPECT_EQ( 0,
        luaL_dostring( L, "o1:get_modes_manager()[ 1 ]:add_step(\'Test step\', -1, -1, -1 )" ) );

    // Test "int enable_step_by_signal::set_bool_property( const char* name,
    // bool value )".
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ][ step.A_ENABLE_STEP_BY_SIGNAL ]:"
            "set_bool_property( 'should_turn_off', false )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 0, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    // Test "int step::get_tag() const".
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ]:get_tag()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( -1, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    // Test "int step::set_tag( int new_tag )".
    EXPECT_EQ( 0,
        luaL_dostring( L, "o1:get_modes_manager()[ 1 ][ 1 ][ 1 ]:set_tag( 20 )" ) );
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ]:get_tag()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 20, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    // Test "unsigned int steps_count() const".
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ operation.RUN ]:steps_count()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 1, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    // Test "int jump_if_action::set_int_property( const char* name,
    // unsigned int idx, int value )".
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ][ step.A_JUMP_IF ]:"
            "set_int_property( 'next_step_n', 0, 1 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 0, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ][ step.A_JUMP_IF ]:"
            "set_int_property( 'no_such_property', 0, 1 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 1, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    // Test "const char* operation::get_name() const".
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ]:get_name()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_STREQ( "Test operation", tolua_tostring( L, -1, 0 ) );
    lua_remove( L, -1 );


    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_PDS00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = PDS()" ) );  //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = PDS( \'PDS1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto PDS1 = static_cast<diff_pressure*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, PDS1 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_get_delta_millisec00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 1, luaL_dostring( L,
        "res = get_delta_millisec()" ) );               //Некорректный вызов.

    ASSERT_EQ( 0, luaL_dostring( L, "ms = get_millisec()" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "dt_0 = get_delta_millisec( 0 )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "dt_ms = get_delta_millisec( ms )" ) );

    lua_getfield( L, LUA_GLOBALSINDEX, "ms" );
    auto ms = tolua_tonumber( L, -1, 0 );
    lua_remove( L, -1 );
    lua_getfield( L, LUA_GLOBALSINDEX, "dt_0" );
    auto dt_0 = tolua_tonumber( L, -1, 0 );
    lua_remove( L, -1 );
    lua_getfield( L, LUA_GLOBALSINDEX, "dt_ms" );
    auto dt_ms = tolua_tonumber( L, -1, 0 );
    lua_remove( L, -1 );

    // Запрашиваем время, прошедшее с момента 0 - результат должен быть
    // неотрицательным.
    EXPECT_GE( dt_0, 0 );
    // Запрашиваем время, прошедшее с момента ms - результат должен быть
    // неотрицательным.
    EXPECT_GE( dt_ms, 0 );
    // Время должно совпать с расчетным.
    EXPECT_NEAR( ms, dt_0, 2 );
    EXPECT_NEAR( dt_ms, 0, 2 );

    // Корректная обработка переполнения - результат должен быть положительным
    // и примерно равен UINT32_MAX.
    ASSERT_EQ( 0, luaL_dostring( L, "ms = get_millisec()" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "res = get_delta_millisec( ms + 10 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto dt3 = tolua_tonumber( L, -1, 0 );
    EXPECT_GT( dt3, 0 );
    EXPECT_NEAR( dt3, UINT32_MAX, 10 + 2 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_get_sec00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L,
        "res = get_sec( 1 )" ) );                       //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = get_sec()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_GE( tolua_tonumber( L, -1, 0 ), 0 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_get_millisec00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L,
        "res = get_millisec( 1 )" ) );                  //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = get_millisec()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_GE( tolua_tonumber( L, -1, 0 ), 0 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_TS00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = TS()" ) );   //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = TS( \'TS1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto TS1 = static_cast<temperature_signal*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, TS1 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_C00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = C()" ) );   //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = C( \'TRC1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto TRC1 = static_cast<i_DO_AO_device*>( tolua_touserdata( L, -1, nullptr ) );
    lua_remove( L, -1 );
    EXPECT_NE( nullptr, TRC1 );
    EXPECT_EQ( STUB(), dynamic_cast<dev_stub*>( TRC1 ) );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_device_param_emulator00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( 4, 1, \'TE1\', \'Test device\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L,
        "TE1 = G_DEVICE_MANAGER():get_device( 4, \'TE1\')" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "TE1" );
    auto TE1 = static_cast<device*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, TE1 );
    lua_remove( L, -1 );

    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    TE1->save_device( buff );
    EXPECT_STREQ(
        "TE1={M=0, ST=1, V=0, E=0, M_EXP=20.0, S_DEV=2.0, P_CZ=0, P_ERR=0},\n",
        buff );

    //Некорректные параметры при вызове метода.
    ASSERT_EQ( 1, luaL_dostring( L, "TE1:param_emulator( 50 )" ) );

    ASSERT_EQ( 0, luaL_dostring( L, "TE1:param_emulator( 50, 5 )" ) );
    TE1->save_device( buff );
    EXPECT_STREQ(
        "TE1={M=0, ST=1, V=0, E=0, M_EXP=50.0, S_DEV=5.0, P_CZ=0, P_ERR=0},\n",
        buff );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_wages_get_state00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_WT, device.DST_WT_ETH, \'WT1\', \'Test wages\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "WT1 = WT( \'WT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "WT1" );
    auto WT1 = static_cast<i_wages*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, WT1 );
    lua_remove( L, -1 );

    auto ip = "0.0.0.0";
    auto field = "IP";
    dynamic_cast<wages_eth*>( WT1 )->set_string_property( field, ip );

    auto st = WT1->get_state();
    //Некорректный вызов - отсуствует self.
    ASSERT_NE( 0, luaL_dostring( L, "res = WT1.get_state()" ) );
    //Корректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = WT1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto lua_st = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    ASSERT_EQ( st, lua_st );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_G00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_G, device.DST_G_IOL_4, \'G1\', \'Test power module\', \'\' )" ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = G()" ) );   //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "G1 = G( \'G1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "G1" );
    auto G1 = static_cast<i_wages*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, G1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }


TEST( toLuapp, tolua_PAC_dev_CAM00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_CAM, device.DST_CAM_DO1_DI2, \'CAM1\', \'Test camera\', \'\' )" ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = CAM()" ) );   //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "CAM1 = CAM( \'CAM1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "CAM1" );
    auto CAM1 = static_cast<i_camera*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, CAM1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_counter_start_daily00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );


    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_FQT, device.DST_FQT_F, \'FQT1\', \'Test device\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1 = FQT( \'FQT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "FQT1" );
    auto FQT1 = static_cast<i_counter*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, FQT1 );
    lua_remove( L, -1 );

    auto dev = G_DEVICE_MANAGER()->get_device( "FQT1" );
    EXPECT_NE( nullptr, dev );

    ASSERT_EQ( 1, luaL_dostring( L, "FQT1.pause_daily()" ) ); //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1:pause_daily()" ) );
    ASSERT_EQ( 1, luaL_dostring( L, "FQT1.start_daily()" ) ); //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1:start_daily()" ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_WATCHDOG00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 1, luaL_dostring( L, "res = WATCHDOG()" ) );  //Некорректный вызов.

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_WATCHDOG, device.DST_WATCHDOG, \'WATCHDOG1\', \'Test watchdog\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "WATCHDOG1 = WATCHDOG( \'WATCHDOG1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "WATCHDOG1" );
    auto WATCHDOG1 = static_cast<i_DI_device*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, WATCHDOG1 );
    lua_remove( L, -1 );

    auto dev = G_DEVICE_MANAGER()->get_device( "WATCHDOG1" );
    EXPECT_NE( nullptr, dev );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }


TEST( toLuapp, tolua_PAC_dev_operation_set_step_cooperate_time_par_n00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    EXPECT_EQ( 0, luaL_dostring( L,
        "o1=tech_object( \'O1\', 1, 1, \'O1\', 1, 1, 10, 1, 1, 1 )" ) );
    EXPECT_EQ( 0, luaL_dostring( L,
        "oper = o1:get_modes_manager():add_operation(\'Test operation\')" ) );
    EXPECT_EQ( 0, luaL_dostring( L,
        "oper:set_step_cooperate_time_par_n( 1 )" ) );
    // Некорректный вызов - без параметра.
    EXPECT_NE( 0, luaL_dostring( L, "oper:set_step_cooperate_time_par_n()" ) );

    lua_close( L );
    }


TEST( toLuapp, tolua_PAC_dev_operation_manager_get_idle_time_sec00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    EXPECT_EQ( 0, luaL_dostring( L,
        "o1=tech_object( \'O1\', 1, 1, \'O1\', 1, 1, 10, 1, 1, 1 )" ) );
    // Некорректный вызов - c параметром.
    EXPECT_NE( 0, luaL_dostring( L,
        "res = o1:get_modes_manager():get_idle_time_sec( 1 )" ) );
    // Корректный вызов.
    EXPECT_EQ( 0, luaL_dostring( L,
        "res = o1:get_modes_manager():get_idle_time_sec()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto res = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    ASSERT_GE( res, 0 );
    lua_close( L );
    }


TEST( toLuapp, tolua_PAC_dev_operation_manager_get_idle_time00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    EXPECT_EQ( 0, luaL_dostring( L,
        "o1=tech_object( \'O1\', 1, 1, \'O1\', 1, 1, 10, 1, 1, 1 )" ) );
    // Некорректный вызов - c параметром.
    EXPECT_NE( 0, luaL_dostring( L,
        "res = o1:get_modes_manager():get_idle_time( 1 )" ) );
    // Корректный вызов.
    EXPECT_EQ( 0, luaL_dostring( L,
        "res = o1:get_modes_manager():get_idle_time()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto res = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    ASSERT_GE( res, 0 );
    lua_close( L );
    }


TEST( toLuapp, STUB )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );


    ASSERT_EQ( 0, luaL_dostring( L, "dev = STUB()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "dev" );
    auto stub = static_cast<dev_stub*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, stub );
    lua_remove( L, -1 );
    ASSERT_EQ( 0, stub->get_abs_quantity() );
    ASSERT_FALSE( stub->is_active() );

    ASSERT_EQ( 0, luaL_dostring( L, "res = dev:get_abs_quantity()" ) );

    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto res = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    ASSERT_EQ( res, 0 );

    ASSERT_EQ( 0, luaL_dostring( L, "active = dev:is_active()" ) );

    lua_getfield( L, LUA_GLOBALSINDEX, "active" );
    auto active = tolua_toboolean( L, -1, 0 );
    lua_pop( L, 1 );
    ASSERT_EQ( active, 0 );

    ASSERT_EQ( 0, luaL_dostring( L, "dev:instant_off()" ) );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_AI00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = AI()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_AI, device.DST_AI, \'AI1\', \'Test AI\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "AI1 = AI( \'AI1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "AI1" );
    auto AI1 = static_cast<i_AI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, AI1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_AO00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = AO()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_AO, device.DST_AO, \'AO1\', \'Test AO\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "AO1 = AO( \'AO1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "AO1" );
    auto AO1 = static_cast<i_AO_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, AO1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_DI00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = DI()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_DI, device.DST_DI, \'DI1\', \'Test DI\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "DI1 = DI( \'DI1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "DI1" );
    auto DI1 = static_cast<i_DI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, DI1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_DO00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = DO()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_DO, device.DST_DO, \'DO1\', \'Test DO\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "DO1 = DO( \'DO1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "DO1" );
    auto DO1 = static_cast<i_DO_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, DO1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_V00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = V()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_V, device.DST_V_DO1, \'V1\', \'Test valve\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "V1 = V( \'V1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "V1" );
    auto V1 = static_cast<valve*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, V1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_VC00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = VC()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_VC, device.DST_VC, \'VC1\', \'Test VC\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "VC1 = VC( \'VC1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "VC1" );
    auto VC1 = static_cast<i_AO_device*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, VC1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_M00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = M()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_M, device.DST_M, \'M1\', \'Test motor\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "M1 = M( \'M1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "M1" );
    auto M1 = static_cast<i_motor*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, M1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_LS00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = LS()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_LS, device.DST_LS_MIN, \'LS1\', \'Test LS\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "LS1 = LS( \'LS1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "LS1" );
    auto LS1 = static_cast<i_DI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, LS1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_FS00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = FS()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_FS, device.DST_FS, \'FS1\', \'Test FS\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "FS1 = FS( \'FS1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "FS1" );
    auto FS1 = static_cast<i_DI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, FS1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_TE00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = TE()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_TE, device.DST_TE, \'TE1\', \'Test TE\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "TE1 = TE( \'TE1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "TE1" );
    auto TE1 = static_cast<i_AI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, TE1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_LT00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = LT()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_LT, device.DST_LT, \'LT1\', \'Test LT\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "LT1 = LT( \'LT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "LT1" );
    auto LT1 = static_cast<level*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, LT1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_GS00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = GS()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_GS, device.DST_GS, \'GS1\', \'Test GS\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "GS1 = GS( \'GS1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "GS1" );
    auto GS1 = static_cast<i_DI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, GS1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_HA00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = HA()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_HA, device.DST_HA, \'HA1\', \'Test HA\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "HA1 = HA( \'HA1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "HA1" );
    auto HA1 = static_cast<i_DI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, HA1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_HL00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = HL()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_HL, device.DST_HL, \'HL1\', \'Test HL\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "HL1 = HL( \'HL1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "HL1" );
    auto HL1 = static_cast<signal_column*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, HL1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_HLA00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = HLA()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_HLA, device.DST_HLA, \'HLA1\', \'Test HLA\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "HLA1 = HLA( \'HLA1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "HLA1" );
    auto HLA1 = static_cast<signal_column*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, HLA1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_SB00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = SB()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_SB, device.DST_SB, \'SB1\', \'Test SB\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "SB1 = SB( \'SB1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "SB1" );
    auto SB1 = static_cast<i_DI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, SB1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_FQT00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = FQT()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_FQT, device.DST_FQT, \'FQT1\', \'Test FQT\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1 = FQT( \'FQT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "FQT1" );
    auto FQT1 = static_cast<i_counter*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, FQT1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_FQT_IOLINK00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = FQT_IOLINK()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_FQT, device.DST_FQT_IOLINK, \'FQT1\', "
        "\'Test FQT IOLINK\', \'IFM.SMF420\' )" ) );
    auto* fqt_cpp = G_DEVICE_MANAGER()->get_FQT_IOLINK( "FQT1" );
    ASSERT_NE( nullptr, fqt_cpp );
    ASSERT_STREQ( "FQT1", fqt_cpp->get_name() );

    ASSERT_EQ( 0, luaL_dostring( L, "FQT1 = FQT_IOLINK( \'FQT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "FQT1" );
    auto FQT1 = static_cast<counter_iolink*>(
        tolua_touserdata( L, -1, nullptr ) );
    ASSERT_NE( nullptr, FQT1 );
    EXPECT_EQ( fqt_cpp, FQT1 );
    lua_remove( L, -1 );

    // Некорректный вызов без self.
    ASSERT_NE( 0, luaL_dostring( L, "res = FQT1.get_temperature()" ) );
    // Корректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res_t = FQT1:get_temperature()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res_t" );
    EXPECT_EQ( 0.f, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    // Некорректный вызов без self.
    ASSERT_NE( 0, luaL_dostring( L, "res = FQT1.get_conductivity()" ) );
    // Корректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res_c = FQT1:get_conductivity()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res_c" );
    EXPECT_EQ( 0.f, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    fqt_cpp->set_cmd( "T", 0, 12.3 );
    fqt_cpp->set_cmd( "C", 0, 456 );

    ASSERT_EQ( 0, luaL_dostring( L, "res_t = FQT1:get_temperature()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res_t" );
    EXPECT_FLOAT_EQ( 12.3f, static_cast<float>( tolua_tonumber( L, -1, 0 ) ) );
    lua_remove( L, -1 );

    ASSERT_EQ( 0, luaL_dostring( L, "res_c = FQT1:get_conductivity()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res_c" );
    EXPECT_FLOAT_EQ( 456.f, static_cast<float>( tolua_tonumber( L, -1, 0 ) ) );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_QT00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = QT()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_QT, device.DST_QT, \'QT1\', \'Test QT\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "QT1 = QT( \'QT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "QT1" );
    auto QT1 = static_cast<i_AI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, QT1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_PT00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = PT()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_PT, device.DST_PT, \'PT1\', \'Test PT\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "PT1 = PT( \'PT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "PT1" );
    auto PT1 = static_cast<i_AI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, PT1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_F00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = F()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_F, device.DST_F, \'F1\', \'Test F\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "F1 = F( \'F1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "F1" );
    auto F1 = static_cast<i_AI_device*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, F1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_EY00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = EY()" ) );

    ASSERT_EQ( 0, luaL_dostring( L, "EY1 = EY( \'EY1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "EY1" );
    auto EY1 = static_cast<signal_column*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, EY1 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_virtual_FQT00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = virtual_FQT()" ) );

    ASSERT_EQ( 0, luaL_dostring( L, "vFQT1 = virtual_FQT( \'vFQT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "vFQT1" );
    auto vFQT1 = static_cast<virtual_counter*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, vFQT1 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_DEVICE00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметров.
    ASSERT_EQ( 1, luaL_dostring( L, "res = DEVICE()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_DI, device.DST_DI, \'DI1\', \'Test device\', "
        "\'\' )" ) );
    // DEVICE принимает только номер устройства как параметр.
    ASSERT_EQ( 0, luaL_dostring( L, "dev1 = DEVICE( 0 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "dev1" );
    auto dev1 = static_cast<device*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, dev1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_DI_device_get_state00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_DI, device.DST_DI, \'DI1\', \'Test DI\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "DI1 = DI( \'DI1\' )" ) );

    // Тест get_state.
    ASSERT_EQ( 0, luaL_dostring( L, "state = DI1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "state" );
    auto state = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0, state );

    // Некорректный вызов без self.
    ASSERT_NE( 0, luaL_dostring( L, "res = DI1.get_state()" ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_DI_device_is_active00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_DI, device.DST_DI, \'DI1\', \'Test DI\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "DI1 = DI( \'DI1\' )" ) );

    // Тест is_active.
    ASSERT_EQ( 0, luaL_dostring( L, "active = DI1:is_active()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "active" );
    auto active = tolua_toboolean( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0, active );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_DO_device_on_off00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        R"(device.DT_DO, device.DST_DO, 'DO1', 'Test DO', '' ))" ) );
    ASSERT_EQ( 0, luaL_dostring( L, R"(DO1 = DO( 'DO1' ))" ) );

    // Тест on.
    ASSERT_EQ( 0, luaL_dostring( L, "DO1:on()" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "res = DO1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto state = lua_tonumber( L, -1 );
    lua_pop( L, 1 );
    EXPECT_EQ( 1, state );

    // Тест off.
    ASSERT_EQ( 0, luaL_dostring( L, "DO1:off()" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "res = DO1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    state = lua_tonumber( L, -1 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0, state );

    // Тест instant_off.
    ASSERT_EQ( 0, luaL_dostring( L, "DO1:instant_off()" ) );

    // Тест direct_off (псевдоним для instant_off).
    ASSERT_EQ( 0, luaL_dostring( L, "DO1:direct_off()" ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_DO_device_set_state00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        R"(device.DT_DO, device.DST_DO, 'DO1', 'Test DO', '' ))" ) );
    ASSERT_EQ( 0, luaL_dostring( L, R"(DO1 = DO( 'DO1' ))" ) );

    ASSERT_EQ( 0, luaL_dostring( L, "res = DO1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto state = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( state, 0 );

    // Тест set_state.
    ASSERT_EQ( 0, luaL_dostring( L, "DO1:set_state( 1 )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "res = DO1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    state = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( state, 1 );

    // Некорректный вызов без параметра.
    ASSERT_NE( 0, luaL_dostring( L, "DO1:set_state()" ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_AI_device_get_value00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_AI, device.DST_AI, \'AI1\', \'Test AI\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "AI1 = AI( \'AI1\' )" ) );

    // Тест get_value.
    ASSERT_EQ( 0, luaL_dostring( L, "val = AI1:get_value()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "val" );
    auto val = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0.0f, val );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_AI_device_get_state00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_AI, device.DST_AI, \'AI1\', \'Test AI\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "AI1 = AI( \'AI1\' )" ) );

    // Тест get_state.
    ASSERT_EQ( 0, luaL_dostring( L, "state = AI1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "state" );
    auto state = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 1, state );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_AO_device_set_value00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_AO, device.DST_AO, \'AO1\', \'Test AO\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "AO1 = AO( \'AO1\' )" ) );

    // Тест set_value.
    ASSERT_EQ( 0, luaL_dostring( L, "AO1:set_value( 10.5 )" ) );

    // Тест off.
    ASSERT_EQ( 0, luaL_dostring( L, "AO1:off()" ) );

    // Некорректный вызов без параметра.
    ASSERT_NE( 0, luaL_dostring( L, "AO1:set_value()" ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_counter_methods00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_FQT, device.DST_FQT, \'FQT1\', \'Test FQT\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1 = FQT( \'FQT1\' )" ) );

    // Тест pause.
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1:pause()" ) );

    // Тест start.
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1:start()" ) );

    // Тест reset.
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1:reset()" ) );

    // Тест restart.
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1:restart()" ) );

    // Тест get_quantity.
    ASSERT_EQ( 0, luaL_dostring( L, "qty = FQT1:get_quantity()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "qty" );
    auto qty = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0, qty );

    // Тест get_flow.
    ASSERT_EQ( 0, luaL_dostring( L, "flow = FQT1:get_flow()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "flow" );
    auto flow = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0.0f, flow );

    // Тест get_state.
    ASSERT_EQ( 0, luaL_dostring( L, "state = FQT1:get_state()" ) );

    // Тест get_abs_quantity.
    ASSERT_EQ( 0, luaL_dostring( L, "abs = FQT1:get_abs_quantity()" ) );

    // Тест abs_reset.
    ASSERT_EQ( 0, luaL_dostring( L, "FQT1:abs_reset()" ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_valve_methods00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_V, device.DST_V_DO1, \'V1\', \'Test valve\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "V1 = V( \'V1\' )" ) );

    // Тест is_opened.
    ASSERT_EQ( 0, luaL_dostring( L, "opened = V1:is_opened()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "opened" );
    auto opened = tolua_toboolean( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0, opened );

    // Тест is_closed - клапан не закрыт, так как нет обратной
    // связи.
    ASSERT_EQ( 0, luaL_dostring( L, "closed = V1:is_closed()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "closed" );
    auto closed = tolua_toboolean( L, -1, 0 );
    lua_pop( L, 1 );
    // Без сигнала обратной связи клапан считается незакрытым.
    EXPECT_EQ( 0, closed );

    // Тест on.
    ASSERT_EQ( 0, luaL_dostring( L, "V1:on()" ) );

    // Тест off.
    ASSERT_EQ( 0, luaL_dostring( L, "V1:off()" ) );

    // Тест instant_off.
    ASSERT_EQ( 0, luaL_dostring( L, "V1:instant_off()" ) );

    // Тест get_on_fb_value.
    ASSERT_EQ( 0, luaL_dostring( L, "fb_on = V1:get_on_fb_value()" ) );

    // Тест get_off_fb_value.
    ASSERT_EQ( 0, luaL_dostring( L, "fb_off = V1:get_off_fb_value()" ) );

    valve::clear_switching_off_queue();
    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_level_get_volume00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_LT, device.DST_LT, \'LT1\', \'Test LT\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "LT1 = LT( \'LT1\' )" ) );

    // Тест get_volume.
    ASSERT_EQ( 0, luaL_dostring( L, "vol = LT1:get_volume()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "vol" );
    auto vol = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0.0f, vol );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_virtual_counter_methods00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L, "vFQT1 = virtual_FQT( \'vFQT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "vFQT1" );
    auto vFQT1 = static_cast<virtual_counter*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, vFQT1 );
    lua_remove( L, -1 );

    // Тест reset.
    ASSERT_EQ( 0, luaL_dostring( L, "vFQT1:reset()" ) );

    // Тест get_quantity.
    ASSERT_EQ( 0, luaL_dostring( L, "qty = vFQT1:get_quantity()" ) );

    // Тест get_flow.
    ASSERT_EQ( 0, luaL_dostring( L, "flow = vFQT1:get_flow()" ) );

    // Тест get_state.
    ASSERT_EQ( 0, luaL_dostring( L, "state = vFQT1:get_state()" ) );

    // Тест get_abs_quantity.
    ASSERT_EQ( 0, luaL_dostring( L, "abs = vFQT1:get_abs_quantity()" ) );

    // Тест abs_reset.
    ASSERT_EQ( 0, luaL_dostring( L, "vFQT1:abs_reset()" ) );

    // Тест eval - 3 параметра: read_value, abs_read_value,
    // read_flow.
    ASSERT_EQ( 0, luaL_dostring( L, "vFQT1:eval( 10, 100, 1.5 )" ) );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_WT00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Некорректный вызов без параметра.
    ASSERT_EQ( 1, luaL_dostring( L, "res = WT()" ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_WT, device.DST_WT_ETH, \'WT1\', \'Test WT\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "WT1 = WT( \'WT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "WT1" );
    auto WT1 = static_cast<i_wages*>(
        tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, WT1 );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_signal_column_methods00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_HL, device.DST_HL, \'HL1\', \'Test HL\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "HL1 = HL( \'HL1\' )" ) );

    // Тест on.
    ASSERT_EQ( 0, luaL_dostring( L, "HL1:on()" ) );

    // Тест off.
    ASSERT_EQ( 0, luaL_dostring( L, "HL1:off()" ) );

    // Тест set_state.
    ASSERT_EQ( 0, luaL_dostring( L, "HL1:set_state( 1 )" ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_motor_methods00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_M, device.DST_M, \'M1\', \'Test motor\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "M1 = M( \'M1\' )" ) );

    // Тест reverse.
    ASSERT_EQ( 0, luaL_dostring( L, "M1:reverse()" ) );

    // Тест get_linear_speed.
    ASSERT_EQ( 0, luaL_dostring( L, "speed = M1:get_linear_speed()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "speed" );
    auto speed = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0.0f, speed );

    // Тест get_amperage.
    ASSERT_EQ( 0, luaL_dostring( L, "amp = M1:get_amperage()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "amp" );
    auto amp = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    EXPECT_EQ( 0.0f, amp );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_PID_controller00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Тест создания PID контроллера.
    ASSERT_EQ( 0, luaL_dostring( L, "pid = PID( 1 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "pid" );
    auto pid = tolua_touserdata( L, -1, nullptr );
    EXPECT_NE( nullptr, pid );
    lua_remove( L, -1 );

    // Тест on.
    ASSERT_EQ( 0, luaL_dostring( L, "pid:on()" ) );

    // Тест off.
    ASSERT_EQ( 0, luaL_dostring( L, "pid:off()" ) );

    // Тест reset.
    ASSERT_EQ( 0, luaL_dostring( L, "pid:reset()" ) );

    // Тест set - принимает 1 параметр.
    ASSERT_EQ( 0, luaL_dostring( L, "pid:set( 50.0 )" ) );

    // Тест eval - принимает current_value и опц. delta_sign.
    ASSERT_EQ( 0, luaL_dostring( L, "pid:eval( 30.0 )" ) );

    // Тест get_state.
    ASSERT_EQ( 0, luaL_dostring( L, "state = pid:get_state()" ) );

    // Тест set_cmd.
    ASSERT_EQ( 0, luaL_dostring( L, "pid:set_cmd( \'cmd\', 0, 1.0 )" ) );

    // Тест print.
    ASSERT_EQ( 0, luaL_dostring( L, "pid:print()" ) );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_timer_functions00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Тест создания таймера.
    ASSERT_EQ( 0, luaL_dostring( L, "t = timer()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "t" );
    auto t = tolua_touserdata( L, -1, nullptr );
    EXPECT_NE( nullptr, t );
    lua_remove( L, -1 );

    // Тест start.
    ASSERT_EQ( 0, luaL_dostring( L, "t:start()" ) );

    // Тест reset.
    ASSERT_EQ( 0, luaL_dostring( L, "t:reset()" ) );

    // Тест pause.
    ASSERT_EQ( 0, luaL_dostring( L, "t:pause()" ) );

    // Тест is_time_up.
    ASSERT_EQ( 0, luaL_dostring( L, "up = t:is_time_up()" ) );

    // Тест get_work_time.
    ASSERT_EQ( 0, luaL_dostring( L, "wt = t:get_work_time()" ) );

    // Тест set_countdown_time.
    ASSERT_EQ( 0, luaL_dostring( L, "t:set_countdown_time( 5000 )" ) );

    // Тест get_countdown_time.
    ASSERT_EQ( 0, luaL_dostring( L, "ct = t:get_countdown_time()" ) );

    // Тест get_state.
    ASSERT_EQ( 0, luaL_dostring( L, "state = t:get_state()" ) );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_G_DEVICE_MANAGER00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Тест получения G_DEVICE_MANAGER.
    ASSERT_EQ( 0, luaL_dostring( L, "dm = G_DEVICE_MANAGER()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "dm" );
    auto dm = tolua_touserdata( L, -1, nullptr );
    EXPECT_NE( nullptr, dm );
    lua_remove( L, -1 );

    // Тест add_io_device через manager.
    ASSERT_EQ( 0, luaL_dostring( L,
        "dm:add_io_device( device.DT_DI, device.DST_DI, "
        "\'DI_TEST\', \'Test device\', \'\' )" ) );

    // Тест get_device.
    ASSERT_EQ( 0, luaL_dostring( L,
        "dev = dm:get_device( device.DT_DI, \'DI_TEST\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "dev" );
    auto dev = tolua_touserdata( L, -1, nullptr );
    EXPECT_NE( nullptr, dev );
    lua_remove( L, -1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_G_IO_MANAGER00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    // Тест получения G_IO_MANAGER.
    ASSERT_EQ( 0, luaL_dostring( L, "iom = G_IO_MANAGER()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "iom" );
    auto iom = tolua_touserdata( L, -1, nullptr );
    EXPECT_NE( nullptr, iom );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_io_manager_add_node00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    G_IO_MANAGER()->init( 1 );
    ASSERT_EQ( 0, luaL_dostring( L, "node = G_IO_MANAGER():add_node("
        "0, 200, 1, '127.0.0.1', 'A100', 0, 0, 0, 0, 0, 0 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "node" );
    auto node = static_cast<node_dev*>( tolua_touserdata( L, -1, nullptr ) );
    EXPECT_NE( nullptr, node );
    lua_remove( L, -1 );

    G_IO_MANAGER()->clear_nodes();
    lua_close( L );
    }
