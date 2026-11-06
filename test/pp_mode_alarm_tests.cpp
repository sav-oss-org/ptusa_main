#include "pp_mode_alarm_tests.h"
#include "PAC_err.h"

#include <array>

using namespace ::testing;

class test_uni_io_manager_PP_mode : public uni_io_manager
    {
    public:
        ~test_uni_io_manager_PP_mode() override = default;

        void set_registers( u_int_2 status, u_int_2 diagnostic )
            {
            status_register_value = status;
            diagnostic_status_register_value = diagnostic;
            }

        void activate_error()
            {
            set_registers( 0x0010, 0x0000 ); // PP mode is active.
            }

        void deactivate_error()
            {
            set_registers( 0x0000, 0x0000 ); // PP mode is inactive.
            }

        void activate_cfg_bus_error()
            {
            set_registers( 0x0000, 0x0104 ); // Bits 2 and 8.
            }

        int read_input_registers( io_node* node, unsigned int address,
            unsigned int quantity, unsigned char station /*= 0*/ ) override
            {
            (void)node;
            (void)station;
            if ( address == PHOENIX_STATUS_REGISTER_ADDRESS &&
                quantity == 2 )
                {
                buff[ 0 ] = static_cast<u_char>( status_register_value >> 8 );
                buff[ 1 ] = static_cast<u_char>( status_register_value & 0xFF );
                buff[ 2 ] = static_cast<u_char>(
                    diagnostic_status_register_value >> 8 );
                buff[ 3 ] = static_cast<u_char>(
                    diagnostic_status_register_value & 0xFF );
                resultbuff = buff;
                return 1;
                }

            auto register_value = status_register_value;
            if ( address == PHOENIX_DIAGNOSTIC_STATUS_REGISTER_ADDRESS )
                {
                register_value = diagnostic_status_register_value;
                }

            buff[ 0 ] = static_cast<u_char>( register_value >> 8 );
            buff[ 1 ] = static_cast<u_char>( register_value & 0xFF );
            resultbuff = buff;
            return 1;
            }

    private:
        u_int_2 status_register_value = 0;
        u_int_2 diagnostic_status_register_value = 0;
    };

// Test PP mode alarm activation.
TEST( pp_mode_alarm, pp_mode_activation_deactivation )
    {
    PAC_critical_errors_manager::get_instance()->reset_all_error();
    EXPECT_FALSE( PAC_critical_errors_manager::get_instance()->is_any_error() );

    tcp_communicator::init_instance( "Тест", "Test" );

    G_IO_MANAGER()->init( 1 );
    G_IO_MANAGER()->add_node( 0,
        io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1, "127.0.0.1", "A100",
        0, 0, 0, 0, 0, 0 );
    auto node = G_IO_MANAGER()->get_node( 0 );

    test_uni_io_manager_PP_mode mngr;
    mngr.activate_error();
    mngr.read_phoenix_status_register( node );

    u_int_2 err_id = 0;
    std::array<char, 300> lua_buff{};
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    auto REF_STR = R"s(	{
	description = "6-1-1 : Узел I/O 'A100' ('127.0.0.1', 'Тест') - активен PP mode (каналы управления заблокированы)",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)s";
    EXPECT_STREQ( lua_buff.data(), REF_STR );    // Есть ошибка.

    mngr.deactivate_error();
    mngr.read_phoenix_status_register( node );
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    EXPECT_STREQ( lua_buff.data(), "" );        // Нет ошибки.

    // После потери связи ошибка должна быть только одна - "нет связи",
    // а не PP mode.
    mngr.activate_error();
    mngr.read_phoenix_status_register( node );
    DeltaMilliSecSubHooker::set_millisec(
        G_PAC_INFO()->par[ PAC_info::P_BK_ANSWER_MAX_WAIT_TIME ] + 100);
    auto res = mngr.e_communicate( node, 1, 1 );
    EXPECT_NE( res, 0 );
    auto REF_STR_NO_CONNECTION = R"s(	{
	description = "1-1-1 : Узел I/O 'A100' ('127.0.0.1', 'Тест') - нет связи",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)s";
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    // Есть только ошибка связи.
    EXPECT_STREQ( lua_buff.data(), REF_STR_NO_CONNECTION );


    DeltaMilliSecSubHooker::set_default_time();
    tcp_communicator::clear_instance();
    PAC_critical_errors_manager::get_instance()->reset_all_error();
    }

// Test that PP mode alarm flag is initialized correctly.
TEST( pp_mode_alarm, initialization )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    // Verify initial state.
    EXPECT_EQ( 0, node.status_register );
    EXPECT_EQ( 0, node.diagnostic_status_register );
    EXPECT_FALSE( node.is_err_mode_alarm_set );
    EXPECT_FALSE( node.is_cfg_bus_error_alarm_set );
    }

// Test PP mode detection with non-Phoenix nodes.
TEST( pp_mode_alarm, non_phoenix_node )
    {
    io_manager::io_node wago_node(
        io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        1, "127.0.0.1", "W100", 0, 0, 0, 0, 0, 0 );

    // Set error bits for Wago node.
    wago_node.status_register = 0x003F;
    wago_node.is_active = true;
    wago_node.state = io_manager::io_node::ST_OK;

    G_PAC_INFO()->emulation_off();
    // Wago nodes should not trigger PP mode warnings.
    EXPECT_EQ( io_manager::io_node::DISPLAY_STATES::DST_OK,
        wago_node.get_display_state() );
    G_PAC_INFO()->emulation_on();
    }

