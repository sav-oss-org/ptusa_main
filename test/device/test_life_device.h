#pragma once
#include <iomanip>

#include "../includes.h"
#include "device/life_device.h"

class mock_DI_device : public device
    {
    public:
        mock_DI_device() :
            device(
                "MockDeviceDI", DEVICE_TYPE::DT_DI, DEVICE_SUB_TYPE::DST_DI, 0 )
            {
            }

        MOCK_METHOD( int, get_state, (), ( const, override ) );
    };

class mock_AI_device : public device
    {
    public:
        mock_AI_device() :
            device(
                "MockDeviceAI", DEVICE_TYPE::DT_AI, DEVICE_SUB_TYPE::DST_AI, 0 )
            {
            }

        MOCK_METHOD( float, get_value, (), ( const, override ) );
    };

class mock_DO_device : public device
    {
    public:
        mock_DO_device() :
            device(
                "MockDeviceDO", DEVICE_TYPE::DT_DO, DEVICE_SUB_TYPE::DST_DO, 0 )
            {
            }
    };

class mock_AO_device : public device
    {
    public:
        mock_AO_device() :
            device(
                "MockDeviceAO", DEVICE_TYPE::DT_AO, DEVICE_SUB_TYPE::DST_AO, 0 )
            {
            }
    };

class watchdog_bit : public ::testing::Test
    {
    protected:
        std::unique_ptr<watchdog> life_bit = std::make_unique<watchdog>(
            "TestDevice", device::DEVICE_SUB_TYPE::DST_WATCHDOG );
        std::unique_ptr<mock_DI_device> mock_DI_dev =
            std::make_unique<mock_DI_device>();
        std::unique_ptr<mock_AI_device> mock_AI_dev =
            std::make_unique<mock_AI_device>();

        std::unique_ptr<mock_DO_device> mock_DO_dev =
            std::make_unique<mock_DO_device>();
        std::unique_ptr<mock_AO_device> mock_AO_dev =
            std::make_unique<mock_AO_device>();

        void SetUp() override
            {
            // Устанавливаем mock устройства.
            life_bit->set_property( "DI_dev", mock_DI_dev.get() );

            life_bit->set_property( "DO_dev", mock_DO_dev.get() );
            life_bit->set_property( "AO_dev", mock_AO_dev.get() );
            }
    };

class watchdog_counter : public ::testing::Test
    {
    protected:
        std::unique_ptr<watchdog> life_counter = std::make_unique<watchdog>(
            "TestDevice", device::DEVICE_SUB_TYPE::DST_WATCHDOG );
        std::unique_ptr<mock_AI_device> mock_AI_dev =
            std::make_unique<mock_AI_device>();

        void SetUp() override
            {
            // Устанавливаем mock устройство.
            life_counter->set_property( "AI_dev", mock_AI_dev.get() );
            }
    };

TEST_F( watchdog_bit, EvaluateIO_NoDiDevice_NoAction )
    {
    life_bit->set_property( "DI_dev", nullptr );// Убираем устройство.
    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 0 );  // Состояние должно быть нулевое.

    life_bit->set_property(
        "DI_dev", mock_DI_dev.get() );  // Добавляем устройство.
    }

TEST_F( watchdog_bit, EvaluateIO_StateChanged_DeviceActivated )
    {
    // Задаем небольшой интервал, так как время изменения выхода задано
    // равным 0, хватит 1 мс.
    DeltaMilliSecSubHooker::set_millisec( 1UL );

    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillRepeatedly(
            ::testing::Return( 1 ) );  // Состояние сигнала изменилось.

    EXPECT_FALSE( life_bit->DO_dev
            ->is_active() );  // Дискретный выход должен быть равным 0.
    EXPECT_EQ( life_bit->AO_dev->get_value(),
        0 );  // Аналоговый выход должен быть равным 0.

    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 1 );  // Устройство должно активироваться.
    EXPECT_TRUE( life_bit->DO_dev
            ->is_active() );  // Дискретный выход должен стать равным 1.
    EXPECT_EQ( life_bit->AO_dev->get_value(),
        1 );  // Аналоговый выход должен стать равным 1.

    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 1 );  // Устройство должно остаться в 1.
    EXPECT_FALSE( life_bit->DO_dev
            ->is_active() );  // Дискретный выход должен стать равным 0.
    EXPECT_EQ( life_bit->AO_dev->get_value(),
        2 );  // Аналоговый выход должен стать равным 2.

    DeltaMilliSecSubHooker::set_default_time();
    }

