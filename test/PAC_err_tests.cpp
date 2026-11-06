#include "PAC_err_tests.h"
#include <array>

using namespace ::testing;

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
