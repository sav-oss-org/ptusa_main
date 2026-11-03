#include "axio_ioctl_driver.h"
#include "fmt/format.h"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace
    {
    uint16_t word( const unsigned char* data )
        {
        return static_cast<uint16_t>( data[0] * 256 + data[1] );
        }

    uint32_t dword( const unsigned char* data )
        {
        return ( static_cast<uint32_t>( word( data ) ) << 16 ) |
            word( data + 2 );
        }

    void put_word( unsigned char* data, uint16_t value )
        {
        data[0] = static_cast<unsigned char>( value >> 8 );
        data[1] = static_cast<unsigned char>( value );
        }

    bool parse_article( const std::vector<unsigned char>& text,
        unsigned int& article )
        {
        article = 0;
        bool digit_seen = false, padding = false;
        for ( auto character : text )
            {
            if ( character == 0 || character == ' ' )
                {
                if ( digit_seen ) padding = true;
                continue;
                }
            if ( padding || character < '0' || character > '9' ) return false;
            auto digit = static_cast<unsigned int>( character - '0' );
            if ( article > ( std::numeric_limits<unsigned int>::max() -
                digit ) / 10 ) return false;
            article = article * 10 + digit;
            digit_seen = true;
            }
        return digit_seen && article != 0;
        }
    }

axio_ioctl_driver::axio_ioctl_driver(
    std::unique_ptr<axio_ioctl_transport> transport )
    : transport( std::move( transport ) )
    {
    if ( !this->transport ) throw std::invalid_argument( "axio transport" );
    }

axio_ioctl_driver::~axio_ioctl_driver()
    {
    if ( opened )
        {
        // Даже после ошибки обмена нельзя оставлять разрешённые выходы.
        hold_outputs();
        transport->close_device();
        }
    }

void axio_ioctl_driver::clear_staged_outputs()
    {
    input_fresh = false;
    for ( auto& module : modules ) module.staged = false;
    }

bool axio_ioctl_driver::hold_outputs()
    {
    clear_staged_outputs();
    outputs_held = true;
    return opened &&
        transport->control( axio_ioctl::SET_SYSFAIL, nullptr ) == 0;
    }

bool axio_ioctl_driver::set_fail_state()
    {
    if ( !opened ) return true;
    if ( hold_outputs() ) return true;
    initialized = false;
    diagnostic.driver_error = true;
    auto code = transport->last_error();
    error_text = fmt::format( "установка SYSFAIL: {} ({})",
        std::strerror( code ), code );
    return false;
    }

bool axio_ioctl_driver::fail( const std::string& text )
    {
    error_text = text;
    initialized = false;
    if ( opened && !hold_outputs() )
        {
        auto code = transport->last_error();
        error_text += fmt::format( "; установка SYSFAIL: {} ({})",
            std::strerror( code ), code );
        }
    return false;
    }

bool axio_ioctl_driver::control( unsigned long command, void* argument )
    {
    if ( transport->control( command, argument ) == 0 ) return true;
    auto code = transport->last_error();
    diagnostic.driver_error = true;
    return fail( fmt::format( "axio ioctl 0x{:X}: {} ({})",
        command, std::strerror( code ), code ) );
    }

bool axio_ioctl_driver::read_diagnostics()
    {
    axio_ioctl::diagnostic_registers registers{};
    if ( !control( axio_ioctl::DIAGNOSTICS, &registers ) ) return false;
    // Аппаратные регистры возвращаются в Big Endian, в отличие от полей MXI ABI.
    const auto* raw = reinterpret_cast<const unsigned char*>( &registers );
    diagnostic = { word( raw ), word( raw + 2 ), word( raw + 4 ),
        false, true };
    return true;
    }

