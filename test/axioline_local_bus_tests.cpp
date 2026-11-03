#include "includes.h"
#include "uni_bus_coupler_io.h"
#include <algorithm>
#include <stdexcept>
#include <array>

TEST( local_bus_diagnostics, io_error_contains_code_slot_and_cause )
    {
    local_bus_driver::diagnostics diag{ 0x02E2, 0x3130, 4, false, true };
    auto text = describe_local_bus_diagnostics( diag );
    EXPECT_THAT( text, testing::HasSubstr( "код=0x3130" ) );
    EXPECT_THAT( text, testing::HasSubstr( "слот 4" ) );
    EXPECT_THAT( text, testing::HasSubstr( "IO_ERROR" ) );
    EXPECT_THAT( text, testing::HasSubstr( "SYS_FAIL" ) );
    EXPECT_THAT( text, testing::HasSubstr( "питание I/O" ) );
    EXPECT_THAT( text, testing::Not( testing::HasSubstr( "RUNNING=0" ) ) );
    }

TEST( local_bus_diagnostics, unavailable_or_unknown_diagnostics_are_explicit )
    {
    EXPECT_TRUE( describe_local_bus_diagnostics( {} ).empty() );
    auto text = describe_local_bus_diagnostics(
        { 0x010C, 0xDEAD, 0, true, true } );
    EXPECT_THAT( text, testing::HasSubstr( "код=0xDEAD" ) );
    EXPECT_THAT( text, testing::HasSubstr( "аппаратного драйвера" ) );
    EXPECT_THAT( text, testing::HasSubstr( "BUS_DIFFERENT" ) );
    EXPECT_THAT( text, testing::HasSubstr( "BUS_ERROR" ) );
    EXPECT_THAT( text, testing::HasSubstr( "CONTROLLER_ERROR" ) );
    EXPECT_THAT( text, testing::HasSubstr( "RUNNING=0" ) );
    EXPECT_THAT( text, testing::Not( testing::HasSubstr( "слот 0" ) ) );
    }

TEST( local_bus_metadata, identification_record_contains_process_byte_length )
    {
    unsigned char record[] = { 0, 0x80, 0, 2, 0, 0, 0, 0xD0 };
    unsigned int bytes = 0;
    ASSERT_TRUE( decode_local_bus_process_data_length(
        record, sizeof( record ), bytes ) );
    EXPECT_EQ( bytes, 2 );
    record[1] = 0x40;
    record[7] = 0xD1;
    ASSERT_TRUE( decode_local_bus_process_data_length(
        record, sizeof( record ), bytes ) );
    EXPECT_EQ( bytes, 2 );
    record[2] = 1;
    record[3] = 0;
    ASSERT_TRUE( decode_local_bus_process_data_length(
        record, sizeof( record ), bytes ) );
    EXPECT_EQ( bytes, 256 );
    record[2] = 4;
    ASSERT_TRUE( decode_local_bus_process_data_length(
        record, sizeof( record ), bytes ) );
    EXPECT_EQ( bytes, 1024 );
    record[2] = 0;
    ASSERT_TRUE( decode_local_bus_process_data_length(
        record, sizeof( record ), bytes ) );
    EXPECT_EQ( bytes, 0 );
    }

TEST( local_bus_metadata, invalid_record_is_rejected )
    {
    unsigned char record[] = { 0, 0x80, 4, 1, 0, 0, 0, 0xD0 };
    unsigned int bytes = 123;
    EXPECT_FALSE( decode_local_bus_process_data_length(
        record, sizeof( record ), bytes ) );
    EXPECT_EQ( bytes, 0 );
    record[2] = 0;
    EXPECT_FALSE( decode_local_bus_process_data_length(
        record, 7, bytes ) );
    EXPECT_FALSE( decode_local_bus_process_data_length(
        nullptr, sizeof( record ), bytes ) );
    }

