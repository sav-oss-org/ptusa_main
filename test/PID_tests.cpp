#include "PID_tests.h"
#include "g_errors.h"

using namespace ::testing;

TEST( PID, get_actuator )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_PID, "TC1",
        "Test controller", "T" );
    ASSERT_EQ( nullptr, res );
    auto p1_dev = G_DEVICE_MANAGER()->get_C( "TC1" );
    ASSERT_NE( nullptr, p1_dev );
    auto p1 = dynamic_cast<PID*>( p1_dev );
    ASSERT_NE( nullptr, p1 );

    ASSERT_EQ( nullptr, p1->get_actuator() );

    p1->set_string_property( "OUT_VALUE", "TC1" );
    auto m = p1->get_actuator();
    ASSERT_EQ( nullptr, m );

    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, "M1", "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    auto m1 = G_DEVICE_MANAGER()->get_M( "M1" );
    ASSERT_NE( nullptr, m1 );
    p1->set_string_property( "OUT_VALUE", "M1" );
    m = p1->get_actuator();
    ASSERT_NE( nullptr, m );
    ASSERT_EQ( m1, m );

    G_DEVICE_MANAGER()->clear_io_devices();
    }

void init_PID_parameters( PID& test_PID, float min_value = 0.f )
    {
    test_PID.init_param( PID::P_k, 1 );
    test_PID.init_param( PID::P_Ti, 15 );
    test_PID.init_param( PID::P_Td, 0.01f );
    test_PID.init_param( PID::P_dt, 1000 );
    test_PID.init_param( PID::P_max, 100 );
    test_PID.init_param( PID::P_min, 0 );
    test_PID.init_param( PID::P_acceleration_time, 30 );
    test_PID.init_param( PID::P_is_manual_mode, 0 );
    test_PID.init_param( PID::P_U_manual, 65 );

    test_PID.init_param( PID::P_out_max, 100 );
    test_PID.init_param( PID::P_out_min, min_value );
    test_PID.init_param( PID::P_is_zero_start, 1 );
    test_PID.save_param();

    test_PID.set( 100.f );
    }

