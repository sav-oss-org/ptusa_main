#include "includes.h"
#include "axio_ioctl_driver.h"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <deque>

namespace
    {
    using bytes = std::vector<unsigned char>;

    struct transaction
        {
        bytes request, response;
        };

    class fake_axio_transport final : public axio_ioctl_transport
        {
        public:
            std::deque<transaction> services;
            std::vector<unsigned long> commands;
            std::vector<bytes> written;
            bytes input{ 0xAA, 0xBB, 0xCC, 0xEE, 0x11, 0x22 };
            bytes diagnostic_bytes;
            uint16_t status = 0x00E0;
            unsigned long failed_command = 0;
            int error = EIO, opens = 0, closes = 0;
            bool open_result = true, held = false, wrong_id = false;
            bool* safely_closed = nullptr;
            uint16_t reply_capacity = 0;
            uint16_t last_id = 0;

            bool open_device() override { ++opens; return open_result; }
            void close_device() override
                {
                ++closes;
                if ( safely_closed ) *safely_closed = held;
                }
            int last_error() const override { return error; }

            int control( unsigned long command, void* argument ) override
                {
                commands.push_back( command );
                if ( command == failed_command ) return -1;
                switch ( command )
                    {
                    case axio_ioctl::SET_SYSFAIL: held = true; break;
                    case axio_ioctl::CLEAR_SYSFAIL: held = false; break;
                    case axio_ioctl::RESET: held = false; break;
                    case axio_ioctl::DIAGNOSTICS:
                        {
                        // ioctl записывает байты регистров, а не значения CPU.
                        bytes raw{ static_cast<unsigned char>( status >> 8 ),
                            static_cast<unsigned char>( status ),
                            0x31, 0x30, 0, 4, 0, 0 };
                        if ( !diagnostic_bytes.empty() ) raw = diagnostic_bytes;
                        std::copy( raw.begin(), raw.end(),
                            static_cast<unsigned char*>( argument ) );
                        break;
                        }
                    case axio_ioctl::SEND:
                        {
                        auto& message = *static_cast<axio_ioctl::message*>(
                            argument );
                        EXPECT_FALSE( services.empty() );
                        if ( services.empty() ) return -1;
                        EXPECT_EQ( bytes( message.data,
                            message.data + message.bytes ),
                            services.front().request );
                        EXPECT_EQ( message.client, 0 );
                        EXPECT_EQ( message.options, 0 );
                        EXPECT_EQ( message.type, 0 );
                        EXPECT_EQ( message.timeout, 5000 );
                        EXPECT_GT( message.id, 0 );
                        EXPECT_LE( message.id, 255 );
                        EXPECT_NE( message.id, last_id );
                        last_id = message.id;
                        if ( message.data[0] == 0x17 ) held = false;
                        if ( message.data[0] == 7 && message.data[1] == 1 )
                            {
                            EXPECT_TRUE( held );
                            status = 0x00E0;
                            }
                        break;
                        }
                    case axio_ioctl::RECEIVE:
                        {
                        auto& message = *static_cast<axio_ioctl::message*>(
                            argument );
                        EXPECT_EQ( message.id, last_id );
                        EXPECT_EQ( message.bytes, 1024 );
                        EXPECT_EQ( message.timeout, 5000 );
                        if ( services.empty() ) return -1;
                        auto reply = services.front().response;
                        services.pop_front();
                        std::copy( reply.begin(), reply.end(), message.data );
                        message.bytes = reply_capacity ? reply_capacity :
                            static_cast<uint16_t>( reply.size() );
                        if ( wrong_id ) ++message.id;
                        break;
                        }
                    case axio_ioctl::READ_PD:
                    case axio_ioctl::WRITE_PD:
                        {
                        auto& data = *static_cast<axio_ioctl::process_data*>(
                            argument );
                        EXPECT_EQ( data.offset, 0 );
                        EXPECT_EQ( data.timeout, 0 );
                        EXPECT_EQ( data.options, 0 );
                        EXPECT_EQ( data.length, input.size() );
                        if ( data.length != input.size() ) return -1;
                        if ( command == axio_ioctl::READ_PD )
                            {
                            std::copy( input.begin(), input.end(), data.data );
                            }
                        else
                            {
                            if ( written.empty() ) EXPECT_TRUE( held );
                            written.emplace_back( data.data,
                                data.data + data.length );
                            }
                        break;
                        }
                    default: ADD_FAILURE() << "ioctl " << command; return -1;
                    }
                return 0;
                }
        };

    class axio_ioctl_test : public testing::Test
        {
        protected:
            fake_axio_transport* transport;
            std::unique_ptr<axio_ioctl_driver> driver;

            void SetUp() override
                {
                auto backend = std::make_unique<fake_axio_transport>();
                transport = backend.get();
                driver = std::make_unique<axio_ioctl_driver>(
                    std::move( backend ) );
                scan();
                }

            void scan()
                {
                // Независимые кадры: нечётный модуль 3 байта и модуль 2 байта.
                transport->services.push_back( {
                    { 3, 0x0B, 0, 1, 0, 7 },
                    { 0x83, 0x0B, 0, 16, 0, 0, 0, 7, 0, 0, 0, 0,
                      0, 0, 0, 2,
                      0, 0x80, 0, 3, 0, 0, 0, 0xD0, 0, 3,
                      0, 0x40, 0, 2, 0, 0, 0, 0xD1, 0, 2 } } );
                transport->services.push_back( {
                    { 0, 0x41, 0, 4, 0, 1, 0, 0, 0, 0x37, 0, 0 },
                    { 0x80, 0x41, 0, 9, 0, 1, 0, 0, 0, 0x37, 0, 8,
                      0, 0, 0, 0x80, 0, 3, 0, 0, 0, 0xD0 } } );
                transport->services.push_back( {
                    { 0, 0x41, 0, 4, 0, 1, 0, 0, 0, 0x0A, 0, 0 },
                    { 0x80, 0x41, 0, 9, 0, 1, 0, 0, 0, 0x0A, 0, 8,
                      0, 0, '2', '6', '8', '8', '0', '2', '2', 0 } } );
                transport->services.push_back( {
                    { 0, 0x41, 0, 4, 0, 2, 0, 0, 0, 0x37, 0, 0 },
                    { 0x80, 0x41, 0, 9, 0, 2, 0, 0, 0, 0x37, 0, 8,
                      0, 0, 0, 0x40, 0, 2, 0, 0, 0, 0xD1 } } );
                transport->services.push_back( {
                    { 0, 0x41, 0, 4, 0, 2, 0, 0, 0, 0x0A, 0, 0 },
                    { 0x80, 0x41, 0, 9, 0, 2, 0, 0, 0, 0x0A, 0, 8,
                      0, 0, '2', '6', '8', '8', '0', '4', '8', 0 } } );
                }

            void stage()
                {
                unsigned char first[]{ 1, 2, 3 }, second[]{ 0x10, 0x20 };
                driver->write_module( 0, first, sizeof( first ) );
                driver->write_module( 1, second, sizeof( second ) );
                }
        };
    }