TEST( pp_mode_alarm, disable_node_no_alarm )
    {
    PAC_critical_errors_manager::get_instance()->reset_all_error();
    EXPECT_FALSE( PAC_critical_errors_manager::get_instance()->is_any_error() );

    tcp_communicator::init_instance( "Тест", "Test" );

    G_IO_MANAGER()->init( 1 );
    G_IO_MANAGER()->add_node( 0,
        io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1, "127.0.0.1", "A100",
        0, 0, 0, 0, 0, 0 );
    auto node = G_IO_MANAGER()->get_node( 0 );

    // No PP mode alarm active.
    node->is_err_mode_alarm_set = false;
    node->state = io_manager::io_node::ST_OK;
    test_uni_io_manager_PP_mode mngr;
    mngr.activate_error();
    mngr.read_phoenix_status_register( node );

    EXPECT_TRUE( PAC_critical_errors_manager::get_instance()->is_any_error() );
    u_int_2 err_id = 0;
    std::array<char, 300> lua_buff{};
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    auto REF_STR_1 = R"s(	{
	description = "6-1-1 : Узел I/O 'A100' ('127.0.0.1', 'Тест') - активен PP mode (каналы управления заблокированы)",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)s";
    EXPECT_STREQ( lua_buff.data(), REF_STR_1 );

    EXPECT_EQ( 0, G_PAC_INFO()->set_cmd( "NODEENABLED", 1, 0 ) );
    EXPECT_TRUE( PAC_critical_errors_manager::get_instance()->is_any_error() );
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    auto REF_STR_2 = R"s(	{
	description = "5-1-1 : Узел I/O 'A100' ('127.0.0.1', 'Тест') - отключен для обслуживания",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)s";
    EXPECT_STREQ( lua_buff.data(), REF_STR_2 );

    PAC_critical_errors_manager::get_instance()->reset_all_error();
    tcp_communicator::clear_instance();
    }

TEST( pp_mode_alarm, e_communicate )
    {
    auto err_mngr = PAC_critical_errors_manager::get_instance();
    err_mngr->reset_all_error();
    EXPECT_FALSE( err_mngr->is_any_error() );

    tcp_communicator::init_instance( "Тест", "Test" );

    G_IO_MANAGER()->init( 1 );
    G_IO_MANAGER()->add_node( 0,
        io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1, "127.0.0.1", "A100",
        0, 0, 0, 0, 0, 0 );
    auto node = G_IO_MANAGER()->get_node( 0 );

    test_uni_io_manager_PP_mode mngr;

    // Устанавливаем бит перехода в PP mode.
    mngr.activate_error();
    mngr.read_phoenix_status_register( node );
    EXPECT_TRUE( err_mngr->is_any_error() ); // Ошибка наличия `PP mode`.

    // Должна появиться ошибка связи и пропасть ошибка наличия `PP mode`.
    G_PAC_INFO()->par[ PAC_info::P_BK_ANSWER_MAX_WAIT_TIME ] = 0;
    mngr.e_communicate( node, 1, 1 );
    EXPECT_TRUE( err_mngr->is_any_error() ); // Ошибка связи.

    // Должна пропасть ошибка связи и появиться ошибка наличия `PP mode`.
    G_PAC_INFO()->par[ PAC_info::P_BK_ANSWER_MAX_WAIT_TIME ] = 10'000;
    mngr.e_communicate( node, 1, 1 );
    EXPECT_FALSE( err_mngr->is_any_error() );
    mngr.read_phoenix_status_register( node );
    EXPECT_TRUE( err_mngr->is_any_error() ); // Ошибка наличия `PP mode`.

    G_PAC_INFO()->reset_params();
    err_mngr->reset_all_error();
    tcp_communicator::clear_instance();
    }

// Test configuration/bus error alarm activation from register 7997.
TEST( pp_mode_alarm, cfg_bus_error_activation_deactivation )
    {
    PAC_critical_errors_manager::get_instance()->reset_all_error();
    EXPECT_FALSE( PAC_critical_errors_manager::get_instance()->is_any_error() );

    tcp_communicator::init_instance( "Тест", "Test" );

    G_IO_MANAGER()->init( 1 );
    G_IO_MANAGER()->add_node( 0,
        io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1, "127.0.0.1", "A100",
        0, 0, 0, 0, 0, 0 );
    auto node = G_IO_MANAGER()->get_node( 0 );

    test_uni_io_manager_PP_mode mngr;
    mngr.activate_cfg_bus_error();
    mngr.read_phoenix_status_register( node );

    u_int_2 err_id = 0;
    std::array<char, 300> lua_buff{};
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str(
        lua_buff.data(), err_id );
    auto REF_STR = R"s(	{
	description = "7-1-1 : Узел I/O 'A100' ('127.0.0.1', 'Тест') - ошибка конфигурации/шины",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)s";
    EXPECT_STREQ( lua_buff.data(), REF_STR );

    mngr.deactivate_error();
    mngr.read_phoenix_status_register( node );
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str(
        lua_buff.data(), err_id );
    EXPECT_STREQ( lua_buff.data(), "" );

    tcp_communicator::clear_instance();
    PAC_critical_errors_manager::get_instance()->reset_all_error();
    }
