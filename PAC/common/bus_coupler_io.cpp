#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_PLCNEXT )
#error You must define OS!
#endif

#include "lua_manager.h"

#include "dtime.h"

#ifdef WIN_OS
#include "uni_bus_coupler_io.h"
#endif

#if defined LINUX_OS && defined PAC_PC
#include "uni_bus_coupler_io.h"
#endif

#if defined LINUX_OS && defined PAC_PLCNEXT
#include "uni_bus_coupler_io.h"
#endif

#include "log.h"

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

auto_smart_ptr < io_manager > io_manager::instance;
int io_device::last_err = 0;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int io_device::get_DO( u_int index ) const
    {
    if ( index < DO_channels.count &&
        DO_channels.char_write_values &&
        DO_channels.char_write_values[ index ] )
        {
        return *DO_channels.char_write_values[ index ];
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_DO(...) - error! " );
        printf( "index = %d, count = %u, char_write_values = %p",
            index, DO_channels.count, DO_channels.char_write_values );
        if ( DO_channels.char_write_values )
            {
            printf( ", char_write_values[ index ]=%p",
                DO_channels.char_write_values[ index ] );
            }
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int io_device::set_DO( u_int index, char value )
    {
    if ( index < DO_channels.count &&
        DO_channels.char_write_values &&
        DO_channels.char_write_values[ index ] )
        {
        *DO_channels.char_write_values[ index ] = value;
        return 0;
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->set_DO(...) - error! %d, %d, %p",
            index, DO_channels.count, DO_channels.char_write_values );
        if ( DO_channels.char_write_values )
            {
            printf( ", char_write_values[ index ]=%p",
                DO_channels.char_write_values[ index ] );
            }
        printf( "\n" );
        }

    return 1;
    }
//-----------------------------------------------------------------------------
int io_device::get_DI( u_int index ) const
    {
    if ( index < DI_channels.count &&
        DI_channels.char_read_values &&
        DI_channels.char_read_values[ index ] )
        {
        return *DI_channels.char_read_values[ index ];
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_DI(...) - error! " );
        printf( "index = %d, DI_channels.count = %d, "
            "DI_channels.char_read_values = %p",
            index, DI_channels.count, DI_channels.char_read_values );
        if ( DI_channels.char_read_values )
            {
            printf( ", DI_channels.char_read_values[ index ]=%p",
                DI_channels.char_read_values[ index ] );
            }
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
float io_device::get_AO( u_int index, float min_value, float max_value ) const
    {
    if ( index < AO_channels.count &&
        AO_channels.int_write_values &&
        AO_channels.int_write_values[ index ] )
        {
        float val = ( float ) *AO_channels.int_write_values[ index ];

        u_int table_n = AO_channels.tables[ index ];
        u_int offset = AO_channels.offsets[ index ];
        u_int module_type = G_IO_MANAGER()->get_node( table_n )->AO_types[ offset ];

        switch ( module_type )
            {
            // Выход модуля 554.
            // Три наименее значащих бита не учитываются.
            //    -----------------------------------------------------------------------
            //    Output          Output          Binary value
            //    current 0-20	  current 4-20                            Hex.      Dec.
            //    -----------------------------------------------------------------------
            //    20              20              0111 1111 1111 1111     7F FF     32767
            //    10              12              0100 0000 0000 0xxx     40 00     16384
            //    5               8               0010 0000 0000 0xxx     20 00      8192
            //    2.5             6               0001 0000 0000 0xxx     10 00      4096
            //    0.156           4.125           0000 0001 0000 0xxx     01 00       256
            //    0.01            4.0078          0000 0000 0001 0xxx     00 10        16
            //    0.005           4.0039          0000 0000 0000 1xxx     00 08         8
            //    0               4               0000 0000 0000 0111     00 07         7
            //    0               4               0000 0000 0000 0000     00 00         0
            //
            case 554:
            case 555:
                if ( 0 == min_value && 0 == max_value )
                    {
                    if ( val < 7 )
                        {
                        val = 0;
                        }
                    else
                        {
                        val = 4 + val / 2047.5f;
                        }
                    }
                else
                    {
                    if ( val < 7 )
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 2047.5f;
                        }
                    val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
                    }

                return val;

                //  Output data     4 mA ... 20 mA
                //-----------------------------------------------------
                //  hex             dec             mA
                //-----------------------------------------------------
                //  7FFF...7F01                     21.3397
                //  7F00            32512           21.3397
                //  7530            30000           20
                //  3A98            15000           12
                //  1               1               4.0005333
                //  0               0               4
                //  FFFF            -1              4
                //  C568            -15000          4
                //  8AD0            -30000          4
                //  8100            -32512          4
                //  80FF...8000*                    Hold last value
                //  8001            Overrange       21.3397
                //  8080            Underrange      Hold last value
                //
                //  * without 8001, 8080

            case 2688527:   //AXL F AO4 1H
            case 2702072:   //AXL F AI2 AO2 1H
                if (0 == min_value && 0 == max_value)
                    {
                    if (val < 0)
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1875.0f;
                        }
                    }
                else
                    {
                    if (val < 0)
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1875.0f;
                        }
                    val = min_value + (val - 4) * (max_value - min_value) / 16;
                    }

            return val;


                //  Output data                 4 mA ... 20 mA
                //-----------------------------------------------------
                //  hex             dec                         mА
                //-----------------------------------------------------
                //  8001            Output range overrange      + 21.339
                //  7FFF ... 43BC   32767 ... 17340             + 21.339
                //  43BB            17339                       + 21.339
                //  3E80            16000                       + 20.0
                //  2710            10000                       + 14.0
                //  1770            6000                        + 10.0
                //  1388            5000                        + 9.0
                //  03E8            1000                        + 5.0
                //  0001            1                           + 4.001
                //  0000            0                           + 4.0
                //  FFFF ... 8100   -1 ... -32512               + 4.0
                //  80FF ... 8000*  -32513 ... -32767           Hold last value
                //  8080            Output range underrange     Hold last value
                //  * without 8001, 8080
            case 1088123:   //AXL SE AO4 I 4-20
                if ( 0 == min_value && 0 == max_value )
                    {
                    if ( val < 0 )
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1000.0f;
                        }
                    }
                else
                    {
                    if ( val < 0 )
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1000.0f;
                        }
                    val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
                    }

                return val;

            default:
                return val;
            }
        }

    if ( G_DEBUG )
        {
        // LCOV_EXCL_START
        fmt::print( "'{}' (", name );
        io_device::print();
        fmt::print( ") io_device->get_AO(...) error: " );
        fmt::print( "index = {}, AO_channels.count = {}, "
            "AO_channels.int_write_values = {}",
            index, AO_channels.count,
            static_cast<void*>( AO_channels.int_write_values ) );
        // LCOV_EXCL_STOP
        if ( index < AO_channels.count && AO_channels.int_write_values )
            {
            // LCOV_EXCL_START
            fmt::print( ", AO_channels.int_write_values[ index ]={}",
                static_cast<void*>( AO_channels.int_write_values[ index ] ) );
            // LCOV_EXCL_STOP
            }
        fmt::print( "\n" );
        }

    return 0.0f;
    }