TEST_F( axio_ioctl_test, di8do8_payload_uses_second_byte_in_hardware_image )
    {
    // Первое устройство имеет один полезный байт; размеры конфигурации
    // и PDI согласованы, а аппаратный образ сохраняет выравнивание до слова.
    auto& config = transport->services[0].response;
    config[19] = 1;
    config[25] = 1;
    transport->services[1].response[17] = 1;
    const char article[] = "2701916";
    std::copy_n( article, 7, transport->services[2].response.begin() + 14 );
    transport->input = { 0xEE, 0xA5, 0x11, 0x22 };
    ASSERT_TRUE( driver->initialize() ) << driver->get_error_text();
    ASSERT_EQ( driver->module( 0 ).article, 2701916 );
    ASSERT_EQ( driver->module( 0 ).bytes, 1 );
    ASSERT_TRUE( driver->read_inputs() );
    unsigned char first = 0, second[2]{};
    driver->read_module( 0, &first, 1 );
    driver->read_module( 1, second, 2 );
    EXPECT_EQ( first, 0xA5 );
    EXPECT_EQ( bytes( second, second + 2 ), ( bytes{ 0x11, 0x22 } ) );
    first = 0x5F;
    second[0] = 0x12;
    second[1] = 0x34;
    driver->write_module( 0, &first, 1 );
    driver->write_module( 1, second, 2 );
    ASSERT_TRUE( driver->write_outputs() );
    EXPECT_EQ( transport->written.back(), ( bytes{ 0, 0x5F, 0x12, 0x34 } ) );
    }

