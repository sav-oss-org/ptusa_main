#ifdef DRIVER
#ifdef USE_STDAFX
#include "stdafx.h" //Стандартный заголовочный файл для использования precompiled headers.
#endif //USE_STDAFX
#else
#include "g_errors.h"
#include "device/device.h"
#include "PAC_err.h"
#endif

#ifdef PAC
auto_smart_ptr < errors_manager > errors_manager::instance;

bool simple_error::is_any_error = false;
bool simple_error::is_any_no_ack_error = false;
bool simple_error::is_new_error = false;

bool tech_obj_error::is_any_message = false;

auto_smart_ptr < siren_lights_manager > siren_lights_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
base_error::base_error(): err_par( 1 ), error_state( AS_NORMAL )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
simple_error::simple_error( i_simple_error* simple_error_owner
                           ): base_error(),
                           simple_error_owner( simple_error_owner )
    {
    simple_error_owner->set_error_params( &err_par );
    }
//-----------------------------------------------------------------------------
int simple_error::save_as_Lua_str( char *str )
    {
    int res = 0;
    str[ 0 ] = 0;

    if ( AS_ALARM == error_state || AS_ACCEPT == error_state ||
        AS_RETURN == error_state ) // Есть ошибка.
        {
        unsigned char alarm_params = err_par[ P_PARAM_N ];

        res += fmt::format_to_n( str + res, MAX_COPY_SIZE, "{{\n" ).size;

        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "description=\"{} - {}\",\n",
            simple_error_owner->get_name(), simple_error_owner->get_error_description() ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "priority={},\n", static_cast<int>( ALARM_CLASS_PRIORITY::P_ALARM ) ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "state={},\n", error_state ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "type={},\n", static_cast<int>( ALARM_TYPE::AT_SPECIAL ) ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "group=\"{}\",\n", "тревога" ).size;

        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "id_n={},\n", simple_error_owner->get_serial_n() ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "id_object_alarm_number={},\n",
            -simple_error_owner->get_error_id() ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "id_type={},\n",
            simple_error_owner->get_error_type() ).size;

        res += fmt::format_to_n( str + res, MAX_COPY_SIZE,
            "suppress={}\n", alarm_params & P_IS_SUPPRESS ? "true" : "false" ).size;
        res += fmt::format_to_n( str + res, MAX_COPY_SIZE, "}},\n" ).size;
        }

    return res;
    }
//-----------------------------------------------------------------------------
void simple_error::evaluate( bool &is_new_state )
    {
    // Проверка текущего состояния устройства.
    if ( simple_error_owner->get_state() < 0 )    // Есть ошибка.
        {

        if ( auto error_id = simple_error_owner->get_error_id();
            prev_error_id != error_id )
            {
            is_new_state = true;
            prev_error_id = error_id;
            }

        switch ( error_state )
            {
            case AS_ACCEPT:
                break;

            case AS_ALARM:
                is_any_no_ack_error = true;
                break;

            case AS_NORMAL:
            case AS_RETURN:
                error_state = AS_ALARM;
                is_new_state = true;

                is_new_error = true; //Появилась новая ошибка.

                is_any_no_ack_error = true;
                break;
            }
        is_any_error = true;

        if ( err_par[ P_PARAM_N ] & P_IS_SUPPRESS )
            {
            is_any_error = false;
            is_new_error = false;
            is_any_no_ack_error = false;
            }
        }
    else // Нет ошибки - все остальные состояния.
        {
        switch ( error_state )
            {
            case AS_NORMAL:
                break;

            case AS_RETURN:
                break;

            case AS_ACCEPT:
                error_state = AS_NORMAL;
                is_new_state = true;
                break;

            case AS_ALARM:
                error_state = AS_RETURN;
                is_new_state = true;
                break;
            }
        }
    // Проверка текущего состояния устройства.-!>
    }
//-----------------------------------------------------------------------------
void simple_error::print() const
    {
    if ( G_DEBUG )
        {
        printf( "%s - state[ %3d ], par[ %d ]\n",
            simple_error_owner->get_name(), error_state, err_par[ P_PARAM_N ] );
        }
    }
//-----------------------------------------------------------------------------
unsigned char simple_error::get_object_type() const
    {
    return static_cast<unsigned char>( simple_error_owner->get_error_type() );
    }
//-----------------------------------------------------------------------------
unsigned int simple_error::get_object_n() const
    {
    return simple_error_owner->get_serial_n();
    }
