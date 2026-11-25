/// @file errors.h
/// @brief Содержит описания перечислений, которые используются в
///  драйвере на сервере для организации информирования об ошибка.
///
/// Класс @ref base_error, служащий для организации работы с ошибкой, содержит
/// всю необходимую информацию. Для хранения всех ошибок служит класс
/// @ref errors_manager.
///
/// @par Описание директив препроцессора:
/// @c PAC - компиляция для контроллера.@n
/// @c DRIVER - компиляция для драйвера (ОС Windows).
///


#ifndef ERRORS_H
#define ERRORS_H

#if !( defined PAC_PC || defined PAC_PLCNEXT ) && \
    !defined DRIVER
#error You must define type!
#endif

#if ( defined PAC_PC || defined PAC_PLCNEXT ) && \
    !defined PAC
#define PAC
#endif

//-----------------------------------------------------------------------------
#if defined PAC || defined DRIVER
enum ALARM_STATE
    {
    AS_NORMAL,
    AS_ALARM,
    AS_RETURN,
    AS_ACCEPT,
    };

enum ALARM_TYPE
    {
    AT_DISCRETE,
    AT_VALUE,
    AT_DEVIATION,
    AT_RATE_OF_CHANGE,
    AT_SPECIAL,
    };

enum ALARM_CLASS_PRIORITY
    {
    P_FATAL         = 0,
    P_ERR_CONNECTION = 100,
    P_ALARM          = 250,
    P_MESSAGE        = 500,
    P_ANSWER         = 750,
    P_REMIND         = 1000,
    };

#endif // defined PAC || defined WIN32
//-----------------------------------------------------------------------------
#ifdef DRIVER
#include <string.h>

typedef unsigned char UCHAR;
typedef unsigned int  UINT;

#pragma pack( push, 8 ) //Выравнивание полей структур по 8 байт.

/// @brief Глобальный идентификатор тревоги.
struct alarm_id
    {
    int object_type;
    int object_number;
    int object_alarm_number;
    };
//-----------------------------------------------------------------------------
struct alarm_params
    {
    double  param1;
    double  param2;
    double  param3;
    double  param4;
    double  param5;
    double  param6;
    double  param7;
    double  param8;
    double  param9;
    double  param10;
    };

struct alarm
    {
    alarm_params params;

    //     atDiscrete      - дискретная ( true/false )
    //     atValue         - контроля значения ( Lo/LoLo, Hi/HiHi )
    //     atDeviation     - отклонения ( MinValue/MaxValue )
    //     atRateOfChange  - изменения скорости ( speed )
    //     atSpecial       - специальная
    ALARM_TYPE type;    ///< Тип тревоги.

    char *description;  ///< Описание тревоги.
    UCHAR enable;       ///< Блокировка тревоги на этапе проектирования.
    char *group;        ///< Определяет принадлежность тревоги какой либо группе тревог.
    UCHAR  inhibit;     ///< Блокировка тревоги во время работы.

    //  Приоритеты тревоги:
    //     0       - системные
    //     1-249   - критические
    //     250-499 - важные
    //     500-749 - маловажные
    //     750-999 - информационные
    int priority; ///< Приоритет тревоги ( 0 - 999 ).

    //     asNormal    - тревоги нет
    //     asAlarm     - тревога есть
    //     asReturn    - контролируемое значение
    //                вернулось в нормальное
    //                состояние
    //     asAccept    - тревога подтверждена
    ALARM_STATE state;  ///< Состояние тревоги.

    bool suppress;      ///< Подавление тревоги клиентами.
    alarm_id id;        ///< Глобальный идентификатор тревоги.
    UCHAR driver_id;    ///< id драйвера.
    };

