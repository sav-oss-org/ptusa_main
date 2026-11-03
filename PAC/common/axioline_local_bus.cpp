#include "axioline_local_bus.h"
#include "axio_ioctl_driver.h"
#include "fmt/format.h"

bool decode_local_bus_process_data_length( const unsigned char* record,
    size_t size, unsigned int& bytes )
    {
    bytes = 0;
    if ( !record || size < 8 ) return false;
    auto length = static_cast<unsigned int>( record[2] ) * 256 + record[3];
    if ( length > 1024 ) return false;
    bytes = length;
    return true;
    }

std::string describe_local_bus_diagnostics(
    const local_bus_driver::diagnostics& diag )
    {
    if ( !diag.valid ) return {};
    auto text = fmt::format( "status=0x{:04X}, код=0x{:04X}, param2=0x{:04X}",
        diag.status, diag.error_code, diag.error_location );
    if ( ( diag.status & 0x0007 ) && diag.error_location )
        {
        text += fmt::format( ", слот {}", diag.error_location );
        }
    if ( diag.driver_error ) text += "; ошибка аппаратного драйвера";
    struct flag_text { uint16_t flag; const char* text; };
    const flag_text flags[] =
        {
        { 0x0001, "предупреждение I/O (IO_WARNING)" },
        { 0x0002, "ошибка I/O (IO_ERROR)" },
        { 0x0004, "ошибка шины (BUS_ERROR)" },
        { 0x0008, "ошибка контроллера (CONTROLLER_ERROR)" },
        { 0x0100, "изменён состав шины (BUS_DIFFERENT)" },
        { 0x0200, "выходные данные заблокированы (SYS_FAIL)" },
        { 0x0400, "режим принудительного управления (FORCE_MODE)" },
        { 0x0800, "ошибка синхронизации (SYNC_FAILED)" },
        { 0x1000, "требуется параметризация (PARAM_REQUIRED)" },
        };
    for ( const auto& flag : flags )
        {
        if ( diag.status & flag.flag ) text += std::string( "; " ) + flag.text;
        }
    if ( !( diag.status & 0x0020 ) ) text += "; обмен остановлен (RUNNING=0)";
    if ( !( diag.status & 0x0040 ) ) text += "; конфигурация не активна (ACTIVE=0)";
    if ( !( diag.status & 0x0080 ) ) text += "; шина не готова (BUS_READY=0)";

    // UM EN AXL F SYS DIAG, таблица 3-5. Неизвестный код остаётся в сообщении.
    if ( diag.status & 0x0003 )
        {
        const char* cause = nullptr;
        switch ( diag.error_code )
            {
            case 0x2340: cause = "перегрузка или короткое замыкание питания"; break;
            case 0x2344: cause = "перегрузка или короткое замыкание выхода"; break;
            case 0x3130: cause = "питание I/O отсутствует или неисправно"; break;
            case 0x3412: cause = "отсутствует питание датчиков"; break;
            case 0x3422: cause = "отсутствует питание исполнительных устройств"; break;
            case 0x6320: cause = "некорректная таблица параметров"; break;
            case 0x7710: cause = "обрыв сигнальной линии"; break;
            case 0x8910: cause = "превышен диапазон измерения"; break;
            case 0x8920: cause = "значение ниже диапазона измерения"; break;
            }
        if ( cause ) text += std::string( "; " ) + cause;
        }
    return text;
    }

#ifdef PTUSA_AXIOBUS
#include "Axiobus/Axiobus.h"
#include "axiobus_process_data.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

