#include "PAC_dev_tests.h"
#include "uni_bus_coupler_io.h"

#include <cstring>
#include <iomanip>
#include <time.h>

#include <fstream>
#include <iostream>
#include <filesystem>

#ifdef LINUX_OS
#include <sys/types.h>
#include <ifaddrs.h>
#endif

using namespace ::testing;
using B = std::byte;

class iolink_dev_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            mngr.init( 1 );
            prev_mngr = io_manager::replace_instance( &mngr );
            mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
                1, "127.0.0.1", "A100", 1, 1, 1, 32, 1, 1 );
            mngr.init_node_AI( 0, 0, 1027843, 0 );
            };

        void TearDown() override
            {
            io_manager::replace_instance( prev_mngr );
            };

        void init_channels( io_device& io_dev ) const
            {
            io_dev.init_and_alloc( 0, 0, 0, 2 );
            io_dev.init_channel( io_device::IO_channels::CT_AI, 0, 0, 0, 1, 1 );
            }

        void set_iol_state_to_OK( io_device& io_dev ) const
            {
            // Bit 0 - IOLink connected, Bit 8 - IOLink data valid.
            *io_dev.AI_channels.int_module_read_values[ 0 ] = 0b1'0000'0001;
            }

        void test_dev_err( device& dev, io_device& io_dev, int expected_dev_state ) const
            {
            EXPECT_STREQ( dev.get_error_description(), "нет ошибок" );

            G_PAC_INFO()->emulation_off();
            init_channels( io_dev );
            dev.evaluate_io();
            EXPECT_EQ( dev.get_state(), -io_device::IOLINKSTATE::NOTCONNECTED );
            EXPECT_STREQ( dev.get_error_description(), "IOL-устройство не подключено" );

            // Bit 0 - IOLink connected.
            *io_dev.AI_channels.int_module_read_values[ 0 ] = 0b1;
            dev.evaluate_io();
            EXPECT_EQ( dev.get_state(), -io_device::IOLINKSTATE::DEVICEERROR );
            EXPECT_STREQ( dev.get_error_description(), "ошибка IOL-устройства" );

            set_iol_state_to_OK( io_dev );
            dev.evaluate_io();
            EXPECT_EQ( dev.get_state(), expected_dev_state );
            EXPECT_STREQ( dev.get_error_description(), "ошибка IOL-устройства" );

            G_PAC_INFO()->emulation_on();
            }

    private:
        uni_io_manager mngr;
        io_manager* prev_mngr;
    };


TEST( signal_column, get_type_name )
    {
    signal_column_iolink test_dev( "test_HL1" );
    EXPECT_STREQ( "Сигнальная колонна", test_dev.get_type_name() );
    }

TEST( signal_column, direct_off )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.direct_on();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.direct_off();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }

TEST( signal_column, turn_off_green )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.turn_on_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYR" );
    test_dev.turn_on_green();
    test_dev.turn_off_green();
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, turn_off_yellow )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.turn_on_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYR" );
    test_dev.turn_on_yellow();
    test_dev.turn_off_yellow();
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, turn_off_red )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.turn_on_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYR" );
    test_dev.turn_on_red();
    test_dev.turn_off_red();
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, turn_off_blue )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    // Без описания колонны синий свет не включается.
    test_dev.turn_on_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );

    test_dev.turn_on_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=1, L_SIREN=0},\n", buff );

    test_dev.turn_off_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.turn_on_blue();
    test_dev.turn_off_blue();
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, turn_off_siren )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.turn_on_siren();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=1},\n", buff );

    test_dev.turn_off_siren();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    test_dev.turn_on_siren();
    test_dev.turn_off_siren();
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, normal_blink_red )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.normal_blink_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.set_rt_par( 1, 1 ); // Мы управляем красным цветом.
    test_dev.normal_blink_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.set_rt_par( 1, 0 ); // Мы не управляем красным цветом.
    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    test_dev.normal_blink_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, slow_blink_red )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.slow_blink_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.set_rt_par( 1, 1 ); // Мы управляем красным цветом.
    test_dev.slow_blink_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.set_rt_par( 1, 0 ); // Мы не управляем красным цветом.
    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    test_dev.slow_blink_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_red();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, normal_blink_yellow )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.normal_blink_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    test_dev.normal_blink_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, slow_blink_yellow )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.slow_blink_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    test_dev.slow_blink_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_yellow();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, normal_blink_green )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.normal_blink_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    test_dev.normal_blink_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, slow_blink_green )
    {
    signal_column_iolink test_dev( "test_HL1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.slow_blink_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.turn_off_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    test_dev.slow_blink_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.turn_off_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, normal_blink_blue )
    {
    signal_column_iolink test_dev( "test_HL1" );
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.normal_blink_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=1, L_SIREN=0},\n", buff );

    test_dev.turn_off_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.normal_blink_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=1, L_SIREN=0},\n", buff );
    test_dev.turn_off_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, slow_blink_blue )
    {
    signal_column_iolink test_dev( "test_HL1" );
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.slow_blink_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=1, L_SIREN=0},\n", buff );

    test_dev.turn_off_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    test_dev.slow_blink_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=1, L_SIREN=0},\n", buff );
    test_dev.turn_off_blue();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    G_PAC_INFO()->emulation_on();
    }

TEST( signal_column, direct_set_state )
    {
    signal_column_iolink test_dev( "test_HL1" );
    test_dev.set_string_property( "SIGNALS_SEQUENCE", "AGYRB" );

    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    test_dev.direct_set_state( signal_column::CMD::GREEN_ON );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.direct_set_state( signal_column::CMD::YELLOW_ON );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.direct_set_state( signal_column::CMD::RED_ON );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=1, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.direct_set_state( signal_column::CMD::BLUE_ON );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=1, L_RED=1, "
        "L_BLUE=1, L_SIREN=0},\n", buff );

    test_dev.direct_set_state( signal_column::CMD::SIREN_ON );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=1, L_RED=1, "
        "L_BLUE=1, L_SIREN=1},\n", buff );

    test_dev.direct_set_state( signal_column::CMD::LIGHTS_OFF );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=1},\n", buff );

    test_dev.direct_set_state( signal_column::CMD::TURN_OFF );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    test_dev.direct_set_state( signal_column::CMD::TURN_ON );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    // Green lamp.
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::GREEN_NORMAL_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::GREEN_SLOW_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.direct_set_state( signal_column::CMD::GREEN_OFF );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    // Yellow lamp
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::YELLOW_NORMAL_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::YELLOW_SLOW_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.direct_set_state( signal_column::CMD::YELLOW_OFF );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    // Red lamp
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::RED_NORMAL_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::RED_SLOW_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.direct_set_state( signal_column::CMD::RED_OFF );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    // Blue lamp
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::BLUE_NORMAL_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=1, L_SIREN=0},\n", buff );
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::BLUE_SLOW_BLINK );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=1, L_SIREN=0},\n", buff );
    test_dev.direct_set_state( signal_column::CMD::BLUE_OFF );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    // SIREN_OFF.
    test_dev.turn_on_siren();
    test_dev.direct_set_state( signal_column::CMD::SIREN_OFF );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    // Do nothing.
    test_dev.off();
    test_dev.direct_set_state( signal_column::CMD::SIREN_OFF + 1 );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }

TEST( signal_column, blink )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    signal_column_iolink test_dev( "test_HL1" );

    G_PAC_INFO()->emulation_off();
    test_dev.slow_blink_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    test_dev.slow_blink_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    test_dev.slow_blink_green();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );

    G_PAC_INFO()->emulation_on();
    DeltaMilliSecSubHooker::set_default_time();
    }

TEST( signal_column, set_rt_par )
    {
    signal_column_iolink test_dev( "test_HL1" );
    // Управление красным цветом.
    test_dev.set_rt_par( 1, 1 );

    //Несуществующий runtime-параметр.
    test_dev.set_rt_par( 0, 1 );
    }

TEST( signal_column, evaluate_io )
    {
    signal_column_iolink test_dev( "test_HL1" );
    // Просто вызываем данный метод - он должен выполниться без исключений.
    test_dev.evaluate_io();
    }

TEST( signal_column, show_error_exists )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    signal_column_iolink test_dev( "test_HL1" );
    // Просто вызываем данный метод - он должен включить мигание красной лампы
    // и сирену.
    test_dev.show_error_exists();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=0, L_RED=1, "
        "L_BLUE=0, L_SIREN=1},\n", buff );
    }

TEST( signal_column, show_message_exists )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    signal_column_iolink test_dev( "test_HL1" );
    // Просто вызываем данный метод - он должен включить мигание желтой лампы.
    test_dev.show_message_exists();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }

TEST( signal_column, show_batch_is_not_running )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    signal_column_iolink test_dev( "test_HL1" );
    // Просто вызываем данный метод - он должен включить желтую лампу.
    test_dev.show_batch_is_not_running();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }

TEST( signal_column, show_batch_is_running )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    signal_column_iolink test_dev( "test_HL1" );
    // Просто вызываем данный метод - он должен включить зелёную лампу.
    test_dev.show_batch_is_running();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }

TEST( signal_column, show_operation_is_not_running )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    signal_column_iolink test_dev( "test_HL1" );
    test_dev.turn_on_green();
    test_dev.turn_on_red();
    test_dev.turn_on_siren();
    // Просто вызываем данный метод - он должен включить желтую лампу.
    test_dev.show_operation_is_not_running();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=0, L_YELLOW=1, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }

TEST( signal_column, show_operation_is_running )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    signal_column_iolink test_dev( "test_HL1" );
    // Просто вызываем данный метод - он должен включить зелёную лампу.
    test_dev.show_operation_is_running();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=1, V=0, L_GREEN=1, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }

TEST( signal_column, show_idle )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    signal_column_iolink test_dev( "test_HL1" );
    test_dev.turn_on_green();
    test_dev.turn_on_red();
    test_dev.turn_on_siren();
    // Просто вызываем данный метод - он должен отключить все лампы и сирену.
    test_dev.show_idle();
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_HL1={M=0, ST=0, V=0, L_GREEN=0, L_YELLOW=0, L_RED=0, "
        "L_BLUE=0, L_SIREN=0},\n", buff );
    }


TEST( signal_column_iolink, get_state )
    {
    signal_column_iolink test_dev( "test_HL1" );

    EXPECT_EQ( test_dev.get_state(), 0 );

    G_PAC_INFO()->emulation_off();
    EXPECT_EQ( test_dev.get_state(), 0 );
    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, signal_column_iolink_get_error_description )
    {
    signal_column_iolink test_dev( "test_HL1" );
    test_dev_err( test_dev, test_dev, 0 );
    }


TEST( valve_iol_terminal, get_state_data )
    {
    valve_iol_terminal_DO1_DI1_on v1( "V1" );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, v1.get_valve_state() );
    }


void test_temperature( AI1* TE1 )
    {
    uni_io_manager mngr;
    mngr.init( 1 );
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.init_node_AI( 0, 0, 491, 0 );

    EXPECT_EQ( TE1->get_state(), 1 );

    G_PAC_INFO()->emulation_off();
    TE1->init_and_alloc( 0, 0, 0, 1 );
    TE1->init_channel( io_device::IO_channels::CT_AI, 0, 0, 0 );
    EXPECT_EQ( TE1->get_state(), 1 );

    *TE1->AI_channels.int_read_values[ 0 ] = -30001;
    EXPECT_EQ( TE1->get_state(), -4 );
    EXPECT_STREQ( TE1->get_error_description(), "вне диапазона" );

    const auto ERR_VALUE = -2000.0f;
    TE1->set_cmd( "P_ERR", 1, ERR_VALUE );
    EXPECT_EQ( TE1->get_value(), ERR_VALUE );

    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }

TEST( temperature_e_analog, get_value )
    {
    temperature_e_analog TE1( "T1" );
    auto v = TE1.get_value();
    EXPECT_EQ( .0f, v );
    }

TEST( temperature_e_analog, get_state )
    {
    temperature_e_analog TE1( "test_TE1" );
    test_temperature( &TE1 );
    }

TEST( temperature_e_analog, get_type_name )
    {
    temperature_e_analog test_dev( "test_TE1" );
    EXPECT_STREQ( "Температура", test_dev.get_type_name() );
    }


TEST_F( iolink_dev_test, temperature_e_iolink_get_value )
    {
    temperature_e_iolink TE1( "T1" );
    EXPECT_EQ( TE1.get_value(), .0f );

    G_PAC_INFO()->emulation_off();

    // Нет привязки к модулям ввода/вывода - должны получить 0.
    EXPECT_EQ( TE1.get_value(), .0f );

    init_channels( TE1 );
    auto err_value = -100.f;
    TE1.set_par( static_cast<u_int> ( temperature_e_iolink::CONSTANTS::P_ERR ),
        TE1.start_param_idx, err_value );
    // Есть привязка к модулям ввода/вывода - должны получить аварийное
    // значение.
    EXPECT_EQ( TE1.get_value(), err_value );
    }

TEST_F( iolink_dev_test, temperature_e_iolink_evaluate_io )
    {
    temperature_e_iolink TE1( "T1" );
    EXPECT_EQ( TE1.get_value(), .0f );

    G_PAC_INFO()->emulation_off();
    init_channels( TE1 );
    set_iol_state_to_OK( TE1 );

    const u_int_2 VALUE = 257;
    *TE1.AI_channels.int_read_values[ 0 ] = VALUE;
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_value(), 0.1f * VALUE );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, temperature_e_iolink_get_error_description )
    {
    temperature_e_iolink TE1( "T1" );
    EXPECT_EQ( TE1.get_state(), 1 );
    test_dev_err( TE1, TE1, 1 );

    TE1.set_cmd( "ST", 0,
        -static_cast<int>( io_device::ERRORS::LAST_ERR_IDX ) - 100 );
    EXPECT_STREQ( TE1.get_error_description(), "неизвестная ошибка" );
    }

TEST_F( iolink_dev_test, temperature_e_iolink_tm311_get_value )
    {
    temperature_e_iolink_tm311 TE1( "T1" );
    EXPECT_EQ( TE1.get_value(), .0f );

    G_PAC_INFO()->emulation_off();

    // Нет привязки к модулям ввода/вывода - должны получить 0.
    EXPECT_EQ( TE1.get_value(), .0f );

    init_channels( TE1 );
    auto err_value = -100.f;
    TE1.set_par( static_cast<u_int> ( temperature_e_iolink_tm311::CONSTANTS::P_ERR ),
        TE1.start_param_idx, err_value );
    // Есть привязка к модулям ввода/вывода - должны получить аварийное
    // значение.
    EXPECT_EQ( TE1.get_value(), err_value );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, temperature_e_iolink_tm311_evaluate_io )
    {
    temperature_e_iolink_tm311 TE1( "T1" );
    EXPECT_EQ( TE1.get_value(), .0f );

    G_PAC_INFO()->emulation_off();
    init_channels( TE1 );
    set_iol_state_to_OK( TE1 );

    // TM311 process data: 4 bytes.
    // Allocate 4 bytes as 2 int_2 values.
    TE1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{};

    // Data layout in memory (before swap in evaluate_io):
    // Byte 0: scale, Byte 1: status,
    // Byte 2-3: temperature (big-endian, will be swapped to little-endian).

    // Test 1: Zero temperature with Bad status (status1=0, status2=0).
    {
    std::byte data[] = { B{0xFF}, B{0x00}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_FLOAT_EQ( TE1.get_value(), 0.0f );
    EXPECT_EQ( TE1.get_state(), -10 ); // Bad status.
    }

    // Test 2: Temperature 25.7°C (257 = 0x0101) with Good status (status1=3, status2=0).
    {
    std::byte data[] = { B{0xFF}, B{0x18}, B{0x01}, B{0x01} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_FLOAT_EQ( TE1.get_value(), 25.7f );
    EXPECT_EQ( TE1.get_state(), 1 ); // Good status.
    }

    // Test 3: Negative temperature -12.3°C (-123 = 0xFF85) with Good status.
    {
    std::byte data[] = { B{0xFF}, B{0x18}, B{0xFF}, B{0x85} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_FLOAT_EQ( TE1.get_value(), -12.3f );
    EXPECT_EQ( TE1.get_state(), 1 );
    }

    // Test 4: Temperature 100.0°C (1000 = 0x03E8) with Good status.
    {
    std::byte data[] = { B{0xFF}, B{0x18}, B{0x03}, B{0xE8} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_FLOAT_EQ( TE1.get_value(), 100.0f );
    EXPECT_EQ( TE1.get_state(), 1 );
    }

    // Test 5: Temperature 10.0°C (100 = 0x0064) with Uncertain status (status1=1).
    {
    std::byte data[] = { B{0xFF}, B{0x08}, B{0x00}, B{0x64} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_FLOAT_EQ( TE1.get_value(), 10.0f );
    EXPECT_EQ( TE1.get_state(), -20 ); // Uncertain status: -10 - 1*10 - 0 = -20.
    }

    delete[] TE1.AI_channels.int_read_values[ 0 ];
    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, temperature_e_iolink_tm311_get_state )
    {
    temperature_e_iolink_tm311 TE1( "T1" );
    EXPECT_EQ( TE1.get_state(), 1 );

    G_PAC_INFO()->emulation_off();
    init_channels( TE1 );
    set_iol_state_to_OK( TE1 );

    // Allocate 4 bytes as 2 int_2 values.
    TE1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };

    // Data layout: Byte 0=scale, Byte 1=status, Byte 2-3=temperature.

    // Test 1: Bad status (status1=0, status2=0).
    {
    std::byte data[] = { B{0xFF}, B{0x00}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -10 ); // -10 - 0*10 - 0 = -10.
    EXPECT_STREQ( TE1.get_error_description(),
        "\'Bad\' - значение не может быть использовано" );
    }

    // Test 2: Uncertain status (status1=1, status2=0).
    {
    std::byte data[] = { B{0xFF}, B{0x08}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -20 ); // -10 - 1*10 - 0 = -20.
    EXPECT_STREQ( TE1.get_error_description(),
        "\'Uncertain\' "
        "— значение может использоваться только ограниченно "
        "(например, температура устройства вне диапазона)" );
    }

    // Test 3: Manual/Fixed status (status1=2, status2=0).
    {
    std::byte data[] = { B{0xFF}, B{0x10}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -30 ); // -10 - 2*10 - 0 = -30.
    EXPECT_STREQ( TE1.get_error_description(),
        "\'Manual/Fixed\' "
        "— значение может использоваться только ограниченно "
        "(например, симуляция)" );
    }

    // Test 4: Good status (status1=3, status2=0).
    {
    std::byte data[] = { B{0xFF}, B{0x18}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), 1 ); // Good status.
    }

    // Test 5: Bad status with low limited (status1=0, status2=1).
    {
    std::byte data[] = { B{0xFF}, B{0x02}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -11 ); // -10 - 1 = -11.
    EXPECT_STREQ( TE1.get_error_description(),
        "\'Bad\' - значение не может быть использовано"
        "; \'Low limited\' - значение нарушило нижний предел" );
    }

    // Test 6: Bad status with high limited (status1=0, status2=2).
    {
    std::byte data[] = { B{0xFF}, B{0x04}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -12 ); // -10 - 2 = -12.
    EXPECT_STREQ( TE1.get_error_description(),
        "\'Bad\' - значение не может быть использовано"
        "; \'High limited\' - значение нарушило верхний предел" );
    }

    // Test 7: Manual/Fixed status (status1=2, status2=3).
    {
    std::byte data[] = { B{0xFF}, B{0x16}, B{0x00}, B{0x00} };
    std::memcpy( TE1.AI_channels.int_read_values[ 0 ], data, sizeof( data ) );
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -33 ); // -10 - 2*10 - 3 = -33.
    EXPECT_STREQ( TE1.get_error_description(),
        "\'Manual/Fixed\' "
        "— значение может использоваться только ограниченно "
        "(например, симуляция)"
        "; \'Constant\' - "
        "значение установлено как постоянное (например, симуляция)" );
    }

    delete[] TE1.AI_channels.int_read_values[ 0 ];
    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, temperature_e_iolink_tm311_get_error_description )
    {
    temperature_e_iolink_tm311 TE1( "T1" );
    EXPECT_EQ( TE1.get_state(), 1 );
    EXPECT_STREQ( TE1.get_error_description(), "нет ошибок" );

    G_PAC_INFO()->emulation_off();
    init_channels( TE1 );
    TE1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };

    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -io_device::IOLINKSTATE::NOTCONNECTED );
    EXPECT_STREQ( TE1.get_error_description(), "IOL-устройство не подключено" );

    // Bit 0 - IOLink connected.
    *TE1.AI_channels.int_module_read_values[ 0 ] = 0b1;
    TE1.evaluate_io();
    EXPECT_EQ( TE1.get_state(), -io_device::IOLINKSTATE::DEVICEERROR );
    EXPECT_STREQ( TE1.get_error_description(), "ошибка IOL-устройства" );

    delete[] TE1.AI_channels.int_read_values[ 0 ];
    G_PAC_INFO()->emulation_on();
    }


template<typename T1 = i_DI_device, typename T2 = T1>
void check_dev( const char* name, int type, int sub_type,
    T1* ( *get_dev )( const char* name ),
    T2* ( *get_dev_by_n )( unsigned int n ) = nullptr,
    const char* article = "Article_1",
    bool is_virtual = false )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        type, sub_type, name, "Test device", article );
    if ( is_virtual ) EXPECT_EQ( nullptr, res );
    else  EXPECT_NE( nullptr, res );

    auto dev = G_DEVICE_MANAGER()->get_device( name );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );

    auto dev_by_name = get_dev( name );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( dev_by_name ) );
    if ( get_dev_by_n )
        {
        auto n = 0;
        if ( isdigit( strlen( name ) - 2 ) )
            {
            n = atoi( name + strlen( name ) - 2 ); // V11.
            }
        else
            {
            n = atoi( name + strlen( name ) - 1 ); // V1.
            }
        auto device_by_n = get_dev_by_n( n );
        EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( device_by_n ) );
        }
    }

TEST( device_manager, add_io_device )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    auto dev = G_DEVICE_MANAGER()->get_device( "NO_DEVICE" );
    EXPECT_EQ( G_DEVICE_MANAGER()->get_stub_device(), dev );


    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    G_DEVICE_MANAGER()->save_device( buff );
    EXPECT_STREQ(
        "t=\n"
        "\t{\n"
        "\t}\n",
        buff );

    check_dev<>( "SB1", device::DT_SB, device::DST_SB, SB );
    check_dev<>( "DI1", device::DT_DI, device::DST_DI, DI, DI );
    check_dev<i_DO_device>( "DO1", device::DT_DO, device::DST_DO, DO, DO );
    check_dev<i_AI_device>( "QT1", device::DT_QT, device::DST_QT, QT, QT );
    check_dev<i_AI_device>( "PT1", device::DT_PT, device::DST_PT, PT, PT );
    check_dev<i_DO_AO_device>( "F1", device::DT_F, device::DST_F, F, F );
    check_dev<signal_column>( "HLA1", device::DT_HLA, device::DST_HLA, HLA );
    check_dev<>( "GS1", device::DT_GS, device::DST_GS, GS );
    check_dev<>( "GS2", device::DT_GS, device::DST_GS_INVERSE, GS );
    check_dev<i_DI_device, i_DI_device>(
        "GS3", device::DT_GS, device::DST_GS_VIRT, GS, nullptr, "Art_1", true );


    check_dev<valve, i_DO_device>( "V1", device::DT_V,
        device::DST_V_DO1, V, V );
    check_dev<valve, i_DO_device>( "V2", device::DT_V,
        device::DST_V_DO2, V, V );
    check_dev<valve, i_DO_device>( "V3", device::DT_V,
        device::DST_V_DO1_DI1_FB_OFF, V, V );
    check_dev<valve, i_DO_device>( "V4", device::DT_V,
        device::DST_V_DO1_DI1_FB_ON, V, V );
    check_dev<valve, i_DO_device>( "V5", device::DT_V,
        device::DST_V_DO1_DI2, V, V );
    check_dev<valve, i_DO_device>( "V6", device::DT_V,
        device::DST_V_DO2_DI2, V, V );
    check_dev<valve, i_DO_device>( "V7", device::DT_V,
        device::DST_V_MIXPROOF, V, V );
    check_dev<valve, i_DO_device>( "V8", device::DT_V,
        device::DST_V_AS_MIXPROOF, V, V );
    check_dev<valve, i_DO_device>( "V9", device::DT_V,
        device::DST_V_BOTTOM_MIXPROOF, V, V );
    check_dev<valve, i_DO_device>( "V91", device::DT_V,
        device::DST_V_AS_DO1_DI2, V, V );
    check_dev<valve, i_DO_device>( "V92", device::DT_V,
        device::V_DO2_DI2_BISTABLE, V, V );
    check_dev<valve, i_DO_device>( "V93", device::DT_V,
        device::V_IOLINK_VTUG_DO1, V, V );
    check_dev<valve, i_DO_device>( "V94", device::DT_V,
        device::V_IOLINK_VTUG_DO1_FB_OFF, V, V );
    check_dev<valve, i_DO_device>( "V95", device::DT_V,
        device::V_IOLINK_VTUG_DO1_FB_ON, V, V );
    check_dev<valve, i_DO_device>( "V96", device::DT_V,
        device::V_IOLINK_VTUG_DO1_DI2, V, V );
    check_dev<valve, i_DO_device>( "V97", device::DT_V,
        device::V_IOLINK_MIXPROOF, V, V );
    check_dev<valve, i_DO_device>(
        "V98", device::DT_V, device::V_IOLINK_DO1_DI2,
        V, V, valve_iolink_shut_off_sorio::SORIO_ARTICLE.c_str() );
    check_dev<valve, i_DO_device>(
        "V99", device::DT_V, device::V_IOLINK_DO1_DI2, V, V /*AlfaLaval*/ );
    check_dev<valve, i_DO_device>(
        "V991", device::DT_V, device::V_IOLINK_DO1_DI2,
        V, V, valve_iolink_gea_tvis_a15_ss::GEA_TVIS_A15_SINGLE_SEAT_ARTICLE.c_str() );
    check_dev<valve, i_DO_device>(
        "V992", device::DT_V, device::V_IOLINK_DO1_DI2,
        V, V, valve_iolink_gea_tvis_a15_ds::GEA_TVIS_A15_DOUBLE_SEAT_ARTICLE.c_str() );

    G_DEVICE_MANAGER()->clear_io_devices();
    check_dev<valve, i_DO_device>(
        "V1", device::DT_V, device::DST_V_MINI_FLUSHING, V, V );
    check_dev<valve, i_DO_device>(
        "V2", device::DT_V, device::V_IOL_TERMINAL_MIXPROOF_DO3, V, V );
    check_dev<valve, i_DO_device>(
        "V3", device::DT_V, device::V_IOL_TERMINAL_MIXPROOF_DO3_DI2, V, V );

    check_dev<valve, i_DO_device>(
        "V91", device::DT_V, device::DST_V_AS_MIXPROOF, V, V, "AL.9615-4002-12" );
    auto Vx = V( "V91" );    // AlfaLaval new mixproof.
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );
    EXPECT_EQ( dynamic_cast<valve_AS_mix_proof*>( Vx )->reverse_seat_connection, true );
    check_dev<valve, i_DO_device>( "V92", device::DT_V, device::DST_V_AS_MIXPROOF, V, V );
    Vx = V( "V92" );        // AlfaLaval old mixproof.
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );
    EXPECT_EQ( dynamic_cast<valve_AS_mix_proof*>( Vx )->reverse_seat_connection, false );

    auto res = G_DEVICE_MANAGER()->add_io_device( device::DT_V,
        device::V_IOL_TERMINAL_MIXPROOF_DO3_DI2 + 1, "V2", "", "" );
    EXPECT_EQ( nullptr, res );

    check_dev<i_motor>( "M1", device::DT_M, device::DST_M, M, M );
    check_dev<>( "FS1", device::DT_FS, device::DST_FS, FS, FS );
    check_dev<i_AI_device>( "AI1", device::DT_AI, device::DST_AI, AI );
    check_dev<i_AO_device>( "AO1", device::DT_AO, device::DST_AO, AO, AO );
    check_dev<i_counter>( "FQT1", device::DT_FQT, device::DST_FQT, FQT, FQT );
    check_dev<i_counter>( "FQT2", device::DT_FQT, device::DST_FQT_IOLINK, FQT, FQT );
    check_dev<i_counter>( "FQT3", device::DT_FQT, device::DST_FQT_F, FQT, FQT );

    check_dev<i_AI_device>( "TE1", device::DT_TE, device::DST_TE, TE, TE );
    check_dev<i_AI_device>( "TE2", device::DT_TE, device::DST_TE_IOLINK, TE, TE );
    check_dev<i_AI_device>( "TE3", device::DT_TE, device::DST_TE_IOLINK,
        TE, TE, "E&H.TM311" );

    check_dev<>( "PDS1", device::DT_PDS, device::DST_PDS, PDS );
    check_dev<i_DI_device, i_DI_device>( "PDS2", device::DT_PDS,
        device::DST_PDS_VIRT, PDS, nullptr, "A", true );
    res = G_DEVICE_MANAGER()->add_io_device( device::DT_PDS,
        device::DST_PDS_VIRT + 1, "PDS3", "Test sensor", "" );
    EXPECT_EQ( nullptr, res );

    check_dev<>( "TS1", device::DT_TS, device::DST_TS, TS );
    check_dev<i_DI_device, i_DI_device>( "TS2", device::DT_TS,
        device::DST_TS_VIRT, TS, nullptr, "A", true );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TS, device::DST_TS_VIRT + 1, "TS3", "Test sensor", "" );
    EXPECT_EQ( nullptr, res );

    check_dev<i_wages>( "WT1", device::DT_WT, device::DST_WT, WT, WT );
    check_dev<i_wages>( "WT2", device::DT_WT, device::DST_WT_RS232, WT, WT );
    check_dev<i_wages>( "WT3", device::DT_WT, device::DST_WT_ETH, WT, WT );
    check_dev<i_wages>( "WT4", device::DT_WT, device::DST_WT_PXC_AXL, WT, WT );

    check_dev<i_DO_AO_device, i_DO_AO_device>(
        "C1", device::DT_REGULATOR, device::DST_REGULATOR_PID, C, nullptr, "", true );
    check_dev<i_DO_AO_device, i_DO_AO_device>(
        "C2", device::DT_REGULATOR, device::DST_REGULATOR_THLD, C, nullptr, "", true );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_THLD + 1, "C3",
        "Test regulator", "C" );
    EXPECT_EQ( nullptr, res );

    check_dev<i_DO_AO_device>( "G1", device::DT_G, device::DST_G_IOL_4, get_G );
    //Добавляем устройство с несуществующим подтипом.
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_G, device::DST_G_IOL_8 + 1, "G2", "Test power unit", "" );
    EXPECT_EQ( nullptr, res );

    check_dev<i_DO_device>( "HA1", device::DT_HA, device::DST_HA, HA );
    check_dev<i_DO_device>( "HL1", device::DT_HL, device::DST_HL, HL );

    check_dev<i_DI_device, i_DI_device>( "POU1LIFEBIT1", device::DT_WATCHDOG,
        device::DST_WATCHDOG, WATCHDOG, nullptr, "Art_1", true );
    check_dev<i_DI_device, i_DI_device>( "POU1LIFECOUNTER1", device::DT_WATCHDOG,
        device::DST_WATCHDOG, WATCHDOG, nullptr, "Art_1", true );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_WATCHDOG, device::DST_WATCHDOG + 1, "POU1LIFEBIT2",
        "Test watchdog", "Art_1" );
    EXPECT_EQ( nullptr, res );

    // Проверка устройства управления узлом сетевых настроек.
    auto* node1 = G_DEVICE_MANAGER()->add_io_device(
        device::DT_NODE, device::DST_NODE, "NODE1", "Test node device", "" );
    EXPECT_EQ( nullptr, node1 );
    // Тестирование несуществующего подтипа.
    res = G_DEVICE_MANAGER()->add_io_device( device::DT_NODE,
        device::DST_NODE + 1, "NODE2", "Test invalid node", "" );
    EXPECT_EQ( nullptr, res );

    G_DEVICE_MANAGER()->clear_io_devices();
    }


