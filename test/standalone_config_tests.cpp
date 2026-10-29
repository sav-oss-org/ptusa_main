#include <chrono>
#include <filesystem>
#include <fstream>

#include "includes.h"
#include "prj_mngr.h"
#include "PAC_info.h"
#include "tcp_cmctr.h"

extern bool G_NO_IO_NODES;
extern bool G_READ_ONLY_IO_NODES;
extern int G_DEBUG;

class StandaloneConfigTest : public testing::Test
    {
    protected:
        void SetUp() override
            {
            const auto id = std::chrono::steady_clock::now()
                .time_since_epoch().count();
            directory = std::filesystem::current_path() /
                ( "standalone config " + std::to_string( id ) );
            std::filesystem::create_directory( directory );
            script = ( directory / "test script.plua" ).generic_string();
            config = ( directory / "settings.ini" ).generic_string();
            std::ofstream( script ) << "system = {}\n";
            }

        void TearDown() override
            {
            G_DEBUG = savedDebug;
            G_NO_IO_NODES = savedNoIo;
            G_READ_ONLY_IO_NODES = savedReadOnlyIo;
            G_PAC_INFO()->set_phoenix_modbus_udp( savedUdp );
            G_PAC_INFO()->set_phoenix_modbus_udp_timeout_ms( savedTimeout );
            tcp_communicator::set_port( savedPort, savedModbusPort );
            std::error_code error;
            std::filesystem::remove_all( directory, error );
            }

        void WriteConfig( const std::string& settings )
            {
            std::ofstream( config ) << "\xEF\xBB\xBF# Настройки\n"
                << "script = \"" << script << "\"\n" << settings;
            }

        int Parse( std::initializer_list<std::string> arguments )
            {
            std::vector<const char*> argv = { "ptusa_main" };
            for ( const auto& argument : arguments )
                {
                argv.push_back( argument.c_str() );
                }
            testing::internal::CaptureStdout();
            const auto result = manager.proc_main_params(
                static_cast<int>( argv.size() ), argv.data() );
            output = testing::internal::GetCapturedStdout();
            return result;
            }

        project_manager manager;
        std::filesystem::path directory;
        std::string script;
        std::string config;
        std::string output;

    private:
        const int savedDebug = G_DEBUG;
        const bool savedNoIo = G_NO_IO_NODES;
        const bool savedReadOnlyIo = G_READ_ONLY_IO_NODES;
        const bool savedUdp = G_PAC_INFO()->is_phoenix_modbus_udp();
        const unsigned int savedTimeout =
            G_PAC_INFO()->get_phoenix_modbus_udp_timeout_ms();
        const int savedPort = tcp_communicator::get_port();
        const int savedModbusPort = tcp_communicator::get_modbus_port();
    };

TEST_F( StandaloneConfigTest, defaults_without_config )
    {
    EXPECT_DOUBLE_EQ( 1, manager.sleep_time_ms );
    EXPECT_DOUBLE_EQ( 5, manager.min_cycle_time );
    ASSERT_EQ( 0, Parse( { script } ) );
    EXPECT_DOUBLE_EQ( 1, manager.sleep_time_ms );
    EXPECT_DOUBLE_EQ( 5, manager.min_cycle_time );
    }

TEST_F( StandaloneConfigTest, loads_console_settings )
    {
    WriteConfig( "\n; Комментарий\n"
        "debug=true\nno_io=false\nread_only_io=true\nrcrc=false\n"
        "port=19000\nopc=r\nphoenix_modbus_udp=true\n"
        "phoenix_modbus_udp_timeout_ms=35\n"
        "path = 'project files'\nsys_path=system\nextra_paths=extra\n"
        "sleep_time=0.1\nmin_cycle_time=12.5\n" );
    ASSERT_EQ( 0, Parse( { "--config", config } ) ) << output;
    EXPECT_EQ( script, manager.main_script );
    EXPECT_EQ( "project files/", manager.path );
    EXPECT_EQ( "system/", manager.sys_path );
    EXPECT_EQ( "extra/", manager.extra_paths );
    EXPECT_DOUBLE_EQ( 0.1, manager.sleep_time_ms );
    EXPECT_DOUBLE_EQ( 12.5, manager.min_cycle_time );
    EXPECT_EQ( 1, G_DEBUG );
    EXPECT_FALSE( G_NO_IO_NODES );
    EXPECT_TRUE( G_READ_ONLY_IO_NODES );
    EXPECT_TRUE( G_PAC_INFO()->is_phoenix_modbus_udp() );
    EXPECT_EQ( 35u, G_PAC_INFO()->get_phoenix_modbus_udp_timeout_ms() );
    EXPECT_EQ( 19000, tcp_communicator::get_port() );
    EXPECT_EQ( 19502, tcp_communicator::get_modbus_port() );
    EXPECT_NE( std::string::npos, output.find( "only read" ) );
    EXPECT_EQ( std::string::npos, output.find( "Resetting parameters" ) );
    }

