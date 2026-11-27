#pragma once

#include "dtime.h"

#include "param_ex.h"
#include "device/device.h"
#include "device/manager.h"

class watchdog : public device
    {
    public:
        enum class PARAM
            {
            P_T_GEN = 1,    ///< Интервал генерации выходных импульсов.
            P_T_ERR,        ///< Интервал ожидания изменения входа.

            PARAMS_COUNT
            };
        inline static const float MAX_OUT_VALUE = 32'767;

        /// @param name - имя.
        explicit watchdog(
            const char* name, device::DEVICE_SUB_TYPE sub_type =
                                  device::DEVICE_SUB_TYPE::DST_WATCHDOG );

        ~watchdog() override = default;

        void evaluate_io() override;

        void set_string_property(
            const char* field, const char* value ) override;
        void set_property( const char* field, device* value ) override;

        const char* get_name_in_Lua() const override;

        const char* get_error_description() override;

        void set_descr( const char* new_description ) override;

#ifndef PTUSA_TEST
    private:
#endif

        device* DI_dev = nullptr;
        device* AI_dev = nullptr;
        device* DO_dev = nullptr;
        device* AO_dev = nullptr;

        int prev_dev_state = 0;     ///< Предыдущее состояние устройства.
        float prev_dev_value = 0.f; ///< Предыдущее значение устройства.

        uint32_t start_in_check_time = get_millisec();
        uint32_t start_out_check_time = get_millisec();

        std::string error_description{};
    };
