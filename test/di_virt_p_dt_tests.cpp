#include "di_virt_p_dt_tests.h"
#include "scoped_fake_clock.h"

#include <array>

using namespace ::testing;

// P_DT = 0 keeps the current passthrough behaviour of DI_VIRT: every raw
// state written by the inter-controller exchange becomes visible
// immediately, without waiting for the filter.
TEST( virtual_device_di_virt_p_dt, p_dt_zero_keeps_passthrough )
    {
    virtual_device di_virt( "LINE1DI101", device::DT_DI,
        device::DST_DI_VIRT );

    EXPECT_EQ( 0, di_virt.get_state() );

    di_virt.set_state( 1 );
    di_virt.evaluate_io();
    EXPECT_EQ( 1, di_virt.get_state() );
    EXPECT_TRUE( di_virt.is_active() );

    di_virt.set_state( 0 );
    di_virt.evaluate_io();
    EXPECT_EQ( 0, di_virt.get_state() );
    EXPECT_FALSE( di_virt.is_active() );
    }

// DI_VIRT must expose a named P_DT parameter so recipes and Lua can
// configure the filter, same as for DI1.
TEST( virtual_device_di_virt_p_dt, save_device_exposes_p_dt )
    {
    std::array<char, 200> buff{};
    virtual_device di_virt( "LINE1DI101", device::DT_DI,
        device::DST_DI_VIRT );

    di_virt.save_device( buff.data() );
    EXPECT_STREQ( "LINE1DI101={M=0, ST=0, P_DT=0},\n", buff.data() );

    buff.fill( 0 );
    virtual_device do_virt( "LINE1DO101", device::DT_DO,
        device::DST_DO_VIRT );

    do_virt.save_device( buff.data() );
    EXPECT_STREQ( "LINE1DO101={M=0, ST=0},\n", buff.data() );
    }

// A short flicker of the signal between controllers (shorter than P_DT)
// must not reach the consumer state of DI_VIRT.
TEST( virtual_device_di_virt_p_dt, short_flicker_is_suppressed )
    {
    scoped_fake_clock vclock;
    virtual_device di_virt( "LINE1DI101", device::DT_DI,
        device::DST_DI_VIRT );
    ASSERT_EQ( 0, di_virt.set_cmd( "P_DT", 0, 500.0f ) );

    // Эмуляция межконтроллерного обмена: сигнал переключается
    // с периодом гораздо короче P_DT.
    for ( int i = 0; i < 10; i++ )
        {
        di_virt.set_state( 1 );
        vclock.advance( 100 );
        di_virt.evaluate_io();

        di_virt.set_state( 0 );
        vclock.advance( 100 );
        di_virt.evaluate_io();

        EXPECT_EQ( 0, di_virt.get_state() );
        EXPECT_FALSE( di_virt.is_active() );
        }
    }

// A stable change of DI_VIRT state passes the filter only after the new
// raw state persists longer than P_DT, then holds until the raw state
// changes again.
TEST( virtual_device_di_virt_p_dt, change_persists_after_p_dt )
    {
    scoped_fake_clock vclock;
    virtual_device di_virt( "LINE1DI101", device::DT_DI,
        device::DST_DI_VIRT );
    ASSERT_EQ( 0, di_virt.set_cmd( "P_DT", 0, 500.0f ) );

    di_virt.set_state( 1 );

    vclock.advance( 400 );
    di_virt.evaluate_io();
    EXPECT_EQ( 0, di_virt.get_state() );

    // Состояние отличается от отфильтрованного дольше P_DT - проходит.
    vclock.advance( 200 );
    di_virt.evaluate_io();
    EXPECT_EQ( 1, di_virt.get_state() );
    EXPECT_TRUE( di_virt.is_active() );

    // Удержание сигнала сохраняет состояние.
    vclock.advance( 1000 );
    di_virt.evaluate_io();
    EXPECT_EQ( 1, di_virt.get_state() );

    di_virt.set_state( 0 );
    vclock.advance( 501 );
    di_virt.evaluate_io();
    EXPECT_EQ( 0, di_virt.get_state() );
    EXPECT_FALSE( di_virt.is_active() );
    }

// The filter belongs to DI_VIRT only: other virtual devices keep the
// immediate passthrough behaviour and have no P_DT parameter at all.
TEST( virtual_device_di_virt_p_dt, other_virtual_types_unaffected )
    {
    scoped_fake_clock vclock;
    virtual_device do_virt( "LINE1DO101", device::DT_DO,
        device::DST_DO_VIRT );

    // У DO_VIRT нет параметров, поэтому P_DT на нём задать нельзя.
    EXPECT_EQ( 1, do_virt.set_cmd( "P_DT", 0, 500.0f ) );

    do_virt.set_state( 1 );
    vclock.advance( 1000 );
    do_virt.evaluate_io();
    EXPECT_EQ( 1, do_virt.get_state() );

    do_virt.direct_on();
    EXPECT_EQ( 1, do_virt.get_state() );

    do_virt.direct_off();
    EXPECT_EQ( 0, do_virt.get_state() );
    }