//-----------------------------------------------------------------------------
int io_device::set_AO( u_int index, float value, float min_value,
                        float max_value )
    {
    if ( index < AO_channels.count &&
        AO_channels.int_write_values &&
        AO_channels.int_write_values[ index ] )
        {
        auto table_n = AO_channels.tables[ index ];
        auto offset = AO_channels.offsets[ index ];

        switch ( auto module_type =
            G_IO_MANAGER()->get_node( table_n )->AO_types[ offset ]; module_type )
            {
            case 554:
            case 555:
                if ( 0 != min_value || 0 != max_value )
                    {
                    value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                    }
                if ( value < 4 ) value = 4;
                if ( value > 20 ) value = 20;
                value = 2047.5f * ( value - 4 );
                break;

            case 2688527:   //AXL F AO4 1H
            case 2702072:   //AXL F AI2 AO2 1H
                if ( 0 != min_value || 0 != max_value )
                    {
                    value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                    }
                if ( value < 4 ) value = 4;
                if ( value > 20 ) value = 20;
                value = 1875.0f * ( value - 4 );
                break;

            case 1088123:   //AXL SE AO4 I 4-20
                if ( 0 != min_value || 0 != max_value )
                    {
                    value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                    }
                if ( value < 4 ) value = 4;
                if ( value > 20 ) value = 20;
                value = 1000.0f * ( value - 4 );
                break;
            }

        *AO_channels.int_write_values[ index ] = ( u_int ) value;
        return 0;
        }

    if ( G_DEBUG )
        {
        // LCOV_EXCL_START
        fmt::print( "'{}' (", name );
        io_device::print();
        fmt::print( ") io_device->set_AO(...) error: " );
        fmt::print( "index = {}, AO_channels.count = {}, "
            "AO_channels.int_write_values = {}",
            index, AO_channels.count,
            static_cast<void*>( AO_channels.int_write_values ) );
        // LCOV_EXCL_STOP
        if ( index < AO_channels.count && AO_channels.int_write_values )
            {
            // LCOV_EXCL_START
            fmt::print( ", AO_channels.int_write_values[ index ]={}",
                static_cast<void*>( AO_channels.int_write_values[ index ] ) );
            // LCOV_EXCL_STOP
            }
        fmt::print( "\n" );
        }

    return 1;
    }
