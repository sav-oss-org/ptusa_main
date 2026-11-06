/// @file PAC_err.h
/// @brief Содержит описания классов, которые используются для организации
/// работы с критическими ошибками PAC.
///
/// Класс @ref PAC_critical_errors_manager, служащий для организации работы с
/// ошибками простых устройств, содержит всю необходимую информацию. Для сбора
/// всех ошибок служит структура @ref PAC_critical_errors_manager::critical_error.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c PAC_ERRORS_H - включение данного файла в компиляцию только один раз.@n
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_ERRORS_H
#define PAC_ERRORS_H

#include <errno.h>

#include <string.h>
#include <vector>

#include "g_errors.h"
#include "smart_ptr.h"
#include "dtime.h"
#include "led.h"

//-----------------------------------------------------------------------------
//0, 1, n - нет связи с узлом I/O номер n.
//0, 2, n - нет связи с панелью номер n.
//0, 3, n - нет связи с Modbus устройством номер n.
//0, 4, 4 -
//0, 5, 5 - нет связи с сервером.
//
//1, 1 - ошибка работы с COM-портом WAGO.
//      1 - CRC error
//
//13 - ошибки времени работы:
//  1, n  - нажата аварийная кнопка номер n.
//-----------------------------------------------------------------------------
class PAC_critical_errors_manager
    {
    public:
        enum ALARM_CLASS          ///< Класс тревоги.
            {
            AC_NO_CONNECTION = 1, ///< Ошибка связи.

            AC_NET = 4,           ///< Ошибки сетевой работы.
            AC_SERVICE = 5,	      ///< Сервисное обслуживание устройства.

            /// Режим конфигурирования устройства (PP mode).
            AC_PP_MODE = 6,

            /// Ошибка конфигурации или локальной шины.
            AC_CFG_BUS_ERROR = 7,
            };

        enum ALARM_SUBCLASS         ///< Подкласс тревоги.
            {
            AS_IO_COUPLER = 1,      ///< Ошибки модулей I/O.
            AS_EASYSERVER = 5,      ///< Ошибки EasyServer.

            //AC_NET,               ///< Ошибки сетевой работы.
            AS_SOCKET_F = 1,        ///< Функция socket.
            AS_BIND_F = 2,          ///< Функция bind.
            AS_SETSOCKOPT_F = 3,
            AS_LISTEN_F = 4,
            };

    public:
        enum GE_CONST
            {
            GE_ERROR_SIZE = 3,      ///< Размер одной ошибки, байт.
            };

        ~PAC_critical_errors_manager();

        void show_errors() const;
        void set_global_error( ALARM_CLASS eclass, ALARM_SUBCLASS p1,
            unsigned int param );
        void reset_global_error( ALARM_CLASS eclass, ALARM_SUBCLASS p1,
            unsigned int param, bool is_print_msg = true );

        void reset_all_error();

        int save_as_Lua_str( char* str, u_int_2& id );

        bool is_any_critical_error() const;

        static PAC_critical_errors_manager* get_instance();

        u_int get_id() const
            {
            return errors_id;
            }

        bool is_any_error() const
            {
            return !errors.empty();
            }

    private:
        static const char* get_alarm_group()
            {
            return "Авария";
            }

        const char* get_alarm_descr( ALARM_CLASS err_class,
            ALARM_SUBCLASS err_sub_class, unsigned int par, bool is_set );


        static auto_smart_ptr < PAC_critical_errors_manager > instance;

        struct critical_error
            {
            int             err_class;           ///< Класс ошибки.
            unsigned int    err_sub_class;       ///< Подкласс ошибки.
            unsigned int    param;               ///< Параметр ошибки.
            int             priority;            ///< Приоритет ошибки.

            critical_error( int err_class = 0, u_int err_sub_class = 0,
                u_int param = 0,
                int priority = ALARM_CLASS_PRIORITY::P_ERR_CONNECTION );
            };

        PAC_critical_errors_manager&
            operator=( PAC_critical_errors_manager&& ) = delete;

        std::vector< critical_error >  errors;

        u_int_2 errors_id{};
    };
//-----------------------------------------------------------------------------
#endif // PAC_ERRORS_H
