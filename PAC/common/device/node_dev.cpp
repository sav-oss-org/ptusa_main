#include "fmt/format.h"

#include "node_dev.h"
#include "bus_coupler_io.h"
#include "log.h"

#include <cstdlib>
#include <string_view>
#include <utility>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#ifdef LINUX_OS
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>

#include <sys/wait.h>
#else
#include <ws2tcpip.h>
#endif


//-----------------------------------------------------------------------------
std::string node_dev::get_A1_ipv4()
    {
    std::string result;

    // Получаем IP-адрес узла контроллера, который в текущей реализации
    // хранится по индексу `0` и имеет имя `A1`.
    if ( const auto NODE_A1 = std::as_const( *G_IO_MANAGER() ).get_node( 0 );
        NODE_A1 != &io_manager::IO_NODE_STUB &&
        strcmp( NODE_A1->name, "A1" ) == 0 )
        {
        result = NODE_A1->ip_address;
        }

    return result;
    }
//----------------------------------------------------------------------------
node_dev::node_dev( const char* name ) :
    device( name, device::DEVICE_TYPE::DT_NODE,
        device::DEVICE_SUB_TYPE::DST_NODE, 0 )
    {
    }
//-----------------------------------------------------------------------------
static std::string replace_action(
    const std::string& cmd, std::string_view from, std::string_view to )
    {
    std::string out{ cmd };

    if ( const auto pos = out.find( from ); pos != std::string::npos )
        {
        out.replace( pos, from.length(), to );
        }
    return out;
    }
//-----------------------------------------------------------------------------
void node_dev::set_io_node( io_manager::io_node* io_node )
    {
    if ( node )
        {
        G_LOG->info( "Node '%s' - already has an I/O node assigned ('%s').",
            get_name(), node->ip_address );
        return;
        }

    if ( !io_node )
        {
        G_LOG->warning(
            "Node '%s' - null pointer passed as I/O node.", get_name() );
        return;
        }

    // Check IPv4.
    if ( struct in_addr ipv4_addr;
        inet_pton( AF_INET, io_node->ip_address, &ipv4_addr ) != 1 )
        {
        G_LOG->warning( "Node '%s' - invalid IPv4 address ('%s').", get_name(),
            io_node->ip_address );
        return;
        }

    // Bind the node once the node IPv4 is validated. Controller IPv4 detection
    // is needed only for WEB forwarding rules.
    node = io_node;

    controller_ip = get_A1_ipv4();
    if ( controller_ip.empty() )
        {
        G_LOG->warning( "Node '%s' - controller IPv4 address was not detected.",
            get_name() );
        return;
        }
    if ( struct in_addr ipv4_addr;
        inet_pton( AF_INET, controller_ip.c_str(), &ipv4_addr ) != 1 )
        {
        G_LOG->warning( "Node '%s' - invalid controller IPv4 address ('%s').",
            get_name(), controller_ip.c_str() );
        controller_ip = "";
        return;
        }

#ifdef LINUX_OS
    port_controller_web = EXTERNAL_WEB_PORT_BASE + node->number;

    const std::string IPTABLES = "sudo -n /usr/sbin/iptables";
    dnat = IPTABLES + " -t nat -A PREROUTING -p tcp -d " + controller_ip +
           " --dport " + std::to_string( port_controller_web ) +
           " -j DNAT --to-destination " + node->ip_address + ":" +
           PORT_NODE_WEB;

    forward_in = IPTABLES + " -A FORWARD -p tcp -d " + node->ip_address +
                 " --dport " + PORT_NODE_WEB + " -j ACCEPT";

    masq = IPTABLES + " -t nat -A POSTROUTING -p tcp -d " + node->ip_address +
           " --dport " + PORT_NODE_WEB + " -j MASQUERADE";

    dnat_delete = replace_action( dnat, " -A ", " -D " );
    forward_in_delete = replace_action( forward_in, " -A ", " -D " );
    masq_delete = replace_action( masq, " -A ", " -D " );

    dnat_check = replace_action( dnat, " -A ", " -C " );
    forward_in_check = replace_action( forward_in, " -A ", " -C " );
    masq_check = replace_action( masq, " -A ", " -C " );

    // Для отладки. Просмотр входящих правил.
    // sudo /usr/sbin/iptables -t nat -S; sudo /usr/sbin/iptables -S FORWARD
    //
    // Это команды просмотра текущих правил, без изменения конфигурации.
    //    •	sudo /usr/sbin/iptables -t nat -S
    //    Показать все правила таблицы nat в “скриптовом” виде ( -A ... ):
    //      • PREROUTING (обычно DNAT),
    //      • POSTROUTING (обычно SNAT/MASQUERADE),
    //      • OUTPUT (локально сгенерированный трафик).
    //
    //    • sudo /usr/sbin/iptables -S FORWARD
    //    Показать правила только цепочки FORWARD (таблица filter по
    //    умолчанию), то есть что разрешено/запрещено для транзитного трафика
    //    между интерфейсами.

#endif // LINUX_OS
    }