TEST( device_manager, clear_io_devices )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "T1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_TE( "T1" ) );   // Search should find device.

    G_DEVICE_MANAGER()->clear_io_devices();
    EXPECT_EQ( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_TE( "T1" ) );   // Search shouldn't find device.
    }

TEST( device_manager, get_FQT_IOLINK )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    auto* missing = G_DEVICE_MANAGER()->get_FQT_IOLINK( "NO_FQT" );
    ASSERT_NE( nullptr, missing );
    EXPECT_STREQ( "stub", missing->get_name() );

    G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT, "FQT_BASE", "Base counter", "" );
    auto* wrong_type = G_DEVICE_MANAGER()->get_FQT_IOLINK( "FQT_BASE" );
    ASSERT_NE( nullptr, wrong_type );
    EXPECT_STREQ( "stub", wrong_type->get_name() );

    G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_IOLINK, "FQT_IOL", "IOL counter", "IFM.SMF420" );
    auto* iol = G_DEVICE_MANAGER()->get_FQT_IOLINK( "FQT_IOL" );
    ASSERT_NE( nullptr, iol );
    EXPECT_STREQ( "FQT_IOL", iol->get_name() );

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( device_manager, get_device )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "T1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );

    // Search should find device.
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_device( static_cast<size_t>( 0 ) ) );

    // Search shouldn't find device.
    EXPECT_EQ( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_device( 1 ) );

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( device_manager, get_name_in_Lua )
    {
    ASSERT_STREQ( G_DEVICE_MANAGER()->get_name_in_Lua(), "Device manager" );
    }

TEST( device_manager, evaluate_io )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "T1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );

    G_DEVICE_MANAGER()->evaluate_io();

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( device_manager, save_device )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "T1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );

    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    G_DEVICE_MANAGER()->save_device( buff );
    EXPECT_STREQ(
        "t=\n"
        "\t{\n"
        "\tT1={M=0, ST=0, V=0},\n"
        "\t}\n",
        buff );

    G_DEVICE_MANAGER()->clear_io_devices();
    }


TEST( dev_stub, is_active )
    {
    EXPECT_FALSE( STUB()->is_active() );
    }

TEST( dev_stub, get_value )
    {
    EXPECT_EQ( STUB()->get_value(), 0.0f );
    }

TEST( dev_stub, direct_set_value )
    {
    // Ничего не происходит.
    STUB()->direct_set_value( 10.0f );
    EXPECT_EQ( STUB()->get_value(), 0.0f );
    }

TEST( dev_stub, direct_set_state )
    {
    // Ничего не происходит.
    STUB()->direct_set_state( 1 );
    EXPECT_EQ( STUB()->get_state(), 0 );
    }

TEST( dev_stub, direct_on )
    {
    // Ничего не происходит.
    STUB()->direct_on();
    EXPECT_EQ( STUB()->get_state(), 0 );
    }

TEST( dev_stub, instant_off )
    {
    // Ничего не происходит.
    STUB()->instant_off();
    EXPECT_EQ( STUB()->get_state(), 0 );
    }

TEST( dev_stub, get_valve_state )
    {
    EXPECT_EQ( STUB()->get_valve_state(), valve::VALVE_STATE::V_OFF );
    }

TEST( dev_stub, print )
    {
    testing::internal::CaptureStdout();
    STUB()->print();
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, "STUB" );
    }

TEST( dev_stub, pause )
    {
    // Ничего не происходит.
    STUB()->pause();
    }

TEST( dev_stub, start )
    {
    // Ничего не происходит.
    STUB()->start();
    }

TEST( dev_stub, reset )
    {
    // Ничего не происходит.
    STUB()->reset();
    }

TEST( dev_stub, abs_reset )
    {
    // Ничего не происходит.
    STUB()->abs_reset();
    }

TEST( dev_stub, get_quantity )
    {
    EXPECT_EQ( STUB()->get_quantity(), 0u );
    }

TEST( dev_stub, get_abs_quantity )
    {
    EXPECT_EQ( STUB()->get_abs_quantity(), 0u );
    }

TEST( dev_stub, get_flow )
    {
    EXPECT_EQ( STUB()->get_flow(), 0.0f );
    }

TEST( dev_stub, get_pump_dt )
    {
    EXPECT_EQ( STUB()->get_pump_dt(), 0u );
    }

TEST( dev_stub, get_min_flow )
    {
    EXPECT_EQ( STUB()->get_min_flow(), 0.0f );
    }

TEST( dev_stub, pause_daily )
    {
    // Ничего не происходит.
    STUB()->pause_daily();
    }

TEST( dev_stub, start_daily )
    {
    // Ничего не происходит.
    STUB()->start_daily();
    }

TEST( dev_stub, tare )
    {
    // Ничего не происходит.
    STUB()->tare();
    }

TEST( dev_stub, process_DO )
    {
    // Ничего не происходит.
    STUB()->process_DO( 0, signal_column::DO_state::OFF, "DO1" );
    }


