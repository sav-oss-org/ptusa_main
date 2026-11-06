/// @file bus_coupler_io.h
/// @brief Работа с устройствами на основе данных с модулей ввода\вывода, узлы
/// ввода\вывода.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef IO_H
#define IO_H

#include "smart_ptr.h"

#include "dtime.h"
//-----------------------------------------------------------------------------
/// @brief Устройство на основе модулей ввода/вывода.
///
/// В общем случае у устройства может быть один или несколько каналов
/// ввода/вывода (дискретных или аналоговых).
class io_device
    {
    public:
        const char* name;

        io_device( const char* name );

        virtual ~io_device();

		enum VENDOR
			{
			WAGO,
			PHOENIX,
			};

		enum IOLINKSTATE
			{
			OK,

			NOTCONNECTED = 100,
			DEVICEERROR,
			};

        enum class ERRORS
            {
            NO_ERR = 0,
            UNDER_RANGE = 2,
            OVER_RANGE = 3,
            OUT_OF_RANGE = 4,

            BAD_IO_DATA = 100,

            LAST_ERR_IDX
            };

#ifndef PTUSA_TEST
    protected:
#endif
        /// @brief Получение состояния канала дискретного выхода.
        ///
        /// @param index - индекс канала в таблице дискретных выходных каналов
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int get_DO( u_int index ) const;

        /// @brief Установка состояния канала дискретного выхода.
        ///
        /// @param index - индекс канала в таблице дискретных выходных каналов
        /// устройства.
        /// @param value - новое состояние канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int set_DO( u_int index, char value );

        /// @brief Получение состояния канала дискретного входа.
        ///
        /// @param index - индекс канала в таблице дискретных входных каналов
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int get_DI( u_int index ) const;

        /// @brief Получение состояния канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        /// @param min_value - минимальное значение канала.
        /// @param max_value - максимальное значение канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        float get_AO( u_int index, float min_value, float max_value ) const;

        /// @brief Установка состояния канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        /// @param value - новое состояние канала.
        /// @param min_value - минимальное значение канала.
        /// @param max_value - максимальное значение канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int set_AO( u_int index, float value, float min_value = 0,
            float max_value = 0 );

        /// @brief Получение состояния канала аналогового входа.
        ///
        /// @param index - индекс канала в таблице аналоговых входных каналов
        /// устройства.
        /// @param min_value - минимальное значение канала.
        /// @param max_value - максимальное значение канала.
        /// @param err - результат, 0 - Ок, >0 - ошибка.
        ///
        /// @return - аналоговое значение.
        float get_AI( u_int index, float min_value = 0, float max_value = 0,
            int& err = last_err ) const;

        /// @brief Получение данных канала аналогового входа.
        ///
        /// @param index - индекс канала в таблице аналоговых входных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int_2* get_AI_data( u_int index ) const;


		/// @brief Получение состояния IO-Link устройства типа AI.
        ///
		/// @param index - индекс канала в таблице аналоговых входных каналов
		/// устройства.
		///
		/// @return -  состояние устройства.
		IOLINKSTATE get_AI_IOLINK_state(u_int index) const;

		/// @brief Получение состояния IO-Link устройства типа AI.
		///
		/// @param index - индекс канала в таблице аналоговых входных каналов
		/// устройства.
		///
		/// @return -  состояние устройства.
		IOLINKSTATE get_AO_IOLINK_state(u_int index) const;

        /// @brief Получение установленных данных канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int_2* get_AO_write_data( u_int index );

        const int_2* get_AO_write_data( u_int index ) const;

        /// @brief Получение текущих данных канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        const int_2* get_AO_read_data( u_int index ) const;

        virtual void print() const;

        /// @brief Check output DO channel network node state.
        ///
        /// Checks if the network node associated with the output channel
        /// is available and not in error/PP mode state.
        ///
        /// @param index - index of the channel in DO channels table.
        ///
        /// @return 1 - node is OK, -1 - node has PP mode,
        ///         0 - no output channels configured.
        int check_output_DO_node_PP_state( u_int index = 0 ) const;

        /// @brief Check output AO channel network node state.
        ///
        /// Checks if the network node associated with the output channel
        /// is available and not in error/PP mode state.
        ///
        /// @param index - index of the channel in AO channels table.
        ///
        /// @return 1 - node is OK, -1 - node has PP mode,
        ///         0 - no output channels configured.
        int check_output_AO_node_PP_state( u_int index = 0 ) const;

