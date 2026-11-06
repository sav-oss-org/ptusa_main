#include "base.h"
#include "PAC_info.h"

#include "fmt/format.h"

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

const std::array<const char*, device::DEVICE_TYPE::C_DEVICE_TYPE_CNT> device::DEV_NAMES =
    {
    "V",       ///< Клапан.
    "VC",      ///< Управляемый клапан.
    "M",       ///< Двигатель.
    "LS",      ///< Уровень (есть/нет).
    "TE",      ///< Температура.
    "FS",      ///< Расход (есть/нет).
    "GS",      ///< Датчик положения.
    "FQT",     ///< Счетчик.
    "LT",      ///< Уровень (значение).
    "QT",      ///< Концентрация.

    "HA",      ///< Аварийная звуковая сигнализация.
    "HL",      ///< Аварийная световая сигнализация.
    "SB",      ///< Кнопка.
    "DI",      ///< Дискретный входной сигнал.
    "DO",      ///< Дискретный выходной сигнал.
    "AI",      ///< Аналоговый входной сигнал.
    "AO",      ///< Аналоговый выходной сигнал.
    "WT",      ///< Тензорезистор.
    "PT",      ///< Давление (значение).
    "F",       ///< Автоматический выключатель.

    "C",       ///< ПИД-регулятор.
    "HLA",     ///< Сигнальная колонна.
    "CAM",     ///< Камера.
    "PDS",     ///< Датчик разности давления.
    "TS",      ///< Сигнальный датчик температуры.
    "G",       ///< Блок питания.
    "WATCHDOG",///< Устройство проверки связи.
    "EY",      ///< Конвертер IO-Link.
    "NODE",    ///< Узел сетевых настроек.
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int par_device::save_device( char* buff ) const
    {
    buff[ 0 ] = 0;

    if ( par == nullptr )
        {
        return 0;
        }

    auto size = 0;
    for ( u_int i = 0; i < par->get_count(); i++ )
        {
        if ( par_name[ i ] )
            {
            auto val = par[ 0 ][ i + 1 ];
            double tmp;
            int precision = modf( val, &tmp ) == 0 ? 0 : 2;
            size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "{}={:.{}f}, ",
                par_name[ i ], val, precision ).size;
            }
        }
    return size;
    }
//-----------------------------------------------------------------------------
int par_device::set_par_by_name( const char* name, double val )
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            if ( strcmp( par_name[ i ], name ) == 0 )
                {
                par->save( i + 1, (float)val );
                return 0;
                }
            }
        }

    if ( G_DEBUG )
        {
        printf( "par_device::set_cmd() - name = %s wasn't found.\n",
            name );
        }
    return 1;
    }
//-----------------------------------------------------------------------------
void par_device::set_par( u_int idx, u_int offset, float value )
    {
    if ( par )
        {
        ( *par )[ offset + idx ] = value;
        }
    }
//-----------------------------------------------------------------------------
par_device::par_device( u_int par_cnt )
    {
    if ( par_cnt )
        {
        par_name = new char* [ par_cnt ];
        for ( u_int i = 0; i < par_cnt; i++ )
            {
            par_name[ i ] = nullptr;
            }

        par = new saved_params_float( par_cnt );
        }
    }
//-----------------------------------------------------------------------------
par_device::~par_device()
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            delete[] par_name[ i ];
            par_name[ i ] = nullptr;
            }

        delete[] par_name;
        par_name = nullptr;

        delete par;
        par = nullptr;
        }
    }
//-----------------------------------------------------------------------------
float par_device::get_par( u_int idx, u_int offset ) const
    {
    if ( par )
        {
        return par[ 0 ][ offset + idx ];
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void par_device::set_par_name( u_int idx, u_int offset, const char* name )
    {
    if ( par )
        {
        if ( offset + idx <= par->get_count() && ( offset + idx > 0 ) )
            {
            if ( strlen( name ) > C_MAX_PAR_NAME_LENGTH )
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name( u_int idx, u_int offset, const char* name ) - "
                        "name length (%zu) > param C_MAX_PAR_NAME_LENGTH (%d).",
                        strlen( name ), C_MAX_PAR_NAME_LENGTH );
                    }

                return;
                }

            if ( nullptr == par_name[ offset + idx - 1 ] )
                {
                par_name[ offset + idx - 1 ] = new char[ strlen( name ) + 1 ];
                strcpy( par_name[ offset + idx - 1 ], name );
                }
            else
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                        "param (%d %d) already has name (%s).",
                        offset, idx, par_name[ offset + idx - 1 ] );
                    }
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                    "offset (%d) + idx (%d) > param count ( %d ).",
                    offset, idx, par->get_count() );
                }
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void device::set_descr( const char* new_description )
    {
    delete[] description;

    //Копирование с учетом нуль-символа.
    int len = strlen( new_description ) + 1;
    description = new char[ len ];
    strcpy( description, new_description );
    }
