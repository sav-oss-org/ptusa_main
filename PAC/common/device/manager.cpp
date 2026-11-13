#include "fmt/format.h"

#include "manager.h"
#include "lua_manager.h"
#include "g_errors.h"

#include "PID.h"

auto_smart_ptr < device_manager > device_manager::instance;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_DO_device* V( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "V%d", dev_n );

    return G_DEVICE_MANAGER()->get_V( name );
    }

valve* V( const char* dev_name )
    {
    return (valve*)G_DEVICE_MANAGER()->get_V( dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* VC( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_VC( dev_name );
    }
//-----------------------------------------------------------------------------
i_motor* M( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "M%d", dev_n );

    return G_DEVICE_MANAGER()->get_M( name );
    }

i_motor* M( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_M( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* LS( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "LS%d", dev_n );

    return G_DEVICE_MANAGER()->get_LS( name );
    }

i_DI_device* LS( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_LS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* FS( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "FS%d", dev_n );

    return G_DEVICE_MANAGER()->get_FS( name );
    }

i_DI_device* FS( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_FS( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* AI( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_AI( dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* AO( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "AO%d", dev_n );

    return G_DEVICE_MANAGER()->get_AO( name );
    }

i_AO_device* AO( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_AO( dev_name );
    }
//-----------------------------------------------------------------------------
i_counter* FQT( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "FQT%d", dev_n );

    return G_DEVICE_MANAGER()->get_FQT( name );
    }

i_counter* FQT( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_FQT( dev_name );
    }

virtual_counter* virtual_FQT( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_virtual_FQT( dev_name );
    }

counter_iolink* FQT_IOLINK( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_FQT_IOLINK( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* TE( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "TE%d", dev_n );

    return G_DEVICE_MANAGER()->get_TE( name );
    }

i_AI_device* TE( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_TE( dev_name );
    }
//-----------------------------------------------------------------------------
level* LT( const char* dev_name )
    {
    return (level*)G_DEVICE_MANAGER()->get_LT( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* GS( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_GS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* HA( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_HA( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* HL( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_HL( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* SB( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_SB( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* DI( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_DI( dev_name );
    }

i_DI_device* DI( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "DI%d", dev_n );
    return G_DEVICE_MANAGER()->get_DI( name );
    }

//-----------------------------------------------------------------------------
i_DO_device* DO( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_DO( dev_name );
    }

i_DO_device* DO( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "DO%d", dev_n );
    return G_DEVICE_MANAGER()->get_DO( name );
    }
//-----------------------------------------------------------------------------
i_AI_device* QT( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "QT%d", dev_n );

    return G_DEVICE_MANAGER()->get_QT( name );
    }

i_AI_device* QT( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_QT( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* PT( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "PT%d", dev_n );

    return G_DEVICE_MANAGER()->get_PT( name );
    }

i_AI_device* PT( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_PT( dev_name );
    }
//-----------------------------------------------------------------------------
i_wages* WT( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "WT%d", dev_n );

    return G_DEVICE_MANAGER()->get_WT( name );
    }

i_wages* WT( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_WT( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* F( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, sizeof( name ), "F%d", dev_n );

    return G_DEVICE_MANAGER()->get_F( name );
    }

i_DO_AO_device* F( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_F( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* C( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_C( dev_name );
    }
//-----------------------------------------------------------------------------
signal_column* HLA( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_HLA( dev_name );
    }
//-----------------------------------------------------------------------------
i_camera* CAM( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_CAM( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* PDS( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_PDS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* TS( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_TS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* get_G( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_G( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* WATCHDOG( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_watchdog( dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* EY( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_EY( dev_name );
    }
//-----------------------------------------------------------------------------
dev_stub* STUB()
    {
    return G_DEVICE_MANAGER()->get_stub();
    }
//-----------------------------------------------------------------------------
device* DEVICE( int s_number )
    {
    return G_DEVICE_MANAGER()->get_device( s_number );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_V( const char* dev_name )
    {
    return get_device( device::DT_V, dev_name );
    }
//-----------------------------------------------------------------------------
device_manager* device_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new device_manager();
        }
    return instance;
    }
//-----------------------------------------------------------------------------
device* device_manager::get_device( int dev_type,
    const char* dev_name )
    {
    if ( auto dev_n = get_device_n( (device::DEVICE_TYPE)dev_type, dev_name );
        dev_n >= 0 )
        {
        try
            {
            return project_devices[ dev_n ];
            }
        catch ( ... )
            {
            return get_stub_device();
            }
        }
    else if ( !disable_error_logging )
        {
        auto size = 0;
        if ( dev_type < device::C_DEVICE_TYPE_CNT )
            {
            size = ( fmt::format_to_n( G_LOG->msg, MAX_COPY_SIZE, "{} ",
                device::DEV_NAMES[ dev_type ] ) ).size;
            }
        else
            {
            size = ( fmt::format_to_n( G_LOG->msg, MAX_COPY_SIZE, "unknown "
                ) ).size;
            }
        auto res = fmt::format_to_n( G_LOG->msg + size, MAX_COPY_SIZE, "\"{}\" not found!",
            dev_name );
        *res.out = '\0';
        G_LOG->write_log( i_log::P_ERR );
        }

    return get_stub_device();
    }
//-----------------------------------------------------------------------------
device* device_manager::get_device( const char* dev_name )
    {
    if ( auto dev_n = get_device_n( dev_name ); dev_n >= 0 )
        {
        try
            {
            return project_devices[ dev_n ];
            }
        catch ( ... )
            {
            return get_stub_device();
            }
        }
    else if ( !disable_error_logging )
        {
        G_LOG->error( "Device \"%s\" not found!", dev_name );
        }

    return get_stub_device();
    }
//-----------------------------------------------------------------------------
device* device_manager::get_device( size_t serial_dev_n )
    {
    if ( serial_dev_n < project_devices.size() )
        {
        return project_devices[ serial_dev_n ];
        }

    return get_stub_device();
    }
//-----------------------------------------------------------------------------
size_t device_manager::get_device_count() const
    {
    return project_devices.size();
    }
//-----------------------------------------------------------------------------
const char* device_manager::get_name_in_Lua() const
    {
    return "Device manager";
    }
//-----------------------------------------------------------------------------
/// @brief Получение фиктивного устройства.
dev_stub* device_manager::get_stub()
    {
    return stub;
    }
//-----------------------------------------------------------------------------
device* device_manager::get_stub_device()
    {
    return dynamic_cast<device*>( dynamic_cast<valve*>( stub ) );
    }
//-----------------------------------------------------------------------------
void device_manager::print() const
    {
    printf( "Device manager [%zu]:\n", project_devices.size() );
    for ( u_int i = 0; i < project_devices.size(); i++ )
        {
        printf( "    %3i. ", i + 1 );
        printf( "%-20s %s",
            project_devices[ i ]->get_name(), project_devices[ i ]->get_description() );
        printf( "\n" );
        }
    }
//-----------------------------------------------------------------------------
device_manager::device_manager() : project_devices( 0 ),
    disable_error_logging( false ),
    stub( new dev_stub() )
    {
    G_DEVICE_CMMCTR->add_device( this );
    }
//-----------------------------------------------------------------------------
device_manager::~device_manager()
    {
    for ( auto dev : project_devices )
        {
        delete dev;
        dev = nullptr;
        }

    delete stub;
    stub = nullptr;

    project_devices.clear();
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_VC( const char* dev_name )
    {
    return get_device( device::DT_VC, dev_name );
    }
//-----------------------------------------------------------------------------
i_motor* device_manager::get_M( const char* dev_name )
    {
    return (i_motor*)get_device( device::DT_M, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_LS( const char* dev_name )
    {
    return get_device( device::DT_LS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_FS( const char* dev_name )
    {
    return get_device( device::DT_FS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_GS( const char* dev_name )
    {
    return get_device( device::DT_GS, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_AI( const char* dev_name )
    {
    return get_device( device::DT_AI, dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_AO( const char* dev_name )
    {
    return get_device( device::DT_AO, dev_name );
    }
//-----------------------------------------------------------------------------
i_counter* device_manager::get_FQT( const char* dev_name )
    {
    if ( auto res = get_device_n( device::DT_FQT, dev_name ); res > -1 )
        {
        device* res_ctr = project_devices.at( res );
        switch ( res_ctr->get_sub_type() )
            {
            case device::DST_FQT_F:
                return (counter_f*)res_ctr;

            case device::DST_FQT:
                return (counter*)res_ctr;

            case device::DST_FQT_VIRT:
                return (virtual_counter*)res_ctr;

            case device::DST_FQT_IOLINK:
                return (counter_iolink*)res_ctr;

            default:
                break;
            }
        }
    G_LOG->error( "FQT \"%s\" not found!", dev_name );

    return stub;
    }
//-----------------------------------------------------------------------------
virtual_counter* device_manager::get_virtual_FQT( const char* dev_name )
    {
    if ( auto res = get_device_n( device::DT_FQT, dev_name ); res > -1 )
        {
        device* res_ctr = project_devices.at( res );
        switch ( res_ctr->get_sub_type() )
            {
            case device::DST_FQT_VIRT:
                return (virtual_counter*)res_ctr;

            default:
                break;
            }
        }
    G_LOG->error( "FQT \"%s\" not found!", dev_name );

    static virtual_counter stub_fqt( "stub" );
    return &stub_fqt;
    }
//-----------------------------------------------------------------------------
counter_iolink* device_manager::get_FQT_IOLINK( const char* dev_name )
    {
    if ( auto res = get_device_n( device::DT_FQT, dev_name ); res > -1 )
        {
        device* res_ctr = project_devices.at( res );
        if ( res_ctr->get_sub_type() == device::DST_FQT_IOLINK )
            {
            return (counter_iolink*)res_ctr;
            }
        }
    G_LOG->error( "FQT_IOLINK \"%s\" not found!", dev_name );

    static counter_iolink stub_fqt_iolink( "stub" );
    return &stub_fqt_iolink;
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_TE( const char* dev_name )
    {
    return get_device( device::DT_TE, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_LT( const char* dev_name )
    {
    return get_device( device::DT_LT, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_DI( const char* dev_name )
    {
    return get_device( device::DT_DI, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_SB( const char* dev_name )
    {
    return get_device( device::DT_SB, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_DO( const char* dev_name )
    {
    return get_device( device::DT_DO, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HA( const char* dev_name )
    {
    return get_device( device::DT_HA, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HL( const char* dev_name )
    {
    return get_device( device::DT_HL, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_PT( const char* dev_name )
    {
    return get_device( device::DT_PT, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_QT( const char* dev_name )
    {
    return get_device( device::DT_QT, dev_name );
    }
//-----------------------------------------------------------------------------
i_wages* device_manager::get_WT( const char* dev_name )
    {
    return (wages*)get_device( device::DT_WT, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* device_manager::get_F( const char* dev_name )
    {
    return (i_DO_AO_device*)get_device( device::DT_F, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* device_manager::get_C( const char* dev_name )
    {
    return dynamic_cast<i_DO_AO_device*>( get_device( device::DT_REGULATOR, dev_name ) );
    }
//-----------------------------------------------------------------------------
signal_column* device_manager::get_HLA( const char* dev_name )
    {
    return (signal_column*)get_device( device::DT_HLA, dev_name );
    }
//-----------------------------------------------------------------------------
i_camera* device_manager::get_CAM( const char* dev_name )
    {
    return (i_camera*)get_device( device::DT_CAM, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_PDS( const char* dev_name )
    {
    return get_device( device::DT_PDS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_TS( const char* dev_name )
    {
    return get_device( device::DT_TS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* device_manager::get_G( const char* dev_name )
    {
    return get_device( device::DT_G, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_watchdog( const char* dev_name )
    {
    return get_device( device::DT_WATCHDOG, dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_EY( const char* dev_name )
    {
    return get_device( device::DT_EY, dev_name );
    }
//-----------------------------------------------------------------------------
/// @brief Получение узла I/O по имени.
node_dev* device_manager::get_node( const char* dev_name )
    {
    return dynamic_cast<node_dev*>( get_device( device::DT_NODE, dev_name ) );
    }
//-----------------------------------------------------------------------------
io_device* device_manager::add_io_device( int dev_type, int dev_sub_type,
    const char* dev_name, const char* descr, const char* article )
    {
    device* new_device = nullptr;
    io_device* new_io_device = nullptr;

    switch ( dev_type )
        {
        case device::DT_V:
            {
            switch ( dev_sub_type )
                {
                case device::DST_V_DO1:
                    new_device = new valve_DO1( dev_name );
                    new_io_device = (valve_DO1*)new_device;
                    break;

                case device::DST_V_DO2:
                    new_device = new valve_DO2( dev_name );
                    new_io_device = (valve_DO2*)new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_OFF:
                    new_device = new valve_DO1_DI1_off( dev_name );
                    new_io_device = (valve_DO1_DI1_off*)new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_ON:
                    new_device = new valve_DO1_DI1_on( dev_name );
                    new_io_device = (valve_DO1_DI1_on*)new_device;
                    break;

                case device::DST_V_DO1_DI2:
                    new_device = new valve_DO1_DI2( dev_name );
                    new_io_device = (valve_DO1_DI2*)new_device;
                    break;

                case device::DST_V_DO2_DI2:
                    new_device = new valve_DO2_DI2( dev_name );
                    new_io_device = (valve_DO2_DI2*)new_device;
                    break;

                case device::DST_V_MIXPROOF:
                    new_device = new valve_mix_proof( dev_name );
                    new_io_device = (valve_mix_proof*)new_device;
                    break;

                case device::DST_V_AS_MIXPROOF:
                    new_device = new valve_AS_mix_proof( dev_name );
                    new_io_device = (valve_AS_mix_proof*)new_device;
                    if ( valve_AS::V70_ARTICLES.count( article ) > 0 )
                        {
                        ( (valve_AS_mix_proof*)new_io_device )->reverse_seat_connection = true;
                        }
                    break;

                case device::DST_V_BOTTOM_MIXPROOF:
                    new_device = new valve_bottom_mix_proof( dev_name );
                    new_io_device = (valve_bottom_mix_proof*)new_device;
                    break;

                case device::DST_V_AS_DO1_DI2:
                    new_device = new valve_AS_DO1_DI2( dev_name );
                    new_io_device = (valve_AS_DO1_DI2*)new_device;
                    break;

                case device::V_DO2_DI2_BISTABLE:
                    new_device = new valve_DO2_DI2_bistable( dev_name );
                    new_io_device = (valve_DO2_DI2_bistable*)new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1:
                    new_device = new valve_iol_terminal_DO1( dev_name );
                    new_io_device = (valve_iol_terminal_DO1*)new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1_DI2:
                    new_device = new valve_iol_terminal_DO1_DI2( dev_name );
                    new_io_device = (valve_iol_terminal_DO1_DI2*)new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1_FB_OFF:
                    new_device = new valve_iol_terminal_DO1_DI1_off( dev_name );
                    new_io_device = (valve_iol_terminal_DO1_DI1_off*)new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1_FB_ON:
                    new_device = new valve_iol_terminal_DO1_DI1_on( dev_name );
                    new_io_device = (valve_iol_terminal_DO1_DI1_on*)new_device;
                    break;

                case device::V_IOLINK_MIXPROOF:
                    if (strcmp(article, valve_iolink_gea_tvis_a15_ds::GEA_TVIS_A15_DOUBLE_SEAT_ARTICLE.c_str()) == 0) {
                        new_device = new valve_iolink_gea_tvis_a15_ds(dev_name);
                        new_io_device = (valve_iolink_gea_tvis_a15_ds*)new_device;
                    }
                    else {
                        new_device = new valve_iolink_mix_proof(dev_name);
                        new_io_device = (valve_iolink_mix_proof*)new_device;
                    }
                    break;

                case device::V_IOLINK_DO1_DI2:
                    {
                    if ( strcmp( article,
                        valve_iolink_shut_off_sorio::SORIO_ARTICLE.c_str() ) == 0 )
                        {
                        new_device = new valve_iolink_shut_off_sorio( dev_name );
                        new_io_device = (valve_iolink_shut_off_sorio*)new_device;
                        }
                    else if (strcmp(article, valve_iolink_gea_tvis_a15_ss::GEA_TVIS_A15_SINGLE_SEAT_ARTICLE.c_str()) == 0)
                        {
                        new_device = new valve_iolink_gea_tvis_a15_ss(dev_name);
                        new_io_device = (valve_iolink_gea_tvis_a15_ss*)new_device;
                        }
                    else
                        {
                        new_device = new valve_iolink_shut_off_thinktop( dev_name );
                        new_io_device = (valve_iolink_shut_off_thinktop*)new_device;
                        }
                    break;
                    }

                case device::DST_V_MINI_FLUSHING:
                    new_device = new valve_mini_flushing( dev_name );
                    new_io_device = (valve_mini_flushing*)new_device;
                    break;

                case device::DST_V_VIRT:
                    new_device = new virtual_valve( dev_name );
                    break;

                case device::V_IOL_TERMINAL_MIXPROOF_DO3:
                    new_device = new valve_iol_terminal_mixproof_DO3( dev_name );
                    new_io_device = (valve_iol_terminal_mixproof_DO3*)new_device;
                    break;

                case device::V_IOL_TERMINAL_MIXPROOF_DO3_DI2:
                    new_device = new valve_iol_terminal_mixproof_DO3_DI2( dev_name );
                    new_io_device = (valve_iol_terminal_mixproof_DO3_DI2*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown V device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;
            }

        case device::DT_VC:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_VC:
                    new_device = new analog_valve( dev_name );
                    new_io_device = (analog_valve*)new_device;
                    break;

                case device::DST_VC_IOLINK:
                    new_device = new analog_valve_iolink( dev_name );
                    new_io_device = (analog_valve_iolink*)new_device;
                    break;

                case device::DST_VC_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_VC, device::DST_VC_VIRT );
                    break;

                case device::DST_VC_EY:
                    new_device = new analog_valve_ey( dev_name );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown VC device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_M:
            switch ( dev_sub_type )
                {
                case device::DST_M:
                case device::DST_M_FREQ:
                case device::DST_M_REV:
                case device::DST_M_REV_FREQ:
                case device::DST_M_REV_2:
                case device::DST_M_REV_FREQ_2:
                case device::M_REV_2_ERROR:
                case device::DST_M_REV_FREQ_2_ERROR:
                    new_device = new motor( dev_name,
                        (device::DEVICE_SUB_TYPE)dev_sub_type );
                    new_io_device = (motor*)new_device;
                    break;

                case device::M_ATV:
                    new_device = new motor_altivar( dev_name,
                        (device::DEVICE_SUB_TYPE)dev_sub_type );
                    new_io_device = (motor_altivar*)new_device;
                    break;

                case device::M_ATV_LINEAR:
                    new_device = new motor_altivar_linear( dev_name );
                    new_io_device = (motor_altivar_linear*)new_device;
                    break;

                case device::DST_M_VIRT:
                    new_device = new virtual_motor( dev_name );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown M device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_LS:
            switch ( dev_sub_type )
                {
                case device::DST_LS_MIN:
                case device::DST_LS_MAX:
                    new_device = new level_s( dev_name,
                        (device::DEVICE_SUB_TYPE)dev_sub_type );
                    new_io_device = (level_s*)new_device;
                    break;

                case device::LS_IOLINK_MAX:
                case device::LS_IOLINK_MIN:
                    new_device = new level_s_iolink( dev_name,
                        (device::DEVICE_SUB_TYPE)dev_sub_type );
                    new_io_device = (level_s_iolink*)new_device;
                    break;

                case device::DST_LS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_LS, device::DST_LS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown LS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_TE:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_TE:
                    new_device = new temperature_e( dev_name );
                    new_io_device = (temperature_e*)new_device;
                    break;

                case device::DST_TE_IOLINK:
                    if ( strcmp( article, temperature_e_iolink_tm311::ARTICLE.c_str() ) == 0 )
                        {
                        new_device = new temperature_e_iolink_tm311( dev_name );
                        new_io_device = (temperature_e_iolink_tm311*)new_device;
                        }
                    else
                        {
                        new_device = new temperature_e_iolink( dev_name );
                        new_io_device = (temperature_e_iolink*)new_device;
                        }
                    break;

                case device::DST_TE_VIRT:
                    new_device = new temperature_e_virtual( dev_name );
                    new_io_device = (temperature_e_virtual*)new_device;
                    break;

                case device::DST_TE_ANALOG:
                    new_device = new temperature_e_analog( dev_name );
                    new_io_device = (temperature_e_analog*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown TE device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_FS:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_FS:
                    new_device = new flow_s( dev_name );
                    new_io_device = (flow_s*)new_device;
                    break;

                case device::DST_FS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_FS, device::DST_FS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown FS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_FQT:
            switch ( dev_sub_type )
                {
                case device::DST_FQT:
                    new_device = new counter( dev_name );
                    new_io_device = (counter*)new_device;
                    break;

                case device::DST_FQT_F:
                    new_device = new counter_f( dev_name );
                    new_io_device = (counter_f*)new_device;
                    break;

                case device::DST_FQT_VIRT:
                    new_device = new virtual_counter( dev_name );
                    break;

                case device::DST_FQT_IOLINK:
                    new_device = new counter_iolink( dev_name );
                    new_io_device = (counter_iolink*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown FQT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_AO:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_AO:
                    new_device = new analog_output( dev_name );
                    new_io_device = (analog_output*)new_device;
                    break;

                case device::DST_AO_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_AO, device::DST_AO_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown AO device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_LT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_LT:
                    new_device = new level_e( dev_name );
                    new_io_device = (level_e*)new_device;
                    break;

                case device::DST_LT_CYL:
                    new_device = new level_e_cyl( dev_name );
                    new_io_device = (level_e_cyl*)new_device;
                    break;

                case device::DST_LT_CONE:
                    new_device = new level_e_cone( dev_name );
                    new_io_device = (level_e_cone*)new_device;
                    break;

                case device::DST_LT_CYL_HOR:
                    new_device = new level_e_cyl_hor( dev_name );
                    new_io_device = (level_e_cyl_hor*)new_device;
                    break;

                case device::DST_LT_IOLINK:
                    new_device = new level_e_iolink( dev_name );
                    new_io_device = (level_e_iolink*)new_device;
                    break;

                case device::DST_LT_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_LT, device::DST_LT_VIRT );
                    break;

                case device::DST_LT_TRUNC:
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown LT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_DI:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_DI:
                    new_device = new DI_signal( dev_name );
                    new_io_device = (DI_signal*)new_device;
                    break;

                case device::DST_DI_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_DI, device::DST_DI_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown DI device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_DO:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_DO:
                    new_device = new DO_signal( dev_name );
                    new_io_device = (DO_signal*)new_device;
                    break;

                case device::DST_DO_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_DO, device::DST_DO_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown DO device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_PT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_PT:
                    new_device = new pressure_e( dev_name );
                    new_io_device = (pressure_e*)new_device;
                    break;

                case device::DST_PT_IOLINK:
                    new_device = new pressure_e_iolink( dev_name );
                    new_io_device = (pressure_e_iolink*)new_device;
                    break;

                case device::DST_PT_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_PT, device::DST_PT_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown PT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_QT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_QT:
                    new_device = new concentration_e( dev_name, device::DST_QT );
                    new_io_device = (concentration_e*)new_device;
                    break;

                case device::DST_QT_OK:
                    new_device = new concentration_e_ok( dev_name );
                    new_io_device = (concentration_e_ok*)new_device;
                    break;

                case device::DST_QT_IOLINK:
                    new_device = new concentration_e_iolink( dev_name );
                    new_io_device = (concentration_e_iolink*)new_device;
                    break;

                case device::DST_QT_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_QT, device::DST_QT_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown QT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_AI:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_AI:
                    new_device = new analog_input( dev_name );
                    new_io_device = (analog_input*)new_device;
                    break;

                case device::DST_AI_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_AI, device::DST_AI_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown AI device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_HA:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_HA:
                    new_device = new siren( dev_name );
                    new_io_device = (siren*)new_device;
                    break;

                case device::DST_HA_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_HA, device::DST_HA_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown HA device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_HL:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_HL:
                    new_device = new lamp( dev_name );
                    new_io_device = (lamp*)new_device;
                    break;

                case device::DST_HL_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_HL, device::DST_HL_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown HL device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_SB:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_SB:
                    new_device = new button( dev_name );
                    new_io_device = (button*)new_device;
                    break;

                case device::DST_SB_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_SB, device::DST_SB_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown SB device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_GS:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_GS:
                    new_device = new state_s( dev_name );
                    new_io_device = (state_s*)new_device;
                    break;

                case device::DST_GS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_GS, device::DST_GS_VIRT );
                    break;

                case device::DST_GS_INVERSE:
                    new_device = new state_s_inverse( dev_name );
                    new_io_device = (state_s_inverse*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown GS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_PDS:
            switch ( dev_sub_type )
                {
                case device::DST_PDS:
                    new_device = new diff_pressure( dev_name );
                    new_io_device = (state_s*)new_device;
                    break;

                case device::DST_PDS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_PDS, device::DST_PDS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown PDS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_WT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_WT:
                    new_device = new wages( dev_name );
                    new_io_device = (wages*)new_device;
                    break;

                case device::DST_WT_VIRT:
                    new_device = new virtual_wages( dev_name );
                    break;

                case device::DST_WT_RS232:
                    new_device = new wages_RS232( dev_name );
                    new_io_device = (wages_RS232*)new_device;
                    break;

                case device::DST_WT_ETH:
                    new_device = new wages_eth( dev_name );
                    new_io_device = (wages_eth*)new_device;
                    break;

                case device::DST_WT_PXC_AXL:
                    new_device = new wages_pxc_axl( dev_name );
                    new_io_device = (wages_pxc_axl*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown WT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_F:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_F:
                    new_device = new circuit_breaker( dev_name );
                    new_io_device = (circuit_breaker*)new_device;
                    break;

                case device::DST_F_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_F, device::DST_F_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown F device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_REGULATOR:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_REGULATOR_PID:
                    new_device = new PID( dev_name );
                    break;

                case device::DST_REGULATOR_THLD:
                    new_device = new threshold_regulator( dev_name );
                    break;

                default:
                    G_LOG->debug( "unknown DT_REGULATOR device subtype %d",
                        dev_sub_type );
                    break;
                }
            break;

        case device::DT_HLA:
            switch ( dev_sub_type )
                {
                case device::DST_HLA:
                    new_device = new signal_column_discrete( dev_name );
                    new_io_device = (signal_column_discrete*)new_device;
                    break;

                case device::DST_HLA_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_HLA, device::DST_HLA_VIRT );
                    break;

                case device::DST_HLA_IOLINK:
                    new_device = new signal_column_iolink( dev_name );
                    new_io_device = (signal_column_iolink*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown HLA device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_CAM:
            switch ( dev_sub_type )
                {
                case device::DST_CAM_DO1_DI1:
                    new_device = new camera( dev_name,
                        static_cast<device::DEVICE_SUB_TYPE> ( dev_sub_type ) );
                    new_io_device = (camera*)new_device;
                    break;

                case device::DST_CAM_DO1_DI2:
                    new_device = new camera_DI2( dev_name,
                        static_cast<device::DEVICE_SUB_TYPE> ( dev_sub_type ) );
                    new_io_device = (camera_DI2*)new_device;
                    break;

                case device::DST_CAM_DO1_DI3:
                    new_device = new camera_DI3( dev_name );
                    new_io_device = (camera_DI3*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown CAM device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_TS:
            switch ( dev_sub_type )
                {
                case device::DST_TS:
                    new_device = new temperature_signal( dev_name );
                    new_io_device = (state_s*)new_device;
                    break;

                case device::DST_TS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_TS, device::DST_TS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown TS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_G:
            switch ( dev_sub_type )
                {
                case device::DST_G_IOL_4:
                case device::DST_G_IOL_8:
                    new_device = new power_unit( dev_name,
                        static_cast<device::DEVICE_SUB_TYPE> ( dev_sub_type ) );
                    new_io_device = (power_unit*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown G device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_WATCHDOG:
            if ( dev_sub_type == device::DST_WATCHDOG )
                {
                new_device = new watchdog( dev_name, device::DST_WATCHDOG );
                }
            else
                {
                G_LOG->alert( "Unknown WATCHDOG device subtype %d!\n",
                    dev_sub_type );
                }
            break;

        case device::DT_EY:
            switch ( dev_sub_type )
                {
                case device::DST_CONV_AO2:
                    new_device = new converter_iolink_ao( dev_name );
                    new_io_device = (converter_iolink_ao*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown Y device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_NODE:
            if ( dev_sub_type == device::DST_NODE )
                {
                new_device = new node_dev( dev_name );
                }
            else
                {
                G_LOG->alert( "Unknown NODE device subtype %d!\n",
                    dev_sub_type );
                }
            break;

        default:
            if ( G_DEBUG )
                {
                printf( "Unknown device type %d!\n", dev_type );
                }
            break;
        }

    // Ошибки.
    if ( !new_device )
        {
        new_device = static_cast<device*>( static_cast<valve*>( new dev_stub() ) );
        }

    G_ERRORS_MANAGER->add_error( new simple_error( new_device ) );

    u_int new_dev_index = project_devices.size();
    project_devices.push_back( new_device );
    new_device->set_serial_n( new_dev_index );
    new_device->set_article( article );

    if ( dev_type >= 0 && dev_type < device::C_DEVICE_TYPE_CNT )
        {
        if ( 0 == is_first_device[ dev_type ] )
            {
            dev_types_ranges[ dev_type ].start_pos = new_dev_index;
            is_first_device[ dev_type ] = 1;
            }
        dev_types_ranges[ dev_type ].end_pos = new_dev_index;
        }

    return new_io_device;
    }
//-----------------------------------------------------------------------------
void device_manager::clear_io_devices()
    {
    for ( auto dev : project_devices )
        {
        delete dev;
        dev = nullptr;
        }

    for ( size_t idx = 0; idx < device::C_DEVICE_TYPE_CNT; idx++ )
        {
        dev_types_ranges[ idx ].start_pos = -1;
        dev_types_ranges[ idx ].end_pos = -1;
        is_first_device[ idx ] = 0;
        }

    project_devices.clear();

    valve::clear_switching_off_queue();
    valve::clear_v_bistable();

    G_ERRORS_MANAGER->clear();
    }
//-----------------------------------------------------------------------------
int device_manager::init_params()
    {
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_params", "device_manager::init_params()" );

    return 0;
    }
//-----------------------------------------------------------------------------
int device_manager::save_device( char* buff ) const
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "t=\n\t{{\n" ) ).size;
    for ( auto dev : project_devices )
        {
        buff[ res++ ] = '\t';
        res += dev->save_device( buff + res );
        }
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE, "\t}}\n" ) ).size;
    return res;
    }
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
int device_manager::add_device( device* new_device, device::DEVICE_TYPE dev_type )
    {
    u_int new_dev_index = project_devices.size();
    project_devices.push_back( new_device );
    new_device->set_serial_n( new_dev_index );

    if ( dev_type >= 0 && dev_type < device::C_DEVICE_TYPE_CNT )
        {
        if ( 0 == is_first_device[ dev_type ] )
            {
            dev_types_ranges[ dev_type ].start_pos = new_dev_index;
            is_first_device[ dev_type ] = 1;
            }
        dev_types_ranges[ dev_type ].end_pos = new_dev_index;
        }
    else
        {
        if ( G_DEBUG )
            {
            G_LOG->debug( "Unknown device type %d!\n", dev_type );
            }

        return 1;
        }

    return 0;
    };
//-----------------------------------------------------------------------------
int device_manager::remove_device( u_int idx )
    {
    project_devices.erase( project_devices.begin() + idx );
    return 0;
    }
#endif
//-----------------------------------------------------------------------------
int device_manager::get_device_n( device::DEVICE_TYPE dev_type, const char* dev_name )
    {
    int l = -1;
    int u = -1;

    if ( dev_type < device::C_DEVICE_TYPE_CNT && dev_type >= device::DT_V )
        {
        l = dev_types_ranges[ dev_type ].start_pos;
        u = dev_types_ranges[ dev_type ].end_pos;
        }

    if ( -1 == l ) return -1; // Нет устройств.

    while ( l <= u )
        {
        int i = ( l + u ) / 2;

        // Store comparison result to avoid calling strcmp twice
        int cmp = strcmp( dev_name, project_devices[ i ]->get_name() );

        if ( cmp == 0 )
            {
            return i;
            }
        else if ( cmp > 0 )
            {
            l = i + 1;
            }
        else
            {
            u = i - 1;
            }
        }

    return -1;
    }
//-----------------------------------------------------------------------------
int device_manager::get_device_n( const char* dev_name )
    {
    for ( int dev_type = device::DT_V; dev_type < device::C_DEVICE_TYPE_CNT; dev_type++ )
        {
        auto res = get_device_n( (device::DEVICE_TYPE)dev_type, dev_name );
        if ( res >= 0 ) return res;
        }

    return -1;
    }
//-----------------------------------------------------------------------------
int device_manager::init_rt_params()
    {
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_rt_params", "device_manager::init_rt_params()" );

    return 0;
    }
//-----------------------------------------------------------------------------
void device_manager::evaluate_io()
    {
    for ( auto dev : project_devices )
        {
        dev->evaluate_io();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
device_manager* G_DEVICE_MANAGER()
    {
    return device_manager::get_instance();
    }
