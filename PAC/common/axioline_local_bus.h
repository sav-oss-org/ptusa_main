#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

/// @brief Доступ к данным локальной шины без зависимости от SDK PLCnext.
class local_bus_driver
    {
    public:
        struct module_info
            {
            unsigned int article;
            unsigned int bytes;
            bool missing;
            };

        /// @brief Последний снимок диагностических регистров Axiobus.
        struct diagnostics
            {
            uint16_t status = 0;
            uint16_t error_code = 0;
            uint16_t error_location = 0;
            bool driver_error = false;
            bool valid = false;
            };

        // I/O warning/error, SYSFAIL, FORCE, SYNC_FAILED, PARAM_REQUIRED.
        static constexpr uint16_t NOTIFICATION_MASK = 0x1E03;

        virtual ~local_bus_driver() = default;
        virtual bool initialize() = 0;
        /// @brief Установить состояние отказа при ошибке проверки или обмена.
        virtual bool set_fail_state() { return true; }
        virtual const char* get_error_text() const { return ""; }
        virtual diagnostics get_diagnostics() const { return {}; }
        virtual bool ready() = 0;
        virtual size_t module_count() = 0;
        virtual module_info module( size_t index ) = 0;
        virtual bool read_inputs() = 0;
        virtual bool write_outputs() = 0;
        virtual void read_module( size_t index, unsigned char* data,
            unsigned int bytes ) = 0;
        virtual void write_module( size_t index, unsigned char* data,
            unsigned int bytes ) = 0;
    };

enum class local_bus_backend { LIBRARY, IOCTL };

/// Байт DI8/DO8 в слове проекта и в выровненном аппаратном слове Axioline.
constexpr unsigned int local_bus_module_byte_offset( unsigned int article,
    unsigned int bytes )
    {
    return bytes == 1 && ( article == 2701916 || article == 2702071 )
        ? 1u : 0u;
    }

/// @brief Создать выбранный транспорт без открытия аппаратного устройства.
/// На неподдерживаемой платформе возвращает nullptr.
std::unique_ptr<local_bus_driver> make_local_bus_driver(
    local_bus_backend backend );

/// @brief Длина процессных данных из записи идентификации PDI 0x0037.
bool decode_local_bus_process_data_length( const unsigned char* record,
    size_t size, unsigned int& bytes );

/// @brief Состояние, код и место ошибки с расшифровкой флагов и известных кодов.
std::string describe_local_bus_diagnostics(
    const local_bus_driver::diagnostics& diagnostics );