//-----------------------------------------------------------------------------
int simple_error::set_cmd( int cmd, int object_alarm_number )
    {
    int res = 0;
    int current_state = err_par[ P_PARAM_N ];

    switch ( cmd )
        {
        case C_CMD_SUPPRESS:
            {
            current_state |= P_IS_SUPPRESS;
            err_par[ P_PARAM_N ] = current_state;
            break;
            }

         case C_CMD_UNSET_SUPPRESS:
             {
             current_state |= P_IS_SUPPRESS;
             current_state ^= P_IS_SUPPRESS;

             err_par[ P_PARAM_N ] = current_state;
             break;
             }

        case C_CMD_ACCEPT:
            if ( AS_ALARM == error_state ||
                AS_RETURN == error_state )
                {
                if ( AS_RETURN == error_state )
                    {
                    error_state = AS_NORMAL;
                    }
                else
                    {
                    error_state = AS_ACCEPT;
                    }
                }
            else
                {
                if ( G_DEBUG )
                    {
                    printf( "[errors] set_cmd failed: "
                        "incorrect current error state; "
                        "error_state=%s(%d), trying to set to ACCEPT.\n",
                        error_state == AS_NORMAL ? "AS_NORMAL" :
                            error_state == AS_ACCEPT ? "AS_ACCEPT" : "UNKNOWN",
                        error_state );
                    }
                res = 1;
                }
            break;

        default:
            res = 2;
            break;
        }

    if ( G_DEBUG )
        {
        printf( "[errors] set_cmd: "
            "error_state = %d, trying to set to %s(%d).\n",
            error_state,
            cmd == C_CMD_SUPPRESS ? "SUPPRESS" :
                cmd == C_CMD_UNSET_SUPPRESS ? "UNSET_SUPPRESS" :
                    cmd == C_CMD_ACCEPT ? "ACCEPT" : "UNKNOWN", cmd );
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int tech_obj_error::save_as_Lua_str( char *str )
    {
    int res = 0;
    str[ 0 ] = 0;

    for ( u_int i = 0; i < tech_dev->get_errors().size(); i++ )
        {
        res += sprintf( str + res, "\t%s\n", "{" );

        res += sprintf( str + res, "\tdescription=\"%s\",\n",
            tech_dev->get_errors()[ i ]->msg );
        res += sprintf( str + res, "\tgroup=\"%s\",\n",
            get_group( tech_dev->get_errors()[ i ]->type ) );

        res += sprintf( str + res, "priority=%d%s",
            get_priority( tech_dev->get_errors()[ i ]->type ), "," );

        res += sprintf( str + res, "state=%d,\n", AS_ALARM );
        res += sprintf( str + res, "type=%d,\n", AT_SPECIAL );

        res += sprintf( str + res, "id_n=%d,\n",
            tech_dev->get_number() );
        res += sprintf( str + res, "id_object_alarm_number=%d,\n",
            tech_dev->get_errors()[ i ]->n );
        res += sprintf( str + res, "id_type=%d,\n", get_object_type() );

        res += sprintf( str + res, "suppress=false\n" );

        res += sprintf( str + res, "},\n" );
        }

    return res;
    }
//-----------------------------------------------------------------------------
void tech_obj_error::evaluate( bool &is_new_state )
    {
    if ( tech_dev->get_errors().size() != prev_size || was_set_cmd )
        {
        prev_size   = tech_dev->get_errors().size();
        was_set_cmd = false;

        is_new_state = true;
        }

    if ( !tech_dev->get_errors().empty() )
        {
        is_any_message = true;
        }
    }
//-----------------------------------------------------------------------------
int tech_obj_error::set_cmd( int cmd, int object_alarm_number )
    {
    auto &errors = tech_dev->get_errors();
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        if( errors[ i ]->n == object_alarm_number )
            {
            delete errors[ i ];
            errors[ i ] = 0;
            errors.erase( errors.begin() + i );

            if ( G_DEBUG )
                {
                printf( "Object \"%s\" (%s %d) set error cmd %d to error %d!\n",
                    tech_dev->get_name_in_Lua(),
                    tech_dev->get_name(), tech_dev->get_number(),
                    cmd, object_alarm_number );
                }
            was_set_cmd = true;
            return 0;
            }
        }
    if ( G_DEBUG )
        {
        printf( "Error! Object \"%s\" (%s %d) set error cmd - alarm number"
            " %d not found!\n",
            tech_dev->get_name_in_Lua(),
            tech_dev->get_name(), tech_dev->get_number(),
            object_alarm_number );
        }

    return 1;
    }
//-----------------------------------------------------------------------------
int tech_obj_error::get_priority( tech_object::ERR_MSG_TYPES err_type )
    {
    switch ( err_type )
        {
        case tech_object::ERR_CANT_ON:
        case tech_object::ERR_ON_WITH_ERRORS:
        case tech_object::ERR_CANT_ON_2_OPER:
        case tech_object::ERR_CANT_ON_2_OBJ:
            return P_ANSWER;

        case tech_object::ERR_OFF:
        case tech_object::ERR_OFF_AND_ON:
        case tech_object::ERR_DURING_WORK:
            return P_MESSAGE;

        case tech_object::ERR_ALARM:
        case tech_object::ERR_TO_FAIL_STATE:
            return P_ALARM;
        }

    return P_ALARM;
    }
//-----------------------------------------------------------------------------
const char* tech_obj_error::get_group( tech_object::ERR_MSG_TYPES err_type )
    {
    switch ( err_type )
        {
        case tech_object::ERR_CANT_ON:
        case tech_object::ERR_ON_WITH_ERRORS:
        case tech_object::ERR_CANT_ON_2_OPER:
        case tech_object::ERR_CANT_ON_2_OBJ:
            return "ответ";

        case tech_object::ERR_OFF:
        case tech_object::ERR_OFF_AND_ON:
        case tech_object::ERR_DURING_WORK:
            return "сообщение";

        case tech_object::ERR_ALARM:
        case tech_object::ERR_TO_FAIL_STATE:
            return "тревога";
        }

    return "?";
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
errors_manager::errors_manager(): errors_id( 0 )
    {
    }
//-----------------------------------------------------------------------------
int errors_manager::save_as_Lua_str( char *str, u_int_2 &id )
    {
    int res = 0;
    str[ 0 ] = 0;

    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        res += s_errors_vector[ i ]->save_as_Lua_str( str + res );
        }

    id = errors_id; //Через параметр возвращаем состояние ошибок.

    return res;
    }
//-----------------------------------------------------------------------------
/// @brief Обновление состояния ошибок.
void errors_manager::evaluate()
    {
    bool is_new_error_state = false;

    simple_error::is_any_error = false;
    simple_error::is_any_no_ack_error = false;
    simple_error::is_new_error = false;
    tech_obj_error::is_any_message = false;

    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        s_errors_vector[ i ]->evaluate( is_new_error_state );
        }

    if ( is_new_error_state )
        {
        errors_id++;        // Ошибки изменились.
        }
    }
