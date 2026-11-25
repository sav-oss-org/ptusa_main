#pragma once

#include "base.h"

#include <unordered_set>

//-----------------------------------------------------------------------------
class valve_DO2_DI2_bistable;
class converter_iolink_ao;

/// @brief Устройство с обратными связями.
///
class valve : public digital_io_device
    {
    public:
        /// @brief Конструктор для клапана c обратными связями.
        ///
        /// @param is_on_fb - использовать обратную связь на включенное состояние.
        /// @param is_off_fb - использовать обратную связь на выключенное состояние.
        /// @param dev_name - имя клапана.
        /// @param type - тип устройства.
        /// @param sub_type - подтип устройства.
        valve( bool is_on_fb, bool is_off_fb,
            const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type );

        /// @brief Конструктор для клапана без обратных связей.
        ///
        /// @param dev_name - имя клапана.
        /// @param type - тип устройства.
        /// @param sub_type - подтип устройства.
        valve( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type );

#ifdef _MSC_VER
#pragma region Отключение клапана с задержкой.
#endif
        void off();

        void on();

        /// @brief Выключение клапанов с задержкой.
        static void evaluate();
#ifdef _MSC_VER
#pragma endregion Отключение клапана с задержкой.
#endif

        /// @brief Получение значения включенного состояния.
        bool is_opened() const;

        /// @brief Получение значения выключенного состояния.
        bool is_closed() const;

        void set_fb_state( bool new_on_fb, bool new_off_fb );

        /// @brief Получение значения обратной связи на включенное состояние.
        virtual bool get_on_fb_value() const;

        /// @brief Получение значения обратной связи на выключенное состояние.
        virtual bool get_off_fb_value() const;

        /// @brief Сохранение дополнительных данных.
        int save_device_ex( char* buff ) const override;

        //Состояния клапана (расширенное).
        enum VALVE_STATE_EX
            {
            VX_ON_FB_OFF = 11,  ///< Включен, ОС отключена.
            VX_OFF_FB_OFF = 10,  ///< Выключен, ОС отключена.

            VX_UPPER_SEAT_FB_OFF = 32, ///< Открыто верхнее седло, ОС отключена.
            VX_LOWER_SEAT_FB_OFF = 33, ///< Открыто нижнее седло, ОС отключена.

            VX_UPPER_SEAT = 2,   ///< Открыто верхнее седло.
            VX_LOWER_SEAT = 3,   ///< Открыто нижнее седло.

            VX_ON_FB_OK = 1,    ///< Включен, ОС ок.
            VX_OFF_FB_OK = 0,    ///< Выключен, ОС ок.

            VX_ON_FB_ERR = -2,  ///< Включен, ошибка ОС.
            VX_OFF_FB_ERR = -3,  ///< Выключен, ошибка ОС.

            //Учет ручного режима.
            VX_ON_FB_OFF_MANUAL = 31,   ///< Включен вручную, ОС отключена.
            VX_OFF_FB_OFF_MANUAL = 30,   ///< Выключен вручную, ОС отключена.

            VX_LOWER_SEAT_MANUAL = 23,   ///< Открыто вручную нижнее седло.
            VX_UPPER_SEAT_MANUAL = 22,   ///< Открыто вручную верхнее седло.

            VX_ON_FB_OK_MANUAL = 21,    ///< Включен вручную, ОС ок.
            VX_OFF_FB_OK_MANUAL = 20,    ///< Выключен вручную, ОС ок.

            VX_ON_FB_ERR_MANUAL = -12,  ///< Включен вручную, ошибка ОС.
            VX_OFF_FB_ERR_MANUAL = -13,  ///< Выключен вручную, ошибка ОС.

            VX_UNKNOWN = 100,            ///< Неизвестное состояние.
            };

        /// @brief Получение расширенного состояния клапана (учет обратной
        /// связи, ручного режима, ...).
        int get_state() const override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        ///Состояние клапана без учета обратной связи.
        enum VALVE_STATE
            {
            V_LOWER_SEAT = 3, ///< Открыто нижнее седло.
            V_UPPER_SEAT = 2, ///< Открыто верхнее седло.

            V_ON = 1,         ///< Включен.
            V_OFF = 0,        ///< Выключен.

            V_STOP = 5,       ///< Остановлен.
            };

        bool is_wash_seat_active() const;

        void set_seat_wash_state( bool wash_flag );

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
        /// @brief Получение состояния клапана без учета обратной связи.
        virtual VALVE_STATE get_valve_state() const;

        /// @brief Получение состояния обратной связи.
        virtual bool get_fb_state() const;

    protected:
        enum FB_STATE
            {
            FB_IS_AND_OFF = 0, ///< Обратная связь отключена.
            FB_IS_AND_ON,      ///< Обратная связь включена.
            };

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 2,///Количество дополнительных параметров.

            P_ON_TIME = 1,
            P_FB,
            };

    private:
        /// @brief Есть обратная связь на включенное состояние.
        bool is_on_fb = false;

        /// @brief Есть обратная связь на выключенное состояние.
        bool is_off_fb = false;

        bool on_fb = false;
        bool off_fb = false;