bool axio_ioctl_driver::service( uint16_t command,
    const std::vector<uint16_t>& parameters,
    std::vector<unsigned char>& response )
    {
    std::lock_guard<std::mutex> lock( service_mutex );
    std::array<unsigned char, axio_ioctl::BUFFER_SIZE> request{}, reply{};
    if ( parameters.size() > ( request.size() - 4 ) / 2 )
        {
        return fail( "запрос MXI превышает размер буфера" );
        }
    put_word( request.data(), command );
    put_word( request.data() + 2,
        static_cast<uint16_t>( parameters.size() ) );
    for ( size_t i = 0; i < parameters.size(); ++i )
        {
        put_word( request.data() + 4 + i * 2, parameters[i] );
        }
    message_id = message_id == 255 ? 1 : message_id + 1;
    axio_ioctl::message outgoing{ 0, 0, 0,
        static_cast<uint16_t>( 4 + parameters.size() * 2 ),
        message_id, 5000, request.data() };
    axio_ioctl::message incoming{ 0, 0, 0,
        static_cast<uint16_t>( reply.size() ),
        message_id, 5000, reply.data() };
    response.clear();
    if ( !control( axio_ioctl::SEND, &outgoing ) ||
        !control( axio_ioctl::RECEIVE, &incoming ) ) return false;
    if ( incoming.id != message_id || incoming.bytes < 4 ||
        incoming.bytes > reply.size() ||
        word( reply.data() ) != ( command | 0x8000 ) )
        {
        return fail( fmt::format( "некорректное подтверждение MXI 0x{:04X}",
            command ) );
        }
    auto size = static_cast<size_t>( word( reply.data() + 2 ) ) * 2 + 4;
    size_t result_offset = command == 0x0041 ? 12 : 4;
    if ( size > incoming.bytes || size < result_offset + 2 )
        {
        return fail( fmt::format( "неполный ответ MXI 0x{:04X}", command ) );
        }
    auto result = word( reply.data() + result_offset );
    if ( result )
        {
        auto additional = size >= result_offset + 4
            ? word( reply.data() + result_offset + 2 ) : 0;
        return fail( fmt::format(
            "MXI 0x{:04X}: код=0x{:04X}, addInfo=0x{:04X}",
            command, result, additional ) );
        }
    response.assign( reply.begin(), reply.begin() + size );
    return true;
    }

bool axio_ioctl_driver::pdi_read( uint16_t slot, uint16_t index,
    std::vector<unsigned char>& data )
    {
    std::vector<unsigned char> response;
    data.clear();
    if ( !service( 0x0041, { slot, 0, index, 0 }, response ) )
        {
        error_text += fmt::format( "; PDI 0x{:04X}, слот {}", index, slot );
        return false;
        }
    auto bytes = response[11];
    // Подтверждение повторяет слот, подслот, индекс и подиндекс запроса.
    if ( word( response.data() + 4 ) != slot ||
        word( response.data() + 6 ) != 0 ||
        word( response.data() + 8 ) != index || response[10] != 0 ||
        14u + bytes > response.size() )
        {
        return fail( fmt::format( "некорректный ответ PDI 0x{:04X}, слот {}",
            index, slot ) );
        }
    data.assign( response.begin() + 14, response.begin() + 14 + bytes );
    return true;
    }