//-----------------------------------------------------------------------------
int errors_manager::add_error( base_error* s_error )
    {
    s_errors_vector.push_back( s_error );
    return 0;
    }
//-----------------------------------------------------------------------------
int errors_manager::update_tech_object( tech_object* old_object,
    tech_object* new_tech_object )
    {
    if ( 0 == old_object || 0 == new_tech_object )
        {
        return -1;
        }

    for ( auto* cur_error : s_errors_vector )
        {
        auto* obj_error = dynamic_cast< tech_obj_error* >( cur_error );
        if ( 0 != obj_error && obj_error->tech_dev == old_object )
            {
            obj_error->tech_dev = new_tech_object;
            return 0;
            }
        }

    return -1;
    }
//-----------------------------------------------------------------------------
void errors_manager::print()
    {
    if ( G_DEBUG )
        {
        printf( "errors_manager\n" );

        for ( u_int i = 0; i < s_errors_vector.size(); i++ )
            {
            s_errors_vector[ i ]->print();
            }

        }
    }
//-----------------------------------------------------------------------------
void errors_manager::reset_errors_params()
    {
    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        s_errors_vector[ i ]->reset_errors_params();
        }
    }
//-----------------------------------------------------------------------------
void errors_manager::set_cmd( unsigned int cmd, unsigned int object_type,
                                 unsigned int object_number,
                                 unsigned int object_alarm_number )
    {
    base_error *res = 0;

    // Поиск нужного устройства.
    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        if ( s_errors_vector[ i ]->get_object_type() == object_type &&
            s_errors_vector[ i ]->get_object_n() == object_number )
            {
            res = s_errors_vector[ i ];
            break;
            }
        }

    // Выполнение команды.
    if ( res )
        {
        int result = res->set_cmd( cmd, object_alarm_number );
        if ( 0 == result )
            {
            errors_id++; // Состояние ошибок изменилось.
            }
        }
    else
        {
        if ( G_DEBUG )
            {
            printf(
                "[errors] set_cmd failed: object not found; "
                "cmd=%s(%u), type=%u, obj=%u, alarm=%u\n",
                cmd == base_error::C_CMD_ACCEPT ? "C_CMD_ACCEPT" :
                ( cmd == base_error::C_CMD_SUPPRESS ? "C_CMD_SUPPRESS" :
                    ( cmd == base_error::C_CMD_UNSET_SUPPRESS ?
                        "C_CMD_UNSET_SUPPRESS" : "UNKNOWN" ) ),
                cmd, object_type, object_number, object_alarm_number );
            }
        }
    }
//-----------------------------------------------------------------------------
errors_manager::~errors_manager()
    {
    clear();
    }
//-----------------------------------------------------------------------------
errors_manager* errors_manager::get_instance()
    {
    if ( static int is_init = 0; 0 == is_init )
        {
        instance = new errors_manager();
        is_init = 1;
        }

    return instance;
    }