#ifdef _MSC_VER
#pragma region Отключение клапана с задержкой.
#endif
        /// @brief Вектор клапанов, ожидающих отключение.
        static std::vector< valve* > to_switch_off;

    protected:
        /// @brief Вектор бистабильных клапанов.
        static std::vector< valve_DO2_DI2_bistable* > v_bistable;

    private:
        /// @brief Определение завершения отключения клапана с задержкой.
        virtual bool is_switching_off_finished();

        bool is_switching_off = false; ///Выключается ли клапан с задержкой.
        uint32_t start_off_time = 0; ///Время начала выключения клапана с задержкой.
        bool was_on_auto = false;      ///Признак включения клапана управляющей программой.

    public:
        static void clear_switching_off_queue();
        static void clear_v_bistable();

#ifdef _MSC_VER
#pragma endregion Отключение клапана с задержкой.
#endif

    protected:
        mutable uint32_t start_switch_time = get_millisec();

    private:
        bool wash_flag = false;
    };
//-----------------------------------------------------------------------------
class virtual_valve : public valve
    {
    public:
        explicit virtual_valve( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления.
class valve_DO1 : public valve
    {
    public:
        explicit valve_DO1( const char* dev_name );

        void direct_on();

        void direct_off();

        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            };

        /// @brief Получение состояния клапана без учета обратной связи.
        VALVE_STATE get_valve_state() const override;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления.
class valve_DO2 : public valve
    {
    public:
        explicit valve_DO2( const char* dev_name );

        /// @brief Получение состояния клапана без учета обратной связи.
        VALVE_STATE get_valve_state() const override;

        int  get_state() const override;
        void direct_on() override;
        void direct_off() override;

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0, ///< Индекс канала дискретного выхода №1.
            DO_INDEX_2,     ///< Индекс канала дискретного выхода №2.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним дискретным выходом и одним дискретным входом.
///
class valve_DO1_DI1_off : public valve
    {
    public:
        explicit valve_DO1_DI1_off( const char* dev_name );

        void direct_on() override;

        void direct_off() override;

#ifndef PTUSA_TEST
    private:
#endif
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.
            DI_INDEX = 0,           ///< Индекс канала дискретного входа.
            };

        // Интерфейс valve для реализации получения расширенного состояния с
        // учетом всех вариантов (ручной режим, обратная связь, ...).
        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const final;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним дискретным выходом и одним дискретным входом.
///
class valve_DO1_DI1_on : public valve
    {
    public:
        explicit valve_DO1_DI1_on( const char* dev_name );

        void direct_on() override;

        void direct_off() override;

#ifndef PTUSA_TEST
    private:
#endif
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.
            DI_INDEX = 0,           ///< Индекс канала дискретного входа.
            };

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        bool get_on_fb_value() const override;

        bool get_off_fb_value() const final;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и двумя обратными связями.
///
class valve_DO1_DI2 : public valve
    {
    public:
        explicit valve_DO1_DI2( const char* dev_name );

        void direct_on() override;

        void direct_off() override;

#ifndef PTUSA_TEST
    private:
#endif

        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления и двумя обратными связями.
///
class valve_DO2_DI2 : public valve
    {
    public:
        explicit valve_DO2_DI2( const char* dev_name );

        void direct_on() override;

        void direct_off() override;

#ifndef PTUSA_TEST
    private:
#endif
        enum CONSTANTS
            {
            DO_INDEX_1 = 0,         ///< Индекс №1 канала дискретного выхода.
            DO_INDEX_2,             ///< Индекс №2 канала дискретного выхода.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления и двумя обратными связями
/// бистабильный.
///
class valve_DO2_DI2_bistable : public valve
    {
    public:
        explicit valve_DO2_DI2_bistable( const char* dev_name );

        int evaluate();

        void direct_on() override;

        void direct_off() override;

        enum CONSTANTS
            {
            DO_INDEX_OPEN = 0,  ///< Индекс канала дискретного выхода Открыть.
            DO_INDEX_CLOSE,     ///< Индекс канала дискретного выхода Закрыть.

            DI_INDEX_OPEN = 0,  ///< Индекс канала дискретного входа Открыт.
            DI_INDEX_CLOSE,     ///< Индекс канала дискретного входа Закрыт.
            };

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;

        void direct_set_state( int new_state ) override;

    private:
        bool is_stoped;
        bool is_opening;
        bool is_closing;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан mixproof.
class valve_mix_proof : public i_mix_proof, public valve
    {
    public:
        explicit valve_mix_proof( const char* dev_name );

        /// @brief Открыть верхнее седло.
        void open_upper_seat() override;

        /// @brief Открыть нижнее седло.
        void open_lower_seat() override;

        void direct_set_state( int new_state ) override;

        void direct_on() override;

        void direct_off() override;

#ifndef PTUSA_TEST
    private:
#endif

        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            DO_INDEX_U,     ///< Индекс канала дискретного выхода верхнего седла.
            DO_INDEX_L,     ///< Индекс канала дискретного выхода нижнего седла.

            DI_INDEX_OPEN = 0, ///< Индекс канала дискретного входа Открыт.
            DI_INDEX_CLOSE,    ///< Индекс канала дискретного входа Закрыт.
            };

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Клапан AS-interface.
class valve_AS : public valve
    {
    public:
        valve_AS( const char* dev_name, device::DEVICE_SUB_TYPE sub_type );

        void print() const override;

        void set_rt_par( u_int idx, float value );

        /// @brief Получение данных состояния устройства.
        char get_state_data( const char* data ) const;

        char* get_data_with_offset( char* data );

        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        /// @brief поменять местами подключение седел клапана.
        bool reverse_seat_connection = false;

        int get_lower_seat_offset() const;

        int get_upper_seat_offset() const;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;

        void direct_off() override;

        void direct_on() override;

        void direct_set_state( int new_state ) override;

        enum CONSTANTS
            {
            AI_INDEX = 0,   ///< Индекс канала аналогового входа.
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.

            C_OPEN_S1 = 0x2,    ///< Открыть соленоид 1.
            C_OPEN_S2 = 0x4,    ///< Открыть соленоид 2.
            C_OPEN_S3 = 0x8,    ///< Открыть соленоид 3.

            S_CLOSED = 0x1,     ///< Клапан закрыт.
            S_OPENED = 0x2,     ///< Клапан открыт.

            MAILBOX_OFFSET = 8
            };

        static std::unordered_set<std::string> V70_ARTICLES;

    private:
        u_int AS_number;    ///< AS-номер устройства.
    };
//-----------------------------------------------------------------------------
/// @brief Клапан AS-mixproof.
class valve_AS_mix_proof : public i_mix_proof, public valve_AS
    {
    public:
        explicit valve_AS_mix_proof( const char* dev_name );

        void open_upper_seat() override;

        void open_lower_seat() override;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан AS-i отсечной.
class valve_AS_DO1_DI2 : public valve_AS
    {
    public:
        explicit valve_AS_DO1_DI2( const char* dev_name );

        void direct_set_state( int new_state );

        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

    private:
        mutable uint32_t start_err_time;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан донный.
class valve_bottom_mix_proof : public i_mix_proof, public valve
    {
    public:
        explicit valve_bottom_mix_proof( const char* dev_name );

        /// @brief Открыть верхнее седло. Не делаем ничего, так как верхнего
        /// седла нет.
        void open_upper_seat() override;

        /// @brief Открыть нижнее седло.
        void open_lower_seat() override;

#ifdef PTUSA_TEST
        void set_mini_closing_state( bool state );
#endif

#ifdef _MSC_VER
#pragma region Выключение мини клапана с задержкой.
#endif
    private:
        uint32_t start_off_time;        // Время начала открытия клапана.

        int is_closing_mini;    // Мини клапан в режиме закрытия.

    public:
        /// @brief Определение завершения отключения клапана с задержкой.
        bool is_switching_off_finished() override;

#ifdef _MSC_VER
#pragma endregion Выключение мини клапана с задержкой.
#endif

#ifndef PTUSA_TEST
    private:
#endif
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            DO_INDEX_MINI_V,///< Индекс канала дискретного выхода мини клапана.
            DO_INDEX_L,     ///< Индекс канала дискретного выхода нижнего седла.

            DI_INDEX_OPEN = 0, ///< Индекс канала дискретного входа Открыт.
            DI_INDEX_CLOSE,     ///< Индекс канала дискретного входа Закрыт.
            };

        void direct_set_state( int new_state ) override;

        void direct_on();
        void direct_off();

        //Интерфейс для реализации получения расширенного состояния с учетом
        //всех вариантов (ручной режим, обратная связь, ...).

#ifndef PTUSA_TEST
    protected:
#endif

        VALVE_STATE get_valve_state() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан донный.
class valve_mini_flushing : public i_mix_proof, public valve
    {
    public:
        explicit valve_mini_flushing( const char* dev_name );

        /// @brief Открыть верхнее седло.
        void open_upper_seat() final;

        /// @brief Открыть нижнее седло.
        void open_lower_seat() final;

#ifndef PTUSA_TEST
    private:
#endif
        enum CONSTANTS_DO
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            DO_INDEX_MINI_V ///< Индекс канала дискретного выхода мини клапана.
            };

        enum CONSTANTS_DI
            {
            DI_INDEX_OPEN = 0,  ///< Индекс канала дискретного входа Открыт.
            DI_INDEX_CLOSE      ///< Индекс канала дискретного входа Закрыт.
            };

        void direct_set_state( int new_state ) final;

        void direct_on() final;
        void direct_off() final;

#ifndef PTUSA_TEST
    protected:
#endif
        //Интерфейс для реализации получения расширенного состояния с учетом
        //всех вариантов (ручной режим, обратная связь, ...).
        VALVE_STATE get_valve_state() const final;

        bool get_fb_state() const final;

        bool get_off_fb_value() const final;
        bool get_on_fb_value() const final;
    };
//-----------------------------------------------------------------------------
class io_link_valve
    {
    public:
        /// @brief Returns a description of the error that is active.
        ///
        /// @return A C-string containing the error description.
        const char* get_error_description( int err_id ) const;

        inline static const int ERROR_ID_FIRST = -116;
        inline static const int ERROR_ID_LAST = -131;

        inline static const int ERROR_CODE_OFFSET = 100;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан AlfaLaval IO-Link mixproof.
class alfalaval_iol_valve
    {
    public:
        /// @brief Returns a description of the error that is active.
        ///
        /// @return A C-string containing the error description or
        /// nullptr if no error.
        const char* get_error_description( int error_id ) const;

    private:
        io_link_device iol_dev;
        io_link_valve iol_valve;
    };
//-----------------------------------------------------------------------------
struct alfalaval_iol_valve_in_data
    {
    int16_t  pos;
    bool     de_en  : 1;    // De-energized.
    bool     main   : 1;    // Main energized position.
    bool     usl    : 1;    // Upper Seat Lift energized position.
    bool     lsp    : 1;    // Lower Seat Push energized position.
    bool     st     : 1;    // Current Valve state.
    uint8_t  unused : 3;
    uint8_t  err    : 5;
    };
static_assert( sizeof( alfalaval_iol_valve_in_data ) == 4,
    "Struct alfalaval_iol_valve_in_data must be 4 bytes size." );

// Swapping the low and high bytes to simplify processing.
#pragma pack(push,1)
struct alfalaval_iol_valve_out_data_swapped
    {
    uint8_t unused1 : 4;
    bool    sv1     : 1;   // Main valve activation.
    bool    sv2     : 1;   // Upper seat lift activation.
    bool    sv3     : 1;   // Lower Seat Push energized position.
    bool    wink    : 1;   // Visual indication.
    };
#pragma pack(pop)

static_assert( sizeof( alfalaval_iol_valve_out_data_swapped ) == 1,
    "Struct alfalaval_iol_valve_out_data_swapped must be 1 byte size." );
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link mixproof.
class valve_iolink_mix_proof : public i_mix_proof, public valve
    {
    public:
        explicit valve_iolink_mix_proof( const char* dev_name );

        void open_upper_seat();

        void open_lower_seat();

        VALVE_STATE get_valve_state() const override;

        int save_device_ex( char* buff ) const override;

        void evaluate_io() override;

        void set_rt_par( u_int idx, float value );

        int get_state() const override;

        float get_value() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;

        void direct_on() override;

        void direct_off() override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        void direct_set_state( int new_state ) override;

        const char* get_error_description() override;

#ifndef PTUSA_TEST
    private:
#endif
        alfalaval_iol_valve_in_data in_info{};
        static alfalaval_iol_valve_out_data_swapped stub_out_info;
        alfalaval_iol_valve_out_data_swapped* out_info = &stub_out_info;

        bool blink = false;     //Visual indication

        //Дополнительное смещение, так как область AO занимает 1 байт.
        int extra_offset = 0;

        enum class CONSTANTS
            {
            C_AI_INDEX = 0,             ///< Индекс канала аналогового входа.
            };

        alfalaval_iol_valve alfalaval_iol_v;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link отсечной ALfaLaval.
class valve_iolink_shut_off_thinktop : public valve
    {
    public:
        explicit valve_iolink_shut_off_thinktop( const char* dev_name );

        ~valve_iolink_shut_off_thinktop() override = default;

        VALVE_STATE get_valve_state() const override;

        int save_device_ex( char* buff ) const override;

        void evaluate_io() override;

        void set_rt_par( u_int idx, float value );

        float get_value() const override;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;

        void direct_on() override;

        void direct_off() override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        void direct_set_state( int new_state ) override;

        const char* get_error_description() override;

        int get_state() const override;

#ifndef PTUSA_TEST
    private:
#endif
        alfalaval_iol_valve_in_data in_info{};
        static alfalaval_iol_valve_out_data_swapped stub_out_info;
        alfalaval_iol_valve_out_data_swapped* out_info = &stub_out_info;

        bool blink = false;     //Visual indication

        //Дополнительное смещение, так как область AO занимает 1 байт.
        int extra_offset = 0;

        enum class CONSTANTS
            {
            C_AI_INDEX = 0,             ///< Индекс канала аналогового входа.
            };

        alfalaval_iol_valve alfalaval_iol_v;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link отсечной Definox.
class valve_iolink_shut_off_sorio : public valve
    {
    public:
        static const std::string SORIO_ARTICLE;

        explicit valve_iolink_shut_off_sorio( const char* dev_name );

        VALVE_STATE get_valve_state() const override;

        int save_device_ex( char* buff ) const override;

        void evaluate_io() override;

        float get_value() const final;

        void direct_set_value( float new_value ) final;

        bool get_fb_state() const override;

        bool get_off_fb_value() const override;

        bool get_on_fb_value() const override;

        void direct_on() override;

        void direct_off() override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        void direct_set_state( int new_state ) override;

    private:
        struct in_data
            {
            int16_t  pos;
            bool de_en : 1; //De-Energized
            bool main : 1; //Main energized position
            uint16_t unused1 : 2;
            uint16_t status : 4;
            bool sv1 : 1; //Current state of solenoid 1
            uint16_t unused2 : 2;
            uint16_t led_state : 5;
            };

        struct out_data_swapped   //Swapped low and high byte for easer processing
            {
            bool sv1 : 1; //Solenoid valve 1 activation
            uint16_t unused1 : 2;
            bool wink : 1; //Visual indication
            bool ext1 : 1; //External RGB LED #1
            bool ext2 : 1; //External RGB LED #2
            bool ext3 : 1; //External RGB LED #3
            uint16_t unused2 : 1;
            };

        in_data in_info{ 0 };
        static out_data_swapped stub_out_info;
        out_data_swapped* out_info = &stub_out_info;

        bool blink = false;     //Visual indication
    };
//-----------------------------------------------------------------------------
/// @brief Клапан GEA T.VIS A-15.
class valve_iolink_gea_tvis_a15 : public valve
    {
    public:
        explicit valve_iolink_gea_tvis_a15( const char* dev_name,
            device::DEVICE_SUB_TYPE device_sub_type );

        int save_device_ex( char* buff ) const final;
        void evaluate_io() final;
        float get_value() const final;

        void direct_set_value( float new_value ) final;

        void direct_off() final;
        void direct_on() final;
        bool get_fb_state() const final;
        bool get_off_fb_value() const final;
        bool get_on_fb_value() const final;

        void set_rt_par( u_int idx, float value ) override;

    protected:
        struct in_data
            {
            bool s1 : 1;    // если односедельный - состояние клапана: 1 - закрыт, 0 - вне допуска
                            // если двухседельный - клапаная тарелка: 1 - закрыт, 0 - вне допуска
            bool s2 : 1;    // основной ход: 1 - клапан в конечном положении, 0 - клапан вышел за пределы допуска
            bool s3 : 1;    // если pv_y1 = 0 - сдвоенная тарелка: 1 - закрыта, 0 - не закрыта или нет внешнего датчика
                            // если pv_y1 = 1 - сдвоенная тарелка: 1 - не разведена, 0 - разведена
            bool s4 : 1;    // если двухседельный - основной ход: 1 - клапан в покое, 0 - клапанная тарелка и сдвоенная тарелка не закрыты
            bool SUP : 1;   // активна процедура Setup
            bool Y7 : 1;    // незанятый бит
            bool Y6 : 1;    // незанятый бит
            bool Y5 : 1;    // незанятый бит
            bool pv_y1_on : 1; // пилотный клапан соленоида y1 активирован
            bool pv_y2_on : 1; // пилотный клапан соленоида y2 активирован
                               // Управление нижним седлом.
            bool pv_y3_on : 1; // если pv_y1_on = 0: пилотный клапан соленоида y3 активирован
                               // если pv_y1_on = 1: пилотный клапан соленоида y4 активирован
                               // Управление верхним седлом.
            bool error_on : 1; // ошибка активна

            uint16_t  pos : 10;     // позиция штока в 10^-4 метра
            uint16_t unused1 : 10;  // неиспользуемое битовое пространство
            };

        struct out_data_swapped
            {
            bool pv_y1 : 1; // активация пилотного соленоида y1
            bool pv_y2 : 1; // активация пилотного соленоида y2
            bool pv_y3 : 1; // в зависимости от pv_y1: активация пилотного клапана соленоида y3 или y4
            bool X4 : 1;    // незанятый бит
            bool X5 : 1;    // незанятый бит
            bool X6 : 1;    // незанятый бит
            bool X7 : 1;    // незанятый бит
            bool HAS : 1;   // активация процедуры Setup
            };

        in_data in_info{ false };
        static out_data_swapped stub_out_info;
        out_data_swapped* out_info = &stub_out_info;

        enum class CONSTANTS
            {
            C_AI_INDEX = 0,             ///< Индекс канала аналогового входа.
            };

    private:
        int extra_offset = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link GEA T.VIS A-15 односедельный отсечной.
class valve_iolink_gea_tvis_a15_ss : public valve_iolink_gea_tvis_a15
    {
    public:
        static const std::string GEA_TVIS_A15_SINGLE_SEAT_ARTICLE;
        explicit valve_iolink_gea_tvis_a15_ss( const char* dev_name );

        VALVE_STATE get_valve_state() const final;

        void direct_set_state( int new_state ) final;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link GEA T.VIS A-15 двухседельный mixproof.
class valve_iolink_gea_tvis_a15_ds : public valve_iolink_gea_tvis_a15, public i_mix_proof
    {
    public:
        static const std::string GEA_TVIS_A15_DOUBLE_SEAT_ARTICLE;
        explicit valve_iolink_gea_tvis_a15_ds( const char* dev_name );

        void open_upper_seat() final;
        void open_lower_seat() final;
        VALVE_STATE get_valve_state() const final;

        void direct_set_state( int new_state ) final;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с управлением от пневмоострова IO-link.
class valve_iol_terminal : public valve
    {
    public:
        explicit valve_iol_terminal( bool is_on_fb, bool is_off_fb,
            const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
            u_int terminal_size = 1 );

        ///< Конструктор для клапана без обратных связей.
        explicit valve_iol_terminal( const char* dev_name,
            device::DEVICE_SUB_TYPE sub_type, u_int terminal_size = 1 );

        void set_rt_par( u_int idx, float value ) override;

        bool check_config() const;

        /// @brief Установка данных состояния устройства.
        void set_state_bit( char* data, unsigned int n ) const;

        void reset_state_bit( char* data, unsigned int n ) const;

        enum class TERMINAL_OUTPUT : unsigned int
            {
            ON = 1,         ///< Включение.
            UPPER_SEAT,     ///< Включение верхнего седла.
            LOWER_SEAT      ///< Включение нижнего седла.
            };

        enum class IO_CONSTANT : unsigned int
            {
            AO_INDEX_1 = 0,///< Индекс канала аналогового выхода.
            AO_INDEX_2 = 1,
            AO_INDEX_3 = 2,

            DI_INDEX_1 = 0,///< Индекс канала дискретного входа обратной связи.
            DI_INDEX_2 = 1,
            };

        unsigned int get_terminal_id( valve_iol_terminal::TERMINAL_OUTPUT n ) const;

        int get_state() const override;
        void direct_set_state( int new_state ) override;

        VALVE_STATE get_valve_state() const override;

        void direct_on() override;
        void direct_off() override;

    protected:
        void set_st( VALVE_STATE new_state );

    private:
        std::vector<unsigned int> terminal_id;

        VALVE_STATE state = VALVE_STATE::V_OFF;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления.
class valve_iol_terminal_DO1 : public valve_iol_terminal
    {
    public:
        explicit valve_iol_terminal_DO1( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления и обратной связью.
class valve_iol_terminal_DO1_DI1_on : public valve_iol_terminal
    {
    public:
        explicit valve_iol_terminal_DO1_DI1_on( const char* dev_name );

        /// @brief Получение состояния обратной связи.
        bool get_fb_state() const override;

        bool get_on_fb_value() const override;

        bool get_off_fb_value() const override;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления и обратной связью.
class valve_iol_terminal_DO1_DI1_off : public valve_iol_terminal
    {
    public:
        explicit valve_iol_terminal_DO1_DI1_off( const char* dev_name );

        /// @brief Получение состояния обратной связи.
        bool get_fb_state() const override;

        bool get_on_fb_value() const override;

        bool get_off_fb_value() const override;
    };
//-----------------------------------------------------------------------------
/// @brief IO-Link клапан (от пневмооострова) с тремя каналом управления.
class valve_iol_terminal_mixproof_DO3 : public i_mix_proof, public valve_iol_terminal
    {
    public:
        explicit valve_iol_terminal_mixproof_DO3( const char* dev_name,
            bool is_on_fb, bool is_off_fb, device::DEVICE_SUB_TYPE sub_type );

        explicit valve_iol_terminal_mixproof_DO3( const char* dev_name,
            device::DEVICE_SUB_TYPE sub_type = V_IOL_TERMINAL_MIXPROOF_DO3 );

        void open_upper_seat() override;

        void open_lower_seat() override;

        void direct_set_state( int new_state ) override;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления и 2-я обратными связями.
class valve_iol_terminal_DO1_DI2 : public valve_iol_terminal
    {
    public:
        explicit valve_iol_terminal_DO1_DI2( const char* dev_name );

        /// @brief Получение состояния обратной связи.
        bool get_fb_state() const override;

        bool get_on_fb_value() const override;

        bool get_off_fb_value() const override;
    };
//-----------------------------------------------------------------------------
/// @brief IO-Link клапан (от пневмооострова) с тремя каналом управления и
/// 2-я обратными связями.
class valve_iol_terminal_mixproof_DO3_DI2 : public valve_iol_terminal_mixproof_DO3
    {
    public:
        explicit valve_iol_terminal_mixproof_DO3_DI2( const char* dev_name );

        /// @brief Получение состояния обратной связи.
        bool get_fb_state() const override;

        bool get_on_fb_value() const override;

        bool get_off_fb_value() const override;
    };
//-----------------------------------------------------------------------------
/// @brief Управляемый клапан.
class analog_valve : public AO1
    {
    public:
        explicit analog_valve( const char* dev_name );

        float get_min_value() const override;

        float get_max_value() const override;

        void direct_on() override;

        void direct_off() override;

        void direct_set_state( int new_state ) override;

        void direct_set_value( float new_value ) override;

    private:
        enum CONSTANTS
            {
            C_MIN = 0,   ///< Минимальное значение.
            C_MAX = 100, ///< Максимальное значение.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Управляемый клапан, подключаемый к конвертеру.
class analog_valve_ey : public device
    {
    public:
        explicit analog_valve_ey( const char* dev_name );

        void set_property( const char* field, device* dev ) override;

        void set_string_property( const char* field, const char* new_value ) override;

        void direct_on() override;
        void direct_off() override;

        void direct_set_value( float new_value ) override;

        float get_value() const override;
        int get_state() const override;

        void set_rt_par( u_int idx, float value ) override;

#ifndef PTUSA_TEST
    private:
#endif // !PTUSA_TEST

        enum class CONSTANTS
            {
            FULL_CLOSED = 0,
            FULL_OPENED = 100,
            };

        converter_iolink_ao* conv{ nullptr };
        int ey_number{ -1 };
    };
//-----------------------------------------------------------------------------
/// @brief Управляемый клапан IO-Link.
class analog_valve_iolink : public AO1
    {
    public:
        explicit analog_valve_iolink( const char* dev_name );

        virtual ~analog_valve_iolink() = default;

        void evaluate_io() override;

        float get_min_value() const override;

        float get_max_value() const override;

        int save_device_ex( char* buff ) const override;

        enum class CONSTANTS
            {
            FULL_CLOSED = 0,
            FULL_OPENED = 100,
            };

        void direct_on() override;

        void direct_off() override;

        void direct_set_value( float new_value ) override;

        float get_value() const override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        int get_state() const override;

        const char* get_error_description() override;

        enum class PAR_CONSTANTS
            {
            P_FB = 1,

            ADDITIONAL_PARAMS_COUNT, ///Количество дополнительных параметров.
            };

#ifndef PTUSA_TEST
    private:
#endif // !PTUSA_TEST

#pragma pack(push,1)
        struct in_data
            {
            float position;         //Valve position in percent.
            float setpoint;         //Used set-point in percent.
            uint8_t namur_state = 0;
            bool closed : 1;        //True = Closed, False = Not closed.
            bool opened : 1;        //True = Opened, False = Not opened.
            uint8_t status : 6;
            };

        struct out_data
            {
            float position;     //Cyclic CMD set-point in percent.
            uint8_t unused : 7;
            bool wink : 1;      //Visual indication.
            };
#pragma pack(pop)

        in_data in_info{};
        static out_data stub_out_info;
        out_data* out_info = &stub_out_info;

        bool blink = false;     //Visual indication.
        io_link_device iol_device;
    };
