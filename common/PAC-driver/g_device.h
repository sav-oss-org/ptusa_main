/// @file g_device.h
/// @brief Содержит описания классов, которые используются совместно в
/// контроллере и драйвере на сервере.
///
/// Описываются базовые интерфейсы, классы.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @c DRIVER   - компиляция для драйвера в OS Windows.@n
/// @c WIN32    - компиляция для PAC с ОС Windows.@n
/// @c LINUX_OS - компиляция для PAC с ОС linux.@n Данные директивы определяют
/// разрядность базовых типов данных (@ref int_2, @ref u_int_4, @ref u_int_2).
/// @c DEBUG_DEV_CMCTR - вывод отладочной информации в консоль.@n
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
#ifndef GENERAL_DEVICE_H
#define GENERAL_DEVICE_H

#ifndef DRIVER

#include <stdlib.h>
#include <vector>

#include "smart_ptr.h"
#include "tcp_cmctr.h"
#include "device/i_base.h"

extern "C" {
#include "zlib.h"
    };

#endif // DRIVER
//-----------------------------------------------------------------------------
/// @brief Коммуникатор устройств - содержит все устройства одного PAC. Служит
/// для передачи информации о них и их состоянии на сервер (PC).
class device_communicator
    {
    public:
        enum CMD
            {
            CMD_GET_INFO_ON_CONNECT = 10, ///< Запрос инф. о PAC перед дальнейшей работой.

            CMD_GET_DEVICES = 100,   ///< Запрос инф. об устройствах PAC.
            CMD_GET_DEVICES_STATES,  ///< Запрос инф. о состоянии устройств PAC.
            CMD_EXEC_DEVICE_COMMAND, ///< Выполнение команды для устройства.

            CMD_GET_PAC_ERRORS,      ///< Запрос инф. об ошибках PAC.
            CMD_SET_PAC_ERROR_CMD,   ///< Выполнение команды для ошибки.

            // Резервное копирование параметров.
            ///@brief Получение параметров устройств.
            ///
            /// Они используются для создания резервной копии на стороне сервера.
            CMD_GET_PARAMS,

            ///@brief Восстановление параметров устройств из скрипта Lua.
            CMD_RESTORE_PARAMS,
            ///@brief Получение контрольной суммы параметров.
            ///
            /// Эта сумма используется для отслеживания изменения параметров PAC
            /// и их записи в этом случае в резервную копию.
            CMD_GET_PARAMS_CRC,
            // Резервное копирование параметров. -!>

            CMD_RM_GET_DEVICES = 200,   ///< Запрос устройств PAC от PAC-мастера.
            CMD_RM_GET_DEVICES_STATES,  ///< Запрос состояния устройств PAC от PAC-мастера.
            };

#ifndef DRIVER
    private:
        /// Единственный экземпляр класса.
        static auto_smart_ptr < device_communicator > instance;
        static char buff[ tcp_communicator::BUFSIZE ];

        inline static bool use_compression{ true };

    public:
        static void switch_on_compression()
            {
            use_compression = true;
            }

        static void switch_off_compression()
            {
            use_compression = false;
            }

        /// @brief Получение единственного экземпляра класса.
        static device_communicator* get_instance()
            {
            if ( instance.is_null() )
                {
                instance = new device_communicator();
                }

            return instance;
            }

        enum CONSTANTS
            {
            C_SERVICE_N = 1, ///< Номер сервиса коммуникатора.
            };

        /// @brief Устройства, информация о них и их состоянии передается на
        /// сервер (PC).
        static std::vector< i_Lua_save_device* > dev;

    public:
        /// @brief Вывод на консоль устройств группы.
        void print() const;

        /// @brief Добавление устройства.
        int add_device( i_Lua_save_device *dev );

        /// @brief Замена устройства на новое (при горячей перезагрузке
        /// технологического объекта).
        ///
        /// @param old_device - прежнее устройство.
        /// @param new_device - новое устройство.
        void update_device( i_Lua_save_device* old_device,
            i_Lua_save_device* new_device );

        /// @brief Сервис для работы с device_communicator.
        static long write_devices_states_service( long len, u_char *data,
            u_char *outdata );

        /// @brief Удаление устройств.
        void clear_devices();

        /// @brief Идентификатор ошибок.
        inline static u_int_2 errors_id{};

#ifdef PTUSA_TEST
        static void clear_errors_id()
            {
            errors_id = 0;
            }
#endif // PTUSA_TEST

#endif // !DRIVER
    };
//-----------------------------------------------------------------------------
#ifndef DRIVER
#define G_DEVICE_CMMCTR device_communicator::get_instance()
#endif // !DRIVER

#endif // DEVICES_H