bool axio_ioctl_driver::scan_modules()
    {
    std::vector<unsigned char> configuration, record, article_text;
    // Complete_Read_Configuration, DEV_TYPE | DEV_ID | DEV_LEN.
    if ( !service( 0x030B, { 7 }, configuration ) ) return false;
    if ( configuration.size() < 16 )
        {
        return fail( "неполное описание конфигурации мастера" );
        }
    auto count = word( configuration.data() + 14 );
    if ( count == 0 || count > 63 ||
        configuration.size() != 16u + count * 10u )
        {
        return fail( "некорректное число модулей в конфигурации мастера" );
        }
    for ( uint16_t slot = 1; slot <= count; ++slot )
        {
        if ( !pdi_read( slot, 0x0037, record ) ) return false;
        unsigned int bytes = 0, article = 0;
        if ( record.size() != 8 || !decode_local_bus_process_data_length(
            record.data(), record.size(), bytes ) )
            {
            return fail( fmt::format( "неверная запись PDI 0x0037, слот {}",
                slot ) );
            }
        const auto* configured = configuration.data() + 16 + ( slot - 1 ) * 10;
        auto padded = ( bytes + 1u ) & ~1u;
        auto length = word( configured + 8 );
        if ( dword( configured ) != dword( record.data() ) ||
            dword( configured + 4 ) != dword( record.data() + 4 ) ||
            ( length != bytes && length != padded ) )
            {
            return fail( fmt::format( "состав шины изменён, слот {}", slot ) );
            }
        if ( padded > inputs.size() - process_bytes )
            {
            return fail( "образ процессных данных превышает 1024 байта" );
            }
        if ( !pdi_read( slot, 0x000A, article_text ) ) return false;
        if ( !parse_article( article_text, article ) )
            {
            return fail( fmt::format( "неверный артикул PDI 0x000A, слот {}",
                slot ) );
            }
        modules.push_back( { { article, bytes, false }, process_bytes, false } );
        process_bytes += padded;
        }
    return true;
    }

bool axio_ioctl_driver::initialize()
    {
    if ( initialized ) return true;
    if ( !opened )
        {
        if ( !transport->open_device() )
            {
            auto code = transport->last_error();
            error_text = fmt::format( "/dev/axio_xfer0: {} ({})",
                std::strerror( code ), code );
            return false;
            }
        opened = true;
        }
    if ( !control( axio_ioctl::SET_SYSFAIL, nullptr ) ||
        !control( axio_ioctl::RESET, nullptr ) ||
        !control( axio_ioctl::SET_SYSFAIL, nullptr ) ||
        !read_diagnostics() ) return false;
    outputs_held = true;
    clear_staged_outputs();
    modules.clear();
    process_bytes = 0;
    inputs.fill( 0 );
    outputs.fill( 0 );
    std::vector<unsigned char> response;
    bool configure = !( diagnostic.status & 0x0040 );
    bool start = configure || !( diagnostic.status & 0x0020 );
    if ( configure )
        {
        // Последовательность DIRECT/EXPLICIT из libAxiobus 11.2.
        if ( !service( 0x1703, {}, response ) ||
            !service( 0x0710, { 1 }, response ) ||
            !service( 0x0728, { 0x3000, 1, 0x0021, 0 }, response ) )
            {
            return false;
            }
        }
    if ( !scan_modules() ) return false;
    axio_ioctl::process_data initial{ outputs.data(), 0,
        static_cast<uint32_t>( process_bytes ), 0, 0 };
    // Шина запускается под SYSFAIL; выходы разрешает первая проверенная запись.
    if ( !control( axio_ioctl::SET_SYSFAIL, nullptr ) ||
        !control( axio_ioctl::WRITE_PD, &initial ) ||
        ( start && !service( 0x0701, { 1 }, response ) ) ||
        !control( axio_ioctl::SET_SYSFAIL, nullptr ) ) return false;
    initialized = true;
    error_text.clear();
    return true;
    }

bool axio_ioctl_driver::ready()
    {
    if ( !initialized ) return false;
    if ( !read_diagnostics() ) return false;
    // Ошибки I/O остаются уведомлениями; FORCE и PARAM_REQUIRED блокируют запись.
    if ( ( diagnostic.status & 0x00E0 ) != 0x00E0 ||
        ( diagnostic.status & 0x1D0C ) != 0 )
        {
        return fail( describe_local_bus_diagnostics( diagnostic ) );
        }
    error_text = describe_local_bus_diagnostics( diagnostic );
    return true;
    }

const char* axio_ioctl_driver::get_error_text() const
    {
    return error_text.c_str();
    }

local_bus_driver::diagnostics axio_ioctl_driver::get_diagnostics() const
    {
    return diagnostic;
    }

