#pragma once

#include "axioline_local_bus.h"
#include <array>
#include <mutex>
#include <vector>

/// ABI драйвера axio_xfer. Указатели имеют размер целевого процесса.
namespace axio_ioctl
    {
    constexpr unsigned long WRITE_PD = 0x20;
    constexpr unsigned long READ_PD = 0x21;
    constexpr unsigned long SEND = 0x30;
    constexpr unsigned long RECEIVE = 0x31;
    constexpr unsigned long DIAGNOSTICS = 0x5A;
    constexpr unsigned long SET_SYSFAIL = 0x87;
    constexpr unsigned long CLEAR_SYSFAIL = 0x88;
    constexpr unsigned long RESET = 0x100;
    constexpr size_t BUFFER_SIZE = 1024;

    #pragma pack(push, 1)
    struct process_data
        {
        unsigned char* data;
        uint32_t offset, length, timeout, options;
        };

    struct message
        {
        uint8_t client, options;
        uint16_t type, bytes, id;
        uint32_t timeout;
        unsigned char* data;
        };
    #pragma pack(pop)

    // Значения регистров в памяти представлены в Big Endian.
    struct diagnostic_registers
        {
        uint16_t status, code, location, reserved;
        };

    static_assert( sizeof( process_data ) == sizeof( void* ) + 16,
        "axio_xfer process_data ABI" );
    static_assert( sizeof( message ) == sizeof( void* ) + 12,
        "axio_xfer message ABI" );
    static_assert( offsetof( message, data ) == 12,
        "axio_xfer message pointer" );
    static_assert( sizeof( diagnostic_registers ) == 8,
        "axio_xfer diagnostics ABI" );
    }

/// Системные вызовы заменяются в тестах без устройства и библиотек PLCnext.
class axio_ioctl_transport
    {
    public:
        virtual ~axio_ioctl_transport() = default;
        virtual bool open_device() = 0;
        virtual void close_device() = 0;
        virtual int control( unsigned long command, void* argument ) = 0;
        virtual int last_error() const = 0;
    };

/// Локальная шина через MXI/PDI и ioctl, без libAxiobus.
/// Настройкой и выходным образом должен владеть только этот процесс.
class axio_ioctl_driver final : public local_bus_driver
    {
    public:
        explicit axio_ioctl_driver(
            std::unique_ptr<axio_ioctl_transport> transport );
        ~axio_ioctl_driver() override;

        bool initialize() override;
        bool ready() override;
        bool set_fail_state() override;
        const char* get_error_text() const override;
        diagnostics get_diagnostics() const override;
        size_t module_count() override;
        module_info module( size_t index ) override;
        bool read_inputs() override;
        bool write_outputs() override;
        void read_module( size_t index, unsigned char* data,
            unsigned int bytes ) override;
        void write_module( size_t index, unsigned char* data,
            unsigned int bytes ) override;

    private:
        struct module_record
            {
            module_info info;
            size_t offset;
            bool staged = false;
            };

        bool read_diagnostics();
        bool hold_outputs();
        bool fail( const std::string& text );
        bool control( unsigned long command, void* argument );
        bool service( uint16_t command,
            const std::vector<uint16_t>& parameters,
            std::vector<unsigned char>& response );
        bool pdi_read( uint16_t slot, uint16_t index,
            std::vector<unsigned char>& data );
        bool scan_modules();
        void clear_staged_outputs();

        std::unique_ptr<axio_ioctl_transport> transport;
        std::vector<module_record> modules;
        std::array<unsigned char, axio_ioctl::BUFFER_SIZE> inputs{}, outputs{};
        size_t process_bytes = 0;
        uint8_t message_id = 0;
        bool opened = false, initialized = false;
        bool input_fresh = false, outputs_held = true;
        diagnostics diagnostic;
        std::string error_text;
        std::mutex service_mutex;
    };

/// Системный транспорт включается только в отдельном процессе PLCnext/Linux.
std::unique_ptr<local_bus_driver> make_axio_ioctl_driver();