//-----------------------------------------------------------------------------
void device::set_article( const char* new_article )
    {
    delete[] article;

    //Копирование  с учетом нуль-символа.
    int len = strlen( new_article ) + 1;
    article = new char[ len ];
    strcpy( article, new_article );
    }
//-----------------------------------------------------------------------------
void device::print() const
    {
    printf( "%s\t", name );
    }
//-----------------------------------------------------------------------------
void device::direct_off()
    {
    state = 0;
    value = 0;
    }
//-----------------------------------------------------------------------------
void device::direct_on()
    {
    state = 1;
    }
//-----------------------------------------------------------------------------
void device::direct_set_state( int new_state )
    {
    state = new_state;
    }
//-----------------------------------------------------------------------------
void device::direct_set_value( float new_value )
    {
    value = new_value;
    }
//-----------------------------------------------------------------------------
void device::off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int device::get_state() const
    {
    return state;
    }
//-----------------------------------------------------------------------------
float device::get_value() const
    {
    return value;
    }
//-----------------------------------------------------------------------------
void device::set_property( const char* field, device* dev )
    {
    // By default do nothing.
    }
//-----------------------------------------------------------------------------
void device::set_string_property( const char* field, const char* new_value )
    {
    G_LOG->debug( R"(%s (%s) set string property "%s" to "%s")",
        name, get_type_str(), field, new_value );
    }
//-----------------------------------------------------------------------------
int device::save_device( char* buff ) const
    {
    // LCOV_EXCL_START
    auto res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE,
        "{}={{M={:d}, ", name, is_manual_mode ).size );
    // LCOV_EXCL_STOP

    if ( type != DT_AO )
        {
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "ST={}, ",
            get_state() ).size;
        }

    if ( type != DT_V &&

        type != DT_FS &&
        type != DT_GS &&

        type != DT_HA &&
        type != DT_HL &&
        type != DT_SB &&
        !( type == DT_LS && ( sub_type == DST_LS_MAX || sub_type == DST_LS_MIN ) ) &&

        type != DT_DI &&
        type != DT_DO )
        {
        auto val = get_value();
        double tmp;
        int precision = modf( val, &tmp ) == 0 ? 0 : 2;
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "V={:.{}f}, ",
            val, precision ).size;
        }

    res += save_device_ex( buff + res );
    res += par_device::save_device( buff + res );

    if ( const auto extra_symbols_length = 2;
        res > extra_symbols_length ) //Remove last " ,".
        {
        res -= extra_symbols_length;
        }
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "}},\n" ).size;
    return res;
    }
//-----------------------------------------------------------------------------
void device::evaluate_io()
    {
    //Do nothing by default.
    }