TEST_F( axio_ioctl_test, active_bus_scans_articles_and_pads_each_module )
    {
    ASSERT_TRUE( driver->initialize() ) << driver->get_error_text();
    EXPECT_TRUE( transport->services.empty() );
    EXPECT_EQ( driver->module_count(), 2 );
    EXPECT_EQ( driver->module( 0 ).article, 2688022 );
    EXPECT_EQ( driver->module( 0 ).bytes, 3 );
    EXPECT_EQ( driver->module( 1 ).article, 2688048 );
    EXPECT_EQ( driver->module( 1 ).bytes, 2 );
    EXPECT_TRUE( transport->held );
    ASSERT_EQ( transport->written.size(), 1 );
    EXPECT_EQ( transport->written[0], bytes( 6, 0 ) );
    ASSERT_TRUE( driver->read_inputs() );
    unsigned char first[3]{}, second[2]{};
    driver->read_module( 0, first, 3 );
    driver->read_module( 1, second, 2 );
    EXPECT_EQ( bytes( first, first + 3 ), ( bytes{ 0xAA, 0xBB, 0xCC } ) );
    EXPECT_EQ( bytes( second, second + 2 ), ( bytes{ 0x11, 0x22 } ) );
    stage();
    ASSERT_TRUE( driver->write_outputs() );
    EXPECT_EQ( transport->written.back(),
        ( bytes{ 1, 2, 3, 0, 0x10, 0x20 } ) );
    EXPECT_FALSE( transport->held );
    EXPECT_EQ( transport->commands.back(), axio_ioctl::CLEAR_SYSFAIL );
    EXPECT_FALSE( driver->write_outputs() );
    EXPECT_TRUE( transport->held );
    }

TEST_F( axio_ioctl_test, cold_bus_configures_mapping_before_enabling_outputs )
    {
    transport->status = 0;
    transport->services.push_front( {
        { 7, 0x28, 0, 4, 0x30, 0, 0, 1, 0, 0x21, 0, 0 },
        { 0x87, 0x28, 0, 1, 0, 0 } } );
    transport->services.push_front( {
        { 7, 0x10, 0, 1, 0, 1 }, { 0x87, 0x10, 0, 1, 0, 0 } } );
    transport->services.push_front( {
        { 0x17, 3, 0, 0 }, { 0x97, 3, 0, 1, 0, 0 } } );
    transport->services.push_back( {
        { 7, 1, 0, 1, 0, 1 }, { 0x87, 1, 0, 1, 0, 0 } } );
    ASSERT_TRUE( driver->initialize() ) << driver->get_error_text();
    EXPECT_TRUE( driver->ready() );
    EXPECT_TRUE( transport->held );
    EXPECT_TRUE( transport->services.empty() );
    }

TEST_F( axio_ioctl_test, diagnostic_02e0_is_ready_with_sysfail_held )
    {
    // Реальный ответ, который на little endian ошибочно читался как 0xE002.
    transport->diagnostic_bytes = { 0x02, 0xE0, 0, 0, 0, 0, 0, 0 };
    ASSERT_TRUE( driver->initialize() ) << driver->get_error_text();
    ASSERT_TRUE( driver->ready() ) << driver->get_error_text();
    auto diag = driver->get_diagnostics();
    EXPECT_EQ( diag.status, 0x02E0 );
    EXPECT_EQ( diag.error_code, 0 );
    EXPECT_EQ( diag.error_location, 0 );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "SYS_FAIL" ) );
    EXPECT_THAT( driver->get_error_text(),
        testing::Not( testing::HasSubstr( "IO_ERROR" ) ) );
    ASSERT_TRUE( driver->read_inputs() );
    stage();
    ASSERT_TRUE( driver->write_outputs() );
    EXPECT_FALSE( transport->held );
    }

TEST_F( axio_ioctl_test, diagnostic_error_code_and_slot_are_big_endian )
    {
    ASSERT_TRUE( driver->initialize() );
    transport->diagnostic_bytes = { 0x02, 0xE2, 0x31, 0x30, 0, 4, 0, 0 };
    ASSERT_TRUE( driver->ready() );
    auto diag = driver->get_diagnostics();
    EXPECT_EQ( diag.status, 0x02E2 );
    EXPECT_EQ( diag.error_code, 0x3130 );
    EXPECT_EQ( diag.error_location, 4 );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "слот 4" ) );
    transport->diagnostic_bytes = { 0x01, 0xE0, 0x0B, 0xF2, 0, 2, 0, 0 };
    EXPECT_FALSE( driver->ready() );
    diag = driver->get_diagnostics();
    EXPECT_EQ( diag.status, 0x01E0 );
    EXPECT_EQ( diag.error_code, 0x0BF2 );
    EXPECT_EQ( diag.error_location, 2 );
    EXPECT_TRUE( transport->held );
    }

TEST_F( axio_ioctl_test, partial_image_and_read_failure_never_release_sysfail )
    {
    ASSERT_TRUE( driver->initialize() );
    ASSERT_TRUE( driver->read_inputs() );
    unsigned char data[]{ 1, 2, 3 };
    driver->write_module( 0, data, 3 );
    EXPECT_FALSE( driver->write_outputs() );
    EXPECT_EQ( transport->written.size(), 1 );
    EXPECT_TRUE( transport->held );
    scan();
    ASSERT_TRUE( driver->initialize() );
    transport->failed_command = axio_ioctl::READ_PD;
    EXPECT_FALSE( driver->read_inputs() );
    EXPECT_FALSE( driver->write_outputs() );
    EXPECT_TRUE( transport->held );
    }

TEST_F( axio_ioctl_test, failed_write_or_sysfail_clear_can_recover_after_rescan )
    {
    for ( auto command : { axio_ioctl::WRITE_PD, axio_ioctl::CLEAR_SYSFAIL } )
        {
        ASSERT_TRUE( driver->initialize() );
        ASSERT_TRUE( driver->read_inputs() );
        stage();
        transport->failed_command = command;
        EXPECT_FALSE( driver->write_outputs() );
        EXPECT_TRUE( transport->held );
        transport->failed_command = 0;
        scan();
        ASSERT_TRUE( driver->initialize() );
        ASSERT_TRUE( driver->read_inputs() );
        stage();
        ASSERT_TRUE( driver->write_outputs() );
        EXPECT_FALSE( transport->held );
        ASSERT_TRUE( driver->set_fail_state() );
        }
    }

TEST_F( axio_ioctl_test, corrupt_confirmations_and_pdi_errors_abort_scan )
    {
    transport->services[1].response[12] = 0x08;
    transport->services[1].response[13] = 0x13;
    transport->services[1].response[14] = 0x12;
    transport->services[1].response[15] = 0x34;
    EXPECT_FALSE( driver->initialize() );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "0x0813" ) );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "0x1234" ) );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "0x0037" ) );
    EXPECT_TRUE( transport->written.empty() );
    EXPECT_TRUE( transport->held );
    }

TEST_F( axio_ioctl_test, changed_identity_is_rejected_before_initial_write )
    {
    transport->services[1].response.back() = 0xD1;
    EXPECT_FALSE( driver->initialize() );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "слот 1" ) );
    EXPECT_TRUE( transport->written.empty() );
    EXPECT_TRUE( transport->held );
    }

TEST_F( axio_ioctl_test, invalid_article_is_rejected )
    {
    transport->services[2].response[14] = 'X';
    EXPECT_FALSE( driver->initialize() );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "артикул" ) );
    EXPECT_TRUE( transport->written.empty() );
    }