#ifdef PTUSA_TEST
        public:
#else
        private:
#endif

        /// @brief Группа каналов ввода/вывода устройства.
        struct IO_channels
            {
            enum CHANNEL_TYPE
                {
                CT_DI = 1,
                CT_DO,
                CT_AI,
                CT_AO,
                };

            u_int  count;   ///< Количество каналов.
            u_int* tables;  ///< Массив таблиц.
            u_int* offsets; ///< Массив смещений в пределах таблиц.
			int* module_offsets; ///< Массив смещений начала адресного пространства модуля IO для канала
			int* logical_ports; ///< Массив логических номеров канала в пределах модуля IO

            int_2  **int_read_values;           ///< Массив значений для чтения.
			int_2  **int_module_read_values;    ///< Массив значений для чтения адресного пространства модуля.
            int_2  **int_write_values;          ///< Массив значений для записи.
            u_char **char_read_values;          ///< Массив значений для чтения.
            u_char **char_write_values;         ///< Массив значений для записи.

            CHANNEL_TYPE type;           ///< Тип канала.

            IO_channels( CHANNEL_TYPE type );

            ~IO_channels();

            void init( int ch_count );

            void init_channel( u_int ch_index, int node, int offset, int module_offset, int logical_port );

            void print() const;
            };

        IO_channels DI_channels;    ///< Каналы дискретного входа.
        IO_channels DO_channels;    ///< Каналы дискретного выхода.
        IO_channels AI_channels;    ///< Каналы аналогового входа.
        IO_channels AO_channels;    ///< Каналы аналогового выхода.

        VENDOR vendor;

    public:

        VENDOR get_io_vendor() const;

        // Lua.
        void init( int DO_count, int DI_count, int AO_count, int AI_count );

        void init_channel( int type, int ch_inex, int node, int offset,
            int module_offset = -1, int logical_port = -1 );

        void set_io_vendor( VENDOR vendor );

        static int last_err;

#ifdef PTUSA_TEST
        void init_and_alloc( int DO_count = 1, int DI_count = 0,
            int AO_count = 0, int AI_count = 0 );
#endif
    };