//-----------------------------------------------------------------------------
/// @brief Отладочная печать объекта в консоль.
const char* device::get_name_in_Lua() const
    {
    return get_name();
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char* prop, u_int idx, double val )
    {
    G_LOG->debug( "%s\t device::set_cmd() - prop = %s, idx = %d, val = %f",
        name, prop, idx, val );

    switch ( prop[ 0 ] )
        {
        case 'S':
            direct_set_state( (int)val );
            break;

        case 'V':
            direct_set_value( (float)val );
            break;

        case 'M':
            if ( val == 0. )
                is_manual_mode = false;
            else if ( val == 1. )
                is_manual_mode = true;
            break;

        case 'P': //Параметры.
            return par_device::set_par_by_name( prop, val );

        default:
            G_LOG->debug( "Error device::set_cmd() - prop = %s, val = %f\n",
                prop, val );
            return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
device::device( const char* dev_name, DEVICE_TYPE type, DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) : par_device( par_cnt ), type( type ),
    sub_type( sub_type )
    {
    if ( dev_name )
        {
        auto [out, size] = fmt::format_to_n(
            this->name, device::CONSTANTS::C_MAX_NAME, "{}", dev_name );
        *out = '\0';
        if ( size > device::CONSTANTS::C_MAX_NAME )
            {
            G_LOG->critical( "Error create device '%s' - name truncated to `%s`!",
                dev_name, this->name );
            }
        }
    else
        {
        strcpy( this->name, "?" );
        }

    description = new char[ 1 ];
    description[ 0 ] = '\0';

    article = new char[ 2 ];
    article[ 0 ] = ' ';
    article[ 1 ] = '\0';
    }
//-----------------------------------------------------------------------------
const char* device::get_type_str() const
    {
    if ( type >= 0 && type < C_DEVICE_TYPE_CNT ) return DEV_NAMES[ type ];

    return "NONE";
    }
//-----------------------------------------------------------------------------
const char* device::get_type_name() const
    {
    switch ( type )
        {
        case DT_V:
            return "Клапан";
        case DT_VC:
            return "Управляемый клапан";
        case DT_M:
            return "Двигатель";

        case DT_LS:
            return "Уровень";
        case DT_TE:
            return "Температура";
        case DT_FS:
            return "Расход";
        case DT_GS:
            return "Датчик положения";
        case DT_FQT:
            return "Счетчик";
        case DT_LT:
            return "Текущий уровень";
        case DT_QT:
            return "Концентрация";
        case DT_HA:
            return "Аварийная звуковая сигнализация";
        case DT_HL:
            return "Аварийная световая сигнализация";
        case DT_SB:
            return "Кнопка";
        case DT_DI:
            return "Дискретный входной сигнал";
        case DT_DO:
            return "Дискретный выходной сигнал";
        case DT_AI:
            return "Аналоговый входной сигнал";
        case DT_AO:
            return "Аналоговый выходной сигнал";
        case DT_WT:
            return "Тензорезистор";
        case DT_PT:
            return "Давление";
        case DT_F:
            return "Автоматический выключатель";
        case DT_REGULATOR:
            return "ПИД-регулятор";
        case DT_HLA:
            return "Сигнальная колонна";
        case DT_CAM:
            return "Камера";
        case DT_G:
            return "Блок питания";
        case DT_WATCHDOG:
            return "Устройство проверки связи";

        default:
            return "???";
        }
    }
//-----------------------------------------------------------------------------
void device::param_emulator( float math_expec, float stddev )
    {
    emulator.param( math_expec, stddev );
    }
//-----------------------------------------------------------------------------
analog_emulator& device::get_emulator()
    {
    return emulator;
    }
//-----------------------------------------------------------------------------
const analog_emulator& device::get_emulator() const
    {
    return emulator;
    }
//-----------------------------------------------------------------------------
bool device::is_emulation() const
    {
    return emulation;
    }
//-----------------------------------------------------------------------------
void device::set_emulation( bool new_emulation_state )
    {
    emulation = new_emulation_state;
    }
//-----------------------------------------------------------------------------
device::~device()
    {
    delete[] description;
    description = nullptr;
    delete[] article;
    article = nullptr;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_motor::i_motor( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
    int params_count ) :
    device( dev_name, DT_M, sub_type, params_count )
    {
    }
//-----------------------------------------------------------------------------
void i_motor::reverse()
    {
    set_state( 2 );
    }
//-----------------------------------------------------------------------------
float i_motor::get_linear_speed() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float i_motor::get_amperage() const
    {
    return 0.0f;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void digital_io_device::print() const
    {
    device::print();
    }
//-----------------------------------------------------------------------------
digital_io_device::digital_io_device( const char* dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    device( dev_name, type, sub_type, par_cnt ),
    io_device( dev_name )
    {
    }
//-----------------------------------------------------------------------------
void digital_io_device::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_set_state( new_state );
        }

    switch ( new_state )
        {
        case 0:
            direct_off();
            break;

        case 1:
            direct_on();
            break;

        default:
            if ( G_DEBUG )
                {
                G_LOG->warning(
                    "%s\t digital_io_device::direct_set_state() - new_state = %d",
                    get_name(), new_state );
                }
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AO1::AO1( const char* dev_name,
    device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) :
    analog_io_device( dev_name, type, sub_type, par_cnt )
    {
    }
//-----------------------------------------------------------------------------
void AO1::evaluate_io()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        // Ничего не делаем.
        return;
        }

    current_state = analog_io_device::get_state();
    // Check if the network node for output channel is available.
    if ( auto node_state = check_output_AO_node_PP_state(); node_state < 0 )
        {
        current_state = -1;
        }
    }
//-----------------------------------------------------------------------------
int AO1::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

    return current_state;
    }
//-----------------------------------------------------------------------------
float AO1::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    return get_AO( AO_INDEX, get_min_value(), get_max_value() );
    }
//-----------------------------------------------------------------------------
void AO1::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        return analog_io_device::direct_set_value( new_value );

    set_AO( AO_INDEX, new_value, get_min_value(), get_max_value() );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
virtual_counter::virtual_counter( const char* dev_name ) :
    device( dev_name, DT_FQT, DST_FQT_VIRT, 0 )
    {
    device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
    }
//-----------------------------------------------------------------------------
int virtual_counter::get_state() const
    {
    return device::get_state();
    }
//-----------------------------------------------------------------------------
void virtual_counter::direct_on()
    {
    start();
    }

void virtual_counter::direct_off()
    {
    reset();
    }
//-----------------------------------------------------------------------------
void virtual_counter::direct_set_state( int new_state )
    {
    switch ( static_cast<STATES>( new_state ) )
        {
        case STATES::S_WORK:
            start();
            break;

        case STATES::S_PAUSE:
            pause();
            break;

        default:
            device::direct_set_state( new_state );
            break;
        }
    }
//-----------------------------------------------------------------------------
void virtual_counter::pause()
    {
    device::direct_set_state( static_cast<int>( STATES::S_PAUSE ) );
    }
//-----------------------------------------------------------------------------
void virtual_counter::start()
    {
    device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
    }
//-----------------------------------------------------------------------------
void virtual_counter::reset()
    {
    device::direct_set_value( 0.f );
    }
//-----------------------------------------------------------------------------
u_int virtual_counter::get_quantity() const
    {
    return static_cast<u_int>( device::get_value() );
    }
//-----------------------------------------------------------------------------
float virtual_counter::get_flow() const
    {
    return flow_value;
    }
//-----------------------------------------------------------------------------
/// @brief Получение абсолютного значения счетчика (без учета
/// состояния паузы).
u_int virtual_counter::get_abs_quantity() const
    {
    return abs_value;
    }

/// @brief Сброс абсолютного значения счетчика.
void  virtual_counter::abs_reset()
    {
    abs_value = 0;
    }
//-----------------------------------------------------------------------------
int virtual_counter::set_cmd( const char* prop, u_int idx, double val )
    {
    switch ( prop[ 0 ] )
        {
        case 'A': //ABS_V
            abs_value = (u_int)val;
            break;

        case 'F':
            flow_value = (float)val;
            break;

        default:
            return device::set_cmd( prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void virtual_counter::set( u_int new_value, u_int new_abs_value, float flow )
    {
    device::direct_set_value( static_cast<float>( new_value ) );
    abs_value = new_abs_value;
    flow_value = flow;
    }
//-----------------------------------------------------------------------------
void virtual_counter::eval( u_int read_value, u_int abs_read_value,
    float read_flow = 0 )
    {
    if ( !is_first_read )
        {
        if ( read_value > last_read_value )
            {
            auto val = device::get_value() +
                static_cast<float>( read_value - last_read_value );
            device::direct_set_value( val );
            }

        if ( abs_read_value > abs_last_read_value )
            {
            abs_value += abs_read_value - abs_last_read_value;
            }
        }
    else
        {
        is_first_read = false;
        }
    last_read_value = read_value;
    abs_last_read_value = abs_read_value;
    flow_value = read_flow;
    }
//-----------------------------------------------------------------------------
//Lua.
int virtual_counter::save_device_ex( char* buff ) const
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "ABS_V={}, F={:.2f}, ",
        get_abs_quantity(), get_flow() ) ).size;
    return res;
    }

u_long virtual_counter::get_pump_dt() const
    {
    return 0;
    }

float virtual_counter::get_min_flow() const
    {
    return .0f;
    }
//-----------------------------------------------------------------------------
void virtual_counter::pause_daily( DAY_CTR n )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void virtual_counter::start_daily( DAY_CTR n )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AI1::AI1( const char* dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    analog_io_device( dev_name, type, sub_type,
    par_cnt + ADDITIONAL_PARAM_COUNT )
    {
    device::set_state( 1 );
    set_par_name( P_ZERO_ADJUST_COEFF, 0, "P_CZ" );
    }
//-----------------------------------------------------------------------------
int AI1::get_state() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::get_state();
        }

    auto err = 0;
    get_AI( C_AI_INDEX, 0, 0, err );
    if ( err > 0 ) return -err;

    return 1;
    }