TEST( device, device )
    {
    device dev( nullptr, device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( dev.get_name(), "?" );
    }

TEST( device, device_too_long_name )
    {
    device dev( "VERY_VERY_LONG_DEVICE_NAME_MORE_THAN_30_SYMBOLS",
        device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( dev.get_name(), "VERY_VERY_LONG_DEVICE_NAME_MOR" );
    }

TEST( device, get_type_str )
    {
    device dev1( "DEV1", device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( dev1.get_type_str(), "NONE" );

    device dev2( "DEV2", device::DEVICE_TYPE::DT_V,
        device::DEVICE_SUB_TYPE::DST_V_VIRT, 0 );
    EXPECT_STREQ( dev2.get_type_str(), "V" );

    device dev3( "DEV3", device::DEVICE_TYPE::DT_WATCHDOG,
        device::DEVICE_SUB_TYPE::DST_WATCHDOG, 0 );
    EXPECT_STREQ( dev3.get_type_str(), "WATCHDOG" );

    device dev4( "DEV4", device::DEVICE_TYPE::DT_NODE,
        device::DEVICE_SUB_TYPE::DST_NODE, 0 );
    EXPECT_STREQ( dev4.get_type_str(), "NODE" );
    }

TEST( device, get_name_in_Lua )
    {
    device dev1( "DEV1", device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( dev1.get_name_in_Lua(), "DEV1" );
    }

TEST( device, save_device )
    {
    temperature_e_analog t1( "T1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    t1.save_device( buff );
    EXPECT_STREQ(
        "T1={M=0, ST=1, V=0, E=0, M_EXP=20.0, S_DEV=2.0, P_CZ=0, "
        "P_ERR=0, P_MIN_V=0, P_MAX_V=0},\n", buff );
    }

TEST( device, set_article )
    {
    temperature_e_analog T1( "T1" );
    auto IFM_TE = "IFM.TE11";
    T1.set_article( IFM_TE );
    EXPECT_STREQ( IFM_TE, T1.get_article() );
    }

TEST( device, get_type_name )
    {
    device dev1( "DEV1", device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( "???", dev1.get_type_name() );
    }

TEST( device, evaluate_io )
    {
    temperature_e_analog T1( "T1" );
    T1.evaluate_io();
    }

TEST( device, set_property )
    {
    temperature_e_analog T1( "T1" );
    T1.set_property( "site", nullptr );
    }


TEST( analog_io_device, set_cmd )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    analog_io_device obj( "OBJ1", device::DEVICE_TYPE::DT_TE,
        device::DEVICE_SUB_TYPE::DST_TS, 0 );

    obj.set_cmd( "M_EXP", 0, 10 );
    obj.set_cmd( "S_DEV", 0, 20 );
    obj.set_cmd( "E", 0, 0 );
    obj.set_cmd( "NOT_EXIST", 0, 1 );       //Несуществующее поле.

    obj.save_device( buff );
    EXPECT_STREQ( "OBJ1={M=0, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );

    // Проверка включения ручного режима.
    obj.set_cmd( "M", 0, 1 );
    obj.save_device( buff );
    EXPECT_STREQ( "OBJ1={M=1, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );

    // Проверка отключения ручного режима.
    obj.set_cmd( "M", 0, 0 );
    obj.save_device( buff );
    EXPECT_STREQ( "OBJ1={M=0, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );

    // Проверка включения ручного режима - только на 1 должен включиться.
    testing::internal::CaptureStdout();
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    auto tm = get_time();
    std::stringstream tmp;
    tmp << std::put_time( &tm, "%Y-%m-%d %H.%M.%S " );

    obj.set_cmd( "M", 0, 100 );
    auto output = testing::internal::GetCapturedStdout();
    auto exp_output = tmp.str() +
#if defined LINUX_OS
        "\x1B[37m" +
#endif
        "DEBUG  (7) -> OBJ1\t analog_io_device::set_cmd() - prop = M, idx = 0, val = 100.000000\n" +
#if defined LINUX_OS
        "\x1B[0m" +
#endif
        tmp.str() +
#if defined LINUX_OS
        "\x1B[37m" +
#endif
        "DEBUG  (7) -> OBJ1\t device::set_cmd() - prop = M, idx = 0, val = 100.000000\n"
#if defined LINUX_OS
        + "\x1B[0m"
#endif
        ;
    EXPECT_EQ( output, exp_output );
    obj.save_device( buff );
    EXPECT_STREQ( "OBJ1={M=0, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );

    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    }


TEST( DO1, get_state )
    {
    DO1 do1( "DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO );
    do1.init( 1, 0, 0, 0 );
    do1.DO_channels.char_write_values[ 0 ] = new u_char{ 0 };
    do1.DO_channels.char_read_values[ 0 ] = new u_char{ 0 };

    G_PAC_INFO()->emulation_off();
    // Когда отключена эмуляция, пишем в буфер обмена с модулями ввода\вывода.
    EXPECT_EQ( do1.get_state(), 0 );
    EXPECT_EQ( do1.DO_channels.char_write_values[ 0 ][ 0 ], 0 );

    uni_io_manager mngr;
    mngr.init( 1 );
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    do1.init_channel( io_device::IO_channels::CT_DO, 0, 0, 0 );

    EXPECT_EQ( do1.get_state(), 0 );

    do1.on();
    mngr.get_node( 0 )->is_active = true;
    mngr.get_node( 0 )->state = io_manager::io_node::STATES::ST_OK;
    do1.evaluate_io();
    EXPECT_EQ( do1.get_state(), 1 );
    EXPECT_EQ( do1.DO_channels.char_write_values[ 0 ][ 0 ], 1 );

    auto node = mngr.get_node( 0 );
    node->status_register = 0x0010; // PP mode.
    do1.evaluate_io();
    EXPECT_EQ( do1.get_state(), -1 );

    do1.evaluate_io();
    EXPECT_EQ( node->status_register, 0x0010 );
    EXPECT_EQ( do1.get_state(), -1 );

    io_manager::replace_instance( prev_mngr );
    G_PAC_INFO()->emulation_on();

    // Когда включена эмуляция, пишем в поле состояния объекта. Здесь буфер
    // обмена с модулями ввода\вывода не должен изменяться [ 1 ].
    EXPECT_EQ( do1.get_state(), 0 );
    do1.on();
    EXPECT_EQ( do1.get_state(), 1 );
    do1.off();
    EXPECT_EQ( do1.get_state(), 0 );
    EXPECT_EQ( do1.DO_channels.char_write_values[ 0 ][ 0 ], 1 );           //1
    }

TEST( DO1, get_type_name )
    {
    DO1 do1( "DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO );
    EXPECT_STREQ( do1.get_type_name(), "Дискретный выходной сигнал" );
    }

TEST( DO1, set_cmd )
    {
    DO1 do1( "DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO );
    EXPECT_EQ( do1.get_state(), 0 );

    do1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( do1.get_state(), 1 );
    }

TEST( DO1, save_device )
    {
    DO1 do1( "DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO );
    const int BUFF_SIZE = 200;
    std::array <char, BUFF_SIZE> buff{};

    do1.save_device( buff.data() );
    EXPECT_STREQ( "DO1={M=0, ST=0},\n", buff.data() );
    }


TEST( AI1, get_max_val )
    {
    AI1 sensor( "AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI, 0 );
    EXPECT_EQ( sensor.get_max_val(), 0 );
    }

TEST( AI1, get_min_val )
    {
    AI1 sensor( "AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI, 0 );
    EXPECT_EQ( sensor.get_min_val(), 0 );
    }

TEST( AI1, get_type_name )
    {
    AI1 test_dev( "test_AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI_VIRT, 0 );
    EXPECT_STREQ( "Аналоговый входной сигнал", test_dev.get_type_name() );
    }

TEST( AI1, get_state )
    {
    AI1 sensor( "AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI, 0 );
    EXPECT_EQ( sensor.get_state(), 1 );

    G_PAC_INFO()->emulation_off();

    // No I\O data.
    auto err = 0;
    auto res = sensor.get_AI( 0 /*sensor::C_AI_INDEX*/, 0, 0, err );
    EXPECT_EQ( err, static_cast<int>( io_device::ERRORS::BAD_IO_DATA ) );
    EXPECT_EQ( res, 0.f );
    EXPECT_EQ( sensor.get_value(), 0.f );

    uni_io_manager mngr;
    mngr.init( 1 );
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    sensor.init( 0, 0, 0, 1 );
    sensor.init_channel( io_device::IO_channels::CT_AI, 0, 0, 0 );

    const auto NO_ERR = static_cast<int>( io_device::ERRORS::NO_ERR );
    const auto UNDER_RANGE = static_cast<int>( io_device::ERRORS::UNDER_RANGE );
    const auto OVER_RANGE = static_cast<int>( io_device::ERRORS::OVER_RANGE );
    const auto OUT_OF_RANGE = static_cast<int>( io_device::ERRORS::OUT_OF_RANGE );

    const auto MIN_V = 10.f;
    const auto MAX_V = 20.f;
    const auto ABS_ERR = .02f;
    const auto ZERO_ABS_ERR = .0f;

    auto test_value{ [ & ]( int in_value, float res_value, int err_value,
        float abs_err, float min = .0f, float max = .0f ) {
        sensor.AI_channels.int_read_values[ 0 ][ 0 ] = in_value;
        res = sensor.get_AI( 0 /*sensor::C_AI_INDEX*/, min, max, err );
        EXPECT_EQ( err, err_value );
        EXPECT_NEAR( res, res_value, abs_err );
        EXPECT_EQ( sensor.get_state(), err_value == 0 ? 1 : -err_value );
        } };

    auto test_m{ [ & ]( int io_module,
        int in_range_value, float in_range_res,
        int under_range_value, float under_range_res,
        int over_range_value, float over_range_res ) {
        mngr.init_node_AI( 0, 0, io_module, 0 );
        test_value( in_range_value, in_range_res, NO_ERR, ABS_ERR );
        test_value( under_range_value, under_range_res, UNDER_RANGE,
            ZERO_ABS_ERR );
        test_value( over_range_value, over_range_res, OVER_RANGE,
            ZERO_ABS_ERR );
        } };

    auto test_m_no_over_range{ [ & ]( int io_module,
        int in_range_value, float in_range_res,
        int under_range_value, float under_range_res ) {
        mngr.init_node_AI( 0, 0, io_module, 0 );
        test_value( in_range_value, in_range_res, NO_ERR, ABS_ERR );
        test_value( under_range_value, under_range_res, UNDER_RANGE,
            ZERO_ABS_ERR );
        } };

    test_m( 461, 100, 10.f, -2001, -1000, 8501, -1000 );// 750-461 Pt100/RTD
    test_m( 450, 100, 10.f, -2001, -1000, 8501, -1000 );// 750-450 R Adjustable
    test_m_no_over_range( 496, 29488, 18.4f, 3, -1 );   // 750-496 8AI 0/4-20mA S.E.
    test_value( 1000, 10.31f, NO_ERR, ABS_ERR, MIN_V, MAX_V );
    test_m_no_over_range( 466, 29488, 18.4f, 3, -1 );   // 750-466 2AI 4-20mA
    test_value( 1000, 10.31f, NO_ERR, ABS_ERR, MIN_V, MAX_V );

    mngr.init_node_AI( 0, 0, 491, 0 );
    test_value( 30001, -1000.f, OUT_OF_RANGE, ABS_ERR );
    test_value( 1000, 0.5f, NO_ERR, ABS_ERR, MIN_V, MAX_V );

    mngr.init_node_AI( 0, 0, 1088106, 0 );
    test_value( -32001, -1000.f, UNDER_RANGE, ZERO_ABS_ERR );
    test_value( 1000, 100.f, NO_ERR, ABS_ERR, MIN_V, MAX_V );
    mngr.init_node_AI( 0, 0, 2688556, 0 );
    test_value( -32001, -1000.f, UNDER_RANGE, ZERO_ABS_ERR );
    test_value( 1000, 100.f, NO_ERR, ABS_ERR, MIN_V, MAX_V );

    mngr.init_node_AI( 0, 0, 2688491, 0 );
    test_value( -32001, -1.f, UNDER_RANGE, ABS_ERR );
    test_value( 1000, 10.33f, NO_ERR, ABS_ERR, MIN_V, MAX_V );
    mngr.init_node_AI( 0, 0, 2702072, 0 );
    test_value( -32001, -1.f, UNDER_RANGE, ZERO_ABS_ERR );
    test_value( 1000, 10.33f, NO_ERR, ABS_ERR, MIN_V, MAX_V );
    mngr.init_node_AI( 0, 0, 1088062, 0 );
    test_value( -32001, -1.f, UNDER_RANGE, ZERO_ABS_ERR );
    test_value( 1000, 10.63f, NO_ERR, ABS_ERR, MIN_V, MAX_V );

    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }

TEST( AI1, direct_set_value )
    {
    AI1 sensor( "AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI, 0 );
    const auto NEW_VAL = 10.1f;
    sensor.direct_set_value( NEW_VAL );
    EXPECT_EQ( sensor.get_value(), NEW_VAL );

    G_PAC_INFO()->emulation_off();
    EXPECT_EQ( sensor.get_value(), .0f );
    G_PAC_INFO()->emulation_on();
    }


TEST( level, get_params_count )
    {
    level L1( "L1", device::DEVICE_SUB_TYPE::DST_LT_CONE, 1 );
    EXPECT_EQ( L1.get_params_count(), 2 );
    }

TEST( level, get_type_name )
    {
    level test_dev( "test_LT1", device::DEVICE_SUB_TYPE::DST_LT_VIRT, 0 );
    EXPECT_STREQ( "Текущий уровень", test_dev.get_type_name() );
    }

TEST( level, level )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    level test_dev( "test_LT1", device::DEVICE_SUB_TYPE::DST_LT_VIRT, 0 );

    test_dev.save_device( buff );
    EXPECT_STREQ( "test_LT1={M=0, ST=1, V=0, CLEVEL=0, P_CZ=0, P_ERR=0},\n", buff );
    }

TEST( level, get_max_val )
    {
    level test_dev( "test_LT1", device::DEVICE_SUB_TYPE::DST_LT_VIRT, 0 );
    EXPECT_EQ( test_dev.get_max_val(), 100.f );
    }

TEST( level, get_min_val )
    {
    level test_dev( "test_LT1", device::DEVICE_SUB_TYPE::DST_LT_VIRT, 0 );
    EXPECT_EQ( test_dev.get_min_val(), 0.f );
    }

TEST( level, get_volume )
    {
    level test_dev( "test_LT1", device::DEVICE_SUB_TYPE::DST_LT_VIRT, 0 );
    EXPECT_EQ( test_dev.get_volume(), 0.f );

    const auto NEW_VAL = 10.f;
    test_dev.direct_set_value( NEW_VAL );
    EXPECT_EQ( test_dev.get_volume(), NEW_VAL );

    test_dev.direct_set_state( -1 );
    const auto ERR_VAL = 100.f;
    test_dev.set_par( 2, 0, ERR_VAL );
    EXPECT_EQ( test_dev.get_volume(), ERR_VAL );
    }


TEST( level_e_cyl, get_volume )
    {
    level_e_cyl test_dev( "test_LT1" );
    EXPECT_EQ( test_dev.get_volume(), 0.f );
    }


TEST( level_e_cone, get_volume )
    {
    level_e_cone test_dev( "test_LT1" );
    EXPECT_EQ( test_dev.get_volume(), 0.f );
    }


TEST( level_e_cyl_hor, get_volume_zero )
    {
    level_e_cyl_hor test_dev( "test_LT1" );
    EXPECT_EQ( test_dev.get_volume(), 0 );
    }

TEST( level_e_cyl_hor, get_volume_half_full )
    {
    level_e_cyl_hor test_dev( "test_LT1" );

    // Set R=1m, L=2m, density=1000 kg/m³, no angle, no offset.
    test_dev.set_cmd( "P_R", 0, 1.0f );
    test_dev.set_cmd( "P_H", 0, 2.0f );
    test_dev.set_cmd( "P_DENSITY", 0, 1000.0f );
    // P_MAX_P = 0.1962 bar → h = 0.5 * P_MAX_P * 1e5 / (rho*g) = 1m = R.
    test_dev.set_cmd( "P_MAX_P", 0, 0.1962f );

    // 50% fill → pressure = 0.0981 bar → h = 1m = R (half full).
    test_dev.direct_set_value( 50.0f );

    // Half-full horizontal cylinder: V = π * R² * L / 2 ≈ π m³.
    // Mass ≈ 1000 * π ≈ 3142 kg.
    EXPECT_EQ( test_dev.get_volume(), 3140 );

    // Apply P_C0.
    test_dev.set_cmd( "P_C0", 0, 0.1f );
    test_dev.direct_set_value( 25.0f );
    EXPECT_EQ( test_dev.get_volume(), 5120 );
    }

TEST( level_e_cyl_hor, get_volume_full )
    {
    level_e_cyl_hor test_dev( "test_LT1" );

    test_dev.set_cmd( "P_R", 0, 1.0f );
    test_dev.set_cmd( "P_H", 0, 2.0f );
    test_dev.set_cmd( "P_DENSITY", 0, 1000.0f );
    // P_MAX_P = 0.4 bar → h = P_MAX_P * 1e5 / (rho*g) ≈ 4 m > 2R at 100%.
    test_dev.set_cmd( "P_MAX_P", 0, 0.4f );
    test_dev.direct_set_value( 100.0f );

    // Full cylinder: V = π * R² * L ≈ 2π m³, M ≈ 6283 kg.
    EXPECT_EQ( test_dev.get_volume(), 6280 );
    }


TEST_F( iolink_dev_test, level_e_iolink_get_error_description )
    {
    level_e_iolink test_dev( "TestDevice" );
    test_dev_err( test_dev, test_dev, 0 );
    }

TEST_F( iolink_dev_test, level_e_iolink_evaluate_io )
    {
    level_e_iolink test_dev( "L1" );

    G_PAC_INFO()->emulation_off();
    test_dev.init( 0, 0, 0, 1 );
    test_dev.init_channel( io_device::IO_channels::CT_AI, 0, 0, 0, 1, 1 );

    // Set IOLink state to OK - Bit 0: IOLink connected, Bit 8: IOLink data valid.
    set_iol_state_to_OK( test_dev );

    *test_dev.AI_channels.int_read_values[ 0 ] = 10;

    // Set P_MAX_P parameter to 1.0 bar for level calculation.
    test_dev.set_par( level_e_iolink::CONSTANTS::P_MAX_P,
        test_dev.start_param_idx, 1.0f );

    // Test parameter setting worked.
    EXPECT_EQ( test_dev.get_par( level_e_iolink::CONSTANTS::P_MAX_P,
        test_dev.start_param_idx ), 1.0f );

    // For now, just test that the level sensor doesn't crash and can be configured
    // The data processing for IOLink appears to need proper byte-formatted data
    // rather than simple integer values.
    test_dev.set_article( "IFM.PM1706" );

    // Basic functionality test - device should not crash when evaluate_io is called.
    test_dev.evaluate_io();
    // Test calculated value.
    EXPECT_NEAR( test_dev.get_value(), 25.6f, .1f );
    test_dev.set_par( level_e_iolink::CONSTANTS::P_R,
        test_dev.start_param_idx, 1.0f );
    EXPECT_EQ( test_dev.get_volume(), 8200.0f );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, level_e_iolink_set_string_property )
    {
    level_e_iolink test_dev( "TestDevice" );

    EXPECT_EQ( test_dev.PT_extra, nullptr );
    test_dev.set_string_property( "PT", "" );
    ASSERT_EQ( STUB(), dynamic_cast<dev_stub*>( test_dev.PT_extra ) );

    auto PT_name = std::string( "PT1" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_PT, device::DST_PT_VIRT, PT_name.c_str(), "Test sensor", "PT" );
    ASSERT_EQ( nullptr, res );
    const auto PT1 = PT( PT_name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( PT1 ) );
    test_dev.set_string_property( "PT", "PT1" );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( test_dev.PT_extra ) );
    ASSERT_EQ( PT1, test_dev.PT_extra );

    G_DEVICE_MANAGER()->clear_io_devices();
    }


TEST( pressure_e, pressure_e )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    pressure_e P1( "P1" );

    P1.save_device( buff );
    EXPECT_STREQ( "P1={M=0, ST=1, V=0, E=0, M_EXP=1.0, S_DEV=0.2, P_CZ=0, "
        "P_MIN_V=0, P_MAX_V=0},\n", buff );
    }

TEST( pressure_e, get_type_name )
    {
    pressure_e test_dev( "test_PT1" );
    EXPECT_STREQ( "Давление", test_dev.get_type_name() );
    }

TEST( pressure_e, get_max_val )
    {
    pressure_e test_dev( "test_PT1" );
    EXPECT_EQ( test_dev.get_max_val(), 0 );
    }

TEST( pressure_e, get_min_val )
    {
    pressure_e test_dev( "test_PT1" );
    EXPECT_EQ( test_dev.get_min_val(), 0 );
    }


TEST( pressure_e_iolink, pressure_e_iolink )
    {
    std::array<char, 200> buff{ '\0' };
    pressure_e_iolink test_dev( "P1" );

    test_dev.save_device( buff.data() );
    EXPECT_STREQ( "P1={M=0, ST=0, V=0, E=0, M_EXP=1.0, S_DEV=0.2, P_ERR=0},\n",
        buff.data() );
    }

TEST( pressure_e_iolink, read_article )
    {
    pressure_e_iolink test_dev( "P1" );
    EXPECT_EQ( test_dev.get_article_n(), pressure_e_iolink::ARTICLE::DEFAULT );
    const auto IFM____ = "IFM.____";
    test_dev.set_article( IFM____ );
    EXPECT_EQ( test_dev.get_article_n(), pressure_e_iolink::ARTICLE::DEFAULT );

    const auto IFM_PM1706 = "IFM.PM1706";
    test_dev.set_article( IFM_PM1706 );
    EXPECT_EQ( test_dev.get_article_n(), pressure_e_iolink::ARTICLE::IFM_PM1706 );
    EXPECT_STREQ( test_dev.get_article(), IFM_PM1706 );

    const auto IFM_PM1717 = "IFM.PM1717";
    test_dev.set_article( IFM_PM1717 );
    EXPECT_EQ( test_dev.get_article_n(), pressure_e_iolink::ARTICLE::IFM_PM1717 );
    EXPECT_STREQ( test_dev.get_article(), IFM_PM1717 );

    const auto EH_PMP23_CA7N2KB3CJ = "E&H.PMP23-CA7N2KB3CJ";
    test_dev.set_article( EH_PMP23_CA7N2KB3CJ );
    EXPECT_EQ( test_dev.get_article_n(), pressure_e_iolink::ARTICLE::EH_PMP23 );
    EXPECT_STREQ( test_dev.get_article(), EH_PMP23_CA7N2KB3CJ );

    const auto EH_PMP23_CA7N1KB3CJ = "E&H.PMP23-CA7N1KB3CJ";
    test_dev.set_article( EH_PMP23_CA7N1KB3CJ );
    EXPECT_EQ( test_dev.get_article_n(), pressure_e_iolink::ARTICLE::EH_PMP23 );
    EXPECT_STREQ( test_dev.get_article(), EH_PMP23_CA7N1KB3CJ );
    }

TEST_F( iolink_dev_test, pressure_e_iolink_evaluate_io )
    {
    pressure_e_iolink test_dev( "P1" );

    G_PAC_INFO()->emulation_off();
    test_dev.init( 0, 0, 0, 1 );
    test_dev.init_channel( io_device::IO_channels::CT_AI, 0, 0, 0, 1, 1 );

    // Set IOLink state to OK - Bit 0: IOLink connected, Bit 8: IOLink data valid
    set_iol_state_to_OK( test_dev );

    test_dev.AI_channels.int_read_values[ 0 ][ 0 ] = 100;

    test_dev.evaluate_io();
    EXPECT_EQ( test_dev.get_value(), 0.0f ); // Default value is 0.

    const auto IFM_PM1706 = "IFM.PM1706";
    test_dev.set_article( IFM_PM1706 );
    // Value should calculate to 2.55f for the IFM.PM1706 (100 as raw
    // input data from the line above).
    test_dev.evaluate_io();
    EXPECT_NEAR( test_dev.get_value(), 2.55f, .01f );

    const auto IFM_PM1717 = "IFM.PM1717";
    test_dev.set_article( IFM_PM1717 );
    test_dev.evaluate_io();
    // Value should calculate to 2.55f for the IFM.PM1717 (100 as raw
    // input data from the line above).
    EXPECT_NEAR( test_dev.get_value(), 2.55f, .01f );

    const auto IFM_PM1708 = "IFM.PM1708";
    test_dev.set_article( IFM_PM1708 );
    test_dev.evaluate_io();
    EXPECT_NEAR( test_dev.get_value(), 0.255f, .01f );

    const auto IFM_PI2715 = "IFM.PI2715";
    test_dev.set_article( IFM_PI2715 );
    test_dev.evaluate_io();
    EXPECT_NEAR( test_dev.get_value(), 6.4f, .01f );

    const auto IFM_PI2794 = "IFM.PI2794";
    test_dev.set_article( IFM_PI2794 );
    test_dev.evaluate_io();
    EXPECT_NEAR( test_dev.get_value(), 64.0f, .01f );

    const auto FES_8001446 = "FES.8001446";
    test_dev.set_article( FES_8001446 );
    test_dev.evaluate_io();
    EXPECT_NEAR( test_dev.get_value(), 3.91f, .01f );

    const auto EH_PMP23_CA7N2KB3CJ = "E&H.PMP23-CA7N2KB3CJ";
    test_dev.set_article( EH_PMP23_CA7N2KB3CJ );
    test_dev.evaluate_io();
    // Value should calculate to 1686.83f for the E&H.PMP23-CA7N2KB3CJ (100 as
    // raw input data from the line above).
    EXPECT_NEAR( test_dev.get_value(), 1686.83f, .01f );

    const auto EH_PMP23_CA7N1KB3CJ = "E&H.PMP23-CA7N1KB3CJ";
    test_dev.set_article( EH_PMP23_CA7N1KB3CJ );
    test_dev.evaluate_io();
    EXPECT_NEAR( test_dev.get_value(), 1686.83f, .01f );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, pressure_e_iolink_get_error_description )
    {
    pressure_e_iolink test_dev( "TestDevice" );
    test_dev_err( test_dev, test_dev, 0 );
    }


TEST( analog_output, get_max_value )
    {
    analog_output A1( "AO1" );
    EXPECT_EQ( A1.get_max_value(), 0 );
    }

TEST( analog_output, get_min_value )
    {
    analog_output A1( "AO1" );
    EXPECT_EQ( A1.get_min_value(), 0 );
    }

TEST( analog_output, get_type_name )
    {
    analog_output test_dev( "test_AO1" );
    EXPECT_STREQ( "Аналоговый выходной сигнал", test_dev.get_type_name() );
    }

TEST( analog_output, get_value )
    {
    analog_output AO01( "AO1" );

    G_PAC_INFO()->emulation_on();
    EXPECT_EQ( AO01.get_value(), 0.f );     // Check default value.
    AO01.set_value( 11.f );
    EXPECT_EQ( AO01.get_value(), 11.f );

    uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.init_node_AO( 0, 0, 555, 0 );
    G_PAC_INFO()->emulation_off();
    AO01.init( 0, 0, 1, 0 );
    AO01.init_channel( io_device::IO_channels::CT_AO, 0, 0, 0 );

    AO01.set_value( 10.f );
    EXPECT_EQ( AO01.get_value(), 10.f );

    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }

TEST( analog_output, save_device )
    {
    analog_output AO01( "AO1" );
    const int BUFF_SIZE = 200;
    std::array <char, BUFF_SIZE> buff{};

    AO01.save_device( buff.data() );
    EXPECT_STREQ( "AO1={M=0, V=0, E=0, M_EXP=1.0, S_DEV=0.2, "
        "P_MIN_V=0, P_MAX_V=0},\n",
        buff.data() );
    }


TEST( flow_s, flow_s )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    flow_s FS1( "FS1" );

    FS1.save_device( buff );
    EXPECT_STREQ( "FS1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( flow_s, get_type_name )
    {
    flow_s test_dev( "test_FS1" );
    EXPECT_STREQ( "Расход", test_dev.get_type_name() );
    }


TEST( DI_signal, DI_signal )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    DI_signal DI1( "DI1" );

    DI1.save_device( buff );
    EXPECT_STREQ( "DI1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( DI_signal, get_type_name )
    {
    DI_signal test_dev( "test_DI1" );
    EXPECT_STREQ( "Дискретный входной сигнал", test_dev.get_type_name() );
    }

TEST( DI_signal, get_state )
    {
    DI_signal DI1( "DI1" );

    EXPECT_EQ( DI1.get_state(), 0 );

    G_PAC_INFO()->emulation_off();
    DI1.init_and_alloc( 0, 1 );
    DI1.evaluate_io();
    EXPECT_EQ( DI1.get_state(), 0 );

    *DI1.DI_channels.char_read_values[ 0 ] = 1;
    DI1.evaluate_io();
    EXPECT_EQ( DI1.get_state(), 1 );

    *DI1.DI_channels.char_read_values[ 0 ] = 0;
    DI1.set_cmd( "P_DT", 0, 1000.0f );
    DI1.evaluate_io();
    // Область чтения изменилась, но не прошло заданное время - состояние не
    // должно изменяться.
    EXPECT_EQ( DI1.get_state(), 1 );

    DeltaMilliSecSubHooker::set_millisec( 1010UL );
    DI1.evaluate_io();
    // Прошло заданное время - состояние должно измениться.
    EXPECT_EQ( DI1.get_state(), 0 );

    // Область чтения не изменилась - состояние не должно изменяться.
    DI1.evaluate_io();
    EXPECT_EQ( DI1.get_state(), 0 );

    DeltaMilliSecSubHooker::set_default_time();
    G_PAC_INFO()->emulation_on();
    }


TEST( button, button )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    button BT1( "BT1" );

    BT1.save_device( buff );
    EXPECT_STREQ( "BT1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( button, get_type_name )
    {
    button test_dev( "test_BT1" );
    EXPECT_STREQ( "Кнопка", test_dev.get_type_name() );
    }


TEST( DO_signal, DO_signal )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    DO_signal DO1( "DO1" );

    DO1.save_device( buff );
    EXPECT_STREQ( "DO1={M=0, ST=0},\n", buff );
    }

TEST( DO_signal, get_type_name )
    {
    DO_signal test_dev( "test_DO1" );
    EXPECT_STREQ( "Дискретный выходной сигнал", test_dev.get_type_name() );
    }


TEST( siren, siren )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    siren S1( "S1" );

    S1.save_device( buff );
    EXPECT_STREQ( "S1={M=0, ST=0},\n", buff );
    }

TEST( siren, get_type_name )
    {
    siren test_dev( "test_HA1" );
    EXPECT_STREQ( "Аварийная звуковая сигнализация", test_dev.get_type_name() );
    }


TEST( lamp, lamp )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    lamp L1( "L1" );

    L1.save_device( buff );
    EXPECT_STREQ( "L1={M=0, ST=0},\n", buff );
    }

TEST( lamp, get_type_name )
    {
    lamp test_dev( "test_HA1" );
    EXPECT_STREQ( "Аварийная световая сигнализация", test_dev.get_type_name() );
    }


TEST( concentration_e, get_type_name )
    {
    concentration_e test_dev( "test_QT1", device::DEVICE_SUB_TYPE::DST_QT_VIRT );
    EXPECT_STREQ( "Концентрация", test_dev.get_type_name() );
    }

TEST( concentration_e, get_max_val )
    {
    concentration_e test_dev( "test_QT1", device::DEVICE_SUB_TYPE::DST_QT_VIRT );
    EXPECT_EQ( test_dev.get_max_val(), 0 );
    }

TEST( concentration_e, get_min_val )
    {
    concentration_e test_dev( "test_QT1", device::DEVICE_SUB_TYPE::DST_QT_VIRT );
    EXPECT_EQ( test_dev.get_min_val(), 0 );
    }

TEST( concentration_e, get_error_description )
    {
    concentration_e test_dev( "test_QT1", device::DEVICE_SUB_TYPE::DST_QT_VIRT );
    EXPECT_STREQ( "нет ошибок", test_dev.get_error_description() );

    test_dev.set_cmd( "ST", 0, -static_cast<int>( io_device::ERRORS::UNDER_RANGE ) );
    EXPECT_STREQ( "ниже предела", test_dev.get_error_description() );

    test_dev.set_cmd( "ST", 0, -static_cast<int>( io_device::ERRORS::OVER_RANGE ) );
    EXPECT_STREQ( "выше предела", test_dev.get_error_description() );

    test_dev.set_cmd( "ST", 0, -static_cast<int>( io_device::ERRORS::OUT_OF_RANGE ) );
    EXPECT_STREQ( "вне диапазона", test_dev.get_error_description() );

    test_dev.set_cmd( "ST", 0, -static_cast<int>( io_device::ERRORS::LAST_ERR_IDX ) );
    EXPECT_STREQ( "неизвестная ошибка", test_dev.get_error_description() );
    }


TEST( concentration_e_ok, get_state )
    {
    concentration_e_ok Q1( "Q1" );
    EXPECT_EQ( Q1.get_state(), 1 );
    }

TEST( concentration_e_ok, save_device )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    concentration_e_ok Q1( "Q1" );

    Q1.save_device( buff );
    EXPECT_STREQ( "Q1={M=0, ST=1, V=0, OK=1, P_CZ=0, P_MIN_V=0, P_MAX_V=0},\n",
        buff );
    }


TEST( concentration_e_iolink, concentration_e_iolink )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    concentration_e_iolink Q1( "Q1" );

    Q1.save_device( buff );
    EXPECT_STREQ( "Q1={M=0, ST=0, V=0, T=0.0, P_ERR=0, P_MAX_V=0},\n", buff );
    }

TEST_F( iolink_dev_test, concentration_e_iolink_get_error_description )
    {
    concentration_e_iolink test_dev( "TestDevice" );
    test_dev_err( test_dev, test_dev, 0 );
    }

TEST_F( iolink_dev_test, concentration_e_iolink_get_value_clamps_to_p_max_v )
    {
    concentration_e_iolink test_dev( "Q1" );

    G_PAC_INFO()->emulation_off();
    init_channels( test_dev );
    set_iol_state_to_OK( test_dev );

    // Set raw conductivity value to 5000 (converts to 5.0 mS/cm
    // when multiplied by 0.001).
    test_dev.info->conductivity = 5000;

    // Without P_MAX_V set (0), value should pass through unchanged.
    EXPECT_NEAR( test_dev.get_value(), 5.0f, .001f );

    // Set P_MAX_V to 3.0 to clamp value.
    test_dev.set_par(
        static_cast<int>( concentration_e_iolink::CONSTANTS::P_MAX_V ),
        0, 3.0f );
    EXPECT_NEAR( test_dev.get_value(), 3.0f, .001f );

    // When value is within range, P_MAX_V should not affect the result.
    test_dev.set_par(
        static_cast<int>( concentration_e_iolink::CONSTANTS::P_MAX_V ),
        0, 10.0f );
    EXPECT_NEAR( test_dev.get_value(), 5.0f, .001f );

    G_PAC_INFO()->emulation_on();
    }


TEST( state_s, is_active )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    state_s GS1( "GS1" );

    EXPECT_FALSE( GS1.is_active() );

    GS1.save_device( buff );
    EXPECT_STREQ( "GS1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( state_s, get_type_name )
    {
    state_s test_dev( "test_GS1" );
    EXPECT_STREQ( "Датчик положения", test_dev.get_type_name() );
    }

TEST( state_s_inverse, is_active )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    state_s_inverse GS1( "GS1" );

    EXPECT_TRUE( GS1.is_active() );

    GS1.save_device( buff );
    EXPECT_STREQ( "GS1={M=0, ST=0, P_DT=0},\n", buff );
    }


TEST( valve, evaluate )
    {
    valve::evaluate();

    valve_mini_flushing V1( "V1" );
    const auto DELAY_TIME = 1;
    G_PAC_INFO()->par[ PAC_info::P_V_OFF_DELAY_TIME ] = DELAY_TIME;

    EXPECT_EQ( valve::V_OFF, V1.get_state() );  //Should be closed.
    V1.on();
    V1.off();
    EXPECT_EQ( valve::V_ON, V1.get_state() );   //Should be opened.
    valve::evaluate();
    sleep_ms( DELAY_TIME + DELAY_TIME );
    valve::evaluate();
    EXPECT_EQ( valve::V_OFF, V1.get_state() );  //Should be closed.

    valve::clear_switching_off_queue();
    }

TEST( valve, is_closed )
    {
    valve V1( true, true, "V1", device::DEVICE_TYPE::DT_V,
        device::DEVICE_SUB_TYPE::DST_V_DO1 );

    EXPECT_TRUE( V1.is_closed() );
    }

TEST( valve, get_fb_state )
    {
    valve V1( true, true, "V1", device::DEVICE_TYPE::DT_V,
        device::DEVICE_SUB_TYPE::DST_V_DO1 );

    EXPECT_TRUE( V1.get_fb_state() );
    }


TEST( valve_DO1_DI1_off, valve_DO1_DI1_off )
    {
    valve_DO1_DI1_off V2( "V1" );
    EXPECT_STREQ( "Клапан", V2.get_type_name() );
    EXPECT_STREQ( "V1", V2.get_name() );
    }

TEST( valve_DO1_DI1_off, set_cmd )
    {
    valve_DO1_DI1_off V1( "V1" );

    EXPECT_EQ( V1.get_state(), 0 );

    V1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( V1.get_state(), 1 );

    G_PAC_INFO()->emulation_off();
    V1.init( 1, 1, 0, 0 );
    V1.DO_channels.char_write_values[ 0 ] = new u_char{ 0 };
    V1.DO_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.set_cmd( "P_FB", 0, 1 );

    V1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_ON );

    V1.set_cmd( "ST", 0, 0 );
    *V1.DI_channels.char_read_values[ 0 ] = 1; // Включаем обратную связь.
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    // Несуществующая команда - состояние не должно изменяться.
    V1.set_cmd( "ST", 0, 10 );
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_on();
    }


void check_fb( valve* V1 )
    {
    // Нет обратной связи для отключенного состояния.
    EXPECT_FALSE( V1->get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1->get_valve_state() );

    *V1->DI_channels.char_read_values[ 1 ] = 1;
    // Есть обратная связь для отключенного состояния.
    EXPECT_TRUE( V1->get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1->get_valve_state() );

    *V1->DO_channels.char_write_values[ 0 ] = 1;
    *V1->DI_channels.char_read_values[ 1 ] = 0;
    // Нет обратной связи для включенного состояния.
    EXPECT_FALSE( V1->get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1->get_valve_state() );

    *V1->DI_channels.char_read_values[ 0 ] = 1;
    // Есть обратная связь для включенного состояния.
    EXPECT_TRUE( V1->get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1->get_valve_state() );
    }


TEST( valve_DO1_DI2, valve_DO1_DI2 )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    valve_DO1_DI2 V1( "V1" );

    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, P_ON_TIME=0, "
        "P_FB=0},\n", buff );
    }

TEST( valve_DO1_DI2, get_fb_state )
    {
    valve_DO1_DI2 V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 1, 2 );

    check_fb( &V1 );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_DO2_DI2, valve_DO2_DI2 )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    valve_DO2_DI2 V1( "V1" );

    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, P_ON_TIME=0, "
        "P_FB=0},\n", buff );
    }

TEST( valve_DO2_DI2, get_fb_state )
    {
    valve_DO2_DI2 V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 2, 2 );

    // Нет управления и нет обратных связей.
    EXPECT_FALSE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    // Нет обратной связь для отключенного состояния.
    *V1.DO_channels.char_write_values[ 0 ] = 1;
    EXPECT_FALSE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    // Есть управление и обратная связь для отключенного состояния.
    *V1.DI_channels.char_read_values[ 0 ] = 1;
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    // Есть управление и обратная связь для включенного состояния.
    *V1.DO_channels.char_write_values[ 0 ] = 0;
    *V1.DI_channels.char_read_values[ 0 ] = 0;
    *V1.DO_channels.char_write_values[ 1 ] = 1;
    *V1.DI_channels.char_read_values[ 1 ] = 1;
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );

    // Нет обратная связь для включенного состояния.
    *V1.DI_channels.char_read_values[ 1 ] = 0;
    EXPECT_FALSE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_DO1, get_fb_state )
    {
    valve_DO1 V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 1 );

    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( V1.get_state(), 0 );
    // Нет обратных связей.
    EXPECT_FALSE( V1.get_off_fb_value() );
    EXPECT_FALSE( V1.get_on_fb_value() );

    *V1.DO_channels.char_write_values[ 0 ] = 1;
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( V1.get_state(), 1 );
    // Нет обратных связей.
    EXPECT_FALSE( V1.get_off_fb_value() );
    EXPECT_FALSE( V1.get_on_fb_value() );

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_DO1_DI1_off, get_fb_state )
    {
    valve_DO1_DI1_off V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 1, 1 );

    // Нет обратной связи для отключенного состояния.
    EXPECT_FALSE( V1.get_fb_state() );
    EXPECT_FALSE( V1.get_off_fb_value() );

    *V1.DI_channels.char_read_values[ 0 ] = 1;
    // Есть обратная связь для включенного состояния.
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_TRUE( V1.get_off_fb_value() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_DO1_DI1_off, direct_off )
    {
    valve_DO1_DI1_off V1( "V1" );
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 1, 1 );
    V1.set_cmd( "P_FB", 0, 1 );

    V1.direct_on();
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_ON );

    // Включение уже включенного клапана.
    V1.direct_on();
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_ON );

    V1.direct_off();
    *V1.DI_channels.char_read_values[ 0 ] = 1;
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    // Отключение уже отключенного клапана.
    V1.direct_off();
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_DO1_DI1_off, get_on_fb_value )
    {
    valve_DO1_DI1_off V1( "V1" );
    EXPECT_FALSE( V1.get_on_fb_value() );
    }

TEST( valve_DO1_DI1_off, get_off_fb_value )
    {
    valve_DO1_DI1_off V1( "V1" );
    EXPECT_TRUE( V1.get_off_fb_value() );
    }


TEST( valve_DO1_DI1_on, get_fb_state )
    {
    valve_DO1_DI1_on V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 1, 1 );

    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_FALSE( V1.get_on_fb_value() );

    *V1.DI_channels.char_read_values[ 0 ] = 1;
    // Нет обратной связь для включенного состояния.
    EXPECT_FALSE( V1.get_fb_state() );
    EXPECT_TRUE( V1.get_on_fb_value() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_DO1_DI1_on, direct_off )
    {
    valve_DO1_DI1_on V1( "V1" );
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 1, 1 );
    V1.set_cmd( "P_FB", 0, 1 );

    V1.direct_on();
    *V1.DI_channels.char_read_values[ 0 ] = 1;
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_ON );

    // Включение уже включенного клапана.
    V1.direct_on();
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_ON );

    V1.direct_off();
    *V1.DI_channels.char_read_values[ 0 ] = 0;
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    // Отключение уже отключенного клапана.
    V1.direct_off();
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_DO1_DI1_on, get_on_fb_value )
    {
    valve_DO1_DI1_on V1( "V1" );
    EXPECT_FALSE( V1.get_on_fb_value() );
    }

TEST( valve_DO1_DI1_on, get_off_fb_value )
    {
    valve_DO1_DI1_on V1( "V1" );
    EXPECT_FALSE ( V1.get_off_fb_value() );
    }


TEST( valve_mix_proof, valve_mix_proof )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    valve_mix_proof V1( "V1" );

    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, P_ON_TIME=0, "
        "P_FB=0},\n", buff );
    }

TEST( valve_mix_proof, get_fb_state )
    {
    valve_mix_proof V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 3, 2 );

    check_fb( &V1 );

    *V1.DO_channels.char_write_values[ 0 ] = 0;
    *V1.DO_channels.char_write_values[ 1 ] = 1;
    *V1.DI_channels.char_read_values[ 0 ] = 0;
    // Есть обратная связь для активного верхнего седла.
    EXPECT_TRUE( V1.get_fb_state() );

    *V1.DO_channels.char_write_values[ 1 ] = 0;
    *V1.DO_channels.char_write_values[ 2 ] = 1;
    // Есть обратная связь для активного нижнего седла.
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_mix_proof, direct_set_state )
    {
    valve_mix_proof V1( "V1" );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 3, 2 );

    V1.set_cmd( "P_FB", 0, 1 );
    V1.direct_set_state( valve::VALVE_STATE::V_UPPER_SEAT );
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_UPPER_SEAT );

    V1.direct_set_state( valve::VALVE_STATE::V_LOWER_SEAT );
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_LOWER_SEAT );

    V1.direct_set_state( valve::VALVE_STATE::V_OFF );
    *V1.DI_channels.char_read_values[ 1 ] = 1;
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_OFF );

    V1.direct_set_state( valve::VALVE_STATE::V_ON );
    *V1.DI_channels.char_read_values[ 0 ] = 1;
    *V1.DI_channels.char_read_values[ 1 ] = 0;
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE::V_ON );

    G_PAC_INFO()->emulation_on();
    }


TEST( analog_valve, save_device )
    {
    analog_valve vc1( "VC1" );
    const int BUFF_SIZE = 200;
    std::array <char, BUFF_SIZE> buff{};

    vc1.save_device( buff.data() );
    EXPECT_STREQ( "VC1={M=0, ST=0, V=0, E=0, M_EXP=1.0, S_DEV=0.2},\n",
        buff.data() );
    }

TEST( analog_valve, get_min_value )
    {
    const analog_valve VC1( "VC1" );
    EXPECT_EQ( VC1.get_min_value(), 0 );
    }

TEST( analog_valve, get_max_value )
    {
    const analog_valve VC1( "VC1" );
    EXPECT_EQ( VC1.get_max_value(), 100 );
    }