TEST_F( watchdog_bit, EvaluateIO_TimerExceeded_DeviceDeactivated )
    {
    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillOnce( ::testing::Return( 1 ) );  // Состояние изменилось.

    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 1 );  // Устройство должно активироваться.

    // Устанавливаем P_T_ERR = 100 мс.
    life_bit->set_par( static_cast<u_int>( watchdog::PARAM::P_T_ERR ), 0, 100 );
    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillRepeatedly( ::testing::Return( 1 ) );  // Состояние не меняется.

    // Эмулируем время - устанавливаем время так, чтобы таймер истек.
    life_bit->start_in_check_time -= 200;
    life_bit->evaluate_io();
    EXPECT_EQ(
        life_bit->get_state(), -1 );  // Устройство должно деактивироваться.


    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillOnce( ::testing::Return( 0 ) );  // Состояние изменилось.

    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 1 );  // Устройство должно активироваться.
    }

TEST_F( watchdog_bit, save_device )
    {
    // Создаем std::array для записи.
    std::array<char, 256> buffer = { 0 };

    // Вызываем метод save_device.
    auto bytes_written = life_bit->save_device( buffer.data() );

    // Проверяем, что метод вернул корректное количество записанных байт.
    EXPECT_GT( bytes_written, 0 );

    EXPECT_STREQ(
        "TestDevice={M=0, ST=0, V=0, P_T_GEN=0, P_T_ERR=0},\n", buffer.data() );
    }

TEST_F( watchdog_bit, SetStringProperty_Sets_DI_Device )
    {
    life_bit->set_property( "DI_dev", nullptr );
    // Проверяем, что изначально DI_dev равен nullptr.
    EXPECT_EQ( life_bit->DI_dev, nullptr );

    // Устанавливаем свойство "DI_dev" с именем устройства.
    const char* device_name = "MockDeviceDI";
    G_DEVICE_MANAGER()->add_device( static_cast<device*>( mock_DI_dev.get() ),
        device::DEVICE_TYPE::DT_WATCHDOG );
    life_bit->set_string_property( "DI_dev", device_name );

    // Проверяем, что DI_dev был установлен.
    EXPECT_EQ( life_bit->DI_dev, mock_DI_dev.get() );

    G_DEVICE_MANAGER()->remove_device( mock_DI_dev->get_serial_n() );
    }


TEST_F( watchdog_bit, SetStringProperty_Sets_AI_Device )
    {
    life_bit->set_property( "AI_dev", nullptr );
    // Проверяем, что изначально AI_dev равен nullptr.
    EXPECT_EQ( life_bit->AI_dev, nullptr );

    // Устанавливаем свойство "AI_dev" с именем устройства.
    const char* device_name = "MockDeviceAI";
    G_DEVICE_MANAGER()->add_device( static_cast<device*>( mock_AI_dev.get() ),
        device::DEVICE_TYPE::DT_WATCHDOG );
    life_bit->set_string_property( "AI_dev", device_name );

    // Проверяем, что AI_dev был установлен.
    EXPECT_EQ( life_bit->AI_dev, mock_AI_dev.get() );

    G_DEVICE_MANAGER()->remove_device( mock_AI_dev->get_serial_n() );
    }

TEST_F( watchdog_bit, SetStringProperty_Sets_DO_Device )
    {
    life_bit->set_property( "DO_dev", nullptr );
    // Проверяем, что изначально DO_dev равен nullptr.
    EXPECT_EQ( life_bit->DO_dev, nullptr );

    // Устанавливаем свойство "DO_dev" с именем устройства.
    const char* device_name = "MockDeviceDO";
    G_DEVICE_MANAGER()->add_device( static_cast<device*>( mock_DO_dev.get() ),
        device::DEVICE_TYPE::DT_WATCHDOG );
    life_bit->set_string_property( "DO_dev", device_name );

    // Проверяем, что DO_dev был установлен.
    EXPECT_EQ( life_bit->DO_dev, mock_DO_dev.get() );

    G_DEVICE_MANAGER()->remove_device( mock_DO_dev->get_serial_n() );
    }

TEST_F( watchdog_bit, SetStringProperty_Sets_AO_Device )
    {
    life_bit->set_property( "AO_dev", nullptr );
    // Проверяем, что изначально AO_dev равен nullptr.
    EXPECT_EQ( life_bit->AO_dev, nullptr );

    // Устанавливаем свойство "AO_dev" с именем устройства.
    const char* device_name = "MockDeviceAO";
    G_DEVICE_MANAGER()->add_device( static_cast<device*>( mock_AO_dev.get() ),
        device::DEVICE_TYPE::DT_WATCHDOG );
    life_bit->set_string_property( "AO_dev", device_name );

    // Проверяем, что AO_dev был установлен.
    EXPECT_EQ( life_bit->AO_dev, mock_AO_dev.get() );

    G_DEVICE_MANAGER()->remove_device( mock_AO_dev->get_serial_n() );
    }

