/// @file uni_bus_coupler_io.h
/// @brief Работа с I/O узлами для OC Linux.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 223 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-17 09:39:32#$.

#ifndef UNI_BUS_COUPLER_IO_H
#define UNI_BUS_COUPLER_IO_H

#ifdef WIN_OS
#include <winsock2.h>
#include "w_tcp_cmctr.h"
#else
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include "l_tcp_cmctr.h"
#endif // WIN_OS

#include "bus_coupler_io.h"
#include "dtime.h"
#include "PAC_err.h"
#include <array>
#include <chrono>
#include <vector>

//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода для OC Linux.
///
///
class uni_io_manager : public io_manager
    {
#ifdef PTUSA_TEST
    public:
#else
    private:
#endif // PTUSA_TEST
        enum CONSTANTS
            {
            MAX_MODBUS_REGISTERS_PER_QUERY = 123,
            BUFF_SIZE = 262,
            PHOENIX_INPUTREGISTERS_STARTADDRESS = 8000,
            PHOENIX_HOLDINGREGISTERS_STARTADDRESS = 9000,
            PHOENIX_STATUS_REGISTER_ADDRESS = 7996,
            PHOENIX_DIAGNOSTIC_STATUS_REGISTER_ADDRESS = 7997,
            BYTE_SHIFT_MULTIPLIER = 256,
            };

        u_char buff[ BUFF_SIZE ] = { 0 };
        u_char* resultbuff = nullptr;
        u_char* writebuff = nullptr;

        struct exchange
            {
            io_node* node = nullptr;
            std::array<u_char, BUFF_SIZE> request{}, response{};
            int send_size = 0, expected_size = 0;
            int sent = 0, received = 0, frame_size = 6;
            int result = 1;
            size_t previous = static_cast<size_t>( -1 );
            bool started = false, done = false, consumed = false;
            std::chrono::steady_clock::time_point deadline{}, started_at{}, sent_at{};
            };
        std::vector<exchange> phase_exchanges;
        bool phase_active = false, phase_executed = false;
        bool phoenix_udp_active = false;

        void sync_phoenix_transport();

        // The phase remains synchronous; only socket waits overlap.
        void prepare_phase( bool writing );
        void run_exchanges( std::vector<exchange>& exchanges );
        int prepare_node( io_node* node );
        void queue_exchange( io_node* node, int send_size, int expected_size );
        void make_read_request( unsigned int address, unsigned int quantity,
            unsigned char function, unsigned char station = 0 );
        void make_write_request( unsigned int address, unsigned int quantity,
            unsigned char station = 0 );
        void make_wago_do_request( io_node* node );
        void make_wago_ao_request( io_node* node );
        void make_phoenix_output( io_node* node, unsigned int start_register,
            unsigned int registers_count, unsigned int& module_type,
            unsigned int& module_offset );

        /// @brief Обмен с узлом I/O.
        ///
        /// @param node             - узел I/O, с которым осуществляется обмен.
        /// @param bytes_to_send    - размер данных для отсылки.
        /// @param bytes_to_receive - размер данных для получения.
        ///
        /// @return -   0 - полный ответ (включая Modbus exception).
        /// @return -   1 - обмен пропущен: подключение или ошибка предыдущего запроса.
        /// @return - < 0 - ошибка.
        virtual int e_communicate( io_node* node, int bytes_to_send,
            int bytes_to_receive );

        virtual int read_input_registers( io_node* node, unsigned int address,
            unsigned int quantity, unsigned char station = 0 );
        int write_holding_registers( io_node* node, unsigned int address,
            unsigned int quantity, unsigned char station = 0 );

        /// @brief Логирование ошибки обмена с узлом.
        ///
        /// @param cmd Команда.
        /// @param node_name Имя узла.
        /// @param node_ip_address IP-адрес узла.
        /// @param exp_fun_code Ожидаемый код функции.
        /// @param rec_fun_code Полученный код функции.
        /// @param exp_size Ожидаемый размер ответа.
        /// @param rec_size Полученный размер ответа.
        virtual void add_err_to_log( const char* cmd,
            const char* node_name, const char* node_ip_address,
            int exp_fun_code, int rec_fun_code, int exp_size, int rec_size ) const;

        /// @brief Read status register for Phoenix BK ETH nodes.
        ///
        /// @param nd - node to read status register from.
        bool read_phoenix_status_register( io_node* nd );

    public:
        struct phase_timing
            {
            uint64_t last_us = 0, max_us = 0, total_us = 0, cycles = 0;
            };
        const phase_timing& get_read_timing() const { return read_timing; }
        const phase_timing& get_write_timing() const { return write_timing; }

        int read_inputs() override;
        int write_outputs() override;

        uni_io_manager();

        ~uni_io_manager() override = default;

        /// @brief Данный класс является некопируемым и неперемещаемым.
        uni_io_manager( const uni_io_manager& ) = delete;
        uni_io_manager& operator=( const uni_io_manager& ) = delete;
        uni_io_manager( uni_io_manager&& ) = delete;

        static constexpr int NET_CONNECTING = 8;

        /// @brief Инициализация соединения с узлом I/O без ожидания.
        ///
        /// @param node - узел I/O, с которым осуществляется соединение.
        ///
        /// @return -   0 - ок.
        /// @return NET_CONNECTING - подключение продолжается; 1..7 - ошибка.
        int net_init( io_node* node ) const;

        /// @brief Отключение от узла.
        ///
        /// @param node - узел, от которого отключаемся.
        void disconnect( io_node* node ) override;

    private:
        phase_timing read_timing, write_timing;
    };
//-----------------------------------------------------------------------------
#endif // UNI_BUS_COUPLER_IO_H