TEST( analog_valve, get_type_name )
    {
    analog_valve test_dev( "test_VC1" );
    EXPECT_STREQ( "Управляемый клапан", test_dev.get_type_name() );
    }

TEST( analog_valve, direct_on_and_direct_off )
    {
    analog_valve VC1( "VC1" );

    // Test direct_on sets value to 100
    VC1.direct_on();
    EXPECT_FLOAT_EQ( 100.0f, VC1.get_value() );
    EXPECT_EQ( 1, VC1.get_state() );

    // Test direct_off resets value to 0
    VC1.direct_off();
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    EXPECT_EQ( 0, VC1.get_state() );

    // Test direct_set_value
    VC1.direct_set_value( 50.0f );
    EXPECT_FLOAT_EQ( 50.0f, VC1.get_value() );

    // Test direct_off again after setting a value
    VC1.direct_off();
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    EXPECT_EQ( 0, VC1.get_state() );
    }

TEST( analog_valve, set_cmd_st_resets_value )
    {
    analog_valve VC1( "VC1" );

    // Simulate the scenario from the issue:
    // Open valve to 100% via set_cmd
    VC1.set_cmd( "V", 0, 100.0 );
    EXPECT_FLOAT_EQ( 100.0f, VC1.get_value() );

    // Close valve via ST=0 (as happens when moving to next step)
    VC1.set_cmd( "ST", 0, 0.0 );
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    EXPECT_EQ( 0, VC1.get_state() );
    }

class analog_valve_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            mngr.init( 1 );
            prev_mngr = io_manager::replace_instance( &mngr );
            mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
                1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
            mngr.init_node_AO( 0, 0, 555, 0 );
            auto node = mngr.get_node( 0 );
            node->state = io_manager::io_node::STATES::ST_OK;

            G_PAC_INFO()->emulation_off();
            VC1.init( 0, 0, 1, 0 );
            VC1.init_channel( io_device::IO_channels::CT_AO, 0, 0, 0 );
            };

        void TearDown() override
            {
            G_PAC_INFO()->emulation_on();
            io_manager::replace_instance( prev_mngr );
            };

        analog_valve VC1{ "VC1" };

    private:
        uni_io_manager mngr;
        io_manager* prev_mngr;
    };


TEST_F( analog_valve_test, io_modules_direct_on_off )
    {
    // По умолчанию 0%.
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    EXPECT_EQ( 0, VC1.get_state() );

    // direct_on -> 100%
    VC1.direct_on();
    VC1.evaluate_io();
    EXPECT_FLOAT_EQ( 100.0f, VC1.get_value() );
    EXPECT_EQ( 1, VC1.get_state() );

    // direct_off -> 0%
    VC1.direct_off();
    VC1.evaluate_io();
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    EXPECT_EQ( 0, VC1.get_state() );

    // direct_set_value -> произвольное значение
    VC1.direct_set_value( 37.5f );
    VC1.evaluate_io();
    EXPECT_FLOAT_EQ( 37.5f, VC1.get_value() );
    EXPECT_EQ( 1, VC1.get_state() );

    // Повторное выключение
    VC1.direct_off();
    VC1.evaluate_io();
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    EXPECT_EQ( 0, VC1.get_state() );
    }

TEST_F( analog_valve_test, io_modules_set_cmd_st_resets_value )
    {
    // Открыть клапан на 100% через set_cmd("V").
    VC1.set_cmd( "V", 0, 100.0 );
    VC1.evaluate_io();
    EXPECT_FLOAT_EQ( 100.0f, VC1.get_value() );
    EXPECT_EQ( 1, VC1.get_state() );

    // Закрыть клапан через ST=0 -> значение должно сброситься в 0%.
    VC1.set_cmd( "ST", 0, 0.0 );
    VC1.evaluate_io();
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    EXPECT_EQ( 0, VC1.get_state() );

    // Открыть клапан через ST=1 -> значение должно сброситься в 100%.
    VC1.set_cmd( "V", 0, 55.0 );
    VC1.set_cmd( "ST", 0, 1.0 );
    VC1.evaluate_io();
    EXPECT_EQ( 1, VC1.get_state() );
    EXPECT_FLOAT_EQ( 100.0f, VC1.get_value() );
    }


TEST( valve_bottom_mix_proof, valve_bottom_mix_proof )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    valve_bottom_mix_proof V1( "V1" );

    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, P_ON_TIME=0, "
        "P_FB=0},\n", buff );
    }

TEST( valve_bottom_mix_proof, get_fb_state )
    {
    valve_bottom_mix_proof V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 3, 2 );

    check_fb( &V1 );

    *V1.DO_channels.char_write_values[ 0 ] = 0;
    *V1.DO_channels.char_write_values[ 2 ] = 1;
    *V1.DI_channels.char_read_values[ 0 ] = 0;
    // Есть обратная связь для активного нижнего седла.
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_bottom_mix_proof, is_switching_off_finished )
    {
    valve_bottom_mix_proof V1( "V1" );
    const auto DELAY_TIME = 1;
    G_PAC_INFO()->par[ PAC_info::P_V_BOTTOM_OFF_DELAY_TIME ] = DELAY_TIME;

    EXPECT_EQ( true, V1.is_switching_off_finished() );

    V1.on();
    EXPECT_EQ( true, V1.is_switching_off_finished() );

    V1.off();
    V1.set_mini_closing_state( true );
    EXPECT_EQ( false, V1.is_switching_off_finished() );
    sleep_ms( DELAY_TIME + DELAY_TIME );
    EXPECT_EQ( true, V1.is_switching_off_finished() );

    valve::clear_switching_off_queue();
    }

TEST( valve_bottom_mix_proof, on )
    {
    valve_bottom_mix_proof V1( "V1" );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 3, 2 );

    V1.on();
    EXPECT_FALSE( V1.get_fb_state() );

    *V1.DI_channels.char_read_values[
        valve_bottom_mix_proof::CONSTANTS::DI_INDEX_OPEN ] = 1;
    // Есть обратная связь для активного нижнего седла.
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_mini_flushing, get_state )
    {
    valve_mini_flushing V1( "V1" );
    const auto DELAY_TIME = 1;
    G_PAC_INFO()->par[ PAC_info::P_V_OFF_DELAY_TIME ] = DELAY_TIME;

    V1.open_upper_seat();
    EXPECT_EQ( valve::V_OFF, V1.get_state() );  //No upper seat.

    V1.open_lower_seat();
    EXPECT_EQ( valve::V_LOWER_SEAT, V1.get_state() );

    V1.off();
    EXPECT_EQ( valve::V_OFF, V1.get_state() );
    }

TEST( valve_mini_flushing, get_fb_state )
    {
    valve_mini_flushing V1( "V1" );
    V1.init_and_alloc( 2, 2 );

    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_off();

    EXPECT_FALSE( V1.get_fb_state() );
    EXPECT_FALSE( V1.get_off_fb_value() );
    EXPECT_FALSE( V1.get_on_fb_value() );

    // Устанавливаем сигнал обратной связи.
    *V1.DI_channels.char_read_values[
        valve_mini_flushing::CONSTANTS_DI::DI_INDEX_CLOSE ] = 1;
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_TRUE( V1.get_off_fb_value() );
    EXPECT_FALSE( V1.get_on_fb_value() );

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_DO2_DI2_bistable, get_fb_state )
    {
    valve_DO2_DI2_bistable V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );
    EXPECT_FALSE( V1.get_on_fb_value() );
    EXPECT_TRUE( V1.get_off_fb_value() );

    G_PAC_INFO()->emulation_off();
    V1.init_and_alloc( 2, 2 );

    check_fb( &V1 );

    *V1.DO_channels.char_write_values[
        valve_DO2_DI2_bistable::CONSTANTS::DO_INDEX_OPEN ] = 1;
    *V1.DO_channels.char_write_values[
        valve_DO2_DI2_bistable::CONSTANTS::DO_INDEX_CLOSE ] = 0;
    *V1.DI_channels.char_read_values[
        valve_DO2_DI2_bistable::CONSTANTS::DI_INDEX_OPEN ] = 1;
    *V1.DI_channels.char_read_values[
        valve_DO2_DI2_bistable::CONSTANTS::DI_INDEX_CLOSE ] = 0;

    // Есть обратная связь для открытия.
    EXPECT_TRUE( V1.get_fb_state() );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    EXPECT_TRUE( V1.get_on_fb_value() );
    EXPECT_FALSE( V1.get_off_fb_value() );

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_iolink_shut_off_sorio, save_device )
    {
    valve_iolink_shut_off_sorio V1( "V1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ(
        "V1={M=0, ST=0, BLINK=0, CS=0, ERR=0, V=0.0, P_ON_TIME=0, P_FB=0},\n", buff );
    }

