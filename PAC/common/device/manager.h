#pragma once

#include "base.h"
#include "valve.h"
#include "life_device.h"
#include "node_dev.h"
#include "device.h"

class dev_stub;
//-----------------------------------------------------------------------------
/// @brief Получение клапана по номеру.
///
/// @param number - номер клапана.
/// @return - клапан с заданным номером. Если нет такого клапана, возвращается
/// заглушка (@ref dev_stub).
i_DO_device* V( u_int dev_n );
valve* V( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение управляемого клапана по номеру.
///
/// @param number - номер клапана.
/// @return - клапан с заданным номером. Если нет такого клапана, возвращается
/// заглушка (@ref dev_stub).
i_AO_device* VC( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение двигателя по номеру.
///
/// @param number - номер двигателя.
/// @return - двигатель с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_motor* M( u_int dev_n );
i_motor* M( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального уровня по номеру.
///
/// @param number - номер сигнального уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* LS( u_int dev_n );
i_DI_device* LS( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального расхода по номеру.
///
/// @param number - номер сигнального расхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* FS( u_int dev_n );
i_DI_device* FS( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового входа по номеру.
///
/// @param number - номер аналогового входа.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* AI( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового выхода по номеру.
///
/// @param number - номер аналогового выхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AO_device* AO( u_int dev_n );
i_AO_device* AO( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение счетчика по номеру.
///
/// @param number - номер счетчика.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_counter* FQT( u_int dev_n );
i_counter* FQT( const char* dev_name );

virtual_counter* virtual_FQT( const char* dev_name );

counter_iolink* FQT_IOLINK( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение температуры по номеру.
///
/// @param number - номер температуры.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* TE( u_int dev_n );
i_AI_device* TE( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение текущего уровня по номеру.
///
/// @param number - номер текущего уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
level* LT( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение датчика положения по номеру.
///
/// @param number - номер датчика положения.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* GS( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение звуковой сигнализации по номеру.
///
/// @param number - номер.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* HA( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение световой сигнализации по номеру.
///
/// @param number - номер.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* HL( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение кнопки по номеру.
///
/// @param number - номер.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* SB( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение обратной связи по номеру.
///
/// @param number - номер обратной связи.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* DI( u_int dev_n );
i_DI_device* DI( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение канала управления по номеру.
///
/// @param number - номер канала управления.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* DO( u_int dev_n );
i_DO_device* DO( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение текущего давления по номеру.
///
/// @param number - номер текущего давления.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* PT( u_int dev_n );
i_AI_device* PT( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение текущей концентрации по номеру.
///
/// @param number - номер текущей концентрации.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* QT( u_int dev_n );
i_AI_device* QT( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение весов по номеру.
///
/// @param number - номер весов.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_wages* WT( u_int dev_n );
i_wages* WT( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение автоматического выключателя по номеру.
///
/// @param number - номер автоматического выключателя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_AO_device* F( u_int dev_n );
i_DO_AO_device* F( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение регулятора по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_AO_device* C( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнальной колонны по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
signal_column* HLA( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение камеры по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_camera* CAM( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение датчика разности давления.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* PDS( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального датчика температуры по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* TS( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение блока питания по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_AO_device* get_G( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение устройства проверки связи.
///
/// @param number - номер обратной связи.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* WATCHDOG( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение конвертера IO-Link.
///
/// @param dev_name - имя конвертера.
/// @return - устройство с заданным именем. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AO_device* EY( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение виртуального устройства.
///
/// @return - виртуальное устройство.
dev_stub* STUB();
//-----------------------------------------------------------------------------
/// @brief Получение устройства по номеру и типу.
///
/// @param type   - тип устройства.
/// @param s_number - порядковый номер устройства.
/// @return - устройство.
device* DEVICE( int s_number );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Менеджер устройств.
///
/// Содержит информацию обо всех устройствах проекта.
class device_manager : public i_Lua_save_device
    {
    public:
        device_manager();

        ~device_manager() override;

        /// @brief Отключение сообщений о ненайденных устройствах.
        bool disable_error_logging;

        /// @brief Получение устройства.
        device* get_device( int dev_type, const char* dev_name );

        /// @brief Получение устройства.
        device* get_device( const char* dev_name );

        /// @brief Получение устройства.
        device* get_device( size_t serial_dev_n );

        /// @brief Получение клапана по его номеру.
        i_DO_device* get_V( const char* dev_name );

        /// @brief Получение управляемого клапана по номеру.
        i_AO_device* get_VC( const char* dev_name );

        /// @brief Получение двигателя по номеру.
        i_motor* get_M( const char* dev_name );

        /// @brief Получение уровня по номеру.
        i_DI_device* get_LS( const char* dev_name );

        /// @brief Получение расхода по номеру.
        i_DI_device* get_FS( const char* dev_name );

        /// @brief Получение датчика положения по номеру.
        i_DI_device* get_GS( const char* dev_name );

        /// @brief Получение аналогового входа по номеру.
        i_AI_device* get_AI( const char* dev_name );

        /// @brief Получение аналогового выхода по номеру.
        i_AO_device* get_AO( const char* dev_name );

        /// @brief Получение счетчика по номеру.
        i_counter* get_FQT( const char* dev_name );
        virtual_counter* get_virtual_FQT( const char* dev_name );
        counter_iolink* get_FQT_IOLINK( const char* dev_name );

        /// @brief Получение температуры по номеру.
        i_AI_device* get_TE( const char* dev_name );

        /// @brief Получение текущего уровня по номеру.
        i_AI_device* get_LT( const char* dev_name );

        /// @brief Получение обратной связи по номеру.
        i_DI_device* get_DI( const char* dev_name );

        /// @brief Получение кнопки по номеру.
        i_DI_device* get_SB( const char* dev_name );

        /// @brief Получение управляющего канала по номеру.
        i_DO_device* get_DO( const char* dev_name );

        /// @brief Получение звуковой сигнализации.
        i_DO_device* get_HA( const char* dev_name );

        /// @brief Получение световой сигнализации.
        i_DO_device* get_HL( const char* dev_name );

        /// @brief Получение текущего давления по номеру.
        i_AI_device* get_PT( const char* dev_name );

        /// @brief Получение текущей концентрации по номеру.
        i_AI_device* get_QT( const char* dev_name );

        /// @brief Получение весов по номеру.
        i_wages* get_WT( const char* dev_name );

        /// @brief Получение регулятора по имени.
        i_DO_AO_device* get_C( const char* dev_name );

        /// @brief Получение сигнальной колонны по имени.
        signal_column* get_HLA( const char* dev_name );

        /// @brief Получение камеры по имени.
        i_camera* get_CAM( const char* dev_name );

        /// @brief Получение датчика разности давления по имени.
        i_DI_device* get_PDS( const char* dev_name );

        /// @brief Получение автоматического выключателя по имени.
        i_DO_AO_device* get_F( const char* dev_name );

        /// @brief Получение сигнального датчика температуры по имени.
        i_DI_device* get_TS( const char* dev_name );

        /// @brief Получение блока питания температуры по имени.
        i_DO_AO_device* get_G( const char* dev_name );

        /// @brief Получение устройства проверки связи по имени.
        i_DI_device* get_watchdog( const char* dev_name );

        /// @brief Получение конвертера IO-Link по имени.
        i_AO_device* get_EY( const char* dev_name );

        /// @brief Получение узла I/O по имени.
        node_dev* get_node( const char* dev_name );


        /// @brief Получение единственного экземпляра класса.
        static device_manager* get_instance();

        ///@brief Получение количества всех устройств.
        size_t get_device_count() const;

        /// @brief Отладочная печать объекта в консоль.
        void print() const;

        const char* get_name_in_Lua() const override;

        /// @brief Получение фиктивного устройства.
        dev_stub* get_stub();

        device* get_stub_device();

        int init_params();

        int init_rt_params();

        void evaluate_io();

        int save_device( char* buff ) const override;

#ifdef PTUSA_TEST
        int add_device( device* new_device, device::DEVICE_TYPE dev_type );

        int remove_device( u_int idx );
#endif

    protected:
        char is_first_device[ device::C_DEVICE_TYPE_CNT ] = { 0 };

        dev_stub *stub;  ///< Устройство-заглушка, фиктивное устройство.

        struct range    ///< Диапазон устройств одного типа.
            {
            int start_pos = -1;
            int end_pos = -1;
            };

        /// Диапазоны устройств всех типов.
        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        int get_device_n( device::DEVICE_TYPE dev_type,
            const char* dev_name );

        int get_device_n( const char* dev_name );

        std::vector< device* > project_devices; ///< Все устройства.

        /// @brief Единственный экземпляр класса.
        static auto_smart_ptr < device_manager > instance;

    public:
        // @brief Добавление устройства.
        //
        // Вызывается из Lua.
        io_device* add_io_device( int dev_type, int dev_sub_type,
            const char* dev_name, const char* descr, const char* article );

        // @brief Очистка списка устройств.
        //
        // Для тестирования.
        void clear_io_devices();
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
///@brief Получение менеджера устройств.
///
///@return Менеджер устройств проекта.
    device_manager* G_DEVICE_MANAGER();