//-----------------------------------------------------------------------------
const char* AI1::get_error_description()
    {
    if ( auto err_id = get_error_id(); err_id < 0 )
        {
        switch ( err_id )
            {
            case -static_cast<int>( io_device::ERRORS::UNDER_RANGE ) :
                return "ниже предела";

            case -static_cast<int>( io_device::ERRORS::OVER_RANGE ) :
                return "выше предела";

            case -static_cast<int>( io_device::ERRORS::OUT_OF_RANGE ) :
                return "вне диапазона";

            default:
                return "неизвестная ошибка";
            }
        }

    return "нет ошибок";
    }
//-----------------------------------------------------------------------------
int AI1::get_params_count() const
    {
    return ADDITIONAL_PARAM_COUNT;
    }
//-----------------------------------------------------------------------------
float AI1::get_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return analog_io_device::get_value();
        }

    return get_par( P_ZERO_ADJUST_COEFF, 0 ) +
        get_AI( C_AI_INDEX, get_min_val(), get_max_val() );
    }
//-----------------------------------------------------------------------------
void AI1::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_set_value( new_value );
        }
    }
//-----------------------------------------------------------------------------
/// @brief Получение максимального значения выхода устройства.
float AI1::get_max_val() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
/// @brief Получение минимального значения выхода устройства.
float AI1::get_min_val() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level::level( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) :AI1(
        dev_name, DT_LT, sub_type,
        par_cnt + static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( static_cast<int>( CONSTANTS::P_ERR ),
        start_param_idx, "P_ERR" );
    }