TEST( valve_iolink_shut_off_sorio, evaluate_io )
    {
    valve_iolink_shut_off_sorio V1( "V1" );
    V1.init( 0, 0, 1, 1 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( V1.AI_channels.int_read_values[ 0 ] );


    EXPECT_EQ( 0, V1.get_value() ); //Default value.


    const int_2 POS = 341;
    V1.AI_channels.int_read_values[ 0 ][ 1 ] = POS;
    std::swap( buff[ 0 ], buff[ 3 ] );  //Reverse byte order to get correct int.
    std::swap( buff[ 2 ], buff[ 1 ] );
    V1.evaluate_io();
    const int BUFF_SIZE = 100;
    char str_buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "V1={M=0, ST=0, BLINK=0, CS=0, ERR=0, V=34.1, P_ON_TIME=0, P_FB=0},\n",
        str_buff );

    V1.direct_set_value( 12.1f );
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "V1={M=0, ST=0, BLINK=0, CS=0, ERR=0, V=12.1, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    }

TEST( valve_iolink_shut_off_sorio, get_fb_state )
    {
    valve_iolink_shut_off_sorio V1( "V1" );
    V1.init( 0, 0, 1, 1 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };

    EXPECT_TRUE( V1.get_fb_state() ); //Default value.
    EXPECT_TRUE( V1.get_off_fb_value() );
    EXPECT_FALSE( V1.get_on_fb_value() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    G_PAC_INFO()->emulation_off();
    EXPECT_FALSE( V1.get_fb_state() );
    EXPECT_TRUE( V1.get_off_fb_value() );
    EXPECT_FALSE( V1.get_on_fb_value() );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_iolink_gea_tvis_a15_ds, save_device_ex )
    {
    valve_iolink_gea_tvis_a15_ds V1( "VGEA2" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ(
        "VGEA2={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, CS=0, SUP=0, ERR=0, "
        "V=0.0, P_ON_TIME=0, P_FB=0},\n", buff );
    }

TEST( valve_iolink_gea_tvis_a15_ds, get_valve_state )
    {
    valve_iolink_gea_tvis_a15_ds V1( "VGEA2" );
    valve::VALVE_STATE VS = V1.get_valve_state();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, VS );
    }

TEST( valve_iolink_gea_tvis_a15_ds, evaluate_io )
    {
    G_PAC_INFO()->emulation_off();

    valve_iolink_gea_tvis_a15_ds V1( "VGEA2" );
    V1.evaluate_io();

    V1.init( 0, 0, 1, 1 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto temp_in = &V1.AI_channels.int_read_values[ 0 ][ 0 ];
    auto pos = &V1.AI_channels.int_read_values[ 0 ][ 1 ];

    auto buff = reinterpret_cast<char*>( V1.AI_channels.int_read_values[ 0 ] );
    EXPECT_EQ( 0, V1.get_value() );

    // Последовательность бит соответствует перевёрнутой последовательности
    // полей структуры out_data_swapped, находящейся в классе
    // valve_iolink_gea_tvis_a15 в PAC/common/device/valve.h.
    *temp_in = static_cast<int_2> ( 0b1001'0000'0000'1001 );

    *pos = 165; // шток, который должен быть
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    const int BUFF_SIZE = 100;
    char str_buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA2={M=0, ST=10, FB_ON_ST=0, FB_OFF_ST=1, CS=0, SUP=0, ERR=0, "
        "V=16.5, P_ON_TIME=0, P_FB=0},\n",
        str_buff );

    V1.direct_on();
    memset( str_buff, '\0', sizeof( str_buff ) );
    *temp_in = 0b0001'0001'0000'0010;
    *pos = 564;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA2={M=0, ST=11, FB_ON_ST=1, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=56.4, P_ON_TIME=0, P_FB=0},\n",
        str_buff );

    V1.open_lower_seat();
    memset( str_buff, '\0', sizeof( str_buff ) );
    *temp_in = 0b0000'0010'0000'1001;
    *pos = 166;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    // Состояние должно быть VX_LOWER_SEAT_FB_OFF - 33.
    EXPECT_STREQ(
        "VGEA2={M=0, ST=33, FB_ON_ST=0, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=16.6, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_TRUE( V1.get_fb_state() );

    V1.open_upper_seat();
    memset( str_buff, '\0', sizeof( str_buff ) );
    *temp_in = static_cast<int_2> ( 0b1001'0100'0000'1001 );
    *pos = 81;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    // Состояние должно быть VX_UPPER_SEAT_FB_OFF - 32.
    EXPECT_STREQ(
        "VGEA2={M=0, ST=32, FB_ON_ST=0, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=8.1, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_TRUE( V1.get_fb_state() );

    V1.direct_off();
    memset( str_buff, '\0', sizeof( str_buff ) );
    *temp_in = 0b0000'0000'0000'1001;
    *pos = 165;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA2={M=0, ST=10, FB_ON_ST=0, FB_OFF_ST=1, CS=0, SUP=0, ERR=0, "
        "V=16.5, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_TRUE( V1.get_fb_state() );

    // На несуществующий номер команды клапан должен открыться.
    V1.direct_set_state( 777 );
    memset( str_buff, '\0', sizeof( str_buff ) );
    *temp_in = 0b0001'0001'0000'0010;
    *pos = 564;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA2={M=0, ST=11, FB_ON_ST=1, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=56.4, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_TRUE( V1.get_fb_state() );

    // Проверка на запрещенное (невозможное) состояние.
    V1.direct_on();
    memset( str_buff, '\0', sizeof( str_buff ) );
    *temp_in = 0b0001'0111'0000'1001;
    *pos = 5;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA2={M=0, ST=10, FB_ON_ST=0, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=0.5, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_FALSE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_iolink_gea_tvis_a15_ss, save_device_ex )
    {
    valve_iolink_gea_tvis_a15_ss V1( "VGEA1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ(
        "VGEA1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, CS=0, SUP=0, ERR=0, "
        "V=0.0, P_ON_TIME=0, P_FB=0},\n", buff );
    }

TEST( valve_iolink_gea_tvis_a15_ss, get_valve_state )
    {
    valve_iolink_gea_tvis_a15_ss V1( "VGEA1" );
    valve::VALVE_STATE VS = V1.get_valve_state();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, VS );
    }

TEST( valve_iolink_gea_tvis_a15_ss, evaluate_io )
    {
    G_PAC_INFO()->emulation_off();

    valve_iolink_gea_tvis_a15_ss V1( "VGEA1" );
    V1.init( 0, 0, 1, 1 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };

    auto buff = reinterpret_cast<char*>( V1.AI_channels.int_read_values[ 0 ] );
    EXPECT_EQ( 0, V1.get_value() );

    // Последовательность бит соответствует перевёрнутой последовательности
    // полей структуры out_data_swapped, находящейся в классе
    // valve_iolink_gea_tvis_a15 в PAC/common/device/valve.h.
    auto temp_in = static_cast<int_2>( 0b1001'0000'0000'1001 );
    V1.AI_channels.int_read_values[ 0 ][ 0 ] = temp_in;
    int_2 pos = 165; // шток, который должен быть
    V1.AI_channels.int_read_values[ 0 ][ 1 ] = pos;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    const int BUFF_SIZE = 100;
    char str_buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA1={M=0, ST=10, FB_ON_ST=0, FB_OFF_ST=1, CS=0, SUP=0, ERR=0, "
        "V=16.5, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_EQ( true, V1.get_fb_state() );

    V1.direct_set_state( valve::VALVE_STATE::V_ON );
    memset( str_buff, '\0', sizeof( str_buff ) );
    temp_in = 0b0001'0001'0000'0010;
    V1.AI_channels.int_read_values[ 0 ][ 0 ] = temp_in;
    pos = 564;
    V1.AI_channels.int_read_values[ 0 ][ 1 ] = pos;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA1={M=0, ST=11, FB_ON_ST=1, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=56.4, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_EQ( true, V1.get_fb_state() );

    V1.direct_set_state( valve::VALVE_STATE::V_OFF );
    memset( str_buff, '\0', sizeof( str_buff ) );
    temp_in = static_cast<int_2>( 0b1001'0000'0000'1001 );
    V1.AI_channels.int_read_values[ 0 ][ 0 ] = temp_in;
    pos = 165;
    V1.AI_channels.int_read_values[ 0 ][ 1 ] = pos;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA1={M=0, ST=10, FB_ON_ST=0, FB_OFF_ST=1, CS=0, SUP=0, ERR=0, "
        "V=16.5, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_EQ( true, V1.get_fb_state() );

    V1.direct_set_state( 777 );
    memset( str_buff, '\0', sizeof( str_buff ) );
    temp_in = 0b0001'0001'0000'0010;
    V1.AI_channels.int_read_values[ 0 ][ 0 ] = temp_in;
    pos = 564;
    V1.AI_channels.int_read_values[ 0 ][ 1 ] = pos;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA1={M=0, ST=11, FB_ON_ST=1, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=56.4, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_EQ( true, V1.get_fb_state() );

    V1.direct_set_state( valve::VALVE_STATE::V_ON );
    memset( str_buff, '\0', sizeof( str_buff ) );
    temp_in = 0b0001'0111'0000'1111;
    V1.AI_channels.int_read_values[ 0 ][ 0 ] = temp_in;
    pos = 1020;
    V1.AI_channels.int_read_values[ 0 ][ 1 ] = pos;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA1={M=0, ST=11, FB_ON_ST=0, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=102.0, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_EQ( false, V1.get_fb_state() );

    V1.evaluate_io();

    V1.direct_set_state( valve::VALVE_STATE::V_ON );
    memset( str_buff, '\0', sizeof( str_buff ) );
    temp_in = 0b0001'0001'0000'0010;
    V1.AI_channels.int_read_values[ 0 ][ 0 ] = temp_in;
    pos = 1020;
    V1.AI_channels.int_read_values[ 0 ][ 1 ] = pos;
    std::swap( buff[ 2 ], buff[ 3 ] );
    V1.evaluate_io();
    V1.save_device( str_buff );
    EXPECT_STREQ(
        "VGEA1={M=0, ST=11, FB_ON_ST=1, FB_OFF_ST=0, CS=1, SUP=0, ERR=0, "
        "V=102.0, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    EXPECT_EQ( true, V1.get_fb_state() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_iol_terminal, direct_set_state )
    {
    valve_iol_terminal V1( "V1", device::DEVICE_SUB_TYPE::V_IOLINK_VTUG_DO1 );
    V1.init( 0, 0, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.set_rt_par( 1, 1 );

    V1.direct_set_state( valve::V_ON );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );

    V1.direct_set_state( valve::V_OFF );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.direct_set_state( valve::V_LOWER_SEAT + 10 );  //Неправильное состояние.
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }


TEST( valve_iol_terminal_DO1, on )
    {
    valve_iol_terminal_DO1 V1( "V1" );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.init( 0, 0, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 5, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 1, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }


TEST( valve_iol_terminal_DO1_DI1_off, get_fb_state )
    {
    valve_iol_terminal_DO1_DI1_off V1( "V1" );

    V1.init( 0, 1, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 2 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_OFF_ST=1, P_ON_TIME=0, P_FB=0},\n", buff );

    EXPECT_EQ( false, V1.get_fb_state() );

    const auto WAIT_TIME = 10;
    V1.set_par( 1 /*valve::CONSTANTS::P_ON_TIME*/, 0, WAIT_TIME );
    EXPECT_EQ( true, V1.get_fb_state() );

    sleep_ms( 2 * WAIT_TIME );
    EXPECT_EQ( false, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;
    EXPECT_EQ( true, V1.get_fb_state() );
    }

TEST( valve_iol_terminal_DO1_DI1_off, get_on_fb_value )
    {
    valve_iol_terminal_DO1_DI1_off V1( "V1" );
    EXPECT_EQ( false, V1.get_on_fb_value() );
    }


TEST( valve_iol_terminal_DO1_DI1_on, get_fb_state )
    {
    valve_iol_terminal_DO1_DI1_on V1( "V1" );

    V1.init( 0, 1, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 2 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=0, P_ON_TIME=0, P_FB=0},\n", buff );

    EXPECT_EQ( true, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;
    const auto WAIT_TIME = 10;
    V1.set_par( 1 /*valve::CONSTANTS::P_ON_TIME*/, 0, WAIT_TIME );
    EXPECT_EQ( true, V1.get_fb_state() );
    sleep_ms( 2 * WAIT_TIME );
    EXPECT_EQ( false, V1.get_fb_state() );

    V1.on();
    EXPECT_EQ( true, V1.get_fb_state() );
    }

TEST( valve_iol_terminal_DO1_DI1_on, get_off_fb_value )
    {
    valve_iol_terminal_DO1_DI1_on V1( "V1" );
    EXPECT_EQ( false, V1.get_off_fb_value() );
    }


TEST( valve_iol_terminal_mixproof_DO3, on )
    {
    valve_iol_terminal_mixproof_DO3 V1( "V1" );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.init( 0, 0, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 5, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 1, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 2, 2 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 3, 3 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }

TEST( valve_iol_terminal_mixproof_DO3, off )
    {
    valve_iol_terminal_mixproof_DO3 V1( "V1" );
    V1.init( 0, 0, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 1 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::UPPER_SEAT ), 2 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::LOWER_SEAT ), 3 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0},\n", buff );

    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    V1.instant_off();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );
    V1.open_upper_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_UPPER_SEAT, V1.get_valve_state() );
    V1.instant_off();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );
    V1.open_lower_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_LOWER_SEAT, V1.get_valve_state() );
    V1.instant_off();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.open_upper_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_UPPER_SEAT, V1.get_valve_state() );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    V1.open_lower_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_LOWER_SEAT, V1.get_valve_state() );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }

TEST( valve_iol_terminal_mixproof_DO3, direct_set_state )
    {
    valve_iol_terminal_mixproof_DO3 V1( "V1" );
    V1.init( 0, 0, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.set_rt_par( 1, 1 );
    V1.set_rt_par( 2, 2 );
    V1.set_rt_par( 3, 3 );

    V1.direct_set_state( valve::V_ON );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );

    V1.direct_set_state( valve::V_UPPER_SEAT );
    EXPECT_EQ( valve::VALVE_STATE::V_UPPER_SEAT, V1.get_valve_state() );

    V1.direct_set_state( valve::V_LOWER_SEAT );
    EXPECT_EQ( valve::VALVE_STATE::V_LOWER_SEAT, V1.get_valve_state() );

    V1.direct_set_state( valve::V_OFF );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.direct_set_state( valve::V_LOWER_SEAT + 10 );  //Неправильное состояние.
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }

TEST( valve_iol_terminal_mixproof_DO3_DI2, get_fb_state )
    {
    valve_iol_terminal_mixproof_DO3_DI2 V1( "V1" );

    V1.init( 0, 2, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.DI_channels.char_read_values[ 1 ] = new u_char{ 0 };

    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 1 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::UPPER_SEAT ), 1 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::LOWER_SEAT ), 1 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ(
        "V1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, P_ON_TIME=0, P_FB=0},\n",
        buff );

    const auto WAIT_TIME = 10;
    V1.set_par( 1 /*valve::CONSTANTS::P_ON_TIME*/, 0, WAIT_TIME );
    //Не прошёл заданый интервал - должно вернуться "истина".
    EXPECT_EQ( true, V1.get_fb_state() );

    sleep_ms( 2 * WAIT_TIME );
    //Прошёл заданый интервал - должно вернуться "ложь".
    EXPECT_EQ( false, V1.get_fb_state() );

    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.
    V1.direct_on();
    EXPECT_EQ( 0b1, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Бит включения 1.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.
    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;
    //Есть обратная связь - должно вернуться "истина".
    EXPECT_EQ( true, V1.get_fb_state() );

    V1.direct_off();
    *( V1.DI_channels.char_read_values[ 0 ] ) = 0;
    *( V1.DI_channels.char_read_values[ 1 ] ) = 1;
    //Есть обратная связь - должно вернуться "истина".
    EXPECT_EQ( true, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 0;
    *( V1.DI_channels.char_read_values[ 1 ] ) = 0;
    V1.open_upper_seat();
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b1,
        V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Бит включения верхнего седла 1.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.

    EXPECT_EQ( true,
        V1.get_fb_state() ); //Открыто верхнее седло - должно вернуться "истина".
    V1.open_lower_seat();
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b1,
        V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Бит включения нижнего седла 1.
    EXPECT_EQ( true,
        V1.get_fb_state() ); //Открыто нижнее седло - должно вернуться "истина".

    V1.direct_off();
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.
    }


TEST( valve_iol_terminal_DO1_DI2, get_fb_state )
    {
    valve_iol_terminal_DO1_DI2 V1( "V1" );

    V1.init( 0, 2, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.DI_channels.char_read_values[ 1 ] = new u_char{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 2 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ(
        "V1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, P_ON_TIME=0, P_FB=0},\n",
        buff );

    EXPECT_EQ( false, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 1 ] ) = 1;//ОС для отключенного состояния.
    EXPECT_EQ( true, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;//ОС для включенного состояния.
    EXPECT_EQ( false, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 1 ] ) = 0;//ОС для отключенного состояния.
    V1.direct_on();
    EXPECT_EQ( true, V1.get_fb_state() );
    }

TEST( valve_DO2, valve_DO2 )
    {
    valve_DO2 V2( "V2" );
    EXPECT_STREQ( "Клапан", V2.get_type_name() );
    EXPECT_STREQ( "V2", V2.get_name() );
    }

TEST( valve_DO2, get_valve_state )
    {
    valve_DO2 V2( "V2" );
    V2.direct_on();
    EXPECT_EQ( 1, V2.get_valve_state() );
    }

TEST( valve_DO2, get_fb_state )
    {
    valve_DO2 V2( "V2" );
    EXPECT_EQ( true, V2.get_fb_state() );
    }


TEST( valve_AS_DO1_DI2, direct_set_state )
    {
    valve_AS_DO1_DI2 v1( "V1" );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, v1.get_state() );
    v1.direct_set_state( 1 );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, v1.get_state() );
    }

TEST( valve_AS_DO1_DI2, get_fb_state )
    {
    valve_AS_DO1_DI2 v1( "V1" );

    G_PAC_INFO()->emulation_off();

    // Задаем AO-область данных устройства.
    v1.init( 0, 0, 1, 1 );
    const auto AO_SIZE = 5;
    v1.AO_channels.int_read_values[ 0 ] = new int_2[ AO_SIZE ]{ 0 };
    v1.AO_channels.int_write_values[ 0 ] = new int_2[ AO_SIZE ]{ 0 };

    EXPECT_FALSE( v1.get_fb_state() );
    EXPECT_EQ( v1.get_valve_state(), valve::VALVE_STATE::V_OFF );
    EXPECT_FALSE( v1.get_off_fb_value() );
    EXPECT_FALSE( v1.get_on_fb_value() );

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_iolink_mix_proof, valve_iolink_mix_proof )
    {
    valve_iolink_mix_proof V1( "V1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=0, FB_OFF_ST=1, BLINK=0, "
        "CS=0, ERR=0, V=0.0, P_ON_TIME=0, P_FB=0},\n", buff );
    }

TEST( valve_iolink_mix_proof, evaluate_io )
    {
    valve_iolink_mix_proof V1( "V1" );
    G_PAC_INFO()->emulation_off();

    V1.evaluate_io();
    // Отключена обратная связь - нет ошибки.
    EXPECT_EQ( valve::VALVE_STATE_EX::VX_OFF_FB_OFF, V1.get_state() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_iolink_mix_proof, get_state )
    {
    valve_iolink_mix_proof V1( "V1" );
    G_PAC_INFO()->emulation_off();

    V1.set_cmd( "P_FB", 0, 1 );       // Включаем проверку обратных связей.
    V1.set_cmd( "P_ON_TIME", 0, 100 );// Задаем время проверки обратных связей.
    V1.direct_on();
    V1.direct_off();
    // Нет обратной связи, но не прошло время проверки, поэтому нет ошибки.
    EXPECT_EQ( valve::VALVE_STATE_EX::VX_OFF_FB_OK, V1.get_state() );

    // Нет обратной связи, но не прошло время проверки, но есть ошибка клапана,
    // поэтому есть ошибки.
    V1.in_info.err = 1;
    EXPECT_EQ( -( io_link_valve::ERROR_CODE_OFFSET + V1.in_info.err ),
        V1.get_state() );

    // Нет обратной связи, но прошло время проверки и есть ошибка клапана,
    // поэтому есть ошибка.
    V1.in_info.err = 0;
    DeltaMilliSecSubHooker::set_millisec( 101UL );
    EXPECT_EQ( valve::VALVE_STATE_EX::VX_OFF_FB_ERR, V1.get_state() );
    DeltaMilliSecSubHooker::set_default_time();


    G_PAC_INFO()->emulation_on();
    }

TEST( valve_iolink_mix_proof, seat_switching_timing )
    {
    valve_iolink_mix_proof V1( "V1" );
    G_PAC_INFO()->emulation_off();

    V1.set_cmd( "P_FB", 0, 1 );       // Включаем проверку обратных связей.
    V1.set_cmd( "P_ON_TIME", 0, 100 );// Задаем время проверки обратных связей.

    // Test that seat switching doesn't immediately cause VX_OFF_FB_ERR.
    // Test upper seat switching timing.
    V1.open_upper_seat();
    auto state = V1.get_state();

    // Should not return VX_OFF_FB_ERR immediately after switching.
    // This validates that the timing grace period is working correctly.
    EXPECT_NE( valve::VALVE_STATE_EX::VX_OFF_FB_ERR, state )
        << "Upper seat should not return error immediately after switching"
        " due to timing grace period";

    DeltaMilliSecSubHooker::set_millisec( 101UL );
    // Should return VX_OFF_FB_ERR after timeout.
    state = V1.get_state();
    EXPECT_EQ( valve::VALVE_STATE_EX::VX_OFF_FB_ERR, state ) <<
        "Upper seat should return error after a timeout after switching";
    DeltaMilliSecSubHooker::set_default_time();

    // Test lower seat switching timing.
    V1.open_lower_seat();
    state = V1.get_state();

    // The key fix: Should not return VX_OFF_FB_ERR immediately after switching.
    // This validates that the timing grace period is working correctly.
    EXPECT_NE( valve::VALVE_STATE_EX::VX_OFF_FB_ERR, state )
        << "Lower seat should not return error immediately after switching"
        " due to timing grace period";

    DeltaMilliSecSubHooker::set_millisec( 101UL );
    // Should return VX_OFF_FB_ERR after timeout.
    state = V1.get_state();
    EXPECT_EQ( valve::VALVE_STATE_EX::VX_OFF_FB_ERR, state ) <<
        "Lower seat should return error after a timeout after switching";
    DeltaMilliSecSubHooker::set_default_time();

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, valve_iolink_mix_proof_get_state )
    {
    valve_iolink_mix_proof V1( "V1" );
    G_PAC_INFO()->emulation_off();
    init_channels( V1 );

    // With feedback disabled (default P_FB=0), module errors should be ignored
    // and valve should report feedback disabled state instead of module error.
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE_EX::VX_OFF_FB_OFF );

    V1.in_info.usl = true;
    V1.in_info.lsp = false;
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE_EX::VX_UPPER_SEAT_FB_OFF );

    V1.in_info.usl = false;
    V1.in_info.lsp = true;
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE_EX::VX_LOWER_SEAT_FB_OFF );


    // Enable feedback to test that module errors are still reported when feedback is enabled.
    V1.set_cmd( "P_FB", 0, 1 );
    EXPECT_EQ( V1.get_state(), -io_device::IOLINKSTATE::NOTCONNECTED );

    G_PAC_INFO()->emulation_on();
    }

class valve_iolink_mix_proof_testable : public valve_iolink_mix_proof
    {
    public:
        using valve_iolink_mix_proof::valve_iolink_mix_proof;

        void set_err( uint16_t err )
            {
            in_info.err = err;
            }
    };

TEST( valve_iolink_mix_proof, get_error_description )
    {
    struct ErrorDescCase
        {
        int16_t err;
        const char* expected;
        };

    valve_iolink_mix_proof_testable v( "V1" );

    const std::vector<ErrorDescCase> cases =
        {
        {0, "нет ошибок"},

        {-116, "не обнаружен магнитный индикатор на штоке клапана (#16)"},
        {-117, "конфигурация не соответствует требованиям автоматической "
            "настройки (#17)"},
        {-118, "ошибка в пневматических соединениях - проверьте подключение "
            "трубок или соленоидов (#18)"},
        {-119, "нет сигнала от датчика верхнего седла (#19)"},
        {-120, "клапан не достиг заданного положения в установленное время "
            "(#20)"},
        {-121, "обнаружен самопроизвольный ход штока (#21)"},
        {-122, "не подключен датчик верхнего седла (#22)"},
        {-123, "не обнаружен соленоидный клапан 1 (#23)"},
        {-124, "не обнаружен соленоидный клапан 2 (#24)"},
        {-125, "не обнаружен соленоидный клапан 3 (#25)"},
        {-126, "активировано несколько входных сигналов соленоидных клапанов "
            "(#26)"},
        {-127, "обнаружено короткое замыкание на цифровых выходах (#27)"},
        {-128, "процесс настройки был прерван (#28)"},
        {-129, "постоянное срабатывание кнопки - проверьте кнопки или "
            "замените плату управления (#29)"},
        {-130, "потеряна связь с системой управления (#30)"},
        {-131, "сработала аварийная остановка - превышен допустимый "
            "ход штока (#31)"},
        };

    for ( const auto& c : cases )
        {
        v.set_state( c.err );
        EXPECT_STREQ( v.get_error_description(), c.expected ) << "err=" << c.err;
        }

    v.set_cmd( "ST", 0, -io_device::IOLINKSTATE::NOTCONNECTED );
    auto res = v.get_error_description();
    EXPECT_STREQ( "IOL-устройство не подключено", res );

    v.set_cmd( "ST", 0, -io_device::IOLINKSTATE::DEVICEERROR );
    res = v.get_error_description();
    EXPECT_STREQ( "ошибка IOL-устройства", res );
    }

TEST( valve_iolink_mix_proof, get_state_with_feedback_disabled_and_al_error )
    {
    valve_iolink_mix_proof_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();

    // Set feedback disabled (P_FB = 0, which is FB_IS_AND_OFF).
    V1.set_cmd( "P_FB", 0, 0 );

    // Simulate AL IO-Link error.
    V1.set_err( 16 );  // Some internal error.

    // When feedback is disabled and there's an error,
    // the valve should NOT report error state.
    auto state = V1.get_state();
    EXPECT_GE( state, 0 ) << "Valve should not report error state when feedback is disabled";

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_iolink_mix_proof, get_state_with_feedback_enabled_and_al_error )
    {
    valve_iolink_mix_proof_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();

    // Set feedback enabled (P_FB = 1, which is FB_IS_AND_ON).
    V1.set_cmd( "P_FB", 0, 1 );

    // Simulate AL IO-Link error.
    V1.set_err( 16 );  // Some internal error.

    // When feedback is enabled and there's an AL error,
    // the valve SHOULD report error state (normal behavior).
    int state = V1.get_state();

    // The valve should be in error state when feedback is enabled.
    EXPECT_LT( state, 0 ) << "Valve should report error state when feedback is enabled";
    EXPECT_EQ( state, -116 ) << "Expected specific error code for AL error 16";

    G_PAC_INFO()->emulation_on();
    }


class valve_iolink_shut_off_thinktop_testable : public valve_iolink_shut_off_thinktop
    {
    public:
        using valve_iolink_shut_off_thinktop::valve_iolink_shut_off_thinktop;

        void set_err( uint16_t err )
            {
            in_info.err = err;
            }
    };


TEST( valve_iolink_shut_off_thinktop, valve_iolink_shut_off_thinktop )
    {
    valve_iolink_shut_off_thinktop V1( "V1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ(
        "V1={M=0, ST=0, BLINK=0, CS=0, ERR=0, V=0.0, P_ON_TIME=0, P_FB=0},\n",
        buff );
    }

TEST( valve_iolink_shut_off_thinktop, get_state )
    {
    valve_iolink_shut_off_thinktop V1( "V1" );

    G_PAC_INFO()->emulation_off();
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE_EX::VX_OFF_FB_OFF );
    EXPECT_EQ( V1.get_off_fb_value(), 0 );
    EXPECT_EQ( V1.get_on_fb_value(), 0 );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_iolink_shut_off_thinktop, get_state_with_feedback_disabled_and_al_error )
    {
    valve_iolink_shut_off_thinktop_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();

    // Set feedback disabled (P_FB = 0, which is FB_IS_AND_OFF).
    V1.set_cmd( "P_FB", 0, 0 );

    // Simulate AL IO-Link error.
    V1.set_err( 16 );  // Some internal error.

    // When feedback is disabled and there's an error,
    // the valve should NOT report error state.
    auto state = V1.get_state();
    EXPECT_GE( state, 0 ) << "Valve should not report error state when feedback is disabled.";

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_iolink_shut_off_thinktop, get_state_with_feedback_enabled_and_al_error )
    {
    valve_iolink_shut_off_thinktop_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();

    // Set feedback enabled (P_FB = 1, which is FB_IS_AND_ON).
    V1.set_cmd( "P_FB", 0, 1 );

    // Simulate AL IO-Link error.
    V1.set_err( 16 );  // Some internal error.

    // When feedback is enabled and there's an AL error,
    // the valve SHOULD report error state (normal behavior).
    int state = V1.get_state();

    // The valve should be in error state when feedback is enabled.
    EXPECT_LT( state, 0 ) << "Valve should report error state when feedback is enabled";
    EXPECT_EQ( state, -116 ) << "Expected specific error code for AL error 16";

    G_PAC_INFO()->emulation_on();
    }


TEST( valve_iolink_shut_off_thinktop, get_fb_state )
    {
    valve_iolink_shut_off_thinktop_testable V1( "V1" );
    EXPECT_TRUE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_off();
    EXPECT_FALSE( V1.get_fb_state() );

    G_PAC_INFO()->emulation_on();
    }


TEST_F( iolink_dev_test, valve_iolink_mix_proof_get_state_with_feedback_disabled_and_module_error )
    {
    valve_iolink_mix_proof_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();
    init_channels( V1 );

    // Set feedback disabled (P_FB = 0, which is FB_IS_AND_OFF).
    V1.set_cmd( "P_FB", 0, 0 );
    V1.evaluate_io();

    // When feedback is disabled and there's a module error,
    // the valve should NOT report error state.
    int state = V1.get_state();

    // The valve should not be in error state when feedback is disabled.
    EXPECT_GE( state, 0 ) << "Valve should not report error state when feedback is disabled";
    EXPECT_EQ( state, valve::VALVE_STATE_EX::VX_OFF_FB_OFF ) << "Expected feedback disabled state";

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, valve_iolink_mix_proof_get_state_with_feedback_enabled_and_module_error )
    {
    valve_iolink_mix_proof_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();
    init_channels( V1 );

    // Set feedback enabled (P_FB = 1, which is FB_IS_AND_ON).
    V1.set_cmd( "P_FB", 0, 1 );
    V1.evaluate_io();

    // When feedback is enabled and there's a module error,
    // the valve SHOULD report error state (normal behavior).
    int state = V1.get_state();

    // The valve should be in error state when feedback is enabled.
    EXPECT_LT( state, 0 ) << "Valve should report error state when feedback is enabled";
    EXPECT_EQ( state, -io_device::IOLINKSTATE::NOTCONNECTED ) << "Expected module error code";

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, valve_iolink_shut_off_thinktop_get_state_with_feedback_disabled_and_module_error )
    {
    valve_iolink_shut_off_thinktop_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();
    init_channels( V1 );

    // Set feedback disabled (P_FB = 0, which is FB_IS_AND_OFF).
    V1.set_cmd( "P_FB", 0, 0 );
    V1.evaluate_io();

    // When feedback is disabled and there's a module error,
    // the valve should NOT report error state.
    int state = V1.get_state();

    // The valve should not be in error state when feedback is disabled.
    EXPECT_GE( state, 0 ) << "Valve should not report error state when feedback is disabled";
    EXPECT_EQ( state, valve::VALVE_STATE_EX::VX_OFF_FB_OFF ) << "Expected feedback disabled state";

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, valve_iolink_shut_off_thinktop_get_state_with_feedback_enabled_and_module_error )
    {
    valve_iolink_shut_off_thinktop_testable V1( "V1" );
    G_PAC_INFO()->emulation_off();
    init_channels( V1 );

    // Set feedback enabled (P_FB = 1, which is FB_IS_AND_ON).
    V1.set_cmd( "P_FB", 0, 1 );
    V1.evaluate_io();

    // When feedback is enabled and there's a module error,
    // the valve SHOULD report error state (normal behavior).
    int state = V1.get_state();

    // The valve should be in error state when feedback is enabled.
    EXPECT_LT( state, 0 ) << "Valve should report error state when feedback is enabled";
    EXPECT_EQ( state, -io_device::IOLINKSTATE::NOTCONNECTED ) << "Expected module error code";

    G_PAC_INFO()->emulation_on();
    }


TEST( analog_valve_iolink, analog_valve_iolink )
    {
    analog_valve_iolink V1( "V1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff );
    EXPECT_STREQ(
        "V1={M=0, ST=0, V=0, NAMUR_ST=0, OPENED=0, CLOSED=1, "
        "BLINK=0, P_FB=0},\n", buff );
    }


TEST( level_s, is_active )
    {
    level_s LS1( "LS1", device::DST_LS_MAX );
    EXPECT_EQ( true, LS1.is_active() );

    level_s LS2( "LS2", device::DST_LS_MIN );
    EXPECT_EQ( false, LS2.is_active() );

    level_s LS3( "LS3", device::DST_NONE );     //Unknown sub type.
    EXPECT_EQ( false, LS3.is_active() );
    }

TEST( level_s, get_type_name )
    {
    level_s test_dev( "test_LS1", device::DEVICE_SUB_TYPE::DST_LS_VIRT );
    EXPECT_STREQ( "Уровень", test_dev.get_type_name() );
    }


class LevelSIOLinkTest : public ::testing::Test
    {
    protected:
        std::unique_ptr<level_s_iolink> device =
            std::make_unique<level_s_iolink>(
                "TestDevice", device::LS_IOLINK_MAX );
    };

TEST_F( LevelSIOLinkTest, SetArticle_ValidArticles )
    {
    struct TestCase
        {
        const char* article;
        level_s_iolink::ARTICLE expected;
        };

    const std::array<TestCase, 11> testCases =
        { {
        {"IFM.LMT100", level_s_iolink::ARTICLE::IFM_LMT100},
        {"IFM.LMT102", level_s_iolink::ARTICLE::IFM_LMT102},
        {"IFM.LMT104", level_s_iolink::ARTICLE::IFM_LMT104},
        {"IFM.LMT105", level_s_iolink::ARTICLE::IFM_LMT105},
        {"IFM.LMT121", level_s_iolink::ARTICLE::IFM_LMT121},
        {"IFM.LMT202", level_s_iolink::ARTICLE::IFM_LMT202},
        {"E&H.FTL33-GR7N2ABW5J", level_s_iolink::ARTICLE::EH_FTL33},
        {"E&H.FTW33-GR7NWVJ", level_s_iolink::ARTICLE::EH_FTW33},
        {"E&H.FTW33-GR7NW5J", level_s_iolink::ARTICLE::EH_FTW33},
        {"E&H.FTW33-XXXXX", level_s_iolink::ARTICLE::EH_FTW33},

        {"E&H.FTW00", level_s_iolink::ARTICLE::DEFAULT}
        } };

    for ( const auto& testCase : testCases )
        {
        device->set_article( testCase.article );
        EXPECT_EQ( device->get_article_n(), testCase.expected )
            << "Failed for article: " << testCase.article;
        }
    }

TEST_F( LevelSIOLinkTest, SetArticle_UnknownArticle )
    {
    const char* unknownArticle = "UNKNOWN.ARTICLE";
    device->set_article( unknownArticle );

    EXPECT_EQ( device->get_article_n(), level_s_iolink::ARTICLE::DEFAULT )
        << "Failed for unknown article: " << unknownArticle;
    }

TEST_F( LevelSIOLinkTest, SetArticle_EmptyArticle )
    {
    const char* emptyArticle = "";
    device->set_article( emptyArticle );

    EXPECT_EQ( device->get_article_n(), level_s_iolink::ARTICLE::DEFAULT )
        << "Failed for empty article.";
    }

TEST_F( LevelSIOLinkTest, get_state )
    {
    device->set_article( "IFM.LMT100" );

    EXPECT_EQ( device->get_state(), 1 );

    G_PAC_INFO()->emulation_off();
    EXPECT_EQ( device->get_state(), 1 );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( LevelSIOLinkTest, is_active )
    {
    device->set_article( "IFM.LMT100" );

    EXPECT_TRUE( device->is_active() );

    G_PAC_INFO()->emulation_off();
    EXPECT_TRUE( device->is_active() );
    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, level_s_iolink_evaluate_io )
    {
    level_s_iolink test_dev_max( "TestDevice", device::LS_IOLINK_MAX );
    test_dev_max.set_article( "IFM.LMT100" );
    EXPECT_TRUE( test_dev_max.is_active() );

    test_dev_max.evaluate_io(); // Correct eval without I/O data.
    EXPECT_TRUE( test_dev_max.is_active() );

    G_PAC_INFO()->emulation_off();

    init_channels( test_dev_max );
    test_dev_max.evaluate_io();
    EXPECT_TRUE( test_dev_max.is_active() );  // Due to IO-Link module error.

    set_iol_state_to_OK( test_dev_max );
    test_dev_max.evaluate_io(); // Correct eval with I/O data.
    EXPECT_FALSE( test_dev_max.is_active() ); // Read 0 from I/O data.

    const auto CHECK_TIME = 100.f;
    test_dev_max.set_par( level_s_iolink::CONSTANTS::P_DT, 0, CHECK_TIME );
    test_dev_max.evaluate_io();
    EXPECT_FALSE( test_dev_max.is_active() ); // Read 0 from I/O data.

    DeltaMilliSecSubHooker::set_millisec(
        static_cast<unsigned long>( CHECK_TIME + 1.f ) );
    *test_dev_max.AI_channels.int_read_values[ 0 ] = 0b0000'0111'0000'0000;
    test_dev_max.evaluate_io();
    EXPECT_TRUE( test_dev_max.is_active() ); // Read 1 from I/O data.
    auto value = test_dev_max.get_value();
    const auto EXPECTED_VALUE = 1.f;
    EXPECT_EQ( value, EXPECTED_VALUE );

    *test_dev_max.AI_channels.int_read_values[ 0 ] = 0b0000'0001'1100'0000;
    test_dev_max.set_article( "E&H.FTW33-GR7NW5J" );
    test_dev_max.evaluate_io();
    EXPECT_TRUE( test_dev_max.is_active() ); // Read 1 from I/O data.
    value = test_dev_max.get_value();
    EXPECT_EQ( value, 0.1f * EXPECTED_VALUE );

    DeltaMilliSecSubHooker::set_default_time();


    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, level_s_iolink_get_error_description )
    {
    level_s_iolink test_dev_max( "TestDevice", device::LS_IOLINK_MAX );
    test_dev_err( test_dev_max, test_dev_max, 0 );
    }

TEST_F( iolink_dev_test, level_s_iolink_get_value )
    {
    level_s_iolink test_dev_max( "TestDevice", device::LS_IOLINK_MAX );

    EXPECT_EQ( test_dev_max.get_value(), .0f );
    G_PAC_INFO()->emulation_off();

    EXPECT_EQ( test_dev_max.get_value(), .0f );

    init_channels( test_dev_max );
    const auto VALUE = 10.f;
    test_dev_max.set_par( level_s_iolink::CONSTANTS::P_ERR, 0, VALUE );
    test_dev_max.evaluate_io();
    EXPECT_EQ( test_dev_max.get_value(), VALUE );

    G_PAC_INFO()->emulation_on();
    }


TEST_F( iolink_dev_test, valve_iolink_shut_off_thinktop_get_error_description )
    {
    valve_iolink_shut_off_thinktop V1( "V1" );
    EXPECT_STREQ( V1.get_error_description(), "нет ошибок" );

    G_PAC_INFO()->emulation_off();
    init_channels( V1 );
    V1.evaluate_io();

    // With feedback disabled (default P_FB=0), module errors should be ignored
    EXPECT_EQ( V1.get_state(), valve::VALVE_STATE_EX::VX_OFF_FB_OFF );
    EXPECT_STREQ( V1.get_error_description(), "нет ошибок" );

    // Enable feedback to test that module errors are reported when feedback is enabled
    V1.set_cmd( "P_FB", 0, 1 );
    V1.evaluate_io();
    EXPECT_EQ( V1.get_state(), -io_device::IOLINKSTATE::NOTCONNECTED );
    EXPECT_STREQ( V1.get_error_description(), "IOL-устройство не подключено" );

    G_PAC_INFO()->emulation_on();
    }


TEST( i_motor, i_motor )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    M1.save_device( buff );
    EXPECT_STREQ( "M1={M=0, ST=0, V=0},\n", buff );
    }

TEST( i_motor, reverse )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    M1.reverse();
    EXPECT_EQ( M1.get_state(), 2 );
    }

TEST( i_motor, get_linear_speed )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    EXPECT_EQ( M1.get_linear_speed(), 0.f );
    }

TEST( i_motor, get_amperage )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    EXPECT_EQ( M1.get_amperage(), 0.f );
    }

TEST( motor, direct_off )
    {
    motor M1( "M1", device::DST_M_FREQ );
    M1.init_and_alloc( 1, 1 );

    EXPECT_EQ( M1.get_state(), 0 );

    G_PAC_INFO()->emulation_off();

    M1.direct_on();
    *M1.DI_channels.char_read_values[ 0 ] = 1;
    EXPECT_EQ( M1.get_state(), 1 );

    M1.direct_off();
    *M1.DI_channels.char_read_values[ 0 ] = 0;
    EXPECT_EQ( M1.get_state(), 0 );

    G_PAC_INFO()->emulation_on();
    }

TEST( motor, direct_set_state )
    {
    motor M1( "M1", device::DST_M_FREQ );
    M1.set_state( 1 );
    EXPECT_EQ( M1.get_state(), 1 );
    }

TEST( motor, save_device )
    {
    uni_io_manager mngr;
    mngr.init( 1 );
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.init_node_AO( 0, 0, 0, 0 );


    motor M1( "M1", device::DST_M_REV_FREQ_2_ERROR );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    M1.save_device( buff );
    EXPECT_STREQ( "M1={M=0, ST=0, V=0, R=0, ERRT=0, P_ON_TIME=0},\n", buff );

    G_PAC_INFO()->emulation_off();
    M1.init_and_alloc( 2, 1, 1 );
    M1.init_channel( io_device::IO_channels::CT_AO, 0, 0, 0 );
    *M1.DI_channels.char_read_values[ 0 ] = 1;
    *M1.DO_channels.char_write_values[ 1 ] = 1;

    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    M1.save_device( buff );
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_STREQ( "M1={M=0, ST=-1, V=0, R=1, ERRT=1, P_ON_TIME=0},\n", buff );


    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }

TEST( motor, get_type_name )
    {
    motor test_dev( "test_M1", device::DST_M_VIRT );
    EXPECT_STREQ( "Двигатель", test_dev.get_type_name() );
    }

TEST( motor, get_value )
    {
    uni_io_manager mngr;
    mngr.init( 1 );
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.init_node_AI( 0, 0, 0, 0 );

    motor M1( "M1", device::DST_M_REV_FREQ_2_ERROR );
    M1.init( 0, 0, 1, 0 );
    M1.init_channel( io_device::IO_channels::CT_AO, 0, 0, 0 );
    M1.AO_channels.int_write_values[ 0 ] = new int_2[ 1 ]{ 0 };
    auto VALUE = 90.f;
    *M1.AO_channels.int_write_values[ 0 ] = static_cast<int_2>( VALUE );

    EXPECT_EQ( M1.get_value(), 0 );

    G_PAC_INFO()->emulation_off();
    EXPECT_EQ( M1.get_value(), VALUE );

    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }

TEST( motor, get_state )
    {
    motor M1( "M1", device::DST_M_REV_FREQ_2_ERROR );
    M1.init( 1, 1, 0, 0 );
    M1.DO_channels.char_write_values[ 0 ] = new u_char{ 0 };
    M1.DO_channels.char_read_values[ 0 ] = new u_char{ 0 };
    M1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };

    EXPECT_EQ( M1.get_state(), 0 );

    G_PAC_INFO()->emulation_off();
    *M1.DO_channels.char_write_values[ 0 ] = 1;
    *M1.DI_channels.char_read_values[ 0 ] = 1;
    DeltaMilliSecSubHooker::set_millisec( 0UL );
    EXPECT_EQ( M1.get_state(), 1 );
    DeltaMilliSecSubHooker::set_millisec( 1UL );
    EXPECT_EQ( M1.get_state(), -1 );
    DeltaMilliSecSubHooker::set_default_time();

    G_PAC_INFO()->emulation_on();
    }


TEST( motor_altivar, set_cmd )
    {
    motor_altivar m1( "M1", device::M_ATV );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    m1.save_device( buff );
    EXPECT_STREQ(
        "M1={M=0, ST=0, V=0, R=0, FRQ=0.0, RPM=0, EST=0, AMP=0.0, "
        "MAX_FRQ=0.0, P_ON_TIME=0},\n", buff );

    m1.set_cmd( "R", 0, 1 );
    m1.set_cmd( "FRQ", 0, 1.1 );
    m1.set_cmd( "RPM", 0, 12 );
    m1.set_cmd( "EST", 0, 2 );
    m1.set_cmd( "AMP", 0, 23.3 );
    m1.save_device( buff );
    EXPECT_STREQ(
        "M1={M=0, ST=0, V=1.10, R=1, FRQ=1.1, RPM=12, EST=2, AMP=23.3, "
        "MAX_FRQ=0.0, P_ON_TIME=0},\n", buff );
    }

TEST( motor_altivar, get_amperage )
    {
    motor_altivar_linear m1( "M1" );
    EXPECT_EQ( .0f, m1.get_amperage() );

    m1.set_cmd( "AMP", 0, 25.7 );
    EXPECT_EQ( 25.7f, m1.get_amperage() );
    }

TEST( motor_altivar, set_string_property )
    {
    motor_altivar_linear M1( "M1" );
    EXPECT_EQ( nullptr, M1.get_atv() );
    M1.set_string_property( "IP", "127.0.0.1" );
    EXPECT_NE( nullptr, M1.get_atv() );
    }

TEST( motor_altivar_linear, get_linear_speed )
    {
    motor_altivar_linear m1( "M1" );
    EXPECT_EQ( .0f, m1.get_linear_speed() );

    m1.set_cmd( "RPM", 0, 100 );
    m1.set_cmd( "P_SHAFT_DIAMETER", 0, 2 );
    m1.set_cmd( "P_TRANSFER_RATIO", 0, 5 );
    EXPECT_EQ( 2.09439516f, m1.get_linear_speed() );
    }


TEST( camera, is_ready )
    {
    camera c1( "C1", device::DST_CAM_DO1_DI1 );
    auto v = c1.is_ready();
    EXPECT_TRUE( v );

    camera_DI2 c2( "C2", device::DST_CAM_DO1_DI2 );
    v = c2.is_ready();
    EXPECT_FALSE( v );

    camera_DI3 c3( "C3" );
    v = c3.is_ready();
    EXPECT_FALSE( v );
    }

TEST( camera, get_type_name )
    {
    camera test_dev( "test_CAM1", device::DST_CAM_DO1_DI1 );
    EXPECT_STREQ( "Камера", test_dev.get_type_name() );
    }

TEST( camera, set_string_property )
    {
    camera test_dev( "test_CAM1", device::DST_CAM_DO1_DI1 );
    EXPECT_EQ( "", test_dev.ip );

    test_dev.set_string_property( "IP", "" );
    EXPECT_EQ( "", test_dev.ip );

    test_dev.set_string_property( "IP", "127.0.0.1" );
    EXPECT_EQ( "127.0.0.1", test_dev.ip );
    }

TEST( camera, get_result )
    {
    camera test_dev( "test_CAM1", device::DST_CAM_DO1_DI1 );
    EXPECT_EQ( test_dev.get_result( 1 ), 0 );
    }

TEST( camera, save_device )
    {
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };

    camera test_dev( "test_CAM1", device::DST_CAM_DO1_DI1 );
    test_dev.save_device( buff );
    EXPECT_STREQ( "test_CAM1={M=0, ST=0, V=0, RESULT=0, READY=true},\n", buff );
    }


TEST( camera_DI3, get_result )
    {
    camera_DI3 test_dev( "test_CAM3" );
    EXPECT_EQ( test_dev.get_result( 1 ), 0 );
    }


TEST( analog_input, get_max_val )
    {
    analog_input test_AI( "test_AI1" );
    EXPECT_EQ( test_AI.get_max_val(), 0 );
    }

TEST( analog_input, get_min_val )
    {
    analog_input test_AI( "test_AI1" );
    EXPECT_EQ( test_AI.get_min_val(), 0 );
    }


TEST( counter_f, get_state )
    {
    counter_f fqt1( "FQT1" );
    fqt1.evaluate_io();
    fqt1.evaluate_io(); // Второй вызов (здесь и далее) необходим
    // для возникновения ошибки.
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Есть расход - ошибка должна появиться.
    //Но минимальный расход задан 0 - поэтому нет ошибки.
    fqt1.set_cmd( "F", 0, 1 );
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Минимальный расход задан, но время ожидания задано 0 - поэтому нет ошибки.
    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, .1 );
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Минимальный расход задан, время ожидания задано, но оно не прошло -
    //поэтому нет ошибки.
    fqt1.set_cmd( "P_DT", 0, 1000 );
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Прошло заданное время, задан минимальный расход, но счетчик считает -
    //поэтому нет ошибки.
    fqt1.set_cmd( "ABS_V", 0, 100 );
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    fqt1.evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Прошло заданное время, задан минимальный расход, счетчик не считает -
    //есть ошибка.
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    fqt1.evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_EQ( (int)i_counter::STATES::S_FLOW_ERROR, fqt1.get_state() );


    //В состоянии паузы ошибки не должно быть.
    fqt1.pause();
    fqt1.evaluate_io();
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );
    fqt1.start();
    fqt1.evaluate_io();
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );


    //Далее проверяем на ошибки при наличии привязанного мотора.
    motor m1( "M1", device::DST_M_FREQ );
    fqt1.set_property( "M", &m1 );
    fqt1.evaluate_io();
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Насос работает, счетчик считает.
    fqt1.set_cmd( "F", 0, 0 );
    m1.on();
    fqt1.evaluate_io();
    fqt1.set_cmd( "ABS_V", 0, 200 );
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Насос работает, но счетчик не считает. Ошибки нет, так как расход
    //нулевой - ниже минимального.
    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 2.f );
    fqt1.evaluate_io();
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    fqt1.evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Устанавливаем расход - ошибки нет, так как расход ниже
    //минимального.
    fqt1.set_cmd( "F", 0, 1.f );
    fqt1.evaluate_io();
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Расход стал выше минимального - ошибка должна появиться.
    fqt1.set_cmd( "F", 0, 2.1f );
    fqt1.evaluate_io();
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    fqt1.evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_EQ( (int)i_counter::STATES::S_PUMP_ERROR, fqt1.get_state() );

    //Сбрасываем ошибку.
    fqt1.set_cmd( "ST", 0, 1 ); // Косвенно вызывается fqt1.start().
    fqt1.evaluate_io();
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.pause();
    fqt1.evaluate_io();
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.start();
    fqt1.evaluate_io();
    fqt1.evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( counter_f, set_cmd )
    {
    counter_f fqt1( "FQT1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "V", 0, 50 );
    EXPECT_EQ( 50, fqt1.get_quantity() );
    EXPECT_EQ( 50.f, fqt1.get_value() );

    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( 100, fqt1.get_abs_quantity() );

    fqt1.set_cmd( "F", 0, 9.9 );
    EXPECT_EQ( 9.9f, fqt1.get_flow() );

    fqt1.save_device( buff );
    EXPECT_STREQ(
        "FQT1={M=0, ST=1, V=50, ABS_V=100, DAY_T1=0, PREV_DAY_T1=0, "
        "DAY_T2=0, PREV_DAY_T2=0, F=9.90, P_MIN_FLOW=0, "
        "P_MAX_FLOW=0, P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );
    }

TEST( counter_f, get_min_flow )
    {
    counter_f fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );

    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 1.1 );
    res = fqt1.get_min_flow();
    EXPECT_EQ( 1.1f, res );
    }

TEST( counter_f, get_error_description )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_error_description(); //Нет ошибок.
    EXPECT_STREQ( "нет ошибок", res );

    fqt1.set_cmd( "ST", 0, -1 );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "неизвестная ошибка", res );

    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_PUMP_ERROR ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "счет импульсов", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "счет импульсов", res );

    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_FLOW_ERROR ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "самотёк", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "самотёк", res );

    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_LOW_ERR ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "канал потока (нижний предел)", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "канал потока (нижний предел)", res );
    }