//-----------------------------------------------------------------------------
void node_dev::evaluate_io()
    {
    // Обновление состояния узла.
    if ( node )
        {
        // Обновление состояния на основе состояния узла.
        device::direct_set_state(
            static_cast<int>( node->get_display_state() ) );
        }
    else
        {
        device::direct_set_state( -2 );
        }
    }
//-----------------------------------------------------------------------------
int node_dev::run_cmd_exit_code( const char* cmd, int expected )
    {
    res_msg[ 0 ] = '\0';

    const std::filesystem::path file_path = "ptusa_cmd_output.txt";
    std::string cmd_debug{ cmd };
    cmd_debug += " > \"" + file_path.string() + "\" 2>&1";

    G_LOG->debug( "node_dev::run_cmd - '%s'", cmd_debug.c_str() );

    int rc = std::system( cmd_debug.c_str() );
#ifdef LINUX_OS
    rc = WIFEXITED( rc ) ? WEXITSTATUS( rc ) : -1;
#endif

    if ( rc != 0 && rc != expected )
        {
        if ( !std::filesystem::exists( file_path ) )
            {
            // LCOV_EXCL_START
            auto res = fmt::format_to_n( res_msg, i_log::C_BUFF_SIZE,
                "command result ('{}') not found", cmd );
            // LCOV_EXCL_STOP
            *res.out = '\0';
            }
        else
            {
            std::ifstream file( file_path );
            std::string content( ( std::istreambuf_iterator<char>( file ) ),
                std::istreambuf_iterator<char>() );
            // LCOV_EXCL_START
            auto res = fmt::format_to_n( res_msg, i_log::C_BUFF_SIZE,
                "command result ('{}'): {}", cmd, content );
            // LCOV_EXCL_STOP
            *res.out = '\0';
            if ( res.out != res_msg && *( res.out - 1 ) == '\n' )
                {
                // Удаляем последний символ новой строки.
                *( res.out - 1 ) = '\0';
                }
            }
        }

    return rc;
    }
//-----------------------------------------------------------------------------
#ifdef LINUX_OS
/// Checks if sudo is available without a password prompt.
bool node_dev::check_sudo_available()
    {
    return node_dev::run_cmd_exit_code( "sudo -n /usr/sbin/iptables -h" ) == 0;
    }
#endif // LINUX_OS
//-----------------------------------------------------------------------------
namespace
    {
    bool ensure_rule( const char* check_cmd, const char* append_cmd )
        {
        if ( const int check_rc = node_dev::run_cmd_exit_code( check_cmd, 1 );
            check_rc == 0 )
            {
            return true;
            }
        else if ( check_rc != 1 )
            {
            return false;
            }

        return node_dev::run_cmd_exit_code( append_cmd ) == 0;
        }

    bool delete_all_matches( const char* check_cmd, const char* delete_cmd )
        {
        for ( ;; )
            {
            const int check_rc = node_dev::run_cmd_exit_code( check_cmd, 1 );
            if ( check_rc == 1 )
                {
                return true;
                }

            if ( check_rc != 0 )
                {
                return false;
                }

            if ( node_dev::run_cmd_exit_code( delete_cmd ) != 0 )
                {
                return false;
                }
            }
        }
    }
//-----------------------------------------------------------------------------
#ifdef LINUX_OS
bool node_dev::check_ip_forward()
    {
    res_msg[ 0 ] = '\0';


    std::ifstream f( "/proc/sys/net/ipv4/ip_forward" );
    if ( !f )
        {
        // LCOV_EXCL_START
        auto res = fmt::format_to_n( res_msg, i_log::C_BUFF_SIZE,
            "cannot open '/proc/sys/net/ipv4/ip_forward'" );
        // LCOV_EXCL_STOP
        *res.out = '\0';
        return false;
        }

    auto ip_forward = 0;
    f >> ip_forward;
    if ( !f )
        {
        // LCOV_EXCL_START
        auto res = fmt::format_to_n( res_msg, i_log::C_BUFF_SIZE,
            "cannot read value from '/proc/sys/net/ipv4/ip_forward'" );
        // LCOV_EXCL_STOP
        *res.out = '\0';
        return false;
        }

    G_LOG->debug(
        "node_dev::check_ip_forward - net.ipv4.ip_forward=%d", ip_forward );

    if ( ip_forward != 1 )
        {
        // LCOV_EXCL_START
        auto res = fmt::format_to_n( res_msg, i_log::C_BUFF_SIZE,
            "IP forwarding is disabled ('net.ipv4.ip_forward=0')" );
        // LCOV_EXCL_STOP
        *res.out = '\0';
        return false;
        }

    return true;
    }
