/// @file node_dev.h
/// @brief Устройство управления узлом сетевых настроек.
///
/// Позволяет централизованно управлять сетевыми настройками узла и
/// отслеживать состояние команд проброса портов.

#pragma once

#include "device/device.h"
#include "bus_coupler_io.h"

#include <string>

//-----------------------------------------------------------------------------
/// @brief Устройство управления узлом сетевых настроек.
///
/// Предоставляет доступ к IP-адресу узла, состоянию узла и командам для
/// управления пробросом портов.
class node_dev : public device
	{
	public:

		/// @param name - имя устройства.
		explicit node_dev( const char* name );

        void set_io_node( io_manager::io_node *io_node );

		~node_dev() override = default;

		void evaluate_io() override;

        int save_device( char* buff ) const override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

		const char* get_name_in_Lua() const override;

		/// @brief Получение IP-адреса узла.
		///
		/// @return - IP-адрес узла в виде строки.
		const char* get_ip() const;

        /// @brief Получение IP-адреса контроллера (узел `А1`).
        ///
        /// @return - IP-адрес узла в виде строки.
        const char* get_controller_ip() const;

        static int run_cmd_exit_code( const char* cmd,
            int expected = 0 );

#ifdef LINUX_OS
        static bool check_sudo_available();

        static bool check_ip_forward();
#endif // LINUX_OS

        static const char* get_cmd_output();

#ifndef PTUSA_TEST
	private:
#endif

        int process_web_cmd( int new_web_value );

        static std::string get_A1_ipv4();
        std::string controller_ip;

        int web_value{};      ///< Состояние проброса порта 80.
        int startup_value{};  ///< Состояние проброса порта 1962.

        io_manager::io_node* node{ nullptr };

        inline static const char* PORT_NODE_WEB{ "80" };
        inline static constexpr int EXTERNAL_WEB_PORT_BASE = 10'000;

        int port_controller_web{};

        std::string dnat{};
        std::string forward_in{};
        std::string masq{};

        std::string dnat_delete{};
        std::string forward_in_delete{};
        std::string masq_delete{};

        std::string dnat_check{};
        std::string forward_in_check{};
        std::string masq_check{};

        inline static char res_msg[ i_log::C_BUFF_SIZE ]{};
	};