TEST_F( axio_ioctl_test, invalid_pdi_echo_and_image_overflow_abort_scan )
    {
    const auto valid = transport->services;
    for ( int kind = 0; kind < 4; ++kind )
        {
        transport->services = valid;
        auto& config = transport->services[0].response;
        auto& record = transport->services[1].response;
        if ( kind < 2 )
            {
            // 1025 байт одного модуля или 1024 + 2 байта всей шины.
            config[18] = config[24] = record[16] = 4;
            config[19] = config[25] = record[17] = kind == 0 ? 1 : 0;
            }
        if ( kind == 2 ) record[9] = 0x38;
        if ( kind == 3 ) record[11] = 9;
        EXPECT_FALSE( driver->initialize() );
        EXPECT_TRUE( transport->written.empty() );
        EXPECT_TRUE( transport->held );
        }
    }

TEST_F( axio_ioctl_test, sysfail_ioctl_failure_is_visible_and_requires_retry )
    {
    ASSERT_TRUE( driver->initialize() );
    transport->failed_command = axio_ioctl::SET_SYSFAIL;
    EXPECT_FALSE( driver->set_fail_state() );
    EXPECT_TRUE( driver->get_diagnostics().driver_error );
    EXPECT_FALSE( driver->ready() );
    EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "SYSFAIL" ) );
    }

TEST_F( axio_ioctl_test, truncated_oversized_and_mismatched_replies_are_rejected )
    {
    const auto valid = transport->services;
    for ( int kind = 0; kind < 5; ++kind )
        {
        transport->services = valid;
        transport->wrong_id = kind == 0;
        transport->reply_capacity = kind == 1 ? 1025 : 0;
        auto& response = transport->services.front().response;
        if ( kind == 2 ) response.resize( 5 );
        if ( kind == 3 ) response[0] = 0x84;
        if ( kind == 4 ) response[15] = 63;
        EXPECT_FALSE( driver->initialize() );
        EXPECT_TRUE( transport->written.empty() );
        EXPECT_TRUE( transport->held );
        }
    }

TEST_F( axio_ioctl_test, transport_failures_report_errno_and_hold_outputs )
    {
    for ( auto command : { axio_ioctl::RESET, axio_ioctl::SEND,
        axio_ioctl::RECEIVE, axio_ioctl::DIAGNOSTICS } )
        {
        transport->failed_command = command;
        EXPECT_FALSE( driver->initialize() );
        EXPECT_THAT( driver->get_error_text(), testing::HasSubstr( "ioctl" ) );
        EXPECT_TRUE( transport->held );
        }
    }

TEST_F( axio_ioctl_test, open_permission_error_does_not_send_commands )
    {
    transport->open_result = false;
    transport->error = EACCES;
    EXPECT_FALSE( driver->initialize() );
    EXPECT_THAT( driver->get_error_text(),
        testing::HasSubstr( "/dev/axio_xfer0" ) );
    EXPECT_TRUE( transport->commands.empty() );
    }

TEST_F( axio_ioctl_test, bus_change_blocks_write_but_io_warning_is_reported )
    {
    ASSERT_TRUE( driver->initialize() );
    transport->status = 0x02E2;
    ASSERT_TRUE( driver->read_inputs() );
    EXPECT_EQ( driver->get_diagnostics().error_location, 4 );
    stage();
    transport->status = 0x01E0;
    EXPECT_FALSE( driver->write_outputs() );
    EXPECT_TRUE( transport->held );
    EXPECT_EQ( transport->written.size(), 1 );
    }

TEST_F( axio_ioctl_test, shutdown_sets_sysfail_before_closing_device )
    {
    ASSERT_TRUE( driver->initialize() );
    ASSERT_TRUE( driver->read_inputs() );
    stage();
    ASSERT_TRUE( driver->write_outputs() );
    bool safe = false;
    transport->safely_closed = &safe;
    driver.reset();
    EXPECT_TRUE( safe );
    }