namespace
    {
    class fake_local_bus final : public local_bus_driver
        {
        public:
            std::vector<module_info> modules;
            std::vector<std::vector<unsigned char>> inputs, outputs;
            bool initialized = true, operational = true;
            bool read_result = true, write_result = true;
            bool throw_on_read = false;
            int reads = 0, writes = 0, initializations = 0, failures = 0;
            diagnostics diagnostic;
            std::string error_text;

            diagnostics get_diagnostics() const override { return diagnostic; }
            const char* get_error_text() const override { return error_text.c_str(); }
            bool set_fail_state() override { ++failures; return true; }

            bool initialize() override
                {
                ++initializations;
                return initialized;
                }
            bool ready() override { return operational; }
            size_t module_count() override { return modules.size(); }
            module_info module( size_t index ) override
                {
                return modules.at( index );
                }
            bool read_inputs() override
                {
                ++reads;
                if ( throw_on_read ) throw std::runtime_error( "read failed" );
                return read_result;
                }
            bool write_outputs() override
                {
                ++writes;
                return write_result;
                }
            void read_module( size_t index, unsigned char* data,
                unsigned int bytes ) override
                {
                std::copy_n( inputs.at( index ).data(), bytes, data );
                }
            void write_module( size_t index, unsigned char* data,
                unsigned int bytes ) override
                {
                outputs.at( index ).assign( data, data + bytes );
                }
        };

    class local_bus_io : public testing::Test
        {
        protected:
            fake_local_bus* driver = nullptr;
            std::unique_ptr<uni_io_manager> manager;
            io_manager::io_node* node = nullptr;
            bool was_emulator = true;

            void SetUp() override
                {
                was_emulator = G_PAC_INFO()->is_emulator();
                auto port = tcp_communicator::get_port();
                auto modbus_port = tcp_communicator::get_modbus_port();
                // Порт выбирает ОС: тестам локальной шины сеть не нужна.
                tcp_communicator::set_port( 0, 0 );
                tcp_communicator::init_instance( "Тест", "Test" );
                tcp_communicator::set_port( port, modbus_port );
                }

            void create( std::vector<local_bus_driver::module_info> modules,
                int type = io_manager::io_node::PHOENIX_AXC_F_2152 )
                {
                auto backend = std::make_unique<fake_local_bus>();
                driver = backend.get();
                driver->modules = modules;
                unsigned int words = 0;
                for ( const auto& module : modules )
                    {
                    words += ( module.bytes + 1 ) / 2;
                    driver->inputs.emplace_back( module.bytes, 0 );
                    driver->outputs.emplace_back( module.bytes, 0 );
                    }
                manager = std::make_unique<uni_io_manager>(
                    std::move( backend ) );
                manager->init( 1 );
                node = manager->add_node( 0, type, 1, "", "A100",
                    words * 16, words * 16, words, words * 2,
                    words, words * 2 );
                node->delay_time = 0;
                unsigned int offset = 0;
                for ( const auto& module : modules )
                    {
                    for ( unsigned int i = 0; i < ( module.bytes + 1 ) / 2;
                        ++i, ++offset )
                        {
                        manager->init_node_AI( 0, offset,
                            module.article, offset * 2 );
                        manager->init_node_AO( 0, offset,
                            module.article, offset * 2 );
                        }
                    }
                }

            void select_at_runtime( std::vector<local_bus_backend>& selected )
                {
                auto modules = driver->modules;
                auto inputs = driver->inputs, outputs = driver->outputs;
                manager->local_driver.reset();
                driver = nullptr;
                manager->local_driver_factory =
                    [this, modules, inputs, outputs, &selected](
                        local_bus_backend backend )
                    {
                    selected.push_back( backend );
                    auto result = std::make_unique<fake_local_bus>();
                    driver = result.get();
                    driver->modules = modules;
                    driver->inputs = inputs;
                    driver->outputs = outputs;
                    return result;
                    };
                }

            void TearDown() override
                {
                PAC_critical_errors_manager::get_instance()->reset_all_error();
                if ( was_emulator ) G_PAC_INFO()->emulation_on();
                else G_PAC_INFO()->emulation_off();
                }

            std::string alarms()
                {
                std::array<char, 8192> buffer{};
                u_int_2 id = 0;
                PAC_critical_errors_manager::get_instance()->save_as_Lua_str(
                    buffer.data(), id );
                return buffer.data();
                }
        };
    }

TEST_F( local_bus_io, controllers_without_ip_are_active )
    {
    for ( auto type : { io_manager::io_node::PHOENIX_AXC_F_2152,
        io_manager::io_node::PHOENIX_AXC_F_3152,
        io_manager::io_node::PHOENIX_AXC_F_2152_IOCTL,
        io_manager::io_node::PHOENIX_AXC_F_3152_IOCTL } )
        {
        create( { { 2688022, 2, false } }, type );
        EXPECT_TRUE( node->is_active );
        EXPECT_TRUE( node->is_local_bus() );
        EXPECT_EQ( manager->read_inputs(), 0 );
        EXPECT_EQ( node->state, io_manager::io_node::ST_OK );
        }
    }

TEST_F( local_bus_io, node_type_selects_transport_and_reuses_driver )
    {
    for ( int type : { 202, 203, 204, 205 } )
        {
        SCOPED_TRACE( type );
        create( { { 2688048, 2, false } }, type );
        std::vector<local_bus_backend> selected;
        select_at_runtime( selected );
        EXPECT_TRUE( selected.empty() );
        EXPECT_EQ( node->is_ioctl_local_bus(), type == 204 || type == 205 );
        ASSERT_EQ( manager->read_inputs(), 0 );
        node->DO_[0] = 1;
        ASSERT_EQ( manager->write_outputs(), 0 );
        ASSERT_EQ( manager->read_inputs(), 0 );
        ASSERT_EQ( selected.size(), 1 );
        EXPECT_EQ( selected[0], type < 204
            ? local_bus_backend::LIBRARY : local_bus_backend::IOCTL );
        EXPECT_EQ( driver->outputs[0][0], 1 );
        }
    }

TEST_F( local_bus_io, transport_change_requires_fresh_input_before_write )
    {
    create( { { 2688048, 2, false } }, 202 );
    std::vector<local_bus_backend> selected;
    select_at_runtime( selected );
    ASSERT_EQ( manager->read_inputs(), 0 );
    node->type = io_manager::io_node::PHOENIX_AXC_F_2152_IOCTL;
    EXPECT_EQ( manager->write_outputs(), 1 );
    ASSERT_EQ( selected.size(), 2 );
    EXPECT_EQ( selected[0], local_bus_backend::LIBRARY );
    EXPECT_EQ( selected[1], local_bus_backend::IOCTL );
    EXPECT_EQ( driver->writes, 0 );
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( manager->write_outputs(), 0 );
    EXPECT_EQ( selected.size(), 2 );
    }

TEST_F( local_bus_io, unavailable_selected_transport_does_not_fall_back )
    {
    create( { { 2688048, 2, false } }, 204 );
    manager->local_driver.reset();
    std::vector<local_bus_backend> selected;
    manager->local_driver_factory = [&selected]( local_bus_backend backend )
        {
        selected.push_back( backend );
        return std::unique_ptr<local_bus_driver>{};
        };
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( manager->write_outputs(), 1 );
    ASSERT_EQ( selected.size(), 1 );
    EXPECT_EQ( selected[0], local_bus_backend::IOCTL );
    EXPECT_TRUE( node->read_io_error_flag );
    }

TEST_F( local_bus_io, empty_controller_does_not_create_runtime_driver )
    {
    create( {}, 204 );
    std::vector<local_bus_backend> selected;
    select_at_runtime( selected );
    EXPECT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( manager->write_outputs(), 0 );
    EXPECT_TRUE( selected.empty() );
    }

TEST_F( local_bus_io, fault_details_are_updated_in_scada_and_cleared_on_recovery )
    {
    create( { { 2688022, 2, false } } );
    driver->operational = false;
    driver->diagnostic = { 0x0084, 0x0BF2, 1, false, true };
    driver->error_text = "BUS_ERROR, код=0x0BF2, слот 1";
    ASSERT_EQ( manager->read_inputs(), 1 );
    EXPECT_THAT( alarms(), testing::HasSubstr( driver->error_text ) );
    EXPECT_EQ( node->local_bus_error_code, 0x0BF2 );
    EXPECT_EQ( node->local_bus_error_location, 1 );
    auto errors = PAC_critical_errors_manager::get_instance();
    auto id = errors->get_id();
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( errors->get_id(), id );
    driver->error_text = "ошибка аппаратного драйвера";
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_NE( errors->get_id(), id );
    EXPECT_THAT( alarms(), testing::HasSubstr( driver->error_text ) );
    driver->operational = true;
    driver->diagnostic = { 0x00E0, 0, 0, false, true };
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_TRUE( alarms().empty() );
    EXPECT_EQ( node->local_bus_error_code, 0 );
    }

TEST_F( local_bus_io, io_error_is_visible_while_exchange_continues )
    {
    create( { { 2688048, 2, false } } );
    G_PAC_INFO()->emulation_off();
    driver->diagnostic = { 0x00E2, 0x3422, 1, false, true };
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_WARNING );
    EXPECT_THAT( alarms(), testing::HasSubstr( "код=0x3422" ) );
    EXPECT_THAT( alarms(), testing::HasSubstr( "priority = 250" ) );
    EXPECT_FALSE( PAC_critical_errors_manager::get_instance()->is_any_critical_error() );
    EXPECT_EQ( manager->write_outputs(), 0 );
    driver->diagnostic = { 0x00E0, 0, 0, false, true };
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( node->get_display_state(),
        io_manager::io_node::DISPLAY_STATES::DST_OK );
    EXPECT_TRUE( alarms().empty() );
    }

TEST_F( local_bus_io, sysfail_notification_is_not_a_communication_failure )
    {
    create( { { 2688048, 2, false } } );
    driver->diagnostic = { 0x02E0, 0, 0, false, true };
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_FALSE( node->read_io_error_flag );
    EXPECT_THAT( alarms(), testing::HasSubstr( "SYS_FAIL" ) );
    EXPECT_THAT( alarms(), testing::HasSubstr( "priority = 500" ) );
    EXPECT_FALSE( PAC_critical_errors_manager::get_instance()->is_any_critical_error() );
    manager->disconnect( node );
    EXPECT_TRUE( alarms().empty() );
    }

TEST_F( local_bus_io, configuration_fault_identifies_slot_and_articles )
    {
    create( { { 2688022, 2, false } } );
    driver->modules[0].article = 2688048;
    ASSERT_EQ( manager->read_inputs(), 1 );
    EXPECT_THAT( alarms(), testing::HasSubstr( "слот 1" ) );
    EXPECT_THAT( alarms(), testing::HasSubstr( "2688022" ) );
    EXPECT_THAT( alarms(), testing::HasSubstr( "2688048" ) );
    }

TEST_F( local_bus_io, write_fault_replaces_notification_with_exchange_alarm )
    {
    create( { { 2688048, 2, false } } );
    driver->diagnostic = { 0x00E1, 0x3130, 1, false, true };
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_TRUE( node->is_cfg_bus_error_alarm_set );
    driver->write_result = false;
    driver->error_text = "ошибка записи: аппаратный драйвер";
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_FALSE( node->is_cfg_bus_error_alarm_set );
    EXPECT_THAT( alarms(), testing::HasSubstr( driver->error_text ) );
    EXPECT_THAT( alarms(), testing::Not( testing::HasSubstr( "7-1-1" ) ) );
    }

TEST_F( local_bus_io, mixed_modules_keep_word_and_bit_offsets )
    {
    create( { { 2688022, 2, false }, { 2688491, 8, false },
        { 2688048, 2, false } } );
    driver->inputs[0] = { 0x81, 0x80 };
    driver->inputs[1] = { 0x12, 0x34, 0xFF, 0xFE, 0x00, 0x01, 0x00, 0x02 };
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( node->AI[1], 0x1234 );
    EXPECT_EQ( node->AI[2], -2 );
    EXPECT_EQ( node->DI[0], 1 );
    EXPECT_EQ( node->DI[7], 1 );
    EXPECT_EQ( node->DI[15], 1 );
    // DO16 после DI16 и AI4: слово 5, бит 80.
    node->DO_[80] = 1;
    node->DO_[95] = 1;
    ASSERT_EQ( manager->write_outputs(), 0 );
    EXPECT_EQ( driver->outputs[2],
        ( std::vector<unsigned char>{ 0x01, 0x80 } ) );
    EXPECT_EQ( node->DO[80], 1 );
    EXPECT_EQ( node->DO[95], 1 );
    }

TEST_F( local_bus_io, di8do8_inputs_use_project_bits_40_to_47 )
    {
    for ( auto type : { io_manager::io_node::PHOENIX_AXC_F_2152,
        io_manager::io_node::PHOENIX_AXC_F_2152_IOCTL } )
        {
        for ( auto article : { 2701916u, 2702071u } )
            {
            create( { { 2688022, 2, false }, { 2688048, 2, false },
                { article, 1, false }, { 2688022, 2, false } }, type );
            driver->inputs[2] = { 0xA5 };
            driver->inputs[3] = { 0x12, 0x34 };
            ASSERT_EQ( manager->read_inputs(), 0 );
            EXPECT_EQ( node->AI[2], 0x00A5 );
            for ( unsigned int bit = 0; bit < 8; ++bit )
                {
                EXPECT_EQ( node->DI[32 + bit], 0 );
                EXPECT_EQ( node->DI[40 + bit], ( 0xA5 >> bit ) & 1 );
                }
            EXPECT_EQ( node->AI[3], 0x1234 );
            EXPECT_EQ( node->DI[49], 1 );
            EXPECT_EQ( node->DI[58], 1 );
            }
        }
    }

TEST_F( local_bus_io, di8do8_outputs_send_each_project_channel_in_one_byte )
    {
    for ( auto type : { io_manager::io_node::PHOENIX_AXC_F_2152,
        io_manager::io_node::PHOENIX_AXC_F_2152_IOCTL } )
        {
        for ( auto article : { 2701916u, 2702071u } )
            {
            create( { { 2688022, 2, false }, { 2688048, 2, false },
                { article, 1, false }, { 2688048, 2, false } }, type );
            ASSERT_EQ( manager->read_inputs(), 0 );
            node->DO_[17] = 1;
            node->DO_[48] = 1;
            node->DO_[63] = 1;
            // Зарезервированный байт не должен попадать на клеммы DI8/DO8.
            std::fill_n( node->DO_ + 32, 8, 1 );
            for ( unsigned int channel = 0; channel < 8; ++channel )
                {
                std::fill_n( node->DO_ + 40, 8, 0 );
                node->DO_[40 + channel] = 1;
                ASSERT_EQ( manager->write_outputs(), 0 );
                EXPECT_EQ( driver->outputs[2],
                    ( std::vector<unsigned char>{
                        static_cast<unsigned char>( 1u << channel ) } ) );
                EXPECT_EQ( driver->outputs[1],
                    ( std::vector<unsigned char>{ 0x02, 0x00 } ) );
                EXPECT_EQ( driver->outputs[3],
                    ( std::vector<unsigned char>{ 0x01, 0x80 } ) );
                EXPECT_EQ( node->DO[40 + channel], 1 );
                }
            }
        }
    }

TEST_F( local_bus_io, analog_outputs_use_big_endian )
    {
    for ( unsigned int article : { 2688527, 1088123, 1088126 } )
        {
        create( { { 2688022, 2, false }, { article, 8, false } } );
        ASSERT_EQ( manager->read_inputs(), 0 );
        node->AO_[1] = 0x1234;
        node->AO_[2] = -2;
        ASSERT_EQ( manager->write_outputs(), 0 );
        EXPECT_EQ( driver->outputs[1],
            ( std::vector<unsigned char>{
                0x12, 0x34, 0xFF, 0xFE, 0, 0, 0, 0 } ) );
        EXPECT_EQ( node->AO[1], 0x1234 );
        }
    }

TEST_F( local_bus_io, iolink_preserves_service_bits_and_payload_bytes )
    {
    create( { { 1027843, 64, false } } );
    driver->inputs[0][6] = 0x12;
    driver->inputs[0][7] = 0x34;
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( reinterpret_cast<unsigned char*>( node->AI )[6], 0x12 );
    EXPECT_EQ( reinterpret_cast<unsigned char*>( node->AI )[7], 0x34 );
    node->DO_[16] = 1;
    reinterpret_cast<unsigned char*>( node->AO_ )[6] = 0x56;
    reinterpret_cast<unsigned char*>( node->AO_ )[7] = 0x78;
    ASSERT_EQ( manager->write_outputs(), 0 );
    EXPECT_EQ( driver->outputs[0][2], 1 );
    EXPECT_EQ( driver->outputs[0][6], 0x56 );
    EXPECT_EQ( driver->outputs[0][7], 0x78 );
    }

TEST_F( local_bus_io, adjacent_counters_restart_control_word )
    {
    create( { { 2688093, 28, false }, { 2688093, 28, false } } );
    ASSERT_EQ( manager->read_inputs(), 0 );
    ASSERT_EQ( manager->write_outputs(), 0 );
    for ( const auto& data : driver->outputs )
        {
        EXPECT_EQ( data[0], 5 );
        EXPECT_EQ( data[1], 5 );
        EXPECT_EQ( data[2], 0 );
        }
    }

TEST_F( local_bus_io, odd_lengths_are_padded_per_module )
    {
    create( { { 100001, 1, false }, { 100002, 1, false } } );
    driver->inputs[0] = { 0x81 };
    driver->inputs[1] = { 0x42 };
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( node->AI[0], static_cast<short>( 0x8100 ) );
    EXPECT_EQ( node->AI[1], 0x4200 );
    EXPECT_EQ( node->DI[7], 1 );
    EXPECT_EQ( node->DI[17], 1 );
    EXPECT_EQ( node->DI[24], 0 );
    }

TEST_F( local_bus_io, wrong_article_or_missing_module_blocks_exchange )
    {
    create( { { 2688022, 2, false } } );
    driver->modules[0].article = 2688048;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_EQ( driver->reads, 0 );
    EXPECT_EQ( driver->writes, 0 );
    driver->modules[0].article = 2688022;
    driver->modules[0].missing = true;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->reads, 0 );
    driver->modules[0].missing = false;
    EXPECT_EQ( manager->read_inputs(), 0 );
    EXPECT_FALSE( node->is_set_err );
    }

TEST_F( local_bus_io, wrong_process_data_size_blocks_exchange )
    {
    create( { { 2688022, 2, false } } );
    driver->modules[0].bytes = 4;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->reads, 0 );
    driver->modules[0].bytes = 0;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->reads, 0 );
    }

TEST_F( local_bus_io, oversized_module_blocks_exchange )
    {
    create( { { 2688022, 2, false } } );
    driver->modules[0].bytes = 1025;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->reads, 0 );
    }