#endif // LINUX_OS
//-----------------------------------------------------------------------------
const char* node_dev::get_cmd_output()
    {
    return res_msg;
    }
//-----------------------------------------------------------------------------
int node_dev::process_web_cmd( int new_web_value )
    {
    if ( dnat_delete.empty() || forward_in_delete.empty() ||
         masq_delete.empty() )
        {
        G_LOG->warning(
            "'%s': no iptables rules defined for web port forwarding.",
            get_name() );
        return 1;
        }

#ifdef LINUX_OS
    if ( !check_sudo_available() )
        {
        G_LOG->error( "'%s': web port command failed - %s.", get_name(),
            node_dev::get_cmd_output() );
        return 1;
        }

    if ( !check_ip_forward() )
        {
        G_LOG->error( "'%s': web port command failed - %s.", get_name(),
            node_dev::get_cmd_output() );
        return 1;
        }
#endif // LINUX_OS

    if ( new_web_value != 0 && web_value == 0 )
        {
        if ( auto res = ensure_rule( dnat_check.c_str(), dnat.c_str() ); !res )
            {
            G_LOG->error( "'%s': failed to enable web port checking - %s.",
                get_name(), node_dev::get_cmd_output() );
            return 1;
            }
        if ( auto res =
                 ensure_rule( forward_in_check.c_str(), forward_in.c_str() );
            !res )
            {
            G_LOG->error( "'%s': failed to enable web port forwarding - %s.",
                get_name(), node_dev::get_cmd_output() );
            return 1;
            }
        if ( auto res = ensure_rule( masq_check.c_str(), masq.c_str() ); !res )
            {
            G_LOG->error( "'%s': failed to enable web port masquerading - %s.",
                get_name(), node_dev::get_cmd_output() );
            return 1;
            }

        G_LOG->info( "'%s': web port forwarding enabled.", get_name() );
        web_value = 1;
        return 0;
        }

    else if ( new_web_value == 0 && web_value == 1 )
        {
        if ( auto res =
                 delete_all_matches( dnat_check.c_str(), dnat_delete.c_str() );
            !res )
            {
            G_LOG->error(
                "'%s': failed to disable web port forwarding (dnat) - %s.",
                get_name(), node_dev::get_cmd_output() );
            return 1;
            }
        if ( auto res = delete_all_matches(
                 forward_in_check.c_str(), forward_in_delete.c_str() );
            !res )
            {
            G_LOG->error(
                "'%s': failed to disable web port forwarding (forward_in) - %s.",
                get_name(), node_dev::get_cmd_output() );
            return 1;
            }
        if ( auto res =
                 delete_all_matches( masq_check.c_str(), masq_delete.c_str() );
            !res )
            {
            G_LOG->error(
                "'%s': failed to disable web port forwarding (masq) - %s.",
                get_name(), node_dev::get_cmd_output() );
            return 1;
            }

        G_LOG->info( "'%s': web port forwarding disabled.", get_name() );
        web_value = 0;
        return 0;
        }

    return 0;
    }

int node_dev::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( strcmp( prop, "WEB" ) == 0 )
        {
        if ( idx != 0 )
            {
            G_LOG->warning( "Invalid index %u for property '%s' of node '%s'.",
                idx, prop, get_name() );
            return 1;
            }

        if ( !node )
            {
            G_LOG->warning( "Node '%s' is not initialized.", get_name() );
            return 1;
            }

        return process_web_cmd( static_cast<int>( val ) );
        }

    else if ( strcmp( prop, "STARTUP" ) == 0 )
        {
        startup_value = static_cast<int>( val );
        return 0;
        }

    return device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
int node_dev::save_device( char* buff ) const
    {
    // LCOV_EXCL_START
    auto res_n = fmt::format_to_n( buff, MAX_COPY_SIZE,
        "{}={{ST={}, WEB={}, STARTUP={}, IP='{}'}},\n", get_name(), get_state(),
        web_value, startup_value, get_ip() );
    // LCOV_EXCL_STOP

    return static_cast<int>( res_n.size );
    }
//-----------------------------------------------------------------------------
const char* node_dev::get_name_in_Lua() const
    {
    return "node_dev";
    }
//-----------------------------------------------------------------------------
const char* node_dev::get_ip() const
    {
    if ( node )
        {
        return node->ip_address;
        }

    return "";
    }
//-----------------------------------------------------------------------------
const char* node_dev::get_controller_ip() const
    {
    return controller_ip.c_str();
    }
//-----------------------------------------------------------------------------