TEST_F( StandaloneConfigTest, empty_config_keeps_defaults )
    {
    std::ofstream( config ) << "# Только консольные параметры\n";
    ASSERT_EQ( 0, Parse( { "-c", config, script } ) ) << output;
    EXPECT_DOUBLE_EQ( 1, manager.sleep_time_ms );
    EXPECT_DOUBLE_EQ( 5, manager.min_cycle_time );
    }

TEST_F( StandaloneConfigTest, console_overrides_file )
    {
    WriteConfig( "debug=true\nno_io=true\nread_only_io=true\n"
        "port=19000\nsleep_time=0.1\nmin_cycle_time=20\nopc=rw\n" );
    G_DEBUG = 0;
    const auto replacement = ( directory / "replacement.plua" ).generic_string();
    std::ofstream( replacement ) << "system = {}\n";
    ASSERT_EQ( 0, Parse( { "--config=" + config, replacement,
        "--debug=false", "--no_io=false", "--read_only_io=false",
        "-p", "18000", "--sleep_time=1.5", "--min_cycle_time=0",
        "--opc=off" } ) ) << output;
    EXPECT_EQ( replacement, manager.main_script );
    EXPECT_DOUBLE_EQ( 1.5, manager.sleep_time_ms );
    EXPECT_DOUBLE_EQ( 0, manager.min_cycle_time );
    EXPECT_EQ( 0, G_DEBUG );
    EXPECT_FALSE( G_NO_IO_NODES );
    EXPECT_FALSE( G_READ_ONLY_IO_NODES );
    EXPECT_EQ( 18000, tcp_communicator::get_port() );
    EXPECT_NE( std::string::npos, output.find( "OPC UA server is disabled" ) );
    }

TEST_F( StandaloneConfigTest, rejects_invalid_config )
    {
    EXPECT_EQ( 2, Parse( { "--config", config } ) );
    EXPECT_NE( std::string::npos, output.find( "Cannot open config file" ) );
    for ( const auto& setting : { "unknown_option=1\n", "sleep_time\n",
        "sleep_time=1\nsleep_time=2\n", "path=\"unclosed\n",
        "no_io=invalid\n", "sleep_time=invalid\n", "port=invalid\n",
        "sleep_time=0\n", "sleep_time=10.1\n", "min_cycle_time=-1\n",
        "min_cycle_time=20.1\n", "sleep_time=nan\n",
        "min_cycle_time=inf\n" } )
        {
        SCOPED_TRACE( setting );
        WriteConfig( setting );
        EXPECT_EQ( 2, Parse( { "--config", config } ) ) << output;
        EXPECT_NE( std::string::npos, output.find( "Error:" ) );
        }
    }

TEST_F( StandaloneConfigTest, config_before_positional_separator )
    {
    WriteConfig( "sleep_time=0.5\nmin_cycle_time=0\n" );
    EXPECT_EQ( 0, Parse( { "--config", config, "--", script } ) ) << output;
    EXPECT_DOUBLE_EQ( 0.5, manager.sleep_time_ms );
    EXPECT_DOUBLE_EQ( 0, manager.min_cycle_time );
    }

TEST_F( StandaloneConfigTest, validates_console_timing )
    {
    for ( const auto& setting : { "--sleep_time=0", "--sleep_time=-1",
        "--sleep_time=10.1", "--sleep_time=nan", "--sleep_time=inf",
        "--min_cycle_time=-1", "--min_cycle_time=20.1",
        "--min_cycle_time=nan", "--min_cycle_time=inf" } )
        {
        SCOPED_TRACE( setting );
        EXPECT_EQ( 2, Parse( { script, setting } ) );
        }
    EXPECT_EQ( 0, Parse( { script, "--sleep_time=10", "--min_cycle_time=20" } ) );
    EXPECT_EQ( 0, Parse( { script, "--sleep_time=0.1", "--min_cycle_time=0" } ) );
    }

TEST_F( StandaloneConfigTest, help_and_version_ignore_config )
    {
    EXPECT_EQ( 1, Parse( { "--config", config, "--help" } ) );
    EXPECT_NE( std::string::npos, output.find( "--config" ) );
    EXPECT_NE( std::string::npos, output.find( "--min_cycle_time" ) );
    EXPECT_EQ( 1, Parse( { "--config", config, "--version" } ) );
    }
