#include "PAC_dev_tests.h"
#include "uni_bus_coupler_io.h"

TEST( ao_node_state, analog_output_get_state_returns_error_on_bad_ao_node )
    {
    analog_output ao1( "AO1" );

    uni_io_manager mngr;
    auto prev_mngr = io_manager::replace_instance( &mngr );

    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1,
        "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.init_node_AO( 0, 0, 555, 0 );

    G_PAC_INFO()->emulation_off();
    ao1.init( 0, 0, 1, 0 );
    ao1.init_channel( io_device::IO_channels::CT_AO, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->is_active = true;
    node->state = io_manager::io_node::ST_OK;
    node->status_register = 0;

    ao1.set_state( 1 );
    ao1.evaluate_io();
    EXPECT_EQ( 1, ao1.get_state() );

    ao1.set_par( 1, 0, 100.0f );
    node->status_register = 0x0010; // PP mode.
    ao1.evaluate_io();
    EXPECT_EQ( -1, ao1.get_state() );

    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }

TEST( ao_node_state, analog_valve_get_state_returns_error_on_bad_ao_node )
    {
    analog_valve vc1( "VC1" );

    uni_io_manager mngr;
    auto prev_mngr = io_manager::replace_instance( &mngr );

    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1,
        "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.init_node_AO( 0, 0, 555, 0 );

    G_PAC_INFO()->emulation_off();
    vc1.init( 0, 0, 1, 0 );
    vc1.init_channel( io_device::IO_channels::CT_AO, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->is_active = true;
    node->state = io_manager::io_node::ST_OK;
    node->status_register = 0;

    vc1.direct_on();
    vc1.evaluate_io();
    EXPECT_EQ( 1, vc1.get_state() );

    node->status_register = 0x0010; // PP mode.
    vc1.set_par( 1, 0, 0.0f );
    vc1.evaluate_io();
    EXPECT_EQ( -1, vc1.get_state() );

    DeltaMilliSecSubHooker::set_default_time();
    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }
