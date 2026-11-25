#pragma once

#include "s_types.h"

//-----------------------------------------------------------------------------
/// @brief Интерфейс устройства, позволяющий сохранить его в потоке байтов.
class i_Lua_save_device
    {
    public:
        virtual ~i_Lua_save_device() = default;

        /// @brief Сохранение самого устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///
        /// @return >= 0 - количество записанных байт.
        virtual int save_device( char* buff ) const = 0;

        /// @brief Отладочная печать объекта в консоль.
        virtual const char* get_name_in_Lua() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс устройства, позволяющий получать команды от сервера.
class i_cmd_device
    {
    public:
        virtual ~i_cmd_device() = default;

        /// @brief Выполнение числовой команды.
        ///
        /// @param prop [ in ] - имя свойства.
        /// @param idx [ in ]  - индекс для свойства.
        /// @param val [ in ]  - значение.
        ///
        /// @return 0 - ок.
        /// @return 1 - ошибка.
        virtual int set_cmd( const char* prop, u_int idx, double val ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс счетчика.
class i_counter
    {
    public:
        virtual ~i_counter() = default;

        /// @brief Приостановка работы счетчика.
        virtual void pause() = 0;

        /// @brief Возобновление работы счетчика.
        virtual void start() = 0;

        /// @brief Сброс счетчика и остановка счета.
        ///
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        virtual void reset() = 0;

        /// @brief Сброс счетчика и продолжение счета.
        virtual void restart();

        /// @brief Получение значения счетчика.
        virtual u_int get_quantity() const = 0;

        /// @brief Получение значения счетчика.
        virtual float get_flow() const = 0;

        /// @brief Получение состояния работы счетчика.
        virtual int get_state() const = 0;

        /// @brief Получение абсолютного значения счетчика (без учета
        /// состояния паузы).
        virtual u_int get_abs_quantity() const = 0;

        /// @brief Сброс абсолютного значения счетчика.
        virtual void  abs_reset() = 0;

        enum DAY_CTR
            {
            DAY_T1,
            DAY_T2
            };

        /// @brief Приостановка работы дневного счетчика.
        virtual void pause_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) = 0;

        /// @brief Возобновление работы дневного счетчика.
        virtual void start_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) = 0;

        enum class STATES
            {
            S_WORK = 1,
            S_PAUSE,

            S_PUMP_ERROR = -9,  // Ошибка связанного насоса.
            S_FLOW_ERROR = -10, // Ошибка расхода (самотёка).

            S_LOW_ERR = -12,
            };

    protected:
        /// @brief Получение времени ожидания работы насоса.
        virtual u_long get_pump_dt() const = 0;

        /// @brief Получение минимального расхода для учета работы связанных
        /// насосов.
        virtual float get_min_flow() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс противосмешивающего клапана (mixproof).
class i_mix_proof
    {
    public:
        virtual ~i_mix_proof() = default;

        /// @brief Открыть верхнее седло.
        virtual void open_upper_seat() = 0;

        /// @brief Открыть нижнее седло.
        virtual void open_lower_seat() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс весов.
class i_wages
    {
    public:
        virtual ~i_wages() = default;

        /// @brief Тарировка.
        virtual void tare() = 0;
        ///@brief Возвращает вес в килограммах.
        virtual float get_value() const = 0;
        ///@brief Возвращает состояние
        virtual int get_state() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного входа.
///
/// Обратная связь, предельный уровень и т.д. являются примерами таких
/// устройств.
class i_DI_device : public i_cmd_device
    {
    public:
        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() const = 0;

        /// @brief Проверка активного состояния.
        ///
        /// Для уровня - сработал он или нет.
        ///
        /// @return 1 - устройство активно.
        /// @return 0 - устройство неактивно.
        virtual bool is_active();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного выхода.
///
/// Клапан, мешалка и т.д. являются примерами таких устройств.
class i_DO_device : public i_DI_device
    {
    public:
        /// @brief Включение устройства с учетом ручного режима.
        virtual void on();

        virtual void print() const = 0;

        /// @brief Выключение устройства с учетом ручного режима.
        virtual void off() = 0;

        /// @brief немедленное выключение устройства
        virtual void direct_off() = 0;

        /// @brief немедленное выключение устройства c учетом ручного режима
        virtual void instant_off();

        /// @brief Установка нового состояния устройства с учетом ручного режима.
        ///
        /// @param new_state - новое состояние устройства.
        virtual void set_state( int new_state );

    protected:
        /// @brief Установка нового состояния устройства.
        ///
        /// @param new_state - новое состояние объекта.
        virtual void direct_set_state( int new_state ) = 0;

        /// @brief Включение устройства.
        ///
        /// Установка устройства в активное состояние. Для клапана это означает
        /// его активирование, то есть если он нормально закрытый - открытие.
        virtual void direct_on() = 0;

        /// @brief Включен ли ручной режим.
        ///
        /// В ручном режиме устройство управляется по командам от сервера
        /// (управляющая программа не влияет на устройство).
        ///
        /// @return true - ручной режим включен.
        /// @return false - ручной режим выключен.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на на основе аналогового входа.
///
/// Температура, расход и т.д. являются примерами таких устройств.
class i_AI_device : public i_cmd_device
    {
    public:
        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде дробного числа.
        virtual float get_value() const = 0;

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе аналогового выхода.
///
/// Аналоговый канал управления и т.д. являются примерами таких устройств.
class i_AO_device : public i_AI_device
    {
    public:
        /// @brief Выключение устройства с учетом ручного режима.
        virtual void off() = 0;

        /// @brief Установка текущего состояния устройства с учетом ручного режима.
        virtual void set_value( float new_value );

    protected:
        /// @brief Установка текущего состояния устройства.
        ///
        /// @param new_value - новое состояние устройства.
        virtual void direct_set_value( float new_value ) = 0;

        /// @brief Включен ли ручной режим.
        ///
        /// В ручном режиме устройство управляется по командам от сервера
        /// (управляющая программа не влияет на устройство).
        ///
        /// @return true - ручной режим включен.
        /// @return false - ручной режим выключен.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс устройства как с аналоговыми, так и дискретными каналами.
class i_DO_AO_device : public i_AO_device, public i_DO_device
    {
    public:
        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        void off() override = 0;

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        int get_state() const override = 0;
    };