//-----------------------------------------------------------------------------
float io_device::get_AI( u_int index, float min_value, float max_value, int& err ) const
    {
    err = static_cast< int >( ERRORS::NO_ERR );
    if ( index < AI_channels.count &&
        AI_channels.int_read_values &&
        AI_channels.int_read_values[ index ] )
        {
        float val = ( float ) *AI_channels.int_read_values[ index ];

        u_int table_n = AI_channels.tables[ index ];
        u_int offset = AI_channels.offsets[ index ];
        u_int module_type = G_IO_MANAGER()->get_node( table_n )->AI_types[ offset ];

        switch ( module_type )
            {
            // Выход модуля 461.
            // Выход модуля 450, Pt100 Setting (IEC 751), Standard Format.
            //   -------------------------------------------------------------------------
            //   Temperature  Voltage     Voltage     Binary value
            //   °C           (Ohm)       (Ohm)                               Hex.     Dec.
            //   -------------------------------------------------------------------------
            //                >400
            //   850          390.481     1384,998    0010 0001 0011 0100     2134     8500
            //   100          138.506     1099,299    0000 0011 1110 1000     03E8     1000
            //   25.5         109.929     1000,391    0000 0000 1111 1111     00FF      255
            //   0.1          100.039     1000        0000 0000 0000 0001     0001        1
            //   0            100         999,619     0000 0000 0000 0000     0000        0
            //  -0.1          99.970      901,929     1111 1111 1111 1111     FFFF       -1
            //  -25.5         90.389      184,936     1111 1111 0000 0001     FF01     -255
            //  -200          18.192                  1111 1000 0011 0000     F830    -2000
            //                <18                     1000 0000 0000 0000     8000   -32767
            //
            case 461:
            case 450:
                if ( val < -2000 )  // Underrange.
                    {
                    err = static_cast<int>( ERRORS::UNDER_RANGE );
                    return -1000.f;
                    }
                if ( val > 8500 )   // Overrange.
                    {
                    err = static_cast<int>( ERRORS::OVER_RANGE );
                    return -1000.f;
                    }

                val *= 0.1f;    // -200..-0.1..0..850 °C
                return val;

            // Input       Numerical value                            Status- LED
            // current                                                byte    Error
            // 4-20 mA                                                hex.    AI 1,2
            //             binary
            //             Measured value      *)XFÜ   hex.    dec.
            //------------------------------------------------------------------
            // <0          not possible (Reverse voltage protection)
            // <4- Δ**)    ’0000.0000.0000.0   011’    0x0003  3      0x41    on
            // <4          ’0000.0000.0000.0   000’    0x0000  0      0x00    off
            // 4           ’0000.0000.0000.0   000’    0x0000  0      0x00    off
            // 6           ’0001.0000.0000.0   000’    0x1000  4096   0x00    off
            // 8           ’0010.0000.0000.0   000’    0x2000  8192   0x00    off
            // 10          ’0011.0000.0000.0   000’    0x3000  12288  0x00    off
            // 12          ’0100.0000.0000.0   000’    0x4000  16384  0x00    off
            // 14          ’0101.0000.0000.0   000’    0x5000  20480  0x00    off
            // 16          ’0110.0000.0000.0   000’    0x6000  24576  0x00    off
            // 18          ’0111.0000.0000.0   000’    0x7000  28672  0x00    off
            // 20          ’0111.1111.1111.1   000’    0x7FF8  32760  0x00    off
            // >20         ’0111.1111.1111.1   001’    0x7FF9  32761  0x42    off
            // >20+ Δ**)   ’0111.1111.1111.1   001’    0x7FF9  32761  0x42    on
            // ------------------------------------------------------------------
            // *) status bits : X = not used, F = short - circuit, Ü = oversize
            // **) Δ = 0,1 ... 2,0 mA
            case 466:
            case 496:
                if ( 3 == val )     // Underrange.
                    {
                    err = static_cast<int>( ERRORS::UNDER_RANGE );
                    return -1.f;
                    }

                if ( 0 == min_value && 0 == max_value )
                    {
                    val = 4 + val / 2047.5f;    // 2047.5f = 32760 / ( 20 - 4 )
                    }
                else
                    {
                    val = 4 + val / 2047.5f;
                    val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
                    }
                return val;

                //Тензорезистор
                // Process values of module 750-491
                //Signal           Numerical value
                //voltage UD       binary
                //
                //ca.-15.5000   '0111.1111.1111.1111' 0x7FFF 32767  0x41 on
                //ca.-15.5000   '0000.0000.0000.0000' 0x0000 0      0x00 off
                //-15.0000      '1000.1010.1101.0000' 0x8AD0 -30000 0x00 off
                //-10.0000      '1011.0001.1110.0000' 0xB1E0 -20000 0x00 off
                //-5.0000       '1101.1000.1111.0000' 0xD8F0 -10000 0x00 off
                //-0.0005       '1111.1111.1111.1111' 0xFFFF -1     0x00 off
                //0.0000        '0000.0000.0000.0000' 0x0000 0      0x00 off
                //0.0005        '0000.0000.0000.0001' 0x0001 1      0x00 off
                //5.0000        '0010.0111.0001.0000' 0x2710 10000  0x00 off
                //10.0000       '0100.1110.0010.0000' 0x4E20 20000  0x00 off
                //15.0000       '0111.0101.0011.0000' 0x7530 30000  0x00 off
                //>ca.15.5000   '0111.1111.1111.1111' 0x7FFF 32767  0x41 on
                //Broken wire   '0111.1111.1111.1111' 0x7FFF 32767  0x41 on
            case 491:
                if ( val >= -30000 && val <= 30000 )
                    {
                    val *= 0.0005f;
                    return val;
                    }

                err = static_cast<int>( ERRORS::OUT_OF_RANGE );
                return -1000;

            case 1088106:   //AXL SE RTD4 PT100.
            case 2688556:   //RTD4 1H.
                if ( val < -32000 )
                    {
                    err = static_cast<int>( ERRORS::UNDER_RANGE );
                    return -1000.f;
                    }
                val *= 0.1f;
                return val;

            case 2688491:   //AXL F AI4 I 1H
            case 2702072:   //AXL F AI2 AO2 1H
				if (val < -32000)
					{
                    err = static_cast<int>( ERRORS::UNDER_RANGE );
					return -1;
					}

				if (0 == min_value && 0 == max_value)
					{
					val = 4 + val / 1875.0f;
					}
				else
					{
					val = 4 + val / 1875.0f;
					val = min_value + (val - 4) * (max_value - min_value) / 16;
					}
				return val;

            case 1088062:   //AXL SE AI4 I 4-20
                if ( val < -32000 )
                    {
                    err = static_cast<int>( ERRORS::UNDER_RANGE );
                    return -1;
                    }

                if ( 0 == min_value && 0 == max_value )
                    {
                    val = 4 + val / 1000.0f;
                    }
                else
                    {
                    val = 4 + val / 1000.0f;
                    val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
                    }
                return val;

            default:
                return val;
            }
        }

    if ( G_DEBUG )
        {
        // LCOV_EXCL_START
        fmt::print( "'{}' (", name );
        io_device::print();
        fmt::print( ") io_device->get_AI(...) error: " );
        fmt::print( "index = {}, AI_channels.count = {}, "
            "AI_channels.int_read_values = {}",
            index, AI_channels.count,
            static_cast<void*>( AI_channels.int_read_values ) );
        // LCOV_EXCL_STOP
        if ( index < AI_channels.count && AI_channels.int_read_values )
            {
            // LCOV_EXCL_START
            fmt::print( ", AI_channels.int_read_values[ index ]={}",
                static_cast<void*>( AI_channels.int_read_values[ index ] ) );
            // LCOV_EXCL_STOP
            }
        fmt::print( "\n" );
        }

    err = static_cast<int>( ERRORS::BAD_IO_DATA );
    return 0.0f;
    }