//-----------------------------------------------------------------------------
int level::get_volume() const
    {
    if ( get_state() < 0 )
        {
        return get_err_volume();
        }

    return calc_volume();
    }
//-----------------------------------------------------------------------------
int level::get_err_volume() const
    {
    return static_cast<int>( get_par( static_cast<int>( CONSTANTS::P_ERR ),
        start_param_idx ) );
    }
//-----------------------------------------------------------------------------
int level::calc_volume() const
    {
    return (int)get_value();
    }
//-----------------------------------------------------------------------------
int level::save_device_ex( char* buff ) const
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "CLEVEL={}, ",
        get_volume() ) ).size;
    return res;
    }
//-----------------------------------------------------------------------------
float level::get_max_val() const
    {
    return 100;
    }
//-----------------------------------------------------------------------------
float level::get_min_val() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int level::get_params_count() const
    {
    return start_param_idx + static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1;
    }
//-----------------------------------------------------------------------------
int level::get_start_param_idx() const
    {
    return start_param_idx;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column::signal_column( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int red_lamp_channel, int yellow_lamp_channel,
    int green_lamp_channel, int blue_lamp_channel, int siren_channel ) :
    device( dev_name, DT_HLA, sub_type, 0 ),
    io_device( dev_name ),
    red_lamp_channel( red_lamp_channel ),
    yellow_lamp_channel( yellow_lamp_channel ),
    green_lamp_channel( green_lamp_channel ),
    blue_lamp_channel( blue_lamp_channel ),
    siren_channel( siren_channel )
    {
    }
//-----------------------------------------------------------------------------
void signal_column::direct_off()
    {
    turn_off_red();
    turn_off_yellow();
    turn_off_green();
    turn_off_blue();

    turn_off_siren();
    }
//-----------------------------------------------------------------------------
void signal_column::direct_on()
    {
    turn_off_red();
    turn_off_yellow();
    turn_off_green();
    turn_off_blue();

    turn_off_siren();

    turn_on_green();
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_red()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( red_lamp_channel, DO_state::OFF, RED_LAMP );

    red.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_yellow()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( yellow_lamp_channel, DO_state::OFF, YELLOW_LAMP );

    yellow.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_green()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( green_lamp_channel, DO_state::OFF, GREEN_LAMP );

    green.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_blue()
    {
    if ( !blue_lamp_channel ) return;

    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( blue_lamp_channel, DO_state::OFF, BLUE_LAMP );

    blue.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_red()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );

    red.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_yellow()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( yellow_lamp_channel, DO_state::ON, YELLOW_LAMP );

    yellow.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_green()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( green_lamp_channel, DO_state::ON, GREEN_LAMP );

    green.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_blue()
    {
    if ( !blue_lamp_channel ) return;

    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( blue_lamp_channel, DO_state::ON, BLUE_LAMP );

    blue.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_red()
    {
    if ( is_const_red )
        {
        blink( red_lamp_channel, red, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
        }
    else
        {
        if ( !G_PAC_INFO()->is_emulator() )
            process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );

        red.step = STEP::on;
        }
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_yellow()
    {
    blink( yellow_lamp_channel, yellow, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_green()
    {
    blink( green_lamp_channel, green, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_blue()
    {
    blink( blue_lamp_channel, blue, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_red()
    {
    if ( is_const_red )
        {
        blink( red_lamp_channel, red, (u_long)CONSTANTS::SLOW_BLINK_TIME );
        }
    else
        {
        if ( !G_PAC_INFO()->is_emulator() )
            process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );

        red.step = STEP::on;
        }
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_yellow()
    {
    blink( yellow_lamp_channel, yellow, (u_long)CONSTANTS::SLOW_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_green()
    {
    blink( green_lamp_channel, green, (u_long)CONSTANTS::SLOW_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_blue()
    {
    blink( blue_lamp_channel, blue, (u_long)CONSTANTS::SLOW_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_siren()
    {
    siren_step = STEP::on;
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( siren_channel, DO_state::ON, SIREN );
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_siren()
    {
    siren_step = STEP::off;
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( siren_channel, DO_state::OFF, SIREN );
    }
//-----------------------------------------------------------------------------
void signal_column::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            is_const_red = (u_int)value;
            break;

        default:
            device::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
int signal_column::get_state() const
    {
    int res = green.step != STEP::off || yellow.step != STEP::off ||
        red.step != STEP::off || blue.step != STEP::off ||
        siren_step != STEP::off;

    return res;
    }
//-----------------------------------------------------------------------------
int signal_column::save_device_ex( char* buff ) const
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "L_GREEN={}, ",
        green.step == STEP::on || green.step == STEP::blink_on ? 1 : 0 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE, "L_YELLOW={}, ",
        yellow.step == STEP::on || yellow.step == STEP::blink_on ? 1 : 0 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE, "L_RED={}, ",
        red.step == STEP::on || red.step == STEP::blink_on ? 1 : 0 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE, "L_BLUE={}, ",
        blue.step == STEP::on || blue.step == STEP::blink_on ? 1 : 0 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE, "L_SIREN={}, ",
        siren_step == STEP::on ? 1 : 0 ) ).size;

    return res;
    }
//-----------------------------------------------------------------------------
void signal_column::evaluate_io()
    {
    //Так как колонну могут использовать несколько аппаратов
    //(агрегатов), то отключаем отображение событий в начале каждого
    // цикла управляющей программы.
    show_state = show_states::idle;
    }
//-----------------------------------------------------------------------------
void signal_column::show_error_exists()
    {
    if ( get_manual_mode() ) return;

    show_state = show_states::error_exists;
    normal_blink_red();
    turn_off_yellow();
    turn_off_green();
    turn_on_siren();
    };
//-----------------------------------------------------------------------------
void signal_column::show_message_exists()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists )
        {
        show_state = show_states::message_exists;
        turn_off_red();
        normal_blink_yellow();
        turn_off_green();
        turn_off_blue();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_batch_is_not_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists )
        {
        show_state = show_states::batch_is_not_running;
        turn_off_red();
        turn_on_yellow();
        turn_off_green();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_batch_is_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running )
        {
        show_state = show_states::batch_is_running;
        turn_off_red();
        turn_off_yellow();
        turn_on_green();
        turn_off_blue();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_operation_is_not_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running )
        {
        show_state = show_states::operation_is_not_running;
        turn_off_red();
        turn_on_yellow();
        turn_off_green();
        turn_off_blue();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_operation_is_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running &&
        show_state != show_states::operation_is_not_running )
        {
        show_state = show_states::operation_is_running;
        turn_off_red();
        turn_off_yellow();
        turn_on_green();
        turn_off_blue();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_idle()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running &&
        show_state != show_states::operation_is_not_running &&
        show_state != show_states::batch_is_running &&
        show_state != show_states::operation_is_running )
        {
        show_state = show_states::idle;
        turn_off_red();
        turn_off_yellow();
        turn_off_green();
        turn_off_blue();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::direct_set_state( int new_state )
    {
    switch ( (CMD)new_state )
        {
        case CMD::TURN_OFF:
            direct_off();
            break;

        case CMD::TURN_ON:
            direct_on();
            break;

        case CMD::LIGHTS_OFF:
            turn_off_red();
            turn_off_yellow();
            turn_off_green();
            turn_off_blue();
            break;

        case CMD::GREEN_ON:
            turn_on_green();
            break;

        case CMD::YELLOW_ON:
            turn_on_yellow();
            break;

        case CMD::RED_ON:
            turn_on_red();
            break;

        case CMD::BLUE_ON:
            turn_on_blue();
            break;

        case CMD::GREEN_NORMAL_BLINK:
            normal_blink_green();
            break;

        case CMD::GREEN_OFF:
            turn_off_green();
            break;

        case CMD::YELLOW_OFF:
            turn_off_yellow();
            break;

        case CMD::RED_OFF:
            turn_off_red();
            break;

        case CMD::BLUE_OFF:
            turn_off_blue();
            break;

        case CMD::YELLOW_NORMAL_BLINK:
            normal_blink_yellow();
            break;

        case CMD::RED_NORMAL_BLINK:
            normal_blink_red();
            break;

        case CMD::BLUE_NORMAL_BLINK:
            normal_blink_blue();
            break;

        case CMD::GREEN_SLOW_BLINK:
            slow_blink_green();
            break;

        case CMD::YELLOW_SLOW_BLINK:
            slow_blink_yellow();
            break;

        case CMD::RED_SLOW_BLINK:
            slow_blink_red();
            break;

        case CMD::BLUE_SLOW_BLINK:
            slow_blink_blue();
            break;

        case CMD::SIREN_ON:
            turn_on_siren();
            break;

        case CMD::SIREN_OFF:
            turn_off_siren();
            break;

        default:
            break;
        }
    }
//-----------------------------------------------------------------------------
void signal_column::blink( int lamp_DO, state_info& info, u_int delay_time )
    {
    switch ( info.step )
        {
        case STEP::off:
        case STEP::on:
            info.start_blink_time = get_millisec();
            info.step = STEP::blink_on;
            break;

        case STEP::blink_on:
            if ( !G_PAC_INFO()->is_emulator() )
                process_DO( lamp_DO, DO_state::ON, "?" );
            if ( get_delta_millisec( info.start_blink_time ) > delay_time )
                {
                info.start_wait_time = get_millisec();
                info.step = STEP::blink_off;
                }
            break;

        case STEP::blink_off:
            if ( !G_PAC_INFO()->is_emulator() )
                process_DO( lamp_DO, DO_state::OFF, "?" );
            if ( get_delta_millisec( info.start_wait_time ) > delay_time )
                {
                info.start_blink_time = get_millisec();
                info.step = STEP::blink_on;
                }
            break;
        }
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char* io_link_device::get_error_description( int err_id ) const
    {
    if ( err_id < 0 )
        {
        switch ( err_id )
            {
            case -static_cast<int>( io_device::IOLINKSTATE::NOTCONNECTED ) :
                return "IOL-устройство не подключено";

            case -static_cast<int>( io_device::IOLINKSTATE::DEVICEERROR ) :
                return "ошибка IOL-устройства";

            default:
                return "неизвестная ошибка";
            }
        }

    return "нет ошибок";
    }