TEST( PID, direct_set_value )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_PID, "TC1",
        "Test controller", "T" );
    ASSERT_EQ( nullptr, res );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "TE1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, "M1", "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    G_DEVICE_MANAGER()->print();

    auto m1 = G_DEVICE_MANAGER()->get_M( "M1" );
    ASSERT_NE( nullptr, m1 );
    auto p1_dev = G_DEVICE_MANAGER()->get_C( "TC1" );
    ASSERT_NE( nullptr, p1_dev );
    PID* p1 = dynamic_cast<PID*>( p1_dev );
    ASSERT_NE( nullptr, p1 );

    p1->set_string_property( "IN_VALUE", "TE1" );
    p1->set_string_property( "OUT_VALUE", "M1" );

    init_PID_parameters( *p1 );

    //PID switched off.
    p1_dev->set_value( 10 );
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 0, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    //PID switched on.
    p1_dev->on();
    p1_dev->set_value( 10 );
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 1, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::ON ), p1_dev->get_state() );

    //PID switched off.
    p1_dev->off();
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 1, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::STOPPING ), p1_dev->get_state() );
    p1_dev->off();
    EXPECT_EQ( 0, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    //Нельзя установить состояние "Выключаюсь" - в него ПИД-регулятор переходит
    //при выключении из работы.
    p1_dev->set_state( static_cast<int>( PID::STATE::STOPPING ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    p1_dev->set_state( static_cast<int>( PID::STATE::ON ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::ON ), p1_dev->get_state() );

    //ПИД-регулятор переходит в состояние "Выключаюсь" при выключении из
    //состояния "Работа".
    p1_dev->set_state( static_cast<int>( PID::STATE::OFF ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::STOPPING ), p1_dev->get_state() );
    //ПИД-регулятор переходит в состояние "Выключено" при выключении из
    //состояни "Выключаюсь".
    p1_dev->set_state( static_cast<int>( PID::STATE::OFF ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    const int BUFF_SIZE = 512;
    char buff[ BUFF_SIZE ] = { 0 };

    p1->save_device( buff );
    EXPECT_STREQ(
        "TC1={M=0, ST=0, V=0, Z=10.00, IN_VALUE='TE1', OUT_VALUE='M1', "
        "P_k=1, P_Ti=15, P_Td=0.01, "
        "P_dt=1000, P_max=100, P_min=0, P_acceleration_time=30, "
        "P_is_manual_mode=0, P_U_manual=65, P_k2=0, P_Ti2=0, P_Td2=0, "
        "P_out_max=100, P_out_min=0, P_is_reverse=0, P_is_zero_start=1},\n",
        buff );

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( PID, save_device )
    {
    const int BUFF_SIZE = 512;
    char buff[ BUFF_SIZE ] = { 0 };

    auto REF_STR =
        "t.PID1=\n"
        "\t{\n"
        "\tST=0,\n"
        "\tS_PAR_F=\n"
        "\t\t{\n"
        "\t\t0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \n"
        "\t\t},\n"
        "\tRT_PAR_F={ 0.00, 0.00 }\n"
        "\t}\n";

    PID pid1( 1 );
    pid1.save_device( buff );
    EXPECT_STREQ( buff, REF_STR );

    PID pid2( "PID2" );
    pid2.save_device( buff );
    EXPECT_STREQ(
        "PID2={M=0, ST=0, V=0, Z=0.00, P_k=0, P_Ti=0, P_Td=0, P_dt=0, "
        "P_max=0, P_min=0, P_acceleration_time=0, P_is_manual_mode=0, "
        "P_U_manual=0, P_k2=0, P_Ti2=0, P_Td2=0, P_out_max=0, P_out_min=0, "
        "P_is_reverse=0, P_is_zero_start=0},\n", buff );
    }


TEST( PID, eval )
    {
    PID test_PID( "PID1" );

    // Корректная работа регулятора, когда параметры не инициализированы.
    test_PID.on();
    auto res = test_PID.eval( 0 );
    EXPECT_EQ( res, .0f );

    auto MIN_VALUE = 35.f;
    init_PID_parameters( test_PID, MIN_VALUE );
    test_PID.reset();
    test_PID.on();

    // Должны получить значение выхода, которое больше минимального.
    // Так должен работать ПИД во время разгона.
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    res = test_PID.eval( 0 );
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_GT( res, MIN_VALUE );

    DeltaMilliSecSubHooker::set_millisec( 2UL * 1001UL );
    res = test_PID.eval( 1 );
    EXPECT_GT( res, MIN_VALUE );
    DeltaMilliSecSubHooker::set_default_time();

    DeltaMilliSecSubHooker::set_millisec( 3UL * 1001UL );
    res = test_PID.eval( 2 );
    EXPECT_GT( res, MIN_VALUE );
    DeltaMilliSecSubHooker::set_default_time();
    }

namespace {

void init_PID_for_output_limit_test( PID& pid, float out_min, float out_max )
    {
    pid.init_param( PID::P_k, 1.f );
    pid.init_param( PID::P_Ti, 6.f );
    pid.init_param( PID::P_Td, 0.01f );
    pid.init_param( PID::P_dt, 1000.f );
    pid.init_param( PID::P_max, 100.f );
    pid.init_param( PID::P_min, 0.f );
    pid.init_param( PID::P_acceleration_time, 0.f );
    pid.init_param( PID::P_is_manual_mode, 0.f );
    pid.init_param( PID::P_out_min, out_min );
    pid.init_param( PID::P_out_max, out_max );
    pid.init_param( PID::P_is_zero_start, 1.f );
    pid.save_param();
    }

} // namespace

TEST( PID, anti_windup_on_output_limits )
    {
    // Тест проверяет, что при ограничении выхода (P_out_min/P_out_max),
    // интегратор не накапливает значения ниже/выше, и регулятор мгновенно
    // реагирует на смену знака ошибки.

    constexpr auto OUT_MAX = 50.f;
    constexpr auto OUT_MIN = 20.f;
    constexpr auto STEP_MS = 1001UL;

    PID test_PID( "PID_antiwindup" );
    init_PID_for_output_limit_test( test_PID, OUT_MIN, OUT_MAX );

    test_PID.set( 100.f );  // Уставка = 100
    test_PID.on();
    test_PID.reset();

    // Фиксируем шаг времени один раз для всего теста.
    DeltaMilliSecSubHooker delta_ms_hooker( STEP_MS );

    // Фаза 1: Нагоняем выход на OUT_MAX при положительной ошибке.
    for ( auto i = 0; i <= 10; ++i )
        {
        auto res = test_PID.eval( 80.f );
        EXPECT_LE( res, OUT_MAX ) << "Выход должен быть <= OUT_MAX";
        }

    auto output_at_max = test_PID.eval( 80.f );
    ASSERT_FLOAT_EQ( OUT_MAX, output_at_max )
        << "Выход должен достичь OUT_MAX при положительной ошибке";

    // Фаза 2: Смена направления.
    auto output_after_reversal = test_PID.eval( 110.f );  // Ошибка = -10

    EXPECT_LT( output_after_reversal, OUT_MAX )
        << "При смене знака ошибки выход должен СРАЗУ упасть ниже OUT_MAX. "
        << "Если это не срабатывает, интегратор может ещё содержать wind-up.";

    for ( auto i = 0; i < 3; ++i )
        {
        auto res = test_PID.eval( 110.f );
        EXPECT_LE( res, OUT_MAX ) << "Выход должен оставаться <= OUT_MAX";
        if ( i > 0 )
            {
            EXPECT_LE( res, output_after_reversal )
                << "Выход должен продолжить уменьшаться";
            output_after_reversal = res;
            }
        }

    // Фаза 3: Проверка на нижнем ограничении.
    init_PID_for_output_limit_test( test_PID, OUT_MIN, OUT_MAX );
    test_PID.set( 0.f );
    test_PID.reset();
    test_PID.on();

    for ( auto i = 0; i <= 10; ++i )
        {
        auto res = test_PID.eval( 110.f );
        EXPECT_GE( res, OUT_MIN ) << "Выход должен быть >= OUT_MIN";
        }

    auto output_at_min = test_PID.eval( 110.f );
    ASSERT_FLOAT_EQ( OUT_MIN, output_at_min )
        << "Выход должен достичь OUT_MIN при отрицательной ошибке";

    test_PID.set( 50.f );
    auto output_after_reversal_2 = test_PID.eval( 5.f );  // Ошибка = +45

    EXPECT_GT( output_after_reversal_2, OUT_MIN )
        << "При смене знака ошибки выход должен СРАЗУ вырасти выше OUT_MIN";
    }