TEST_F( watchdog_bit, SetStringProperty_InvalidField_NoAction )
    {
    // Устанавливаем несуществующее поле.
    const char* invalid_field = "INVALID";
    life_bit->set_string_property( invalid_field, "SomeValue" );

    // Проверяем, что DI_dev не изменился.
    EXPECT_EQ( life_bit->DI_dev, mock_DI_dev.get() );
    }

TEST_F( watchdog_bit, GetNameInLua_ReturnsCorrectName )
    {
    const char* expected_name = "TestDevice";
    const char* lua_name = life_bit->get_name_in_Lua();
    ASSERT_STREQ( lua_name, expected_name )
        << "get_name_in_Lua() should"
           "return the correct device name.";
    }


TEST_F( watchdog_counter, EvaluateIO_TimerExceeded_DeviceDeactivated )
    {
    EXPECT_CALL( *mock_AI_dev, get_value() )
        .WillOnce( ::testing::Return( 10 ) );  // Состояние изменилось.
    life_counter->evaluate_io();
    EXPECT_EQ(
        life_counter->get_state(), 1 );  // Устройство должно активироваться.

    // Устанавливаем P_T_ERR = 100 мс.
    life_counter->set_par(
        static_cast<u_int>( watchdog::PARAM::P_T_ERR ), 0, 100 );
    EXPECT_CALL( *mock_AI_dev, get_value() )
        .WillRepeatedly( ::testing::Return( 10 ) );  // Состояние не меняется.

    // Эмулируем время - устанавливаем время так, чтобы таймер истек.
    life_counter->start_in_check_time -= 200;
    life_counter->evaluate_io();
    EXPECT_EQ(
        life_counter->get_state(), -1 );  // Устройство должно деактивироваться

    EXPECT_CALL( *mock_AI_dev, get_value() )
        .WillOnce( ::testing::Return( 11 ) );  // Состояние изменилось.
    life_counter->evaluate_io();
    EXPECT_EQ(
        life_counter->get_state(), 1 );  // Устройство должно активироваться.
    }

TEST( watchdog, get_type_name )
    {
    watchdog dev( "POU1LF1" );
    EXPECT_STREQ( "Устройство проверки связи", dev.get_type_name() );
    }

TEST( watchdog, get_error_description )
    {
    watchdog dev( "POU1WATCHDOG1" );

    EXPECT_STREQ( dev.get_error_description(), "нет ошибок" );

    const std::string DESCR = "ПОУ №3";
    dev.set_descr( DESCR.c_str() );
    const float WAIT_TIME_MS = 1.f;
    dev.set_par(
        static_cast<u_int>( watchdog::PARAM::P_T_ERR ), 0, WAIT_TIME_MS );
    auto DI_dev = std::make_unique<mock_DI_device>();
    dev.DI_dev = DI_dev.get();
    EXPECT_CALL( *DI_dev.get(), get_state() )
        .WillOnce( ::testing::Return( 0 ) );  // Состояние не изменилось.

    DeltaMilliSecSubHooker::set_millisec(
        static_cast<unsigned long>( WAIT_TIME_MS ) + 1UL );
    dev.evaluate_io();
    EXPECT_STREQ( dev.get_error_description(),
        ( std::string( "ошибка связи - " ) + '\'' + DESCR + '\'' ).c_str() );
    DeltaMilliSecSubHooker::set_default_time();
    }

TEST( watchdog, set_property )
    {
    const std::string WD_NAME = "POU1LF1";
    watchdog dev( WD_NAME.c_str() );

    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ), SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );
    const std::string WD_PROPERTY_NAME = "BAD_NAME_PROPERTY";
    std::string out_str = FIXED_TIME_STR + ' ' +
#ifdef LINUX_OS
                          "\x1B[37m" +  // Цвет текста для Linux
#endif
                          "DEBUG  (7) -> " + WD_NAME +
                          "\t watchdog::set_property() - field = \"" +
                          WD_PROPERTY_NAME + "\", val = \"nullptr\"\n" +
#ifdef LINUX_OS
                          "\x1B[0m" +  // Сброс цвета для Linux
#endif
                          FIXED_TIME_STR + ' ' +
#ifdef LINUX_OS
                          "\x1B[1;31m" +  // Цвет текста для Linux
#endif
                          "ALERT  (1) -> " + WD_NAME +
                          "\t watchdog::set_property() - Unknown field \"" +
                          WD_PROPERTY_NAME + "\"\n"
#ifdef LINUX_OS
                          + "\x1B[0m"  // Сброс цвета для Linux
#endif
        ;

    // Проверяем, что изначально DI_dev равен nullptr.
    EXPECT_EQ( dev.DI_dev, nullptr );

    testing::internal::CaptureStdout();
    dev.set_property( WD_PROPERTY_NAME.c_str(), nullptr );
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, out_str );

    // Проверяем, что после DI_dev равен nullptr.
    EXPECT_EQ( dev.DI_dev, nullptr );

    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    }