TEST( counter, set_cmd )
    {
    counter fqt1( "FQT1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "V", 0, 50 );
    EXPECT_EQ( 50, fqt1.get_quantity() );
    EXPECT_EQ( 50.f, fqt1.get_value() );

    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( 100, fqt1.get_abs_quantity() );

    fqt1.save_device( buff );
    EXPECT_STREQ( "FQT1={M=0, ST=1, V=50, ABS_V=100, DAY_T1=0, PREV_DAY_T1=0, "
        "DAY_T2=0, PREV_DAY_T2=0},\n", buff );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, -10 );
    EXPECT_EQ( (int)i_counter::STATES::S_FLOW_ERROR, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, -10 );
    EXPECT_EQ( (int)i_counter::STATES::S_FLOW_ERROR, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "DAY_T1", 0, 100 );
    fqt1.set_cmd( "DAY_T2", 0, 200 );
    fqt1.set_cmd( "PREV_DAY_T1", 0, 10 );
    fqt1.set_cmd( "PREV_DAY_T2", 0, 20 );
    fqt1.save_device( buff );
    EXPECT_STREQ( "FQT1={M=0, ST=2, V=50, ABS_V=100, DAY_T1=100, "
        "PREV_DAY_T1=10, DAY_T2=200, PREV_DAY_T2=20},\n", buff );
    }

TEST( counter, get_pump_dt )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_pump_dt();
    EXPECT_EQ( 0, res );
    }

TEST( counter, get_min_flow )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );
    }

TEST( counter, get_max_raw_value )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_max_raw_value();
    EXPECT_EQ( USHRT_MAX, res );
    }

TEST( counter, get_type_name )
    {
    counter test_dev( "test_FQT1" );
    EXPECT_STREQ( "Счетчик", test_dev.get_type_name() );
    }


TEST( virtual_counter, virtual_counter )
    {
    virtual_counter fqt1( "FQT1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.save_device( buff );
    EXPECT_STREQ( "FQT1={M=0, ST=1, V=0, ABS_V=0, F=0.00},\n", buff );
    }

TEST( virtual_counter, direct_on )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.pause();
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );
    fqt1.direct_on();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( virtual_counter, start )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.pause();
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );
    fqt1.start();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( virtual_counter, reset )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.set_cmd( "V", 0, 200. );
    EXPECT_EQ( fqt1.get_value(), 200.f );
    EXPECT_EQ( fqt1.get_quantity(), 200 );

    fqt1.reset();
    EXPECT_EQ( fqt1.get_value(), 0.f );
    EXPECT_EQ( fqt1.get_quantity(), 0 );
    }

TEST( virtual_counter, abs_reset )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.set_cmd( "ABS_V", 0, 200. );
    EXPECT_EQ( fqt1.get_abs_quantity(), 200 );

    fqt1.abs_reset();
    EXPECT_EQ( fqt1.get_abs_quantity(), 0 );
    }

TEST( virtual_counter, direct_off )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.set_cmd( "V", 0, 200. );
    EXPECT_EQ( fqt1.get_value(), 200.f );
    EXPECT_EQ( fqt1.get_quantity(), 200 );

    fqt1.direct_off();
    EXPECT_EQ( fqt1.get_value(), 0.f );
    EXPECT_EQ( fqt1.get_quantity(), 0.f );
    }

TEST( virtual_counter, direct_set_state )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.direct_set_state( static_cast<int>( i_counter::STATES::S_PAUSE ) );
    EXPECT_EQ( static_cast<int>( i_counter::STATES::S_PAUSE ), fqt1.get_state() );

    fqt1.direct_set_state( static_cast<int>( i_counter::STATES::S_WORK ) );
    EXPECT_EQ( static_cast<int>( i_counter::STATES::S_WORK ), fqt1.get_state() );


    fqt1.direct_set_state( static_cast<int>( i_counter::STATES::S_FLOW_ERROR ) );
    EXPECT_EQ( static_cast<int>( i_counter::STATES::S_FLOW_ERROR ), fqt1.get_state() );
    }

TEST( virtual_counter, set )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.set( 200, 200, 2.f );
    EXPECT_EQ( fqt1.get_value(), 200.f );
    EXPECT_EQ( fqt1.get_quantity(), 200 );
    EXPECT_EQ( fqt1.get_abs_quantity(), 200 );
    EXPECT_EQ( fqt1.get_flow(), 2.f );
    }

TEST( virtual_counter, eval )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.eval( 200, 200, 2.f );
    EXPECT_EQ( fqt1.get_value(), 0.f );
    EXPECT_EQ( fqt1.get_quantity(), 0 );
    EXPECT_EQ( fqt1.get_abs_quantity(), 0 );
    EXPECT_EQ( fqt1.get_flow(), 2.f );

    fqt1.eval( 300, 300, 3.f );
    EXPECT_EQ( fqt1.get_value(), 100.f );
    EXPECT_EQ( fqt1.get_quantity(), 100 );
    EXPECT_EQ( fqt1.get_abs_quantity(), 100 );
    EXPECT_EQ( fqt1.get_flow(), 3.f );

    fqt1.eval( 100, 100, 3.f );
    EXPECT_EQ( fqt1.get_value(), 100.f );
    EXPECT_EQ( fqt1.get_quantity(), 100 );
    EXPECT_EQ( fqt1.get_abs_quantity(), 100 );
    EXPECT_EQ( fqt1.get_flow(), 3.f );
    }

TEST( virtual_counter, get_pump_dt )
    {
    virtual_counter fqt1( "FQT1" );
    auto res = fqt1.get_pump_dt();
    EXPECT_EQ( 0, res );
    }

TEST( virtual_counter, get_min_flow )
    {
    virtual_counter fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );
    }

TEST( virtual_counter, set_cmd )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "F", 0, 100. );
    EXPECT_EQ( fqt1.get_flow(), 100.f );

    fqt1.set_cmd( "V", 0, 200. );
    EXPECT_EQ( fqt1.get_value(), 200.f );
    EXPECT_EQ( fqt1.get_quantity(), 200.f );
    }

TEST( virtual_counter, pause_daily )
    {
    virtual_counter fqt1( "FQT1" );
    fqt1.pause_daily();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    fqt1.start_daily();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( counter_iolink, set_cmd )
    {
    counter_iolink fqt1( "FQT1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_temperature() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "V", 0, 50 );
    EXPECT_EQ( 50, fqt1.get_quantity() );
    EXPECT_EQ( 50.f, fqt1.get_value() );

    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( 100, fqt1.get_abs_quantity() );

    fqt1.set_cmd( "F", 0, 9.9 );
    EXPECT_EQ( 9.9f, fqt1.get_flow() );

    fqt1.set_cmd( "T", 0, 1.1 );
    EXPECT_EQ( 1.1f, fqt1.get_temperature() );

    fqt1.save_device( buff );
    EXPECT_STREQ(
        "FQT1={M=0, ST=1, V=50, ABS_V=100, DAY_T1=0, PREV_DAY_T1=0, "
        "DAY_T2=0, PREV_DAY_T2=0, F=9.90, T=1.1, "
        "P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( counter_iolink, evaluate_io )
    {
    counter_iolink fqt1( "FQT1" );
    fqt1.init( 0, 0, 0, 1 );
    fqt1.AI_channels.int_read_values[ 0 ] = new int_2[ 8 ]{ 0 };
    auto buff = reinterpret_cast<char*>( fqt1.AI_channels.int_read_values[ 0 ] );
    G_PAC_INFO()->emulation_off();

    *reinterpret_cast<float*>( fqt1.AI_channels.int_read_values[ 0 ] ) = 11.11f;
    std::swap( buff[ 0 ], buff[ 3 ] );  //Reverse byte order to get correct float.
    std::swap( buff[ 2 ], buff[ 1 ] );
    fqt1.evaluate_io();
    EXPECT_EQ( 0, fqt1.get_quantity() );//First read.

    *reinterpret_cast<float*>( fqt1.AI_channels.int_read_values[ 0 ] ) = 22.22f;
    std::swap( buff[ 0 ], buff[ 3 ] );  //Reverse byte order to get correct float.
    std::swap( buff[ 2 ], buff[ 1 ] );
    fqt1.AI_channels.int_read_values[ 0 ][ 2 ] = static_cast<int_2>( 22 );
    fqt1.AI_channels.int_read_values[ 0 ][ 3 ] = static_cast<int_2>( 33 << 2 );
    std::swap( buff[ 5 ], buff[ 4 ] );  //Reverse byte order to get correct int16.
    std::swap( buff[ 7 ], buff[ 6 ] );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 11.11, fqt1.get_quantity() );
    EXPECT_EQ( 22 * 0.01f, fqt1.get_flow() );
    EXPECT_EQ( 33 * 0.1f, fqt1.get_temperature() );

    G_PAC_INFO()->emulation_on();
    }

tm get_time_next_day()
    {
    static struct tm timeInfo_;
#ifdef LINUX_OS
    auto t_ = time( nullptr );
    localtime_r( &t_, &timeInfo_ );
#else
    static time_t t_ = time( nullptr );
    localtime_s( &timeInfo_, &t_ );
#endif // LINUX_OS
    timeInfo_.tm_yday++;

    return timeInfo_;
    }

TEST( counter_iolink, get_error_description )
    {
    counter_iolink fqt1( "FQT1" );
    auto res = fqt1.get_error_description(); //Нет ошибок.
    EXPECT_STREQ( "нет ошибок", res );

    fqt1.set_cmd( "ST", 0, -io_device::IOLINKSTATE::NOTCONNECTED );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "IOL-устройство не подключено", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "IOL-устройство не подключено", res );


    fqt1.set_cmd( "ST", 0, -io_device::IOLINKSTATE::DEVICEERROR );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "ошибка IOL-устройства", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "ошибка IOL-устройства", res );
    }

TEST( counter_iolink, get_quantity )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    class counter_iolink_test :public counter_iolink
        {
        float totalizer = .0f;

        public:
            counter_iolink_test( const char* dev_name ) :
                counter_iolink( dev_name )
                {};

            float get_raw_value() const
                {
                return totalizer;
                };

            void set_raw_value( float totalizer )
                {
                this->totalizer = totalizer;
                };
        };

    counter_iolink_test fqt1( "FQT1" );
    auto res = fqt1.get_quantity();
    EXPECT_EQ( 0, res );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );

    fqt1.set_raw_value( 10 );
    fqt1.evaluate_io();
    res = fqt1.get_quantity();
    EXPECT_EQ( 0, res );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );

    fqt1.set_raw_value( 20 );
    fqt1.evaluate_io();
    res = fqt1.get_quantity();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, res );
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_abs_quantity() );

    fqt1.save_device( buff );
    EXPECT_STREQ(
        "FQT1={M=0, ST=1, V=10000, ABS_V=10000, DAY_T1=10, PREV_DAY_T1=0, "
        "DAY_T2=10, PREV_DAY_T2=0, F=0.00, T=0.0, "
        "P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_time_next_day ), SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );
    fqt1.evaluate_io();         // New day.
    fqt1.save_device( buff );
    EXPECT_STREQ(
        "FQT1={M=0, ST=1, V=10000, ABS_V=10000, DAY_T1=0, PREV_DAY_T1=10, "
        "DAY_T2=0, PREV_DAY_T2=10, F=0.00, T=0.0, "
        "P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );

    fqt1.off();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, res );
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_abs_quantity() );

    fqt1.pause();
    fqt1.pause_daily( base_counter::DAY_CTR::DAY_T1 );
    fqt1.pause_daily( base_counter::DAY_CTR::DAY_T2 );

    fqt1.set_raw_value( 30 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_abs_quantity() );
    fqt1.save_device( buff );
    EXPECT_STREQ(
        "FQT1={M=0, ST=2, V=10000, ABS_V=20000, DAY_T1=0, PREV_DAY_T1=10, "
        "DAY_T2=0, PREV_DAY_T2=10, F=0.00, T=0.0, "
        "P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );

    fqt1.on();
    fqt1.start_daily( base_counter::DAY_CTR::DAY_T1 );
    fqt1.start_daily( base_counter::DAY_CTR::DAY_T2 );
    fqt1.set_raw_value( 40 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 30, fqt1.get_abs_quantity() );
    fqt1.save_device( buff );
    EXPECT_STREQ(
        "FQT1={M=0, ST=1, V=20000, ABS_V=30000, DAY_T1=10, PREV_DAY_T1=10, "
        "DAY_T2=10, PREV_DAY_T2=10, F=0.00, T=0.0, "
        "P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );


    fqt1.set_state( 0 );        //Off.
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 30, fqt1.get_abs_quantity() );

    fqt1.set_state( 2 );        //Pause.
    fqt1.set_raw_value( 50 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 40, fqt1.get_abs_quantity() );

    fqt1.set_state( 1 );        //Start
    fqt1.set_raw_value( 60 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 30, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 50, fqt1.get_abs_quantity() );


    //Test reset to 0 physical counter after its power reboot.
    fqt1.set_raw_value( 10 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 40, fqt1.get_quantity() );

    //Test physical counter overflow.
    fqt1.set_raw_value( fqt1.get_max_raw_value() - 10 );
    fqt1.evaluate_io();
    fqt1.get_quantity();
    fqt1.reset();
    fqt1.set_raw_value( 10 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_quantity() );

    fqt1.pause();
    fqt1.restart();
    EXPECT_EQ( 0, fqt1.get_quantity() );


    fqt1.abs_reset();
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );

    subhook_remove( get_time_hook );
    }

TEST( counter_iolink, get_pump_dt )
    {
    counter_iolink fqt1( "FQT1" );
    auto res = fqt1.get_pump_dt();
    EXPECT_EQ( 0, res );

    fqt1.set_cmd( "P_DT", 0, 11 );
    res = fqt1.get_pump_dt();
    EXPECT_EQ( 11, res );
    }

TEST( counter_iolink, get_min_flow )
    {
    counter_iolink fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );

    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 1.1 );
    res = fqt1.get_min_flow();
    EXPECT_EQ( 1.1f, res );
    }

static void test_counter_iolink_article( const char* article, float gradient )
    {
    counter_iolink fqt( "FQT1" );
    fqt.set_article( article );
    G_PAC_INFO()->emulation_off();

    // Test flow gradient for the specified article.
    fqt.set_cmd( "F", 0, 9.9 );
    EXPECT_NEAR( 9.9f, fqt.get_flow(), 0.01f );

    // Test with raw value.
    fqt.init( 0, 0, 0, 1 );
    fqt.AI_channels.int_read_values[ 0 ] = new int_2[ 8 ]{ 0 };
    auto buff = reinterpret_cast<std::byte*>( fqt.AI_channels.int_read_values[ 0 ] );

    // Initialize with zero data.
    *reinterpret_cast<float*>( fqt.AI_channels.int_read_values[ 0 ] ) = 0.0f;
    std::swap( buff[ 0 ], buff[ 3 ] );
    std::swap( buff[ 2 ], buff[ 1 ] );
    fqt.evaluate_io();

    // Set flow raw value.
    *reinterpret_cast<float*>( fqt.AI_channels.int_read_values[ 0 ] ) = 0.0f;
    std::swap( buff[ 0 ], buff[ 3 ] );
    std::swap( buff[ 2 ], buff[ 1 ] );
    fqt.AI_channels.int_read_values[ 0 ][ 2 ] = static_cast<int_2>( 1000 );
    std::swap( buff[ 5 ], buff[ 4 ] );
    fqt.evaluate_io();
    EXPECT_NEAR( 1000 * gradient, fqt.get_flow(), 0.01f );

    delete[] fqt.AI_channels.int_read_values[ 0 ];
    fqt.AI_channels.int_read_values[ 0 ] = nullptr;
    G_PAC_INFO()->emulation_on();
    }

TEST( counter_iolink, article_sm4000 )
    {
    test_counter_iolink_article( "IFM.SM4000", 0.001f );
    }

TEST( counter_iolink, article_sm6100 )
    {
    test_counter_iolink_article( "IFM.SM6100", 0.01f );
    }

TEST_F( iolink_dev_test, counter_iolink_get_state_iolink_errors )
    {
    counter_iolink fqt1( "FQT1" );
    init_channels( fqt1 );
    G_PAC_INFO()->emulation_off();

    EXPECT_EQ( -io_device::IOLINKSTATE::NOTCONNECTED, fqt1.get_state() );

    // Bit 0 - IO-Link connected.
    *fqt1.AI_channels.int_module_read_values[ 0 ] = 0b1;
    EXPECT_EQ( -io_device::IOLINKSTATE::DEVICEERROR, fqt1.get_state() );

    set_iol_state_to_OK( fqt1 );
    EXPECT_EQ( static_cast<int>( i_counter::STATES::S_WORK ), fqt1.get_state() );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, counter_iolink_smfx20_evaluate_io_and_getters )
    {
    counter_iolink fqt1( "FQT1" );
    fqt1.set_article( "IFM.SMF420" );

    init_channels( fqt1 );
    fqt1.AI_channels.int_read_values[ 0 ] = new int_2[ 8 ]{ 0 };
    auto data = reinterpret_cast<std::byte*>( fqt1.AI_channels.int_read_values[ 0 ] );
    set_iol_state_to_OK( fqt1 );
    G_PAC_INFO()->emulation_off();

    auto set_smfx20_payload = [&]( float totalizer, int16_t flow, int16_t temp,
                                   uint16_t conductivity )
        {
        std::memset( data, 0, 16 );

        std::memcpy( data, &totalizer, sizeof( totalizer ) );
        std::swap( data[ 3 ], data[ 0 ] );
        std::swap( data[ 1 ], data[ 2 ] );

        std::memcpy( data + 4, &flow, sizeof( flow ) );
        std::swap( data[ 4 ], data[ 5 ] );

        std::memcpy( data + 6, &temp, sizeof( temp ) );
        std::swap( data[ 6 ], data[ 7 ] );

        std::memcpy( data + 8, &conductivity, sizeof( conductivity ) );
        std::swap( data[ 8 ], data[ 9 ] );
        };

    // First read initializes internal base values.
    set_smfx20_payload( 10.0f, 0, 0, 0 );
    fqt1.evaluate_io();
    EXPECT_EQ( 0, fqt1.get_quantity() );

    set_smfx20_payload( 20.0f, 111, 333, 250 );
    fqt1.evaluate_io();

    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_quantity() );
    EXPECT_FLOAT_EQ( 20.0f, fqt1.get_raw_value() );
    EXPECT_NEAR( 11.1f, fqt1.get_flow(), 0.01f );
    EXPECT_FLOAT_EQ( 33.3f, fqt1.get_temperature() );
    EXPECT_FLOAT_EQ( 250.0f, fqt1.get_conductivity() );

    fqt1.set_cmd( "C", 0, 321.0 );
    fqt1.set_cmd( "F", 0, 9.9 );
    fqt1.set_cmd( "T", 0, 4.2 );

    EXPECT_FLOAT_EQ( 321.0f, fqt1.get_conductivity() );
    EXPECT_NEAR( 9.9f, fqt1.get_flow(), 0.01f );
    EXPECT_FLOAT_EQ( 4.2f, fqt1.get_temperature() );

    std::array<char, 300> buff{};
    fqt1.save_device( buff.data() );
    EXPECT_THAT( std::string( buff.data() ), HasSubstr( "C=321" ) );
    EXPECT_THAT( std::string( buff.data() ), HasSubstr( "F=9.90" ) );
    EXPECT_THAT( std::string( buff.data() ), HasSubstr( "T=4.2" ) );

    delete[] fqt1.AI_channels.int_read_values[ 0 ];
    fqt1.AI_channels.int_read_values[ 0 ] = nullptr;
    G_PAC_INFO()->emulation_on();
    }


TEST( virtual_wages, get_value )
    {
    virtual_wages w1( "W1" );
    EXPECT_EQ( 0.0f, w1.get_value() );
    }

TEST( virtual_wages, get_state )
    {
    virtual_wages w1( "W1" );
    EXPECT_EQ( 0, w1.get_state() );
    }


TEST( wages_RS232, get_value_from_wages )
    {
    wages_RS232 w1( "W1" );
    w1.init( 0, 0, 1, 1 );

    //1 - Тест на пустой указатель
    //2, 3 - Данные корректные
    //4, 5 - Некорректные данные, возвращает 0
    //6 - Если в первом байте 1, буффер не пустой. Переключение в режим
    // чтения данных. Возвращает старое значение (в данном случае 0)
    //7 - Если в первом байте 0, буффер пустой. Вернуть старое значение
    //(в данном случае 0).
    w1.AI_channels.int_read_values[ 0 ] = nullptr;                         //1
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( -1, w1.get_state() );

    char tmp_str[] = "   +0000.00k";                                       //2
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( 1, w1.get_state() );

    strcpy( tmp_str, "   +0012.34k" );                                     //3
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 12.34f, w1.get_value_from_wages() );
    EXPECT_EQ( 1, w1.get_state() );

    strcpy( tmp_str, "  +12.34k   " );                                     //4
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( -1, w1.get_state() );

    strcpy( tmp_str, "   -0001.34k" );                                     //5
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( -1, w1.get_state() );

    w1.AI_channels.int_read_values[ 0 ] =                                  //6
        new int_2[ 7 ]{ 1, 0, 0, 0, 0, 0, 0 };
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );

    w1.AI_channels.int_read_values[ 0 ] =                                  //7
        new int_2[ 7 ]{ 0, 0, 0, 0, 0, 0, 0 };
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    }

