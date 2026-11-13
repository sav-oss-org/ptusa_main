/// @file PID.h
/// @brief Содержит описание класса ПИД-контроллера.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef _PID_H
#define _PID_H

#include "dtime.h"

#include "param_ex.h"
#include "device/device.h"
#include "device/manager.h"

class PID : public device
    {
    public:
        enum PARAM
            {
            P_k = 1,               ///< Параметр k.
            P_Ti,                  ///< Параметр Ti.
            P_Td,                  ///< Параметр Td.
            P_dt,                  ///< Интервал расчёта
            P_max,                 ///< Мax значение входной величины.
            P_min,                 ///< Мin значение входной величины.
            P_acceleration_time,   ///< Время выхода на режим регулирования.
            P_is_manual_mode,      ///< Ручной режим.
            P_U_manual,            ///< Заданное ручное значение выходного сигнала.

            P_k2,                  ///< Параметр k2.
            P_Ti2,                 ///< Параметр Ti2.
            P_Td2,                 ///< Параметр Td2.

            P_out_max,             ///< Мax значение выходной величины.
            P_out_min,             ///< Мin значение выходной величины.

            P_is_reverse,	       ///Обратного (реверсивного) действия.
            P_is_zero_start,	   ///Нулевое стартовое значение.

            PARAMS_COUNT
            };

        /// TODO Удалить после обновления, оставлено для совместимости.
        const char* WORK_PARAMS_NAME = "RT_PAR_F";
        enum WORK_PARAM
            {
            WP_Z = 1,  ///< Требуемое значение.
            WP_U,      ///< Выход ПИД.
            };

        /// @param n - номер.
        ///
        /// TODO Удалить после обновления, оставлено для совместимости.
        PID( int n );

        /// @param name - имя.
        PID( const char* name );

        virtual ~PID();

        void on( char is_not_zero_start = 0 );

#ifndef __GNUC__
#pragma region Интерфейс device.
#endif
        void direct_on() override;
        void direct_set_state( int st ) override;
        void direct_off() override;
        int get_state() const override;

        void direct_set_value( float val ) override;
        float get_value() const override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        void set_string_property( const char* field, const char* value ) override;
#ifndef __GNUC__
#pragma endregion
#endif

        /// @brief Сброс ПИД.
        ///
        /// Сбрасываем все переменные.
		void reset();

        /// @brief Работа ПИД.
        float eval( float current_value, int delta_sign = 1 );

        /// @brief Установка нового задания ПИД.
        void set( float new_z );

        /// @brief Получение задания ПИД.
        float get_assignment();

        /// @brief Инициализация параметра ПИД.
        void init_param( PARAM par_n, float val );

        /// @brief Инициализация рабочего параметра ПИД.
        void init_work_param( int par_n, float val );

        /// @brief Сохранение параметров ПИД.
        void save_param();

        /// @brief Использование kN, TiN, TdN.
        void set_used_par ( int par_n );

        int save_device_ex( char* buff ) const override;
        int save_device( char *buff ) const override;

        const char* get_name_in_Lua() const override;

        device* get_actuator() const;

        /// @brief Отключаем, если перешли в состояние отключения.
        void evaluate_io() override;

        enum class STATE
            {
            OFF = 0,
            ON = 1,

            STOPPING = 10,
            };

        enum class CONSTANTS
            {
            DEFAULT_DELTA_MS = 1'000
            };

    private:
        float uk_1;
        float ek_1;
        float ek_2;
        float q0;
        float q1;
        float q2;
        float Uk;
        float dUk;
        uint32_t start_time;
        uint32_t last_time;

        char prev_manual_mode;

        STATE state;
        float out_value;
        float set_value;

        int used_par_n;
        float start_value;

        bool is_old_style;

        device* sensor;
        device* actuator;
        char in_value_name[ C_MAX_NAME + 1 ] = { 0 };
        char out_value_name[ C_MAX_NAME + 1 ] = { 0 };

        const float MIN_OUT_VALUE = .0f;
        const float MAX_OUT_VALUE = 100.f;
	};
#endif