namespace
    {
    class axiobus_driver final : public local_bus_driver
        {
        public:
            bool initialize() override
                {
                if ( bus && bus->isInitialized() && metadata_ready )
                    {
                    error_text.clear();
                    return true;
                    }
                if ( !check_access( "/dev/axio_xfer0", false ) ||
                    !check_access( "/tmp/axiopdi", true ) ) return false;
                if ( !bus )
                    {
                    bus = std::make_unique<PLCnext::Axiobus>(
                        PLCnext::Axiobus::DIRECT,
                        PLCnext::Axiobus::EXPLICIT );
                    }
                else if ( !bus->isInitialized() )
                    {
                    bus->initialize();
                    }
                if ( !bus->isInitialized() )
                    {
                    auto error = bus->getInitializationError();
                    error_text = fmt::format( "инициализация Axiobus: {} (код {})",
                        error == PLCnext::Axiobus::PDI_MUTEX_ERROR
                            ? "недоступен mutex PDI"
                            : error == PLCnext::Axiobus::MODULE_SCAN_ERROR
                                ? "ошибка сканирования модулей" : "ошибка драйвера",
                        error );
                    return false;
                    }
                process_bytes.clear();
                for ( auto module : bus->getModules() )
                    {
                    std::vector<unsigned char> record;
                    auto result = module->pdiRead( 0, 0x0037, 0, record );
                    unsigned int bytes = 0;
                    if ( !result || !decode_local_bus_process_data_length(
                        record.data(), record.size(), bytes ) )
                        {
                        error_text = fmt::format(
                            "чтение описания PDI 0x0037: слот {}, код=0x{:04X}, размер={}",
                            module->getSlotNumber(), result.ErrorCode,
                            record.size() );
                        return false;
                        }
                    process_bytes.push_back( bytes );
                    }
                metadata_ready = true;
                error_text.clear();
                return true;
                }

            const char* get_error_text() const override { return error_text.c_str(); }
            diagnostics get_diagnostics() const override { return diagnostic; }

            bool ready() override
                {
                auto diag = bus->getDiagnosticsInfo();
                diagnostic = { diag.status, diag.param1, diag.param2,
                    diag.driverError, true };
                // RUNNING, ACTIVE, BUS_READY; ошибки шины и контроллера.
                bool operational = !diag.driverError &&
                    ( diag.status & 0x00E0 ) == 0x00E0 &&
                    ( diag.status & 0x010C ) == 0;
                error_text = describe_local_bus_diagnostics( diagnostic );
                if ( ( diag.status & 0x080C ) && diag.param1 )
                    {
                    error_text += "; " + PLCnext::Axiobus::busErrorToString( diag.param1 );
                    }
                return operational;
                }

            size_t module_count() override
                {
                return bus->getModules().size();
                }

            module_info module( size_t index ) override
                {
                auto module = bus->getModules().at( index );
                return { module->getOrderNumber(),
                    process_bytes.at( index ), module->isMissing() };
                }

            bool read_inputs() override
                {
                if ( bus->readInputs() ) return true;
                ready();
                error_text = "ошибка чтения: " + error_text;
                return false;
                }
            bool write_outputs() override
                {
                if ( bus->writeOutputs() ) return true;
                ready();
                error_text = "ошибка записи: " + error_text;
                return false;
                }

            void read_module( size_t index, unsigned char* data,
                unsigned int bytes ) override
                {
                const auto info = module( index );
                axiobus_process_data<PLCnext::AXLModule>::read(
                    *bus->getModules().at( index ), data, bytes,
                    info.bytes, local_bus_module_byte_offset(
                        info.article, info.bytes ) );
                }

            void write_module( size_t index, unsigned char* data,
                unsigned int bytes ) override
                {
                const auto info = module( index );
                axiobus_process_data<PLCnext::AXLModule>::write(
                    *bus->getModules().at( index ), data, bytes,
                    info.bytes, local_bus_module_byte_offset(
                        info.article, info.bytes ) );
                }

        private:
            bool check_access( const char* path, bool optional )
                {
                struct stat info;
                if ( stat( path, &info ) != 0 )
                    {
                    if ( optional && errno == ENOENT ) return true;
                    }
                else if ( access( path, R_OK | W_OK ) == 0 ) return true;
                auto code = errno;
                error_text = fmt::format( "{}: {} ({})",
                    path, std::strerror( code ), code );
                return false;
                }

            std::unique_ptr<PLCnext::Axiobus> bus;
            std::vector<unsigned int> process_bytes;
            bool metadata_ready = false;
            diagnostics diagnostic;
            std::string error_text;
        };
    }
#endif

std::unique_ptr<local_bus_driver> make_local_bus_driver(
    local_bus_backend backend )
    {
    if ( backend == local_bus_backend::IOCTL ) return make_axio_ioctl_driver();
#ifdef PTUSA_AXIOBUS
    return std::make_unique<axiobus_driver>();
#else
    return nullptr;
#endif
    }