TEST( wages_RS232, get_value )
    {
    wages_RS232 w1( "W1" );
    w1.init( 0, 0, 1, 1 );

    char tmp_str[] = "   +0000.00k";
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    w1.evaluate_io();

    EXPECT_EQ( 0.0f, w1.get_value() );

    w1.set_par( 1, 0, 10 );                    //Установка параметра P_CZ = 10
    EXPECT_EQ( 10.0f, w1.get_value() );
    }

TEST( wages_RS232, get_state )
    {
    wages_RS232 w1( "W1" );
    w1.init( 0, 0, 1, 1 );

    w1.AI_channels.int_read_values[ 0 ] = nullptr;
    w1.get_value_from_wages();
    EXPECT_EQ( -1, w1.get_state() );

    char tmp_str[] = "   +0000.00k";
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    w1.get_value_from_wages();
    EXPECT_EQ( 1, w1.get_state() );
    }

TEST( wages_RS232, tare )
    {
    wages_RS232 w1( "W1" );
    w1.tare();
    }

TEST( wages_RS232, evaluate_io )
    {
    wages_RS232 w1( "W1" );
    w1.evaluate_io();
    }


TEST( wages, save_device )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    wages w1( "W1" );
    w1.save_device( buff );
    EXPECT_STREQ(
        "W1={M=0, ST=0, V=0, W=0.000, P_NOMINAL_W=0, P_RKP=0, P_CZ=0, P_DT=0},\n",
        buff );
    }

TEST( wages, get_type_name )
    {
    wages test_dev( "test_W1" );
    EXPECT_STREQ( "Тензорезистор", test_dev.get_type_name() );
    }

TEST( wages, get_weight )
    {
    uni_io_manager mngr;
    mngr.init( 1 );
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 2, 2 );
    mngr.init_node_AI( 0, 0, 491, 0 );
    mngr.init_node_AI( 0, 1, 491, 1 );

    wages test_dev( "test_W1" );
    test_dev.init( 0, 0, 0, 2 );
    test_dev.init_channel( io_device::IO_channels::CT_AI, 0, 0, 0 );
    test_dev.init_channel( io_device::IO_channels::CT_AI, 1, 0, 1 );
    test_dev.AI_channels.int_read_values[ 0 ] = new int_2[ 1 ]{ 0 };
    test_dev.AI_channels.int_read_values[ 1 ] = new int_2[ 1 ]{ 0 };
    *test_dev.AI_channels.int_read_values[ 1 ] = 65;

    test_dev.set_cmd( "P_NOMINAL_W", 0, 5 );
    test_dev.set_cmd( "P_RKP", 0, 2 );

    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    EXPECT_EQ( test_dev.get_weight(), 0 );
    DeltaMilliSecSubHooker::set_default_time();

    test_dev.set_cmd( "P_CZ", 0, 2.f );
    EXPECT_EQ( test_dev.get_weight(), 2.f );
    test_dev.tare();
    EXPECT_EQ( test_dev.get_weight(), 0 );

    io_manager::replace_instance( prev_mngr );
    }


TEST( wages_eth, evaluate_io )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    w1.evaluate_io();
    EXPECT_EQ( 0, w1.get_value() );

    // Корректное создание с именем в виде пустой строки.
    iot_wages_eth w( 10, "127.0.0.2", 10000, nullptr );

    iot_wages_eth w2( 1, "127.0.0.1", 10000, "W2" );

    w2.evaluate();
    EXPECT_EQ( 0, w2.get_wages_value() );
    }

TEST( wages_eth, get_value )
    {
    G_PAC_INFO()->emulation_off();
    wages_eth w1( "W1" );
    EXPECT_EQ( 0, w1.get_value() );
    G_PAC_INFO()->emulation_on();

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    w1.direct_set_value( 10.0f );
    EXPECT_EQ( 10, w1.get_value() );
    w1.set_par( 1, 0, 10 );
    EXPECT_EQ( 20, w1.get_value() );
    }

TEST( wages_eth, tare )
    {
    wages_eth w1( "W1" );
    w1.tare();
    }

TEST( wages_eth, get_state )
    {
    G_PAC_INFO()->emulation_off();
    wages_eth w1( "W1" );
    EXPECT_EQ( 0, w1.get_state() );
    G_PAC_INFO()->emulation_on();

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    EXPECT_EQ( 0, w1.get_state() );
    }

TEST( wages_eth, direct_set_value )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );
    w1.direct_set_value( 10.0f );
    EXPECT_EQ( 10, w1.get_value() );
    w1.direct_set_value( -10.0f );
    EXPECT_EQ( 10, w1.get_value() );

    G_PAC_INFO()->emulation_off();
    w1.direct_set_value( 10.0f );
    EXPECT_EQ( 10, w1.get_value() );
    w1.direct_set_value( -10.0f );
    EXPECT_EQ( 10, w1.get_value() );
    G_PAC_INFO()->emulation_on();
    }

TEST( wages_eth, direct_set_state )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );
    w1.direct_set_state( 1 );
    EXPECT_EQ( 1, w1.get_state() );
    w1.direct_set_state( 0 );
    EXPECT_EQ( 0, w1.get_state() );

    G_PAC_INFO()->emulation_off();
    w1.direct_set_state( 1 );
    EXPECT_EQ( 1, w1.get_state() );
    w1.direct_set_state( 0 );
    EXPECT_EQ( 0, w1.get_state() );
    G_PAC_INFO()->emulation_on();
    }

TEST( wages_eth, direct_off )
    {
    wages_eth w1( "W1" );
    w1.direct_on();
    EXPECT_EQ( 1, w1.get_state() );

    w1.direct_off();
    EXPECT_EQ( 0, w1.get_state() );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );
    G_PAC_INFO()->emulation_off();

    w1.direct_on();
    EXPECT_EQ( 1, w1.get_state() );

    w1.direct_off();
    EXPECT_EQ( 0, w1.get_state() );
    G_PAC_INFO()->emulation_on();
    }

TEST( wages_eth, direct_set_tcp_buff )
    {
    G_PAC_INFO()->emulation_off();

    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    //Корректные данные.
    char new_value[] = "+91234.5kg";
    const auto SIZE = sizeof( new_value );
    w1.direct_set_tcp_buff( new_value, SIZE, 1 );
    const auto CORRECT_VALUE = 91234.5f;
    EXPECT_EQ( CORRECT_VALUE, w1.get_value() );

    //Некорректные данные. Сохраняется предыдущее значение веса.
    const auto NOT_VALID_DATA = {
        "          ", "+00000.5kt", "+00000.5zg", "+00000.5zt" };
    for ( auto str : NOT_VALID_DATA )
        {
        strcpy( new_value, str );
        w1.direct_set_tcp_buff( new_value, SIZE, 1 );
        EXPECT_EQ( 0, w1.get_state() );
        EXPECT_EQ( CORRECT_VALUE, w1.get_value() );
        }

    G_PAC_INFO()->emulation_on();
    }

TEST( wages_eth, set_string_property )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "NOT_IP";
    w1.set_string_property( field, ip );
    }


TEST( wages_pxc_axl, evaluate_io )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );

    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_OVERRANGE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -1, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_WIRE_BREAK );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -2, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_SHORT_CIRCUIT );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -3, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_INVALID_VALUE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -4, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_FAULTY_SUPPLY_VOLTAGE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -5, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_FAULTY_DEVICE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -6, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_UNDERRANGE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -7, w1.get_state() );
    }

TEST( wages_pxc_axl, tare )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.tare();
    EXPECT_EQ( 0.0f, w1.get_value() );
    }

TEST( wages_pxc_axl, reset_tare )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.tare();
    EXPECT_EQ( .0f, w1.get_value() );

    w1.reset_tare();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );
    }

TEST( wages_pxc_axl, direct_set_state )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.direct_set_state( 1 );   // CMDS::TARE
    EXPECT_EQ( .0f, w1.get_value() );

    w1.direct_set_state( 2 );   // CMDS::RESET_TARE
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.direct_set_state( 0 );   // No such command.
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );
    }

TEST( wages_pxc_axl, direct_set_value )
    {
    wages_pxc_axl w1( "W1" );
    w1.direct_set_value( 10.f );
    EXPECT_EQ( 10.f, w1.get_value() );
    w1.direct_set_value( 0.0f );
    EXPECT_EQ( 0.0f, w1.get_value() );

    G_PAC_INFO()->emulation_off();
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    w1.direct_set_value( VALUE );   // Do nothing.
    EXPECT_EQ( 0, w1.get_value() );

    G_PAC_INFO()->emulation_on();
    }


TEST( temperature_e, save_device )
    {
    temperature_e T1( "T1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    T1.save_device( buff );
    EXPECT_STREQ(
        "T1={M=0, ST=1, V=0, E=0, M_EXP=20.0, S_DEV=2.0, P_CZ=0, P_ERR=0},\n",
        buff );

    T1.set_cmd( "E", 0, 1 );
    T1.save_device( buff );
    EXPECT_STRNE(
        "T1={M=0, ST=1, V=0, E=1, M_EXP=20.0, S_DEV=2.0, P_CZ=0, P_ERR=0},\n",
        buff );
    }

TEST( temperature_e, get_type_name )
    {
    temperature_e test_dev( "test_TE1" );
    EXPECT_STREQ( "Температура", test_dev.get_type_name() );
    }

TEST( temperature_e, get_state )
    {
    temperature_e TE1( "test_TE1" );
    test_temperature( &TE1 );
    }


TEST( temperature_e_iolink, save_device )
    {
    temperature_e_iolink T1( "T1" );
    const int BUFF_SIZE = 200;
    std::array <char, BUFF_SIZE> buff = { 0 };

    T1.save_device( buff.data() );
    EXPECT_STREQ(
        "T1={M=0, ST=1, V=0, E=0, M_EXP=1.0, S_DEV=0.2, P_CZ=0, P_ERR=0},\n",
        buff.data() );
    }


TEST( threshold_regulator, set_value )
    {
    threshold_regulator TRC1( "TRC1" );
    device* dev = static_cast<device*>( &TRC1 );

    auto TE_name = std::string( "TE1" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, TE_name.c_str(), "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    const auto TE1 = TE( TE_name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( TE1 ) );
    auto M_name = std::string( "M1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, M_name.c_str(), "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    const auto M1 = G_DEVICE_MANAGER()->get_M( M_name.c_str() );
    ASSERT_NE( nullptr, M1 );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( M1 ) );

    const auto SET_VALUE = 10;
    const auto DELTA_VALUE = 1;

    //Correct processing when we have no set sensor and actuator.
    dev->set_value( SET_VALUE );

    dev->set_string_property( "IN_VALUE", TE_name.c_str() );
    dev->set_string_property( "OUT_VALUE", M_name.c_str() );
    dev->set_cmd( "P_DELTA", 0, DELTA_VALUE );
    dev->off();

    //Regulator switched off - it should not switch on actuator regardless
    //temperature.
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    dev->on();

    //Regulator switched on, temperature is below range - it should switch
    //on actuator.
    TE1->set_cmd( "V", 0, SET_VALUE / 10 );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 1, M1->get_state() );

    //Regulator switched on, temperature is above range - it should switch off
    //actuator.
    TE1->set_cmd( "V", 0, SET_VALUE * 2 );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    //Regulator switched on, temperature is inside range - it should not change
    //actuators state.
    TE1->set_cmd( "V", 0, SET_VALUE );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    dev->set_cmd( "P_is_reverse", 0, 1 );

    //Regulator switched on, temperature is above range, but reverse is on -
    //it should switch on actuators.
    TE1->set_cmd( "V", 0, SET_VALUE * 2 );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 1, M1->get_state() );

    //Regulator switched on, temperature is below range, but reverse is on -
    //it should switch on actuators.
    TE1->set_cmd( "V", 0, SET_VALUE / 10 );
    dev->set_value( SET_VALUE * 2 );
    EXPECT_EQ( 0, M1->get_state() );

    dev->set_cmd( "P_is_reverse", 0, .0 );
    dev->off();

    //Regulator switched off.
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    dev->on();

    //Use counter as a sensor.
    auto name = std::string( "FQT1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_VIRT, name.c_str(), "Test counter", "F" );
    ASSERT_EQ( nullptr, res );
    const auto FQT1 = virtual_FQT( name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( FQT1 ) );
    dev->set_string_property( "IN_VALUE", name.c_str() );

    //Regulator switched on, flow is below range - it should switch
    //on actuator.
    FQT1->set_cmd( "F", 0, SET_VALUE );
    dev->set_value( SET_VALUE * 2 );
    EXPECT_EQ( 1, M1->get_state() );

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( threshold_regulator, set_cmd )
    {
    threshold_regulator p1( "C1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    p1.save_device( buff );
    EXPECT_STREQ(
        "C1={M=0, ST=0, V=0, P_is_reverse=0, P_delta=0},\n", buff );

    //Set a property that does not exist.
    auto res = p1.set_cmd( "NO_SUCH_PROPERTY", 0, 1 );
    EXPECT_EQ( 1, res );
    //Set a parameter that does not exist.
    res = p1.set_cmd( "P_NO_SUCH_PARAMETER", 0, 1 );
    EXPECT_EQ( 1, res );

    res = p1.set_cmd( "P_delta", 0, 10 );
    EXPECT_EQ( 0, res );
    res = p1.set_cmd( "P_is_reverse", 0, 1 );
    EXPECT_EQ( 0, res );
    p1.on();
    p1.save_device( buff );
    EXPECT_STREQ(
        "C1={M=0, ST=1, V=0, P_is_reverse=1, P_delta=10},\n", buff );
    }

TEST( threshold_regulator, set_state )
    {
    threshold_regulator p1( "C1" );

    //Switch on regulator.
    p1.set_state( 1 );
    EXPECT_EQ( 1, p1.get_state() );

    //Switch on regulator (already on).
    p1.set_state( 1 );
    EXPECT_EQ( 1, p1.get_state() );

    //Non correct new state - should be no changes.
    p1.set_state( 2 );
    EXPECT_EQ( 1, p1.get_state() );

    //Switch off regulator.
    p1.set_state( 0 );
    EXPECT_EQ( 0, p1.get_state() );
    }

TEST( threshold_regulator, get_name_in_Lua )
    {
    auto name = "C1";
    threshold_regulator p1( name );

    EXPECT_STREQ( name, p1.get_name_in_Lua() );
    }

TEST( threshold_regulator, set_string_property )
    {
    auto name = "C1";
    threshold_regulator p1( name );

    //TODO - refactor set_string_property() to return result.
    p1.set_string_property( nullptr, nullptr );
    p1.set_string_property( "IN_VALUE", "FQT1" );
    p1.set_string_property( "OUT_VALUE", "M1" );
    p1.set_string_property( "NO_SUCH_PROPERTY", "AA1" );
    }

TEST( threshold_regulator, get_type_name )
    {
    threshold_regulator test_dev( "test_C1" );
    EXPECT_STREQ( "ПИД-регулятор", test_dev.get_type_name() );
    }


TEST( par_device, get_par )
    {
    const auto IDX = 0;
    const auto OFFSET = 0;
    par_device dev1( 2 );
    dev1.set_par( IDX, OFFSET, 10.f );

    EXPECT_EQ( dev1.get_par( IDX, OFFSET ), 10.f );

    par_device dev2( 0 );
    // Корректный вызов, когда нет параметров.
    EXPECT_EQ( dev2.get_par( IDX, OFFSET ), 0.f );
    }

TEST( par_device, set_par_name )
    {
    const auto IDX = 1;
    const auto OFFSET = 0;

    par_device dev( 1 );
    dev.set_par_name( 1, OFFSET, "TEST_VERY_VERY_VERY_LONG_NAME" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    dev.save_device( buff );
    EXPECT_STREQ( "", buff );

    dev.set_par_name( IDX, OFFSET, "TEST_NAME" );
    dev.save_device( buff );
    EXPECT_STREQ( "TEST_NAME=0, ", buff );

    // Попытка повторной установки имени.
    dev.set_par_name( IDX, OFFSET, "TEST_NAME" );
    // Попытка установки имени для несуществующего индекса параметра.
    dev.set_par_name( IDX, OFFSET + 1, "TEST_NAME" );
    }

TEST( par_device, get_name_in_Lua )
    {
    par_device dev( 1 );
    EXPECT_STREQ( dev.get_name_in_Lua(), "" );
    }


TEST( valve_AS, get_lower_seat_offset )
    {
    valve_AS valve( "V1", device::DST_V_AS_MIXPROOF );
    EXPECT_EQ( valve.C_OPEN_S2, valve.get_lower_seat_offset() );
    valve.reverse_seat_connection = true;
    EXPECT_EQ( valve.C_OPEN_S3, valve.get_lower_seat_offset() );
    }

TEST( valve_AS, get_upper_seat_offset )
    {
    valve_AS valve( "V1", device::DST_V_AS_MIXPROOF );
    EXPECT_EQ( valve.C_OPEN_S3, valve.get_upper_seat_offset() );
    valve.reverse_seat_connection = true;
    EXPECT_EQ( valve.C_OPEN_S2, valve.get_upper_seat_offset() );
    }

TEST( valve_AS, get_valve_state )
    {
    valve_AS valve( "V1", device::DST_V_AS_MIXPROOF );

    EXPECT_EQ( valve.get_valve_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_off();
    // Задаем AO-область данных устройства.
    valve.init( 0, 0, 1, 1 );
    const auto AO_SIZE = 5;
    valve.AO_channels.int_read_values[ 0 ] = new int_2[ AO_SIZE ]{ 0 };
    valve.AO_channels.int_write_values[ 0 ] = new int_2[ AO_SIZE ]{ 0 };

    EXPECT_EQ( valve.get_valve_state(), valve::VALVE_STATE::V_OFF );

    valve.set_state( valve::VALVE_STATE::V_UPPER_SEAT );
    std::memcpy( *valve.AO_channels.int_read_values,
        *valve.AO_channels.int_write_values, AO_SIZE * sizeof( int_2 ) );
    EXPECT_EQ( valve.get_valve_state(), valve::VALVE_STATE::V_UPPER_SEAT );

    valve.set_state( valve::VALVE_STATE::V_LOWER_SEAT );
    std::memcpy( *valve.AO_channels.int_read_values,
        *valve.AO_channels.int_write_values, AO_SIZE * sizeof( int_2 ) );
    EXPECT_EQ( valve.get_valve_state(), valve::VALVE_STATE::V_LOWER_SEAT );

    valve.off();
    std::memcpy( *valve.AO_channels.int_read_values,
        *valve.AO_channels.int_write_values, AO_SIZE * sizeof( int_2 ) );
    EXPECT_EQ( valve.get_valve_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_AS, get_fb_state )
    {
    valve_AS valve( "V1", device::DST_V_AS_MIXPROOF );

    EXPECT_TRUE( valve.get_fb_state() );

    G_PAC_INFO()->emulation_off();
    // Задаем AO-область данных устройства.
    valve.init( 0, 0, 1, 1 );
    valve.AO_channels.int_read_values[ 0 ] = new int_2[ 5 ]{ 0 };

    EXPECT_FALSE( valve.get_fb_state() );

    G_PAC_INFO()->emulation_on();
    }

TEST( valve_AS, direct_off )
    {
    valve_AS valve( "V1", device::DST_V_AS_MIXPROOF );

    G_PAC_INFO()->emulation_off();
    // Задаем AO-область данных устройства.
    valve.init( 0, 0, 1, 1 );
    const auto AO_SIZE = 5;
    valve.AO_channels.int_read_values[ 0 ] = new int_2[ AO_SIZE ]{ 0 };
    valve.AO_channels.int_write_values[ 0 ] = new int_2[ AO_SIZE ]{ 0 };

    valve.direct_on();
    std::memcpy( *valve.AO_channels.int_read_values,
        *valve.AO_channels.int_write_values, AO_SIZE * sizeof( int_2 ) );
    EXPECT_EQ( valve.get_valve_state(), valve::VALVE_STATE::V_ON );

    valve.direct_off();
    std::memcpy( *valve.AO_channels.int_read_values,
        *valve.AO_channels.int_write_values, AO_SIZE * sizeof( int_2 ) );
    EXPECT_EQ( valve.get_valve_state(), valve::VALVE_STATE::V_OFF );

    G_PAC_INFO()->emulation_on();
    }


TEST( circuit_breaker, circuit_breaker )
    {
    const int BUFF_SIZE = 1000;
    char str_buff[ BUFF_SIZE ] = { 0 };
    circuit_breaker F1( "F1" );

    F1.save_device( str_buff );
    EXPECT_STREQ(
        "F1={M=0, ST=0, V=0, ERR=0, M=0, "
        "NOMINAL_CURRENT_CH={0,0,0,0}, LOAD_CURRENT_CH={0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0}, ERR_CH={0,0,0,0}},\n",
        str_buff );
    }

TEST( circuit_breaker, get_type_name )
    {
    circuit_breaker F1( "F1" );
    auto res = F1.get_type_name();
    EXPECT_STREQ( "Автоматический выключатель", res );
    }

TEST( circuit_breaker, set_cmd )
    {
    circuit_breaker F1( "F1" );

    EXPECT_EQ( F1.get_state(), 0 );

    F1.set_cmd( "ST", 0, 1. );
    EXPECT_EQ( F1.get_state(), 1 );

    F1.set_cmd( "ST", 0, 0. );
    EXPECT_EQ( F1.get_state(), 0 );
    }


TEST( power_unit, evaluate_io )
    {
    power_unit G1( "G1" );
    G1.init( 0, 0, 1, 1 );
    G1.AO_channels.int_write_values[ 0 ] = new int_2[ 7 ]{ 0 };
    G1.AI_channels.int_read_values[ 0 ] = new int_2[ 18 ]{ 0 };

    EXPECT_EQ( 0, G1.get_value() ); //Default value.

    const int BUFF_SIZE = 1000;
    char str_buff[ BUFF_SIZE ] = { 0 };
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    // ST = DC_not_OK, OUT_POWER_90, VOLTAGE = 25.7
    G1.AI_channels.int_read_values[ 0 ][ 0 ] = 0b1'0001'1001;
    // SUM_CURRENTS = 0.1
    G1.AI_channels.int_read_values[ 0 ][ 1 ] = 0b1'0000'0000;
    // Status, channel 1
    G1.AI_channels.int_read_values[ 0 ][ 2 ] = 0b0'0100'0000;
    // Nominal current, channel 1
    G1.AI_channels.int_read_values[ 0 ][ 3 ] = 0b0'0000'1000;
    // G1.AI_channels.int_read_values[ 0 ][ 4 ];
    // Load current, channel 1
    G1.AI_channels.int_read_values[ 0 ][ 5 ] = 0b0'0000'1000;
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0.10, NOMINAL_CURRENT_CH={2.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.8,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.1, VOLTAGE=25.7, OUT_POWER_90=1, ERR=1},\n",
        str_buff );

    std::memset( G1.AI_channels.int_read_values[ 0 ], 0, 18 );
    std::memset( G1.AO_channels.int_write_values[ 0 ], 0, 7 );

    G1.on();
    G1.evaluate_io();
    // Управляющий бит еще не должен быть включен (все каналы устанавливаются
    // в 1).
    EXPECT_EQ( 255, G1.AO_channels.int_write_values[ 0 ][ 3 ] );

    G1.set_cmd_time( get_millisec() - G1.WAIT_DATA_TIME - 10 );
    G1.evaluate_io();
    // Управляющий бит уже должен быть включен (устанавливается управляющий
    // бит в 1).
    EXPECT_EQ( 255, G1.AO_channels.int_write_values[ 0 ][ 3 ] );
    EXPECT_EQ( 128, G1.AO_channels.int_write_values[ 0 ][ 0 ] );

    G1.set_cmd_time( get_millisec() - G1.WAIT_CMD_TIME - 10 );
    G1.evaluate_io();
    // Управляющий бит уже должен быть отключен (сбрасывается управляющий бит
    // в 0, а также, так как нет записи в реальное устройство, сбрасываются
    // все каналы в 0).
    EXPECT_EQ( 0, G1.AO_channels.int_write_values[ 0 ][ 3 ] );
    EXPECT_EQ( 0, G1.AO_channels.int_write_values[ 0 ][ 0 ] );
    }

TEST( power_unit, get_type_name )
    {
    power_unit G1( "G1" );
    auto res = G1.get_type_name();
    EXPECT_STREQ( "Блок питания", res );
    }

TEST( power_unit, set_value )
    {
    power_unit G1( "G1" );
    // Функция set_value() ничего не делает - состояние не должно измениться.
    G1.set_value( 0.f );
    EXPECT_EQ( 0, G1.get_state() );
    }

TEST( power_unit, on )
    {
    const int BUFF_SIZE = 1000;
    char str_buff[ BUFF_SIZE ] = { 0 };
    power_unit G1( "G1" );
    G1.off();
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.on();
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,1,1,1,1,1,1,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    }

TEST( power_unit, set_cmd )
    {
    const int BUFF_SIZE = 1000;
    char str_buff[ BUFF_SIZE ] = { 0 };
    power_unit G1( "G1" );
    G1.off();

    G1.set_cmd( "ST", 0, 1 );       // On().
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,1,1,1,1,1,1,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.set_cmd( "ST", 0, 0 );       // Off().
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.set_cmd( "ST_CH", 1, 1 );    // Channel 1.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 2, 1 );    // Channel 2.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,1,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 3, 1 );    // Channel 3.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,1,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 4, 1 );    // Channel 4.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,1,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 5, 1 );    // Channel 5.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,1,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 6, 1 );    // Channel 6.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,1,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 7, 1 );    // Channel 7.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,1,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 8, 1 );    // Channel 8.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.set_cmd( "ST_CH", 9, 1 );    // Channel 9 - incorrect.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.off();
    G1.set_cmd( "NOMINAL_CURRENT_CH", 1, 1 );    // Nominal current 1.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 2, 1 );    // Nominal current 2.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,2.0,1.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 3, 1 );    // Nominal current 3.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,2.0,2.0,1.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 4, 1 );    // Nominal current 4.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,2.0,2.0,2.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 5, 1 );    // Nominal current 5.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,2.0,2.0,2.0,2.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 6, 1 );    // Nominal current 6.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,2.0,2.0,2.0,2.0,2.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 7, 1 );    // Nominal current 7.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,2.0,2.0,2.0,2.0,2.0,2.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 8, 1 );    // Nominal current 8.
    G1.evaluate_io();
    G1.save_device( str_buff );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    }

TEST( power_unit, decode_nominal_current )
    {
    // Test case from issue: verify that nominal current codes are decoded correctly
    const int BUFF_SIZE = 1000;
    std::array<char, BUFF_SIZE> str_buff = { 0 };
    power_unit G1( "G1" );
    G1.init( 0, 0, 1, 1 );
    G1.AO_channels.int_write_values[ 0 ] = new int_2[ 7 ]{ 0 };
    G1.AI_channels.int_read_values[ 0 ] = new int_2[ 18 ]{ 0 };

    // Set nominal current codes from the issue:
    // Ch1: code=5 (should show 8.0A)
    // Ch2: code=1 (should show 2.0A)
    // Ch3: code=2 (should show 3.8A)
    // Ch4: code=3 (should show 4.0A)

    // Nominal current ch1=5, ch2=1
    G1.AI_channels.int_read_values[ 0 ][ 3 ] = 0b00'101'001;  // ch1=5, ch2=1
    // Nominal current ch3=2, ch4=3
    G1.AI_channels.int_read_values[ 0 ][ 3 ] |= ( 0b00'010'011 << 8 );  // ch3=2, ch4=3

    G1.evaluate_io();
    G1.save_device( str_buff.data() );

    // Verify the decoded values match expectations
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={8.0,2.0,3.8,4.0,1.0,1.0,1.0,1.0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff.data() );
    }


TEST( analog_valve_ey, analog_valve_ey )
    {
    analog_valve_ey VC1( "VC1" );
    EXPECT_STREQ( VC1.get_name(), "VC1" );
    EXPECT_EQ( VC1.get_type(), device::DT_VC );
    EXPECT_EQ( VC1.get_sub_type(), device::DST_VC_EY );
    }

TEST( analog_valve_ey, set_property )
    {
    analog_valve_ey VC1( "VC1" );

    VC1.set_property( "UNKNOWN", nullptr );
    }

TEST( analog_valve_ey, set_string_property )
    {
    analog_valve_ey VC1( "VC1" );

    VC1.set_string_property( nullptr, nullptr );    // No crash.
    VC1.set_string_property( "UNKNOWN", "TEST" );   // No crash.

    // Valid property but invalid value.
    VC1.set_string_property( "TERMINAL", "TEST" );  // No crash.
    VC1.set_string_property( "TERMINAL", "Y1" );    // No crash, but no effect.

    // Valid property and valid value.
    G_DEVICE_MANAGER()->add_io_device(
        device::DT_EY, device::DST_CONV_AO2, "Y1", "Test device", "Y" );
    auto Y1 = G_DEVICE_MANAGER()->get_EY( "Y1" );
    VC1.set_string_property( "TERMINAL", "Y1" );
    EXPECT_EQ( Y1, VC1.conv );

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( analog_valve_ey, get_state_without_converter )
    {
    analog_valve_ey VC1( "VC1" );
    // No converter bound -> special error code.
    EXPECT_EQ( VC1.get_state(), -200 );

    // Should be no crash and no change in value.
    VC1.direct_on();
    EXPECT_EQ( 0.0f, VC1.get_value() );
    VC1.direct_off();
    EXPECT_EQ( 0.0f, VC1.get_value() );
    }

TEST( analog_valve_ey, direct_set_value_and_get_value_channel1_and2 )
    {
    analog_valve_ey VC1( "VC1" );
    converter_iolink_ao Y1( "Y1" );

    EXPECT_EQ( 0u, Y1.p_data_out->setpoint_ch1 );
    EXPECT_EQ( 0u, Y1.p_data_out->setpoint_ch2 );

    // Bind converter.
    VC1.set_property( "TERMINAL", &Y1 );

    // After binding both channels should be 4 mA (40'975).
    EXPECT_EQ( 40'975u, Y1.p_data_out->setpoint_ch1 );
    EXPECT_EQ( 40'975, Y1.p_data_out->setpoint_ch2 );

    // Select channel 1 and check routing to converter value 1.
    VC1.set_rt_par( 1, 1 );
    VC1.direct_set_value( 55.5f );
    EXPECT_FLOAT_EQ( 55.5f, Y1.get_channel_value( 1 ) );
    EXPECT_FLOAT_EQ( 55.5f, VC1.get_value() );

    // direct_on/off should route to channel 1
    VC1.direct_on();
    EXPECT_FLOAT_EQ( 100.0f, Y1.get_channel_value( 1 ) );
    EXPECT_FLOAT_EQ( 100.0f, VC1.get_value() );

    VC1.direct_off();
    EXPECT_FLOAT_EQ( 0.0f, Y1.get_channel_value( 1 ) );
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );

    // Switch to channel 2 and verify independent value path.
    VC1.set_rt_par( 1, 2 );
    VC1.direct_set_value( 33.3f );
    // Channel 1 remains unchanged at last set (0.0), channel 2 reflects new value.
    EXPECT_FLOAT_EQ( 0.0f, Y1.get_channel_value( 1 ) );
    EXPECT_FLOAT_EQ( 33.3f, Y1.get_channel_value( 2 ) );
    EXPECT_FLOAT_EQ( 33.3f, VC1.get_value() );

    // direct_on/off should route to channel 2 now.
    VC1.direct_on();
    EXPECT_FLOAT_EQ( 100.0f, Y1.get_channel_value( 2 ) );
    EXPECT_FLOAT_EQ( 100.0f, VC1.get_value() );

    VC1.direct_off();
    EXPECT_FLOAT_EQ( 0.0f, Y1.get_channel_value( 2 ) );
    EXPECT_FLOAT_EQ( 0.0f, VC1.get_value() );
    }

TEST( analog_valve_ey, set_rt_par_validation )
    {
    analog_valve_ey VC1( "VC1" );
    converter_iolink_ao Y1( "Y1" );
    VC1.set_property( "TERMINAL", &Y1 );

    // Set valid channel first.
    VC1.set_rt_par( 1, 1 );
    VC1.direct_set_value( 12.5f );
    EXPECT_FLOAT_EQ( 12.5f, Y1.get_channel_value( 1 ) );

    // Try to set invalid channel, ey_number must remain unchanged (still 1).
    VC1.set_rt_par( 1, 3 );
    VC1.direct_set_value( 66.6f );
    EXPECT_FLOAT_EQ( 66.6f, Y1.get_channel_value( 1 ) );   // Affects channel 1

    // Another invalid channel value below range.
    VC1.set_rt_par( 1, 0 );
    VC1.direct_set_value( 77.7f );
    EXPECT_FLOAT_EQ( 77.7f, Y1.get_channel_value( 1 ) );
    }

TEST( analog_valve_ey, get_state_with_converter )
    {
    analog_valve_ey VC1( "VC1" );
    converter_iolink_ao Y1( "Y1" );
    VC1.set_property( "TERMINAL", &Y1 );

    // Default state from converter.
    EXPECT_EQ( VC1.get_state(), Y1.get_state() );

    // Change converter state via its API and compare.
    Y1.direct_on();
    EXPECT_EQ( VC1.get_state(), Y1.get_state() );

    Y1.direct_off();
    EXPECT_EQ( VC1.get_state(), Y1.get_state() );
    }


TEST( converter_iolink_ao, constructor )
    {
    converter_iolink_ao Y1( "Y1" );
    EXPECT_STREQ( Y1.get_name(), "Y1" );
    EXPECT_EQ( Y1.get_type(), device::DT_EY );
    EXPECT_EQ( Y1.get_sub_type(), device::DST_CONV_AO2 );
    }

TEST( converter_iolink_ao, direct_on_off )
    {
    converter_iolink_ao Y1( "Y1" );

    // Test initial state.
    EXPECT_EQ( Y1.get_state(), 0 );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 0.0f );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 0.0f );

    // Test turning on.
    Y1.direct_on();
    EXPECT_EQ( Y1.get_state(), 1 );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 100.0f );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 100.0f );


    // Test turning off
    Y1.direct_off();
    EXPECT_EQ( Y1.get_state(), 0 );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 0.0f );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 0.0f );
    }

TEST( converter_iolink_ao, set_value )
    {
    converter_iolink_ao Y1( "Y1" );

    // Test setting values for non existing channel.
    Y1.set_channel_value( 3, 200.0f );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 0.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch1, 0u );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 0.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch2, 0u );

    // Test setting different values for channel 1.
    Y1.set_channel_value( 1, 10.0f );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 10.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch1, 57'365u );

    Y1.set_channel_value( 1, 100.0f );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 100.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch1, 8'270u );

    Y1.set_channel_value( 1, 200.0f );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 100.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch1, 8'270u );

    Y1.set_channel_value( 1, 0.0f );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 0.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch1, 40'975u );

    // Test setting different values for channel 2.
    Y1.set_channel_value( 2, 10.0f );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 10.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch2, 57'365u );

    Y1.set_channel_value( 2, 0.0f );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 0.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch2, 40'975u );

    Y1.set_channel_value( 2, 200.0f );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 100.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch2, 8'270u );

    // Test getting values for non existing channel.
    EXPECT_EQ( Y1.get_channel_value( 3 ), 0.0f );

    // Test on/off.
    Y1.direct_on();
    EXPECT_EQ( Y1.get_channel_value( 1 ), 100.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch1, 8'270u );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 100.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch2, 8'270u );

    Y1.direct_off();
    EXPECT_EQ( Y1.get_channel_value( 1 ), 0.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch1, 40'975u );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 0.0f );
    EXPECT_EQ( Y1.p_data_out->setpoint_ch2, 40'975u );
    }

TEST( converter_iolink_ao, set_cmd )
    {
    converter_iolink_ao Y1( "Y1" );

    // Test state command.
    EXPECT_EQ( Y1.set_cmd( "ST", 0, 1 ), 0 );

    // Test value command.
    EXPECT_EQ( Y1.set_cmd( "CH", 1, 75.5 ), 0 );
    EXPECT_EQ( Y1.get_channel_value( 1 ), 75.5f );

    EXPECT_EQ( Y1.set_cmd( "CH", 2, 25.5 ), 0 );
    EXPECT_EQ( Y1.get_channel_value( 2 ), 25.5f );

    // Test unknown command
    EXPECT_EQ( Y1.set_cmd( "UNKNOWN", 0, 1 ), 1 );
    }

TEST( converter_iolink_ao, save_device_ex )
    {
    converter_iolink_ao Y1( "Y1" );
    char buff[ 1000 ]{};

    auto len = Y1.save_device( buff );
    EXPECT_GT( len, 0 );
    EXPECT_STRCASEEQ( buff,
        "Y1={M=0, ST=0, V=0, E=0, M_EXP=1.0, S_DEV=0.2, CH={0.00,0.00}},\n" );

    Y1.set_channel_value( 1, 42.5f );
    len = Y1.save_device( buff );
    EXPECT_GT( len, 0 );
    EXPECT_STRCASEEQ( buff,
        "Y1={M=0, ST=1, V=0, E=0, M_EXP=1.0, S_DEV=0.2, CH={42.50,0.00}},\n" );

    Y1.set_channel_value( 2, 21.5f );
    len = Y1.save_device( buff );
    EXPECT_GT( len, 0 );
    EXPECT_STRCASEEQ( buff,
        "Y1={M=0, ST=1, V=0, E=0, M_EXP=1.0, S_DEV=0.2, CH={42.50,21.50}},\n" );
    }


TEST_F( iolink_dev_test, converter_iolink_ao_evaluate_io )
    {
    converter_iolink_ao Y1( "Y1" );
    EXPECT_EQ( Y1.get_value(), .0f );

    G_PAC_INFO()->emulation_off();
    init_channels( Y1 );
    set_iol_state_to_OK( Y1 );

    *Y1.AI_channels.int_read_values[ 0 ] = 0b100000;
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), -2 );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, converter_iolink_ao_get_error_description )
    {
    converter_iolink_ao Y1( "Y1" );
    test_dev_err( Y1, Y1, 0 );

    G_PAC_INFO()->emulation_off();

    *Y1.AI_channels.int_read_values[ 0 ] = 0b10000;
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), -1 );
    EXPECT_STREQ( Y1.get_error_description(), "требуется обслуживание" );

    *Y1.AI_channels.int_read_values[ 0 ] = 0b100000;
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), -2 );
    EXPECT_STREQ( Y1.get_error_description(), "не соответствует спецификации" );

    *Y1.AI_channels.int_read_values[ 0 ] = 0b110000;
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), -3 );
    EXPECT_STREQ( Y1.get_error_description(), "функциональная проверка" );

    *Y1.AI_channels.int_read_values[ 0 ] = 0b1000000;
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), -4 );
    EXPECT_STREQ( Y1.get_error_description(), "отказ" );

    G_PAC_INFO()->emulation_on();
    }