size_t axio_ioctl_driver::module_count()
    {
    return modules.size();
    }

local_bus_driver::module_info axio_ioctl_driver::module( size_t index )
    {
    return modules.at( index ).info;
    }

bool axio_ioctl_driver::read_inputs()
    {
    if ( !ready() ) return false;
    clear_staged_outputs();
    axio_ioctl::process_data data{ inputs.data(), 0,
        static_cast<uint32_t>( process_bytes ), 0, 0 };
    if ( !control( axio_ioctl::READ_PD, &data ) ) return false;
    input_fresh = true;
    return true;
    }

bool axio_ioctl_driver::write_outputs()
    {
    if ( !initialized || !input_fresh )
        {
        return fail( "запись Axiobus требует успешного чтения входов" );
        }
    for ( const auto& module : modules )
        {
        if ( module.info.bytes && !module.staged )
            {
            return fail( "выходной образ Axiobus заполнен не полностью" );
            }
        }
    if ( !ready() ) return false;
    axio_ioctl::process_data data{ outputs.data(), 0,
        static_cast<uint32_t>( process_bytes ), 0, 0 };
    if ( !control( axio_ioctl::WRITE_PD, &data ) ) return false;
    if ( outputs_held && !control( axio_ioctl::CLEAR_SYSFAIL, nullptr ) )
        {
        return false;
        }
    outputs_held = false;
    clear_staged_outputs();
    return true;
    }

void axio_ioctl_driver::read_module( size_t index, unsigned char* data,
    unsigned int bytes )
    {
    const auto& record = modules.at( index );
    if ( !input_fresh || bytes > record.info.bytes || ( bytes && !data ) )
        {
        throw std::runtime_error( "некорректное чтение данных модуля Axiobus" );
        }
    auto data_offset = local_bus_module_byte_offset(
        record.info.article, record.info.bytes );
    if ( bytes ) std::memcpy( data,
        inputs.data() + record.offset + data_offset, bytes );
    }

void axio_ioctl_driver::write_module( size_t index, unsigned char* data,
    unsigned int bytes )
    {
    auto& record = modules.at( index );
    if ( !input_fresh || bytes != record.info.bytes || ( bytes && !data ) )
        {
        throw std::runtime_error( "некорректная запись данных модуля Axiobus" );
        }
    auto data_offset = local_bus_module_byte_offset(
        record.info.article, record.info.bytes );
    if ( bytes ) std::memcpy(
        outputs.data() + record.offset + data_offset, data, bytes );
    record.staged = true;
    }

#ifdef PTUSA_AXIO_IOCTL
#include <fcntl.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace
    {
    class linux_axio_transport final : public axio_ioctl_transport
        {
        public:
            bool open_device() override
                {
                fd = open( "/dev/axio_xfer0", O_RDWR | O_SYNC | O_CLOEXEC );
                if ( fd < 0 )
                    {
                    error = errno;
                    return false;
                    }
                // Блокировка защищает от второго экземпляра нашего адаптера.
                if ( flock( fd, LOCK_EX | LOCK_NB ) != 0 )
                    {
                    error = errno;
                    close_device();
                    return false;
                    }
                return true;
                }

            void close_device() override
                {
                if ( fd >= 0 ) close( fd );
                fd = -1;
                }

            int control( unsigned long command, void* argument ) override
                {
                auto result = ioctl( fd, command, argument );
                error = result < 0 ? errno : 0;
                return result;
                }

            int last_error() const override { return error; }

        private:
            int fd = -1, error = 0;
        };
    }
#endif

std::unique_ptr<local_bus_driver> make_axio_ioctl_driver()
    {
#ifdef PTUSA_AXIO_IOCTL
    return std::make_unique<axio_ioctl_driver>(
        std::make_unique<linux_axio_transport>() );
#else
    return nullptr;
#endif
    }