struct all_alarm
    {
    int     cnt;
    alarm   *alarms;
    int     id;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct error_cmd
    {
    int cmd;
    int object_type;
    int object_number;
    int object_alarm_number;
    };

enum ERR_CONSTANTS
    {
    MAX_PROJECTS_CNT = 256,
    MAX_ALARMS_CNT   = 200,

    MAX_DESCR_LEN    = 600,
    MAX_GROUP_LEN    = 50,
    };

int save_to_stream( alarm &a, char *buff );
int load_from_stream( alarm &a, char *buff );

#endif // DRIVER
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef PAC
#include "param_ex.h"
#include "smart_ptr.h"
#include "device/i_tech_dev_error_device.h"

#include "tech_def.h"

#include "fmt/format.h"

class device;

//-----------------------------------------------------------------------------
/// @brief Базовый класс с информацией об ошибке устройства.
///
class base_error
    {
    public:
        base_error();

        virtual ~base_error() = default;

        /// @brief Сохранение ошибки в поток для передачи на сервер.
        ///
        /// @param stream       - поток байт.
        /// @param is_new_state - изменилось ли состояние ошибки
        ///                       (0 - нет, 1 - да).
        ///
        /// @return - количество записанных байт.
        virtual int save_as_Lua_str( char *str ) = 0;

        /// @brief Обновление состояния ошибок.
        /// @param is_new_state - изменилось ли состояние ошибки
        ///                       (0 - нет, 1 - да).
        virtual void evaluate( bool &is_new_state ) = 0;

        /// @brief Отладочный вывод содержимого класса в консоль.
        virtual void print() const = 0;

        /// @brief Получение типа объекта.
        virtual unsigned char get_object_type() const = 0;

        /// @brief Получение номера объекта.
        virtual unsigned int get_object_n() const = 0;

        /// @brief Выполнение команды над ошибкой.
        virtual int set_cmd( int cmd, int object_alarm_number ) = 0;

        /// @brief Сброс параметров ошибки в значение по умолчанию (0).
        void reset_errors_params()
            {
            err_par[ P_PARAM_N ] = 0;
            }

        enum PARAMS  ///< Параметры ошибки, определяют номера битов.
            {
            P_PARAM_N = 1,	  //Номер параметра.

            P_IS_ENABLE = 1,  ///< Блокировка тревоги на этапе проектирования.
            P_IS_INHIBIT = 2, ///< Блокировка тревоги во время работы.
            P_IS_SUPPRESS = 4,///< Подавление тревоги клиентами.
            };

        enum COMMANDS                ///< Константы.
            {

            C_CMD_ACCEPT   = 100,    ///< Подтвердить ошибку.
            C_CMD_SUPPRESS = 200,    ///< Подавить ошибку.
            C_CMD_UNSET_SUPPRESS,    ///< Убрать подавление ошибки.
            };

    protected:
        saved_params_u_int_4 err_par;

        unsigned char error_state;    ///< Состояние ошибки.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию о простой ошибке ( Modbus-клиент,
/// простое устройство - клапан, насос и т.п.).
///
/// У простого устройства может быть только одна ошибка (ошибка обратной
/// связи).
class simple_error: public base_error
    {
    friend class siren_lights_manager;
    friend class errors_manager;

    public:
        simple_error( i_simple_error* simple_error_owner );
        virtual ~simple_error() = default;


        int save_as_Lua_str( char *str );

        void evaluate( bool &is_new_state );

        /// @brief Отладочный вывод содержимого класса в консоль.
        void print() const;

        /// @brief Получение типа объекта.
        unsigned char get_object_type() const;

        /// @brief Получение номера объекта.
        unsigned int get_object_n() const;

        /// @brief Выполнение команды над ошибкой.
        int set_cmd( int cmd, int object_alarm_number );

    protected:
        bool static is_any_error;        ///< Наличие тревоги.
        bool static is_any_no_ack_error; ///< Наличие неподтвержденной тревоги.
        bool static is_new_error;        ///< Наличие новой тревоги.

    private:
        i_simple_error* simple_error_owner;     ///< Владелец ошибки.
        int prev_error_id = 0;                  ///< Предыдущая ошибка.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об ошибке сложного устройства (танк,
/// гребенка...).
///
/// У сложного устройства может быть несколько ошибок (сообщение, ответ, ...).
class tech_obj_error: public base_error
    {
    friend class errors_manager;
    friend class siren_lights_manager;

    public:
        // Интерфейс base_error.
        tech_obj_error( tech_object* tech_dev ): tech_dev( tech_dev ),
            prev_size( 0 ), was_set_cmd( false )
            {
            }

        int save_as_Lua_str( char *str );

        void evaluate( bool &is_new_state );

        void print() const
            {
#ifdef DEBUG
            printf( "%s\n",
                tech_dev->get_name_in_Lua() );
#endif // DEBUG
            }

        unsigned char get_object_type() const
            {
            return TE_TYPE + tech_dev->get_object_type();
            }

        unsigned int get_object_n() const
            {
            return tech_dev->get_number();
            }

        static const char* get_group( tech_object::ERR_MSG_TYPES err_type );

        static int get_priority( tech_object::ERR_MSG_TYPES err_type );

        // Реализована следующая обработка любой команды - удаление сообщения из
        // вектора, для которого адресована команда.
        int set_cmd( int cmd, int object_alarm_number );

    private:
        tech_object* tech_dev; ///< Сложное устройство.

        enum TECH_DEV_ERROR_CONST   ///< Константы.
            {
            TE_TYPE = 100,          ///< Тип ошибки.
            };

        u_int prev_size;
        bool was_set_cmd;

        static bool is_any_message;
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об всех ошибках технологических устройств,
/// объектов.
class errors_manager
    {

    public:
        ~errors_manager();

        /// @brief Сохранение всех ошибок в поток для передачи на сервер.
        ///
        /// @param stream - поток байт.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ОК.
        int save_as_Lua_str( char *str, u_int_2 &id );

        /// @brief Обновление состояния ошибок.
        void evaluate();

        /// @brief Добавление ошибки в массив ошибок.
        ///
        /// @param s_error - добавляемая ошибка.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ОК.
        int add_error( base_error *s_error );

        /// @brief Сброс параметров всех ошибок в значение по умолчанию (0).
        void reset_errors_params();

        /// @brief Отладочный вывод содержимого класса в консоль.
        void print();

        /// @brief Изменение параметров ошибки.
        void set_cmd( unsigned int cmd, unsigned int object_type,
            unsigned int object_number, unsigned int object_alarm_number );

        /// @brief Получение единственного экземпляра класса.
        static errors_manager* get_instance();

        /// @brief Очистка всех связанных устройств.
        int clear();

        u_int_2 get_errors_id() const
            {
            return errors_id;
            }

    private:
        u_int_2 errors_id; // Состояние ошибок.

        /// Единственный экземпляр класса.
        static auto_smart_ptr < errors_manager > instance;

        /// @param s_errors_cnt - количество ошибок.
        errors_manager();

        enum DEM_CONST
            {
            DEM_MAX_ERRORS_CNT = 29,
            };

        std::vector< base_error* > s_errors_vector;    ///< Массив ошибок.
    };

//Совместимость с предыдущей версией драйвера EasyDrv. FIXME.
#define dev_errors_manager errors_manager
//-----------------------------------------------------------------------------
#define G_ERRORS_MANAGER errors_manager::get_instance()
//-----------------------------------------------------------------------------
class siren_lights_manager: public i_Lua_save_device
    {
    public:
        static siren_lights_manager* get_instance()
            {
            if ( instance.is_null() )
                {
                instance = new siren_lights_manager();
                }

            return instance;
            }

        /// @brief Задание типа мигания для красной лампочки.
        ///
        /// @param type - 0 - реализуем сами, 1 - встроенный в сирену.
        void set_red_blink( int type )
            {
            is_red_built_in_blink = type;
            }

        int init( device *red, device *yellow, device *green, device *srn );

        void eval();

        enum PARAMS
            {
            P_MANUAL_MODE = 1,
            };

        int save_device( char *buff ) const override;

        int set_cmd( const char *prop, u_int idx, double val );

        /// @brief Отладочная печать объекта в консоль.
        const char* get_name_in_Lua() const override
            {
            return "G_SIREN_MNGR";
            }

        virtual ~siren_lights_manager()
            {
            }

    private:
        siren_lights_manager();

        run_time_params_u_int_4 par;

        device *green;
        device *red;
        device *yellow;

        device *srn;

        ///Тип мигания (0 - реализуем сами, 0< - встроенный в сирену).
        int is_red_built_in_blink;

        unsigned int  critical_error_n; // Номер текущей критической ошибки.
        uint32_t st_time;

        /// Единственный экземпляр класса.
        static auto_smart_ptr< siren_lights_manager > instance;
    };
//-----------------------------------------------------------------------------
siren_lights_manager* G_SIREN_LIGHTS_MANAGER();
//-----------------------------------------------------------------------------
#endif // PAC

#endif //ERRORS_H