//-----------------------------------------------------------------------------
int_2* io_device::get_AI_data( u_int index ) const
    {
    if ( index < AI_channels.count && AI_channels.int_read_values )
        {
        return AI_channels.int_read_values[ index ];
        }

    if ( G_DEBUG )
        {
        // LCOV_EXCL_START
        fmt::print( "'{}' (", name );
        io_device::print();
        fmt::print( ") io_device->get_AI_data(...) error: " );
        fmt::println( "index = {}, AI_channels.count = {}, "
            "AI_channels.int_read_values = {}",
            index, AI_channels.count,
            static_cast<void*>( AI_channels.int_read_values ) );
        // LCOV_EXCL_STOP
        }

    return nullptr;
    }
//-----------------------------------------------------------------------------
io_device::IOLINKSTATE io_device::get_AI_IOLINK_state( u_int index ) const
    {
    if ( vendor == PHOENIX )
        {
        if ( index < AI_channels.count && AI_channels.int_module_read_values[ index ] )
            {
            u_int module_states = *( (u_int*)( AI_channels.int_module_read_values[ index ] ) );
            int_2 logical_port = AI_channels.logical_ports[ index ];
            if ( logical_port > 0 && logical_port <= 8 )
                {
                bool iolinkconnected = module_states & ( 1 << ( logical_port - 1 ) );
                bool iolinkdatavalid = module_states & ( 1 << ( logical_port + 7 ) );
                if ( !iolinkconnected ) return IOLINKSTATE::NOTCONNECTED;
                if ( !iolinkdatavalid ) return IOLINKSTATE::DEVICEERROR;
                }
            }
        }
    return IOLINKSTATE::OK;
    }

io_device::IOLINKSTATE io_device::get_AO_IOLINK_state( u_int index ) const
    {
    if ( vendor == PHOENIX )
        {
        if ( index < AO_channels.count && AO_channels.int_module_read_values[ index ] )
            {
            u_int module_states = *( (u_int*)( AO_channels.int_module_read_values[ index ] ) );
            int_2 logical_port = AO_channels.logical_ports[ index ];
            if ( logical_port > 0 && logical_port <= 8 )
                {
                bool iolinkconnected = module_states & ( 1 << ( logical_port - 1 ) );
                bool iolinkdatavalid = module_states & ( 1 << ( logical_port + 7 ) );
                if ( !iolinkconnected ) return IOLINKSTATE::NOTCONNECTED;
                if ( !iolinkdatavalid ) return IOLINKSTATE::DEVICEERROR;
                }
            }
        }
    return IOLINKSTATE::OK;
    }

//-----------------------------------------------------------------------------
int_2* io_device::get_AO_write_data( u_int index )
    {
    if ( index < AO_channels.count && AO_channels.int_write_values )
        {
        return AO_channels.int_write_values[ index ];
        }

    if ( G_DEBUG )
        {
        // LCOV_EXCL_START
        fmt::print( "'{}' (", name );
        io_device::print();
        fmt::print( ") io_device->get_AO_write_data(...) error: " );
        fmt::println( "index = {}, AO_channels.count = {}, "
            "AO_channels.int_write_values = {}",
            index, AO_channels.count,
            static_cast<void*>( AO_channels.int_write_values ) );
        // LCOV_EXCL_STOP
        }

    return nullptr;
    }
//-----------------------------------------------------------------------------
const int_2* io_device::get_AO_write_data( u_int index ) const
    {
    if ( index < AO_channels.count && AO_channels.int_write_values )
        {
        return AO_channels.int_write_values[ index ];
        }

    if ( G_DEBUG )
        {
        // LCOV_EXCL_START
        fmt::print( "'{}' (", name );
        io_device::print();
        fmt::print( ") io_device->get_AO_write_data(...) error: " );
        fmt::println( "index = {}, AO_channels.count = {}, "
            "AO_channels.int_write_values = {}",
            index, AO_channels.count,
            static_cast<void*>( AO_channels.int_write_values ) );
        // LCOV_EXCL_STOP
        }

    return nullptr;
    }
//-----------------------------------------------------------------------------
const int_2* io_device::get_AO_read_data( u_int index ) const
    {
    if ( index < AO_channels.count && AO_channels.int_read_values )
        {
        return AO_channels.int_read_values[ index ];
        }

    if ( G_DEBUG )
        {
        // LCOV_EXCL_START
        fmt::print( "'{}' (", name );
        io_device::print();
        fmt::print( ") io_device->get_AO_read_data(...) error: " );
        fmt::println( "index = {}, AO_channels.count = {}, "
            "AO_channels.int_read_values = {}",
            index, AO_channels.count,
            static_cast<void*>( AO_channels.int_read_values ) );
        // LCOV_EXCL_STOP
        }

    return nullptr;
    }
//-----------------------------------------------------------------------------
void io_device::print() const
    {
    if ( !DI_channels.count && !DO_channels.count &&
        !AI_channels.count && !AO_channels.count )
        {
        fmt::print( "I/O: 0" );
        return;
        }

    DI_channels.print();
    DO_channels.print();
    AI_channels.print();
    AO_channels.print();
    }
//-----------------------------------------------------------------------------
io_device::io_device( const char* name ) : name( name ), vendor( PHOENIX ),
    DI_channels( IO_channels::CT_DI ),
    DO_channels( IO_channels::CT_DO ),
    AI_channels( IO_channels::CT_AI ),
    AO_channels( IO_channels::CT_AO )
    {
    }
//-----------------------------------------------------------------------------
io_device::~io_device()
    {
    }
//-----------------------------------------------------------------------------
void io_device::init( int DO_count, int DI_count, int AO_count,
                       int AI_count )
    {
    if ( DO_count > 0 )
        {
        DO_channels.init( DO_count );
        }
    if ( DI_count > 0 )
        {
        DI_channels.init( DI_count );
        }
    if ( AO_count > 0 )
        {
        AO_channels.init( AO_count );
        }
    if ( AI_count > 0 )
        {
        AI_channels.init( AI_count );
        }
    }
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
void io_device::init_and_alloc( int DO_count, int DI_count, int AO_count,
    int AI_count )
    {
    init( DO_count, DI_count, AO_count, AI_count );

    for ( auto i = 0; i < DO_count; i++ )
        {
        DO_channels.char_read_values[ i ] = new u_char{ 0 };
        DO_channels.char_write_values[ i ] = new u_char{ 0 };
        }
    for ( auto i = 0; i < DI_count; i++ )
        {
        DI_channels.char_read_values[ i ] = new u_char{ 0 };
        }
    for ( auto i = 0; i < AO_count; i++ )
        {
        AO_channels.int_read_values[ i ] = new int_2 { 0 };
        AO_channels.int_module_read_values[ i ] = new int_2 { 0 };
        AO_channels.int_write_values[ i ] = new int_2 { 0 };
        }
    for ( auto i = 0; i < AI_count; i++ )
        {
        AI_channels.int_read_values[ i ] = new int_2 { 0 };
        AI_channels.int_module_read_values[ i ] = new int_2 { 0 };
        }
    }
#endif
//-----------------------------------------------------------------------------
io_device::VENDOR io_device::get_io_vendor() const
    {
    return vendor;
    }
//-----------------------------------------------------------------------------
void io_device::set_io_vendor( VENDOR vendor )
    {
    this->vendor = vendor;
    }
//-----------------------------------------------------------------------------
int io_device::check_output_DO_node_PP_state( u_int index ) const
    {
    // If no channels configured or tables not initialized, skip node check.
    if ( index >= DO_channels.count ||
        !DO_channels.tables ||
        !DO_channels.char_write_values ||
        !DO_channels.char_write_values[ index ] )
        {
        return 0; // No output channels configured.
        }

    auto io_mgr = G_IO_MANAGER();
    auto node_index = DO_channels.tables[ index ];
    if ( auto node = io_mgr->get_node( node_index ); node->is_pp_mode_active() )
        {
        return -1;
        }

    return 1; // Node is OK.
    }
//-----------------------------------------------------------------------------
int io_device::check_output_AO_node_PP_state( u_int index ) const
    {
    // If no channels configured or tables not initialized, skip node check.
    if ( index >= AO_channels.count || !AO_channels.tables ||
        !AO_channels.int_write_values || !AO_channels.int_write_values[ index ] )
        {
        return 0; // No output channels properly configured.
        }

    auto io_mgr = G_IO_MANAGER();
    auto node_index = AO_channels.tables[ index ];
    if ( auto node = io_mgr->get_node( node_index ); node->is_pp_mode_active() )
        {
        return -1;
        }

    return 1; // Node is OK.
    }
//-----------------------------------------------------------------------------
void io_device::init_channel( int type, int ch_inex, int node, int offset,
    int module_offset /*= -1*/, int logical_port /*= -1 */ )
    {
    switch ( type )
        {
        case IO_channels::CT_DI:
            DI_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;

        case IO_channels::CT_DO:
            DO_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;

        case IO_channels::CT_AI:
            AI_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;

        case IO_channels::CT_AO:
            AO_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
io_device::IO_channels::IO_channels( CHANNEL_TYPE type ) : count( 0 ),
    tables( 0 ),
    offsets( 0 ),
	module_offsets ( 0 ),
	logical_ports ( 0 ),
    int_read_values( 0 ), int_module_read_values(0),
	int_write_values( 0 ),
    char_read_values( 0 ), char_write_values( 0 ),
    type( type )
    {
    }
//-----------------------------------------------------------------------------
io_device::IO_channels::~IO_channels()
    {
    if ( count )
        {
        delete [] tables;
        delete [] offsets;
		delete [] module_offsets;
		delete [] logical_ports;
        count = 0;
        }
    if ( int_read_values )
        {
        delete [] int_read_values;
        int_read_values = 0;
        }
	if (int_module_read_values)
		{
		delete[] int_module_read_values;
		int_module_read_values = 0;
		}
    if ( int_write_values )
        {
        delete [] int_write_values;
        int_write_values = 0;
        }
    if ( char_read_values )
        {
        delete [] char_read_values;
        char_read_values = 0;
        }

    if ( char_write_values )
        {
        delete [] char_write_values;
        char_write_values = 0;
        }
    }
//-----------------------------------------------------------------------------
void io_device::IO_channels::init( int ch_count )
    {
    if ( ch_count > 0 )
        {
        count = ch_count;

        tables = new u_int[ count ]{};
        offsets = new u_int[ count ]{};
        module_offsets = new int[ count ]{};
        logical_ports = new int[ count ]{};

        switch ( type )
            {
            case IO_channels::CT_DI:
                char_read_values = new u_char*[ count ]{};
                break;

            case IO_channels::CT_DO:
                char_read_values  = new u_char*[ count ]{};
                char_write_values = new u_char*[ count ]{};
                break;

            case IO_channels::CT_AI:
                int_read_values = new int_2*[ count ]{};
				int_module_read_values = new int_2*[count]{};
                break;

            case IO_channels::CT_AO:
                int_read_values  = new int_2*[ count ]{};
				int_module_read_values = new int_2*[count]{};
                int_write_values = new int_2*[ count ]{};
                break;
            }
        }
    }
//-----------------------------------------------------------------------------
void io_device::IO_channels::print() const
    {
    if ( count )
        {
        switch ( type )
            {
            case CT_DI:
                printf( "DI" );
                break;

            case CT_DO:
                printf( "DO" );
                break;

            case CT_AI:
                printf( "AI" );
                break;

            case CT_AO:
                printf( "AO" );
                break;
            }

        printf( ":%d; ", count );
        }
    }
//-----------------------------------------------------------------------------
void io_device::IO_channels::init_channel( u_int ch_index, int node, int offset,
    int module_offset, int logical_port )
    {
    if ( ch_index < count )
        {
        tables[ ch_index ]  = node;
        offsets[ ch_index ] = offset;
		module_offsets[ ch_index ] = module_offset;
		logical_ports[ ch_index ] = logical_port;

        switch ( type )
            {
            case CT_DI:
                char_read_values[ ch_index ] = io_manager::get_instance()->
                    get_DI_read_data( tables[ ch_index ], offsets[ ch_index ] );
                break;

            case CT_DO:
                char_read_values[ ch_index ] = io_manager::get_instance()->
                    get_DO_read_data( tables[ ch_index ], offsets[ ch_index ] );
                char_write_values[ ch_index ] = io_manager::get_instance()->
                    get_DO_write_data( tables[ ch_index ], offsets[ ch_index ] );
                break;

            case CT_AI:
                int_read_values[ ch_index ] = io_manager::get_instance()->
                    get_AI_read_data( tables[ ch_index ], offsets[ ch_index ] );
				if (module_offsets[ch_index] >= 0)
					{
					int_module_read_values[ch_index] = io_manager::get_instance()->
						get_AI_read_data(tables[ch_index], module_offsets[ch_index]);
					}
				else
					{
					int_module_read_values[ch_index] = 0;
					}
                break;

            case CT_AO:
                int_read_values[ ch_index ] = io_manager::get_instance()->
                    get_AO_read_data( tables[ ch_index ], offsets[ ch_index ] );
                int_write_values[ ch_index ] = io_manager::get_instance()->
                    get_AO_write_data( tables[ ch_index ], offsets[ ch_index ] );
				if (module_offsets[ch_index] >= 0)
					{
					int_module_read_values[ch_index] = io_manager::get_instance()->
						get_AI_read_data(tables[ch_index], module_offsets[ch_index]);
					}
				else
					{
					int_module_read_values[ch_index] = 0;
					}
                break;
            }
        }


    else
        {
        if ( G_DEBUG )
            {
            printf( "Error io_device::IO_channels::init_channel - index out of bound!\n" );
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void io_manager::init( int new_nodes_count )
    {
#ifdef PTUSA_TEST
    if ( nodes_count && nodes )
        {
        for ( u_int i = 0; i < nodes_count; i++ )
            {
            delete nodes[ i ];
            nodes[ i ] = nullptr;
            }

        delete[] nodes;
        nodes = nullptr;
        nodes_count = 0;
        }
#endif // PTUSA_TEST

    nodes_count = new_nodes_count;

    if ( nodes_count )
        {
        nodes = new io_node*[ nodes_count ];
        for ( u_int i = 0; i < nodes_count; i++ )
            {
            nodes[ i ] = nullptr;
            }
        }
    }
//-----------------------------------------------------------------------------
io_manager* io_manager::get_instance()
    {
    if ( instance.is_null() )
        {
#ifdef WIN_OS
        instance = new uni_io_manager();
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
        instance = new uni_io_manager();
#endif // defined LINUX_OS && defined PAC_PC

#if defined LINUX_OS && defined PAC_PLCNEXT
        instance = new uni_io_manager();
#endif // defined LINUX_OS && defined PAC_PC
        }

    return instance;
    }
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
/// @brief Получение единственного экземпляра класса.
io_manager* io_manager::replace_instance( io_manager* new_inst )
    {
    io_manager* prev_inst = instance;
    instance.replace_without_free( new_inst );
    return prev_inst;
    }
#endif
//-----------------------------------------------------------------------------
u_char* io_manager::get_DI_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DI_cnt )
            {
            return &nodes[ node_n ]->DI[ offset ];
            }
        }

    if ( G_DEBUG )
        {
        printf( "io_manager::get_DI_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
u_char* io_manager::get_DO_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DO_cnt )
            {
            return &nodes[ node_n ]->DO[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_DO_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_manager::get_AI_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < io_node::C_ANALOG_BUF_SIZE )
            {
            return &( nodes[ node_n ]->AI[ offset ] );
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_AI_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_manager::get_AO_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < io_node::C_ANALOG_BUF_SIZE )
            {
            return &nodes[ node_n ]->AO[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_AO_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
u_char* io_manager::get_DO_write_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DO_cnt )
            {
            return &nodes[ node_n ]->DO_[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_DO_write_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_manager::get_AO_write_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < io_node::C_ANALOG_BUF_SIZE )
            {
            return &nodes[ node_n ]->AO_[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_AO_write_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
io_manager::io_manager() :nodes_count( 0 ), nodes( 0 )
    {
    }
//-----------------------------------------------------------------------------
io_manager::~io_manager()
    {
    if ( nodes_count && nodes )
        {
        for ( u_int i = 0; i < nodes_count; i++ )
            {
            delete nodes[ i ];
            }

        delete [] nodes;
        nodes = nullptr;
        nodes_count = 0;
        }
    }
//-----------------------------------------------------------------------------
const io_manager::io_node* io_manager::get_node( u_int node_n ) const
    {
    if ( node_n < nodes_count )
        {
        return nodes[ node_n ] ? nodes[ node_n ] : &IO_NODE_STUB;
        }

    return &IO_NODE_STUB;
    }

io_manager::io_node* io_manager::get_node( u_int node_n )
    {
    if ( node_n < nodes_count )
        {
        return nodes[ node_n ] ? nodes[ node_n ] : &io_node_stub;
        }

    return &io_node_stub;
    }

u_int io_manager::get_nodes_count()
	{
	return nodes_count;
	}
//-----------------------------------------------------------------------------
io_manager::io_node* io_manager::add_node( u_int index, int ntype, int address,
    const char* IP_address, const char *name,
    int DO_cnt, int DI_cnt,
    int AO_cnt, int AO_size, int AI_cnt, int AI_size )
    {
    if ( index < nodes_count )
        {
        nodes[ index ] = new io_node( ntype, address, IP_address, name, DO_cnt,
            DI_cnt, AO_cnt, AO_size, AI_cnt, AI_size );

        return nodes[ index ];
        }

    return nullptr;
    }
//-----------------------------------------------------------------------------
void io_manager::init_node_AO( u_int node_index, u_int AO_index,
                                u_int type, u_int offset )
    {
    if ( node_index < nodes_count && AO_index < nodes[ node_index ]->AO_cnt )
        {
        nodes[ node_index ]->AO_types[ AO_index ]   = type;
        nodes[ node_index ]->AO_offsets[ AO_index ] = offset;
        }
    }
//-----------------------------------------------------------------------------
void io_manager::init_node_AI( u_int node_index, u_int AI_index,
                                u_int type, u_int offset )
    {
    if ( node_index < nodes_count && AI_index < nodes[ node_index ]->AI_cnt )
        {
        nodes[ node_index ]->AI_types[ AI_index ]   = type;
        nodes[ node_index ]->AI_offsets[ AI_index ] = offset;
        }
    }
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
void io_manager::clear_nodes()
    {
    if ( nodes_count && nodes )
        {
        for ( u_int i = 0; i < nodes_count; i++ )
            {
            delete nodes[ i ];
            }

        delete[] nodes;
        nodes = nullptr;
        nodes_count = 0;
        }
    }
#endif
//-----------------------------------------------------------------------------
void io_manager::print() const
    {
    printf( "I\\O manager [%d]:\n", nodes_count );
    for ( u_int i = 0; i < nodes_count; i++ )
        {
        nodes[ i ]->print();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
io_manager::io_node::~io_node()
    {
    if ( DO_cnt )
        {
        delete [] DO;
        delete [] DO_;
        DO_cnt = 0;
        }

    if ( AO_cnt )
        {
        delete [] AO_offsets;
        delete [] AO_types;
        AO_cnt = 0;
        }

    if ( DI_cnt )
        {
        delete [] DI;
        DI_cnt = 0;
        }

    if ( AI_cnt )
        {
        delete [] AI_offsets;
        delete [] AI_types;
        AI_cnt = 0;
        }
    }
//-----------------------------------------------------------------------------
io_manager::io_node::io_node( int node_type, int number, const char* str_ip_address,
    const char* new_name,
    int DO_cnt, int DI_cnt, int AO_cnt, int AO_size, int AI_cnt,
    int AI_size ) : type( static_cast<TYPES>( node_type ) ),
    number( number ),
    DO_cnt( DO_cnt ),
    AO_cnt( AO_cnt ),
    AO_size( AO_size ),
    DI_cnt( DI_cnt ),
    AI_cnt( AI_cnt ),
    AI_size( AI_size )
    {
    if ( new_name )
        {
        strcpy( name, new_name );
        }

    if ( str_ip_address )
        {
        strcpy( ip_address, str_ip_address );
        }

    if ( ip_address[ 0 ] == 0 && type >= WAGO_750_XXX_ETHERNET )
        {
        is_active = false;
        sprintf( G_LOG->msg,
            "Узел \"%s\" отключен, так как не задан его IP адрес.", name );
        G_LOG->write_log( i_log::P_NOTICE );
        }

    if ( type == EMPTY )
        {
        is_active = false;
        sprintf( G_LOG->msg,
            "Узел \"%s\" отключен, так как не задан его тип.", name );
        G_LOG->write_log( i_log::P_NOTICE );
        }

    if ( AI_cnt )
        {
        AI_offsets = new u_int[ AI_cnt ]{};
        AI_types = new u_int[ AI_cnt ]{};
        }
    if ( AO_cnt )
        {
        AO_types = new u_int[ AO_cnt ]{};
        AO_offsets = new u_int[ AO_cnt ]{};
        }

    if ( DI_cnt )
        {
        DI = new u_char[ DI_cnt ]{};
        }

    if ( DO_cnt )
        {
        DO = new u_char[ DO_cnt ]{};
        DO_ = new u_char[ DO_cnt ]{};
        }
    }

//-----------------------------------------------------------------------------
void io_manager::io_node::print()
    {
    printf( "\"%s\" - type %d, number %d, IP \"%s\", "
        "DI %d [%d], DO %d [%d], AI %d [%d], AO %d [%d].\n",
        name, type, number, ip_address,
        DI_cnt, DI_cnt / CHAR_BIT, DO_cnt, DO_cnt / CHAR_BIT,
        AI_cnt, AI_size, AO_cnt, AO_size );
    }
//-----------------------------------------------------------------------------
bool io_manager::io_node::is_pp_mode_active() const
    {
    return type == PHOENIX_BK_ETH &&
        ( status_register & STATUS_REG_PP_MODE_MASK ) != 0;
    }
//-----------------------------------------------------------------------------
io_manager::io_node::DISPLAY_STATES io_manager::io_node::get_display_state() const
    {
    if ( !is_active || G_PAC_INFO()->is_emulator() )
        {
        return io_node::DISPLAY_STATES::DST_NO_CONNECT;
        }

    if ( state != ST_OK )
        {
        return io_node::DISPLAY_STATES::DST_ERROR;
        }

    // Check error/warning bits in Phoenix status registers.
    if ( type == PHOENIX_BK_ETH &&
        ( ( status_register & STATUS_REG_ERROR_MASK ) ||
        ( diagnostic_status_register &
            DIAG_STATUS_REG_CFG_BUS_ERROR_MASK ) ) )
        {
        return io_node::DISPLAY_STATES::DST_WARNING;
        }

    return io_node::DISPLAY_STATES::DST_OK;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
io_manager* G_IO_MANAGER()
    {
    return io_manager::get_instance();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef WIN_OS
#pragma warning(pop)
#endif // WIN_OS