TEST_F( local_bus_io, changed_configuration_before_write_blocks_output )
    {
    create( { { 2688048, 2, false } } );
    ASSERT_EQ( manager->read_inputs(), 0 );
    driver->modules[0].article = 2688022;
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_EQ( driver->writes, 0 );
    EXPECT_EQ( driver->failures, 1 );
    }

TEST_F( local_bus_io, build_without_driver_reports_error )
    {
    manager = std::make_unique<uni_io_manager>( nullptr );
    manager->init( 1 );
    node = manager->add_node( 0, 202, 1, "", "A100",
        16, 16, 1, 2, 1, 2 );
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_TRUE( node->read_io_error_flag );
    EXPECT_EQ( manager->write_outputs(), 1 );
    }

TEST_F( local_bus_io, empty_controller_does_not_initialize_driver )
    {
    create( {} );
    EXPECT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( manager->write_outputs(), 0 );
    EXPECT_EQ( driver->initializations, 0 );
    }

TEST_F( local_bus_io, driver_read_failure_blocks_outputs_and_recovers )
    {
    create( { { 2688048, 2, false } } );
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_EQ( driver->writes, 0 );
    ASSERT_EQ( manager->read_inputs(), 0 );
    driver->read_result = false;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->failures, 1 );
    EXPECT_TRUE( node->read_io_error_flag );
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_EQ( driver->writes, 0 );
    driver->read_result = true;
    ASSERT_EQ( manager->read_inputs(), 0 );
    EXPECT_EQ( manager->write_outputs(), 0 );
    }

TEST_F( local_bus_io, write_failure_does_not_publish_output_values )
    {
    create( { { 2688048, 2, false } } );
    ASSERT_EQ( manager->read_inputs(), 0 );
    node->DO_[0] = 1;
    driver->write_result = false;
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_EQ( driver->failures, 1 );
    EXPECT_EQ( node->DO[0], 0 );
    EXPECT_TRUE( node->read_io_error_flag );
    }

TEST_F( local_bus_io, unavailable_driver_and_exceptions_are_errors )
    {
    create( { { 2688022, 2, false } } );
    driver->initialized = false;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->reads, 0 );
    driver->initialized = true;
    driver->operational = false;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->reads, 0 );
    driver->operational = true;
    driver->throw_on_read = true;
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( driver->failures, 3 );
    EXPECT_EQ( node->state, io_manager::io_node::ST_NO_CONNECT );
    }

TEST_F( local_bus_io, two_local_controllers_block_exchange )
    {
    create( { { 2688022, 2, false } } );
    manager->init( 2 );
    auto first = manager->add_node( 0, 202, 1, "", "A100",
        16, 16, 1, 2, 1, 2 );
    auto second = manager->add_node( 1, 203, 2, "", "A200",
        16, 16, 1, 2, 1, 2 );
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_TRUE( first->read_io_error_flag );
    EXPECT_TRUE( second->read_io_error_flag );
    EXPECT_EQ( driver->initializations, 0 );
    }

TEST_F( local_bus_io, library_and_ioctl_nodes_do_not_open_bus_together )
    {
    create( { { 2688022, 2, false } } );
    std::vector<local_bus_backend> selected;
    select_at_runtime( selected );
    manager->init( 2 );
    auto first = manager->add_node( 0, 202, 1, "", "A100",
        16, 16, 1, 2, 1, 2 );
    auto second = manager->add_node( 1, 204, 2, "", "A200",
        16, 16, 1, 2, 1, 2 );
    EXPECT_EQ( manager->read_inputs(), 1 );
    EXPECT_EQ( manager->write_outputs(), 1 );
    EXPECT_TRUE( first->read_io_error_flag );
    EXPECT_TRUE( second->read_io_error_flag );
    EXPECT_TRUE( selected.empty() );
    }