TEST_F( iolink_dev_test, converter_iolink_ao_get_state )
    {
    // In emulator mode, state is always 0.
    converter_iolink_ao Y1( "Y1" );
    EXPECT_EQ( Y1.get_state(), 0 );
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), 0 );
    init_channels( Y1 );
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), 0 );

    Y1.set_cmd( "ST", 0, 1 );
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), 1 );

    Y1.set_cmd( "ST", 0, -100 );
    Y1.evaluate_io();
    EXPECT_EQ( Y1.get_state(), -100 );
    }


TEST_F( iolink_dev_test, analog_valve_iolink_get_error_description_and_state )
    {
    analog_valve_iolink VC1( "VC1" );
    // Включаем параметр P_FB, чтобы проверка ошибок IO-Link была активна.
    VC1.set_par( static_cast<int>( analog_valve_iolink::PAR_CONSTANTS::P_FB ),
        0, 1.0f );

    // Повторно используем универсальную проверку ошибок IO-Link.
    // Ожидаемое состояние при OK — in_info.status, по умолчанию 0.
    test_dev_err( VC1, VC1, 0 );
    }

TEST_F( iolink_dev_test, analog_valve_iolink_get_state_respects_P_FB )
    {
    analog_valve_iolink VC1( "VC1" );

    G_PAC_INFO()->emulation_off();
    // Настраиваем только AI-канал для проверки IOLINK state.
    init_channels( VC1 );

    // Включаем обратную связь (P_FB=1).
    VC1.set_par( static_cast<int>( analog_valve_iolink::PAR_CONSTANTS::P_FB ),
        0, 1.0f );
    // Без подключения IO-Link и P_FB=1 — ожидаем ошибку NOTCONNECTED.
    VC1.evaluate_io();
    EXPECT_EQ( VC1.get_state(), -io_device::IOLINKSTATE::NOTCONNECTED );

    // Отключаем обратную связь (P_FB=0) — ошибки устройства игнорируются,
    // get_state() должен вернуть 1.
    VC1.set_par( static_cast<int>( analog_valve_iolink::PAR_CONSTANTS::P_FB ),
        0, 0.0f );
    VC1.evaluate_io();
    EXPECT_EQ( VC1.get_state(), 1 );

    // Подключено, но данные не валидны — при P_FB=0 также должен вернуть 1.
    *VC1.AI_channels.int_module_read_values[ 0 ] = 0b1; // Только connected.
    VC1.evaluate_io();
    EXPECT_EQ( VC1.get_state(), 1 );

    // Состояние OK — возвращаем in_info.status (по умолчанию 0).
    set_iol_state_to_OK( VC1 );
    VC1.evaluate_io();
    EXPECT_EQ( VC1.get_state(), 0 );

    G_PAC_INFO()->emulation_on();
    }


TEST( device_manager, get_EY )
    {
    // Cleanup before test.
    G_DEVICE_MANAGER()->clear_io_devices();

    // Add a converter device.
    auto* Y1 = G_DEVICE_MANAGER()->add_io_device(
        device::DT_EY, device::DST_CONV_AO2, "CAB1EY10", "", "IFM.DP1213" );

    EXPECT_NE( Y1, nullptr );

    // Test accessor function.
    auto* retrieved_Y1 = EY( "CAB1EY10" );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( retrieved_Y1 ) );

    // Test non-existent device.
    auto* non_existent = EY( "NON_EXISTENT" );
    EXPECT_EQ( STUB(), dynamic_cast<dev_stub*>( non_existent ) );
    }


class node_dev_set_cmd_test : public ::testing::Test
    {
    public:

        static int run_cmd_exit_code_expected(
            [[maybe_unused]] const char* cmd, int expected = 0 )
            {
            return expected;
            }

        static std::string get_A1_ipv4()
            {
            return "127.0.0.1";
            }

        static std::string get_bad_A1_ipv4()
            {
            return "127.0.0.344";
            }

    protected:
        io_manager::io_node node{ io_manager::io_node::TYPES::PHOENIX_BK_ETH,
            1, "127.0.0.1", "A100", 1, 1, 1, 32, 1, 1 };
    };

TEST( node_dev, basic_functionality )
    {
    io_manager::io_node nd( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.10", "A100", 0, 0, 0, 0, 0, 0 );
    io_manager::io_node nd_2( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.11", "A200", 0, 0, 0, 0, 0, 0 );

    // Добавление устройства node_dev.
    auto* io_dev = G_DEVICE_MANAGER()->add_io_device(
        device::DT_NODE, device::DST_NODE, "A100", "Test node device", "" );
    ASSERT_EQ( io_dev, nullptr );

    // Получение указателя на @node_dev для доступа к специфическим методам.
    auto node = dynamic_cast<node_dev*>(
        G_DEVICE_MANAGER()->get_device( "A100" ) );
    ASSERT_NE( node, nullptr );

    // Проверка получения IP-адреса в случае, когда нет привязанного узла.
    EXPECT_STREQ( node->get_ip(), "" );

    // Проверка при передаче пустого указателя.
    node->set_io_node( nullptr );
    EXPECT_STREQ( node->get_ip(), "" );

    // Проверка при передаче узла с некорректным IP-адресом.
    auto r = fmt::format_to_n( nd.ip_address, sizeof( nd.ip_address ) - 1,
        "34" );
    *r.out = 0;

    node->set_io_node( &nd );
    EXPECT_STREQ( node->get_ip(), "" );

    // Проверка при передаче узла с корректным IP-адресом.
    r = fmt::format_to_n( nd.ip_address, sizeof( nd.ip_address ) - 1,
        "127.0.0.10" );
    *r.out = 0;
    node->set_io_node( &nd );
    // Проверка получения IP-адреса.
    EXPECT_STREQ( node->get_ip(), "127.0.0.10" );
    EXPECT_TRUE( node_dev::get_A1_ipv4().empty() );

    // Проверка при повторной привязке узла.
    node->set_io_node( &nd_2 );
    EXPECT_STREQ( node->get_ip(), "127.0.0.10" );

    G_PAC_INFO()->emulation_off();

    // Проверка evaluate_io().
    node->evaluate_io();

    // Сохранение устройства.
    const int BUFF_SIZE = 200;
    std::array <char, BUFF_SIZE> buff{};
    node->save_device( buff.data() );
    EXPECT_STREQ( buff.data(),
        "A100={ST=-1, WEB=0, STARTUP=0, IP='127.0.0.10'},\n" );

    // Очистка после теста.
    G_DEVICE_MANAGER()->clear_io_devices();
    G_ERRORS_MANAGER->clear();
    G_PAC_INFO()->emulation_on();
    }

TEST( node_dev, get_A1_ipv4 )
    {
    // Инициализация io_manager.
    uni_io_manager mngr;
    mngr.init( 1 );
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    const auto TEST_IP_10 = "127.0.0.10";


    // Если нет узлов, то при получении IP-адреса возвращается пустая строка.
    auto res = node_dev::get_A1_ipv4();
    EXPECT_TRUE( res.empty() );

    // Если первый узел не "А1", то при получении IP-адреса возвращается пустая
    // строка.
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1,
        TEST_IP_10, "A11", 0, 0, 0, 0, 0, 0 );
    res = node_dev::get_A1_ipv4();
    EXPECT_TRUE( res.empty() );

    // Используем корректное для контроллера название: "A1".
    mngr.clear_nodes();
    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1,
        TEST_IP_10, "A1", 0, 0, 0, 0, 0, 0 );
    res = node_dev::get_A1_ipv4();
    EXPECT_STREQ( res.c_str(), TEST_IP_10);


    // Очистка после теста.
    G_DEVICE_MANAGER()->clear_io_devices();
    io_manager::replace_instance( prev_mngr );
    }

TEST_F( node_dev_set_cmd_test, set_cmd_web )
    {
    node_dev dev( "A100" );

    // Команда WEB должна работать только для индекса 0 (второй параметр).
    EXPECT_EQ( 1, dev.set_cmd( "WEB", 1, 1 ) );

    // Нет узла, команда должна вернуть ошибку.
    EXPECT_EQ( 1, dev.set_cmd( "WEB", 0, 1 ) );
    EXPECT_EQ( 1, dev.set_cmd( "WEB", 0, 0 ) );

    auto get_local_ipv4_hook = subhook_new(
        reinterpret_cast<void*>( &node_dev::get_A1_ipv4 ),
        reinterpret_cast<void*>( &node_dev_set_cmd_test::get_A1_ipv4 ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_local_ipv4_hook );

    dev.set_io_node( &node );

#ifdef WIN_OS
    // Узел есть, но команда должна выполниться неуспешно, так как команды для
    // проброса портов на Windows нет (пустые строки).
    EXPECT_EQ( 1, dev.set_cmd( "WEB", 0, 1 ) );
#else
    auto run_cmd_0_hook = subhook_new(
        reinterpret_cast<void*>( &node_dev::run_cmd_exit_code ),
        reinterpret_cast<void*>(
            &node_dev_set_cmd_test::run_cmd_exit_code_expected ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( run_cmd_0_hook );

    // Включаем проброс портов, команда должна выполниться успешно,
    // возвращая 0.
    EXPECT_EQ( 0, dev.set_cmd( "WEB", 0, 1 ) );

    // Команда должна работать повторно, возвращая 0.
    EXPECT_EQ( 0, dev.set_cmd( "WEB", 0, 1 ) );

    // Выключаем проброс портов, команда должна выполниться успешно,
    // возвращая 0.
    EXPECT_EQ( 0, dev.set_cmd( "WEB", 0, 0 ) );

    subhook_remove( run_cmd_0_hook );
    subhook_free( run_cmd_0_hook );
#endif // WIN_OS

    subhook_remove( get_local_ipv4_hook );
    subhook_free( get_local_ipv4_hook );
    }

TEST_F( node_dev_set_cmd_test, set_cmd_web_sudo_available )
    {
#ifdef WIN_OS
    GTEST_SKIP() << "Linux only test";
#else
    node_dev dev( "A100" );

    auto get_local_ipv4_hook = subhook_new(
        reinterpret_cast<void*>( &node_dev::get_A1_ipv4 ),
        reinterpret_cast<void*>( &node_dev_set_cmd_test::get_A1_ipv4 ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_local_ipv4_hook );

    dev.set_io_node( &node );

    subhook_remove( get_local_ipv4_hook );
    subhook_free( get_local_ipv4_hook );

    // В Linux проброс портов должен работать, так как команда для проброса
    // портов есть.
    // Проверяем на права.
    auto res = node_dev::check_sudo_available();
    if ( !res )
        {
        // Получаем ошибку: `sudo is not available without a password`
        // и завершаем тест.
        // Пишем соответствующее сообщение в тестовом отчете.
        GTEST_SKIP() << "sudo is not available without a password";

        return;
        }
    else
        {
        GTEST_LOG_( INFO ) <<
            "Выполняется реальный проброс портов через sudo /usr/sbin/iptables";
        // Проверяем, что команды выполняется успешно.
        EXPECT_EQ( 0, dev.set_cmd( "WEB", 0, 1 ) );
        EXPECT_EQ( 0, dev.set_cmd( "WEB", 0, 0 ) );
        }
#endif // WIN_OS
    }

TEST_F( node_dev_set_cmd_test, set_cmd_web_bad_controller_ip )
    {
    node_dev dev( "A100" );

    auto get_local_bad_ipv4_hook = subhook_new(
        reinterpret_cast<void*>( &node_dev::get_A1_ipv4 ),
        reinterpret_cast<void*>( &node_dev_set_cmd_test::get_bad_A1_ipv4 ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_local_bad_ipv4_hook );

    dev.set_io_node( &node );
    EXPECT_STREQ( dev.get_controller_ip(), "" );

    // Узел есть, но команда должна выполниться неуспешно, так как команды для
    // проброса портов - пустые строки.
    EXPECT_EQ( 1, dev.set_cmd( "WEB", 0, 1 ) );

    subhook_remove( get_local_bad_ipv4_hook );
    subhook_free( get_local_bad_ipv4_hook );
    }

TEST_F( node_dev_set_cmd_test, set_cmd_startup )
    {
    node_dev dev( "A100" );

    // Команда STARTUP должна работать.
    EXPECT_EQ( 0, dev.set_cmd( "STARTUP", 0, 1 ) );
    }

namespace
    {
    bool exists_no_file( const std::filesystem::path& p )
        {
        return false;
        }
    }

TEST_F( node_dev_set_cmd_test, set_cmd_to_device )
    {
    node_dev dev( "A100" );

    // Команды, которые не обрабатываются устройством, передаются дальше.
    // Команды 'TEST' нет, должна вернуться ошибка.
    EXPECT_EQ( 1, dev.set_cmd( "TEST", 0, 1 ) );
    }

TEST( node_dev, run_cmd_exit_code )
    {
    node_dev dev( "A100" );
#ifdef WIN_OS
    SetConsoleOutputCP( CP_UTF8 ); // 65001
    SetConsoleCP( CP_UTF8 );
    setlocale( LC_ALL, ".UTF-8" );
#else
    setlocale( LC_ALL, "en_US.UTF-8" );
#endif

    auto res = node_dev::run_cmd_exit_code( "lls" );
    // Verify output contains the expected debug message pattern with time.
    auto reference_out =
#ifdef WIN_OS
        "command result ('lls'): "
        "'lls' is not recognized as an internal or external command,\n"
        "operable program or batch file.";
#else
        "command result ('lls'): sh: 1: lls: not found";
#endif
    EXPECT_STREQ( node_dev::get_cmd_output(), reference_out );
    EXPECT_NE( 0, res );

    // Check "command result ('cmd') not found" error.
    auto exists_no_file_hook = subhook_new(
        reinterpret_cast<void*>( static_cast<bool( * )
            ( const std::filesystem::path& )>( &std::filesystem::exists ) ),
        reinterpret_cast<void*>( &exists_no_file ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( exists_no_file_hook );
    res = node_dev::run_cmd_exit_code( "lls" );
    EXPECT_NE( 0, res );

    subhook_remove( exists_no_file_hook );
    subhook_free( exists_no_file_hook );
    }


TEST( timer_manager, get_count )
    {
    const auto TIMERS_COUNT = 10;
    const timer_manager TM1( TIMERS_COUNT );
    EXPECT_EQ( TM1.get_count(), TIMERS_COUNT );
    }


class DO1_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            G_PAC_INFO()->emulation_off();

            mngr.init( 1 );
            prev_mngr = io_manager::replace_instance( &mngr );
            mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
                1, "127.0.0.1", "A100", 1, 0, 0, 0, 0, 0 );

            dev.init_and_alloc( 1, 0, 0, 0 );
            dev.init_channel( io_device::IO_channels::CT_DO, 0, 0, 0 );

            // Set DO channel value to 1.
            *dev.DO_channels.char_write_values[ 0 ] = 1;

            node = mngr.get_node( 0 );
            node->is_active = true;
            node->state = io_manager::io_node::ST_OK;
            node->status_register = 0;
            };

        void TearDown() override
            {
            io_manager::replace_instance( prev_mngr );
            G_PAC_INFO()->emulation_on();
            };

        DO1 dev{ "TEST_DO", device::DT_DO, device::DST_DO };
        io_manager::io_node* node{};

    private:
        uni_io_manager mngr;
        io_manager* prev_mngr;
    };

TEST_F( DO1_test, get_state_with_node_check_ok )
    {
    // Node is OK, should return channel value.
    dev.evaluate_io();
    EXPECT_EQ( 1, dev.get_state() );

    // Set DO channel value to 0.
    *dev.DO_channels.char_write_values[ 0 ] = 0;
    dev.evaluate_io();

    // Node is OK, should return channel value.
    EXPECT_EQ( 0, dev.get_state() );
    }

TEST_F( DO1_test, get_state_with_node_pp_mode )
    {
    node->status_register = 0x0010;  // PP mode active.
    dev.evaluate_io();

    // Node in PP mode, should return error (-1).
    EXPECT_EQ( -1, dev.get_state() );
    }

TEST_F( DO1_test, get_state_with_node_not_connected )
    {
    node->state = io_manager::io_node::ST_NO_CONNECT;
    dev.evaluate_io();

    // Node not connected, should not return error (-1).
    EXPECT_EQ( 1, dev.get_state() );
    }

TEST_F( DO1_test, get_state_with_node_not_active )
    {
    node->is_active = false;  // Node not active.
    dev.evaluate_io();

    // Node not active, should not return error (-1).
    EXPECT_EQ( 1, dev.get_state() );
    }