//-----------------------------------------------------------------------------
int errors_manager::clear()
    {
    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        delete s_errors_vector.at( i );
        s_errors_vector.at( i ) = nullptr;
        }

    s_errors_vector.clear();
    errors_id = 0;

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void siren_lights_manager::eval()
    {
    if ( green == NULL || red == NULL || yellow == NULL || srn == NULL )
        {
        return;
        }

    if ( get_delta_millisec( st_time ) < 5000 )
        {
        critical_error_n = PAC_critical_errors_manager::get_instance()->get_id();
        return; //Задержка при включении контроллера.
        }

    if ( par[ P_MANUAL_MODE ] != 0 )
        {
        critical_error_n = PAC_critical_errors_manager::get_instance()->get_id();
        return; //Ручной режим для сирены и лампочек.
        }

    //Красный свет - аварии и тревоги.
    red->off();
    if ( PAC_critical_errors_manager::get_instance()->is_any_error() ||
        simple_error::is_any_error )
        {
        if ( is_red_built_in_blink )
            {
            red->on();
            }
        else
            {
            static int step = 0;
            static uint32_t start_blink_time = 0;
            static uint32_t start_wait_time = 0;

            switch ( step )
                {
                case 0:
                    red->on();
                    if ( get_delta_millisec( start_blink_time ) > 250 )
                        {
                        start_wait_time = get_millisec();
                        step = 1;
                        }
                    break;

                case 1:
                    //red->off();
                    if ( get_delta_millisec( start_wait_time ) > 250 )
                        {
                        start_blink_time = get_millisec();
                        step = 0;
                        }
                    break;
                }
            }
        }

    //Дополнительное включение сирены при появлении аварии (узлы Wago, ...).
    if ( PAC_critical_errors_manager::get_instance()->get_id() != critical_error_n )
        {
        critical_error_n = PAC_critical_errors_manager::get_instance()->get_id();
        srn->on();

        if ( G_DEBUG )
            {
            //Print( "PAC_critical_errors_manager::get_error_n() != critical_error_n\n" );
            }
        }

    //Дополнительное включение сирены при появлении тревоги (ошибки устройств).
    if ( simple_error::is_new_error )
        {
        srn->on();

        if ( G_DEBUG )
            {
            //Print( "base_error::is_new_active_alarm\n" );
            }
        }

    //Желтый свет - сообщения (технологические объекты).
    yellow->off();
    if ( tech_obj_error::is_any_message )
        {
        static int step = 0;
        static uint32_t start_blink_time = 0;
        static uint32_t start_wait_time = 0;

        switch ( step )
            {
            case 0:
                yellow->on();
                if ( get_delta_millisec( start_blink_time ) > 1000 )
                    {
                    start_wait_time = get_millisec();
                    step = 1;
                    }
                break;

            case 1:
                //yellow->Off();
                if ( get_delta_millisec( start_wait_time ) > 1000 )
                    {
                    start_blink_time = get_millisec();
                    step = 0;
                    }
                break;
            }
        }

    //Отключаем сирену, если нет аварий.
    if ( PAC_critical_errors_manager::get_instance()->is_any_error() == false &&
        false == simple_error::is_any_no_ack_error )
        {
        srn->off();
        }

    //Зеленая лампочка.
    green->off();
    if ( G_TECH_OBJECT_MNGR()->is_any_important_mode() )
        {
        green->on();
        }
    }
//-----------------------------------------------------------------------------
int siren_lights_manager::init( device *red, device *yellow, device *green,
                               device *srn )
    {
    this->green = green;
    this->red = red;
    this->yellow = yellow;
    this->srn = srn;

    return 0;
    }
//-----------------------------------------------------------------------------
int siren_lights_manager::save_device( char *buff ) const
    {
    int answer_size = sprintf( buff,
        "t.%s = \n"
        "\t{\n"
        "\tMANUAL_MODE=%d,\n"
        "\t}\n",
        "G_SIREN_MNGR", par[ P_MANUAL_MODE ] );

    return answer_size;
    }
//-----------------------------------------------------------------------------
int siren_lights_manager::set_cmd( const char *prop, u_int idx, double val )
    {
    if ( 0 == strcmp( prop, "MANUAL_MODE" ) )
        {
        par[ P_MANUAL_MODE ] = val > 0 ? 1 : 0;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
siren_lights_manager::siren_lights_manager() : par( run_time_params_u_int_4( 1 ) ),
    green( 0 ), red( 0 ),  yellow( 0 ), srn( 0 ),
    is_red_built_in_blink( 0 ), critical_error_n( 0 )
    {
    par[ P_MANUAL_MODE ] = 0;

    st_time = get_millisec();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
siren_lights_manager* G_SIREN_LIGHTS_MANAGER()
    {
    return siren_lights_manager::get_instance();
    }
//-----------------------------------------------------------------------------
#endif // PAC
