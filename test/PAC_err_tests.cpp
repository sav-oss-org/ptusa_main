#include "PAC_err_tests.h"
#include <array>
extern "C"
    {
    #include "lua.h"
    #include "lauxlib.h"
    }

using namespace ::testing;

TEST( PAC_critical_errors_manager, network_alarm_identifies_listener )
    {
    auto manager = PAC_critical_errors_manager::get_instance();
    manager->reset_all_error();
    manager->set_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_BIND_F, 0 );
    manager->set_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_BIND_F, 1 );
    std::array<char, 4096> buffer{};
    u_int_2 id = 0;
    manager->save_as_Lua_str( buffer.data(), id );
    EXPECT_THAT( buffer.data(), HasSubstr( "Мастер : вызов функции bind(...)" ) );
    EXPECT_THAT( buffer.data(), HasSubstr( "Modbus : вызов функции bind(...)" ) );
    EXPECT_THAT( buffer.data(), Not( HasSubstr( "{}" ) ) );
    manager->reset_all_error();
    }

TEST( PAC_critical_errors_manager, details_update_alarm_without_duplicates )
    {
    auto manager = PAC_critical_errors_manager::get_instance();
    manager->reset_all_error();
    auto error_class = PAC_critical_errors_manager::ALARM_CLASS( 0 );
    auto subclass = PAC_critical_errors_manager::ALARM_SUBCLASS( 0 );
    manager->set_global_error( error_class, subclass, 1, "код 1", P_MESSAGE );
    auto first_id = manager->get_id();
    EXPECT_FALSE( manager->is_any_critical_error() );
    manager->set_global_error( error_class, subclass, 1, "код 1", P_MESSAGE );
    EXPECT_EQ( manager->get_id(), first_id );
    manager->set_global_error( error_class, subclass, 1, "код 2", P_ALARM );
    EXPECT_NE( manager->get_id(), first_id );
    EXPECT_FALSE( manager->is_any_critical_error() );
    std::array<char, 4096> buffer{};
    u_int_2 id = 0;
    manager->save_as_Lua_str( buffer.data(), id );
    EXPECT_THAT( buffer.data(), HasSubstr( "код 2" ) );
    EXPECT_THAT( buffer.data(), Not( HasSubstr( "код 1" ) ) );
    EXPECT_THAT( buffer.data(), HasSubstr( "priority = 250" ) );
    manager->reset_global_error( error_class, subclass, 1, false );
    EXPECT_FALSE( manager->is_any_error() );
    }

TEST( PAC_critical_errors_manager, diagnostic_text_round_trips_through_lua )
    {
    auto manager = PAC_critical_errors_manager::get_instance();
    manager->reset_all_error();
    std::string details = "путь \"/tmp/axiopdi\"\\test\nстрока\t";
    details.push_back( '\0' );
    details += "123";
    manager->set_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1, details );
    std::array<char, 4096> buffer{};
    u_int_2 id = 0;
    manager->save_as_Lua_str( buffer.data(), id );
    auto state = luaL_newstate();
    ASSERT_NE( state, nullptr );
    auto script = std::string( "return {" ) + buffer.data() + "}";
    auto result = luaL_dostring( state, script.c_str() );
    EXPECT_EQ( result, 0 );
    if ( result == 0 )
        {
        lua_rawgeti( state, -1, 1 );
        lua_getfield( state, -1, "description" );
        size_t size = 0;
        auto description = lua_tolstring( state, -1, &size );
        EXPECT_EQ( std::string( description, size ), "0-0-1 : ?: " + details );
        }
    lua_close( state );
    manager->reset_all_error();
    }

TEST( PAC_critical_errors_manager, long_diagnostic_remains_valid_lua )
    {
    auto manager = PAC_critical_errors_manager::get_instance();
    manager->reset_all_error();
    manager->set_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1,
        std::string( 3000, '"' ) );
    std::array<char, 4096> buffer{};
    u_int_2 id = 0;
    manager->save_as_Lua_str( buffer.data(), id );
    auto state = luaL_newstate();
    ASSERT_NE( state, nullptr );
    auto script = std::string( "return {" ) + buffer.data() + "}";
    EXPECT_EQ( luaL_dostring( state, script.c_str() ), 0 );
    EXPECT_THAT( buffer.data(), HasSubstr( "..." ) );
    lua_close( state );
    manager->reset_all_error();
    }

TEST( PAC_critical_errors_manager, set_global_error )
    {
    auto mngr = PAC_critical_errors_manager::get_instance();
    EXPECT_FALSE( mngr->is_any_error() );
    EXPECT_FALSE( mngr->is_any_critical_error() );

    // Ошибки неизвестного класса.
    mngr->set_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_TRUE( mngr->is_any_error() );
    EXPECT_TRUE( mngr->is_any_critical_error() );

    // Пробуем повторно установить ошибку.
    mngr->set_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_TRUE( mngr->is_any_error() );
    EXPECT_TRUE( mngr->is_any_critical_error() );

    std::array<char, 256> buff{};
    u_int_2 id{};
    const auto REF_STR = R"(
	{
	description = "0-0-1 : ?",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)" + 1;

    mngr->save_as_Lua_str( buff.data(), id );
    EXPECT_STREQ( REF_STR, buff.data() );

    mngr->reset_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_FALSE( mngr->is_any_error() );

    // Сброс ошибки без записи в лог.
    mngr->set_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    testing::internal::CaptureStdout();
    mngr->reset_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1, false );
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, "" );

    // Пробуем сбросить ошибку, которой нет.
    mngr->reset_global_error( PAC_critical_errors_manager::ALARM_CLASS( 0 ),
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_FALSE( mngr->is_any_error() );

    mngr->save_as_Lua_str( buff.data(), id );
    EXPECT_STREQ( "", buff.data() );


    mngr->set_global_error( PAC_critical_errors_manager::AC_NO_CONNECTION,
        PAC_critical_errors_manager::AS_EASYSERVER, 1 );
    EXPECT_TRUE( mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_NO_CONNECTION,
        PAC_critical_errors_manager::AS_EASYSERVER, 1 );
    EXPECT_FALSE( mngr->is_any_error() );

    mngr->set_global_error( PAC_critical_errors_manager::AC_NO_CONNECTION,
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_TRUE( mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_NO_CONNECTION,
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_FALSE( mngr->is_any_error() );

    // Ошибки класса `AC_NET`.
    mngr->set_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_SOCKET_F, 1 );
    EXPECT_TRUE( mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_SOCKET_F, 1 );

    mngr->set_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_BIND_F, 1 );
    EXPECT_TRUE( mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_BIND_F, 1 );
    EXPECT_FALSE( mngr->is_any_error() );

    mngr->set_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_SETSOCKOPT_F, 2 );
    EXPECT_TRUE( mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_SETSOCKOPT_F, 2 );
    EXPECT_FALSE( mngr->is_any_error() );

    mngr->set_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_LISTEN_F, 0 );
    EXPECT_TRUE( mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::AS_LISTEN_F, 0 );
    EXPECT_FALSE( mngr->is_any_error() );

    mngr->set_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_TRUE( mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_NET,
        PAC_critical_errors_manager::ALARM_SUBCLASS( 0 ), 1 );
    EXPECT_FALSE( mngr->is_any_error() );
    }