//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода.
///
/// Реализация чтения и записи состояний модулей ввода/вывода.
class io_manager
    {
    public:
        virtual ~io_manager();

        void print() const;

        /// @brief Чтение модулей ввода.
        ///
        /// @return - 0 - Ок.
        virtual int read_inputs() = 0;

        /// @brief Чтение модулей вывода.
        ///
        /// @return - 0 - Ок.
        virtual int write_outputs() = 0;

        /// @brief Получение единственного экземпляра класса.
        static io_manager* get_instance();

#ifdef PTUSA_TEST
        /// @brief Получение единственного экземпляра класса.
        static io_manager* replace_instance( io_manager* );
#endif

        /// @brief Получение области данных заданного канала дискретного входа.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        u_char* get_DI_read_data( u_int node_n, u_int offset );

        /// @brief Получение области чтения данных заданного канала
        /// дискретного выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        u_char* get_DO_read_data( u_int node_n, u_int offset );

        /// @brief Получение области записи данных заданного канала
        /// дискретного выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        u_char* get_DO_write_data( u_int node_n, u_int offset );

        /// @brief Получение области данных заданного канала аналогового входа.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int_2* get_AI_read_data( u_int node_n, u_int offset );

        /// @brief Получение области чтения данных заданного канала
        /// аналогового выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int_2* get_AO_read_data( u_int node_n, u_int offset );

        /// @brief Получение области записи данных заданного канала
        /// аналогового выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int_2* get_AO_write_data( u_int node_n, u_int offset );

		//---------------------------------------------------------------------
		/// @brief Узел модулей ввода/вывода.
		//
		///
		struct io_node
			{
            io_node( int type, int number,
                const char* str_ip_addres, const char* name,
                int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
                int AI_cnt, int AI_size );

			~io_node();

			void print();

            enum W_CONST
                {
                //Max 63 устройства PXC Axioline F.
                C_ANALOG_BUF_SIZE = 63 * 32,///< Размер буфера аналоговых модулей.

                C_MAX_DELAY = 60'000,       ///< Макс. время задержки переподключения, мсек.
                C_CNT_TIMEOUT_US = 100'000, ///< Время ожидания подключения от модуля, мксек.

                C_RCV_TIMEOUT_SEC = 0,      ///< Время ожидания ответа от модуля, сек.
                C_RCV_TIMEOUT_US = 250'000, ///< Время ожидания ответа от модуля, мксек.

                ///< Изначальная задержка переподключения к узлу при ошибке связи, мсек.
                C_INITIAL_RECONNECT_DELAY = 500,
                };

			enum TYPES ///< Типы модулей.
			    {
				EMPTY = -1,   ///< Не задан.

				WAGO_750_86x		  = 0,  ///< Wago 750-863.
				WAGO_750_820x		  = 2,  ///< Wago 750_820x.

				WAGO_750_XXX_ETHERNET = 100,///< Wago Ethernet 750-341 и т.д.
				PHOENIX_BK_ETH		  = 200,///< Phoenix 2702177
				};

			enum STATES           ///< Состояния работы с узлом.
				{
				ST_NO_CONNECT = 0,
				ST_OK = 1,
				};

            enum class DISPLAY_STATES  ///< Отображение работы с узлом.
                {
                DST_ERROR = -1,  ///< Node enabled but no connection.
                DST_NO_CONNECT = 0,
                DST_OK = 1,

                ///< Node has error/warning (Status Register bits 0-5 set).
                DST_WARNING = 2,
                };

			/// Bits 0-5 of Status Register (7996) indicate error/PP
			/// mode conditions. When any of these bits are set, the
			/// node is in error/PP mode state.
			static constexpr u_int_2 STATUS_REG_ERROR_MASK = 0x003F;

            /// Bit 4 of Status Register (7996) indicates PP mode.
            static constexpr u_int_2 STATUS_REG_PP_MODE_MASK = 0x0010;

            /// Bits 2 and 8 of Diagnostic Status Register (7997)
            /// indicate configuration/bus error conditions.
            static constexpr u_int_2 DIAG_STATUS_REG_CFG_BUS_ERROR_MASK =
                0x0104;

            ///< Состояние работы с узлом.
            io_node::STATES state{ io_node::STATES::ST_NO_CONNECT };

			TYPES   type;               ///< Тип.
			u_int   number;             ///< Номер.
            char    ip_address[ 16 ]{}; ///< IP-адрес.
            char    name[ 20 ]{};       ///< Имя.

            bool is_active{ true };          ///< Признак работающего узла.
            bool read_io_error_flag{ false };///< Флаг ошибки чтения узла.

            uint32_t last_poll_time{ get_millisec() }; ///< Время последнего опроса.
            bool is_set_err{};       ///< Установлена ли ошибка связи.
            int sock{};              ///< Сокет соединения.

			// Digital outputs ( DO ).
			u_int  DO_cnt;      ///< Amount of DO.
            u_char* DO{};       ///< Current values.
            u_char* DO_{};      ///< To write.

            // Analog outputs ( AO ).
            u_int AO_cnt;       			///< Amount of AO.
            int_2 AO[ C_ANALOG_BUF_SIZE ] = {};    ///< Current values.
            int_2 AO_[ C_ANALOG_BUF_SIZE ] = {};   ///< To write.
            u_int* AO_offsets{};  			///< Offsets in common data.
            u_int* AO_types{};    			///< Channels type.
            u_int AO_size;

            // Digital inputs ( DI ).
            u_int  DI_cnt;      ///< Amount of DI.
            u_char* DI{};       ///< Current values.

            // Analog inputs ( AI ).
            u_int AI_cnt;       			///< Amount of AI.
            int_2 AI[ C_ANALOG_BUF_SIZE ] = {};    ///< Current values.
            u_int* AI_offsets{};  			///< Offsets in common data.
            u_int* AI_types{};   			///< Channels type.
            u_int AI_size;

            ///< Время последней попытки подключиться, мсек.
            uint32_t last_init_time{};

            ///< Время ожидания до попытки подключиться, мсек.
            uint32_t delay_time{ W_CONST::C_INITIAL_RECONNECT_DELAY };

			stat_time recv_stat;  ///< Статистика работы с сокетом.
			stat_time send_stat;  ///< Статистика работы с сокетом.

            bool flag_error_read_message{ false }; ///< Флаг для вывода сообщений об ошибке чтения.
            bool flag_error_write_message{ false }; ///< Флаг для вывода сообщений об ошибке записи.

            /// Status register (7996) for Phoenix BK ETH nodes.
            /// Used to detect error/warning conditions (bits 0-5),
            /// including PP mode, startup failures, net failures,
            /// etc. Remains 0 for other node types.
            u_int_2 status_register{};

            /// Previous status register value for detecting changes.
            u_int_2 prev_status_register{};

            /// Diagnostic status register (7997) for Phoenix BK ETH nodes.
            /// Bits 2 and 8 indicate configuration/bus errors.
            u_int_2 diagnostic_status_register{};

            /// Previous diagnostic register value for detecting changes.
            u_int_2 prev_diagnostic_status_register{};

            /// Flag indicating PP mode alarm is currently active.
            bool is_err_mode_alarm_set = false;

            /// Flag indicating configuration/bus error alarm is currently
            /// active for diagnostic status register errors (bits 2 and 8
            /// of register 7997).
            bool is_cfg_bus_error_alarm_set = false;

            /// @brief Checks PP mode state of the node.
            /// @return true if PP mode is active (bit 4 set), else false.
            bool is_pp_mode_active() const;

            /// @return One of `io_node::DISPLAY_STATES`:
            ///         `DST_OK` for a connected and healthy node.
            ///         `DST_NO_CONNECT` for an inactive node, emulator,
            ///         or node without connection.
            ///         `DST_ERROR` when `state != ST_OK`.
            ///         `DST_WARNING` when warning/error conditions are
            ///         present in Phoenix status/diagnostic registers.
            io_node::DISPLAY_STATES get_display_state() const;

            private:
                io_node( const io_node& io_node_copy ); // Not implemented.
            };
		//---------------------------------------------------------------------

    protected:
        io_manager();
        u_int     nodes_count;        ///< Количество узлов.
        io_node **nodes;              ///< Узлы.

        /// Единственный экземпляр класса.
        static auto_smart_ptr < io_manager > instance;

    public:
        const io_node* get_node( u_int node_n ) const;

        io_manager::io_node* get_node( u_int node_n );

		u_int get_nodes_count();

        /// @brief Установка числа модулей.
        ///
        /// Вызывается из Lua.
        void init( int new_nodes_count );

        /// @brief Инициализация модуля.
        ///
        /// Вызывается из Lua.
        io_manager::io_node* add_node( u_int index, int ntype, int address,
            const char* IP_address, const char *name,
            int DO_cnt, int DI_cnt,
            int AO_cnt, int AO_size, int AI_cnt, int AI_size );

        /// @brief Инициализация параметров канала аналогового вывода.
        ///
        /// Вызывается из Lua.
        void init_node_AO( u_int node_index, u_int AO_index,
            u_int type, u_int offset );

        /// @brief Инициализация параметров канала аналогового ввода.
        ///
        /// Вызывается из Lua.
        void init_node_AI( u_int node_index, u_int AI_index,
            u_int type, u_int offset );

		/// @brief Завершает соединение с узлом
		virtual void disconnect(io_node *node) = 0;

        inline static io_node io_node_stub{ io_manager::io_node::PHOENIX_BK_ETH,
            1, "127.0.0.1", "Axxx", 0, 0, 0, 0, 0, 0 };
        inline static const io_node IO_NODE_STUB{ io_manager::io_node::PHOENIX_BK_ETH,
            1, "127.0.0.1", "Axxx", 0, 0, 0, 0, 0, 0 };
#ifdef PTUSA_TEST
        void clear_nodes();
#endif

        // Explicitly delete the copy constructors.
        io_manager( io_manager const& ) = delete;
        io_manager( io_manager&& ) = delete;
        io_manager& operator=( io_manager const& ) = delete;
        io_manager& operator=( io_manager&& ) = delete;
    };
//-----------------------------------------------------------------------------
io_manager* G_IO_MANAGER();
//-----------------------------------------------------------------------------
#endif // IO_H
