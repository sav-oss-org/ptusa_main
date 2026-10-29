#include "includes.h"
#include "axiobus_process_data.h"
#include "axioline_local_bus.h"
#include <array>

namespace
    {
    class axl_module_with_zero_length
        {
        public:
            axl_module_with_zero_length( unsigned char* input,
                unsigned char* output )
                : m_inOffset( reinterpret_cast<uintptr_t>( input ) ),
                  m_outOffset( reinterpret_cast<uintptr_t>( output ) ) {}

            unsigned int getProcessDataLength() const { return 0; }

        protected:
            uintptr_t m_inOffset;
            uintptr_t m_outOffset;
        };

    using process_data = axiobus_process_data<axl_module_with_zero_length>;
    }

TEST( axiobus_process_data, copies_input_despite_zero_library_length )
    {
    unsigned char input[] = { 0x80, 0x02 };
    axl_module_with_zero_length module( input, nullptr );
    ASSERT_EQ( module.getProcessDataLength(), 0 );
    std::array<unsigned char, 4> data{ 0xCC, 0, 0, 0xDD };
    process_data::read( module, data.data() + 1, 2, 2 );
    EXPECT_EQ( data, ( std::array<unsigned char, 4>{
        0xCC, 0x80, 0x02, 0xDD } ) );
    }

TEST( axiobus_process_data, copies_in2m1_output_despite_zero_library_length )
    {
    std::array<unsigned char, 4> output{ 0xCC, 0, 0, 0xDD };
    axl_module_with_zero_length module( nullptr, output.data() + 1 );
    ASSERT_EQ( module.getProcessDataLength(), 0 );
    unsigned char command[] = { 0x02, 0x00 };
    process_data::write( module, command, 2, 2 );
    EXPECT_EQ( output, ( std::array<unsigned char, 4>{
        0xCC, 0x02, 0x00, 0xDD } ) );
    }

TEST( axiobus_process_data, di8do8_uses_second_byte_of_hardware_word )
    {
    for ( auto article : { 2701916u, 2702071u } )
        {
        // Реальный образ Axiobus: каждый модуль занимает целое слово,
        // но PDI у DI8/DO8 сообщает только один полезный байт.
        unsigned char input[]{ 0xEE, 0xA5 };
        std::array<unsigned char, 4> output{ 0xCC, 0, 0, 0xDD };
        axl_module_with_zero_length module( input, output.data() + 1 );
        auto offset = local_bus_module_byte_offset( article, 1 );
        unsigned char value = 0;
        process_data::read( module, &value, 1, 1, offset );
        EXPECT_EQ( value, 0xA5 );
        value = 0x5F;
        process_data::write( module, &value, 1, 1, offset );
        EXPECT_EQ( output, ( std::array<unsigned char, 4>{
            0xCC, 0, 0x5F, 0xDD } ) );
        }
    }

TEST( axiobus_process_data, invalid_hardware_offset_preserves_buffers )
    {
    unsigned char input[]{ 0x11, 0x22 };
    std::array<unsigned char, 2> output{ 0x33, 0x44 };
    axl_module_with_zero_length module( input, output.data() );
    unsigned char value = 0xA5;
    EXPECT_THROW( process_data::write( module, &value, 1, 1, 2 ),
        std::runtime_error );
    EXPECT_THROW( process_data::read( module, &value, 1, 1, 2 ),
        std::runtime_error );
    EXPECT_EQ( output, ( std::array<unsigned char, 2>{ 0x33, 0x44 } ) );
    EXPECT_EQ( value, 0xA5 );
    axl_module_with_zero_length missing( nullptr, nullptr );
    EXPECT_THROW( process_data::write( missing, &value, 1, 1, 1 ),
        std::runtime_error );
    EXPECT_THROW( process_data::read( missing, &value, 1, 1, 1 ),
        std::runtime_error );
    }

TEST( axiobus_process_data, rejects_invalid_length_without_changing_buffers )
    {
    std::array<unsigned char, 3> data{ 0x11, 0x22, 0x33 };
    std::array<unsigned char, 3> output{ 0xAA, 0xBB, 0xCC };
    axl_module_with_zero_length module( output.data(), output.data() );
    auto original_data = data;
    auto original_output = output;
    EXPECT_THROW( process_data::write( module, data.data(), 3, 2 ),
        std::runtime_error );
    EXPECT_THROW( process_data::read( module, data.data(), 3, 2 ),
        std::runtime_error );
    EXPECT_THROW( process_data::write( module, data.data(), 2, 1025 ),
        std::runtime_error );
    EXPECT_EQ( data, original_data );
    EXPECT_EQ( output, original_output );
    }

TEST( axiobus_process_data, rejects_missing_buffer_and_accepts_empty_module )
    {
    axl_module_with_zero_length module( nullptr, nullptr );
    unsigned char data[2]{};
    EXPECT_THROW( process_data::read( module, data, 2, 2 ),
        std::runtime_error );
    EXPECT_THROW( process_data::write( module, data, 2, 2 ),
        std::runtime_error );
    EXPECT_NO_THROW( process_data::read( module, nullptr, 0, 0 ) );
    EXPECT_NO_THROW( process_data::write( module, nullptr, 0, 0 ) );
    }
