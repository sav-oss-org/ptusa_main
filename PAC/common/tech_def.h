/// @file tech_def.h
/// @brief Реализована базовая функциональность следующих объектов, описывающих
/// технологический процесс - танк (технологическая емкость),
/// гребенка (совокупность клапанов) и т.д.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef TECH_DEFINITION_H
#define TECH_DEFINITION_H

#include <stdio.h>
#include <vector>
#include <locale>

#include "i_tech_def.h"

#include "g_device.h"
#include "device/device.h"

#include "tcp_cmctr.h"
#include "param_ex.h"

#include "operation_mngr.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include    "lua.h"
#include    "lauxlib.h"
#include    "lualib.h"

#ifdef  __cplusplus
    };
#endif
#include    "tolua++.h"

//-----------------------------------------------------------------------------
/// @brief Работа с технологическим объектом.
///
/// Базовый класс для технологического объекта (танка, гребенки). Содержит
/// основные методы работы - работа с режимами и т.д.
/// Для информировании сервера (события, аварии, ...) используется следующий
/// подход: имеется вектор из сообщений и методы работы с ним.
///
class tech_object: public i_tech_object, public i_Lua_save_device,
    public i_params_owner
    {
    public:
        /// @param name                     - название ("Гребенка", ...).
        /// @param number                   - номер.
        /// @param type                     - тип.
        /// @param name_Lua                 - имя в Lua.
        /// @param states_count             - количество режимов.
        /// @param timers_count             - количество таймеров.
        /// @param par_float_count          - количество сохраняемых параметров типа float.
        /// @param runtime_par_float_count  - количество рабочих параметров типа float.
        /// @param par_uint_count           - количество сохраняемых параметров типа uint.
        /// @param runtime_par_uint_count   - количество рабочих параметров типа uint.
        tech_object( const char* name, u_int number, u_int type, const char* name_Lua,
            u_int states_count,
            u_int timers_count,
            u_int par_float_count, u_int runtime_par_float_count,
            u_int par_uint_count, u_int runtime_par_uint_count );

        virtual ~tech_object();

        /// @brief Включение/выключение режима.
        ///
        /// @param mode      - режим.
        /// @param new_state - новое состояние режима.
        ///
        /// @return  0 - режим включен (отключен).
        /// @return  1 - режим ранее был включен (отключен).
        /// @return  3 - нет такого режима.
        int set_mode( u_int mode, int new_state );

        /// @brief Получение состояния режима.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим включен.
        /// @return 0 - режим не включен.
        int get_mode( u_int mode );

        /// @brief Получение состояния операции.
        ///
        /// @param mode - режим.
        ///
        /// @return ... - режим в ...
        /// @return 2 - режим в паузе.
        /// @return 1 - режим включен.
        /// @return 0 - режим не включен.
        int get_operation_state( u_int mode ) const;

        virtual int  evaluate();

        /// @brief Проверка возможности включения режима.
        ///
        /// Если данный метод возвращает 1, то тогда режим не включается.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим нельзя включить.
        /// @return 0 - режим можно включить.
        int check_on_mode( u_int mode, char* reason, int max_len );

        /// @brief Проверка возможности выключения режима.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим нельзя выключить.
        /// @return 0 - режим можно выключить.
        int check_off_mode( u_int mode );

        /// @brief Завершение режима.
        ///
        /// При завершении режима выполнение нужных действий -
        /// выключение маршрута, включение/выключение клапанов и т.д.
        ///
        /// @param mode - режим.
        ///
        /// @return 0 - ок.
        int final_mode( u_int mode );

        /// @brief Инициализирует сохраняемые параметры значением 0.
        ///
        /// Данные нулевые значения сохраняются в энергонезависимой памяти.
        virtual int init_params();

        /// @brief Инициализирует рабочие параметры значением 0.
        virtual int init_runtime_params();

        /// @brief Выполнение команды.
        ///
        /// Здесь могут выполняться какие-либо действия (включаться/выключаться
        /// другие режимы).
        ///
        int exec_cmd( u_int cmd )
            {
            if ( G_DEBUG )
                {
                printf ( "\'%s\' - exec command command = %2d\n",
                    name, cmd );
                }
            return 0;
            }

        /// @brief Запрос отсутствия выполняющихся режимов.
        ///
        bool is_idle() const;

        /// @brief Включен ли хотя бы один важный режим технологического объекта.
        bool is_any_important_mode();

        int get_number() const
            {
            return number;
            }

        u_int get_modes_count() const
            {
            return operations_count;
            }

        /// @brief Проверка на возможность включения операции.
        ///
        int check_operation_on( u_int operation_n, bool show_error = true );

        saved_params_float      par_float;      ///< Сохраняемые параметры, тип float.
        run_time_params_float   rt_par_float;   ///< Рабочие параметры, тип float.
        saved_params_u_int_4    par_uint;       ///< Сохраняемые параметры, тип u_int.
        run_time_params_u_int_4 rt_par_uint;    ///< Рабочие параметры, тип u_int.

        timer_manager           timers;         ///< Таймеры объекта.

        // Lua implemented methods.
        int lua_exec_cmd( u_int cmd );

        int lua_get_run_step_after_pause( u_int mode ) const;

        // Check functions.
        int lua_check_function( const char* function_name, const char* comment,
            u_int mode, bool show_error );
        int lua_check_on_start( u_int mode, bool show_error = true );
        int lua_check_on_pause( u_int mode, bool show_error = true );
        int lua_check_on_stop( u_int mode, bool show_error = true );

        int  lua_check_on_mode( u_int mode, bool show_error = true );
        void lua_init_mode( u_int mode );

        int  lua_check_off_mode( u_int mode );
        int  lua_final_mode( u_int mode );
        int  lua_init_params();
        int  lua_init_runtime_params();

        int lua_on_start( u_int mode );
        int lua_on_pause( u_int mode );
        int lua_on_stop( u_int mode );
        // Lua implemented methods.

#ifndef __GNUC__
#pragma region Работа с ошибками.
#endif

        /// @brief Проверка необходимости проверки устройств на ошибку обратной
        /// связи перед включением режима.  Реализован на Lua.
        ///
        /// @return 0 - не надо проверять.
        /// @return 1 - надо проверять.
        virtual int is_check_mode( int mode ) const;

        struct err_info
            {
            enum CONSTANTS
                {
                MAX_STR_LENGTH = 600,
                };

            char          msg[ MAX_STR_LENGTH ];
            int           n;
            ERR_MSG_TYPES type;
            };

        int set_err_msg( const char *err_msg, int mode, int new_mode = 0,
            ERR_MSG_TYPES type = ERR_CANT_ON );

        static const char* get_type( ERR_MSG_TYPES err_type );
        static int get_priority( ERR_MSG_TYPES err_type );

         std::vector< err_info* >& get_errors()
             {
             return errors;
             }

         int get_object_type()
             {
             return type;
             }

         /// @brief Наличие активных событий.
         ///
         bool is_any_message() const
             {
             return !errors.empty();
             }

         /// @brief Наличие активных аварий.
         ///
         bool is_any_error() const;

#ifndef __GNUC__
#pragma endregion
#endif

        /// @brief Отладочная печать объекта.
         void print() const;

        const char* get_name_in_Lua() const
            {
            static char tmp[ 100 ];
            sprintf( tmp, "%s", name_Lua );
            return tmp;
            }

        const char* get_name() const
            {
            return name;
            }

        const char* get_full_name() const
            {
            return full_name;
            }

        //Получение параметров для шага. Для старого описания используем
        //параметры par_uint, для нового описания вместо них используем
        //par_float.
        float get_step_param( u_int idx ) const
            {
            if ( idx >= 1 )
                {
                if ( par_uint.get_count() >= idx )
                    {
                    return (float)par_uint[ idx ];
                    }
                else
                    {
                    if ( par_float.get_count() >= idx )
                        {
                        return par_float[ idx ];
                        }
                    }
                }

            return -1.;
            }

        const saved_params_float* get_params() const
            {
            return &par_float;
            }

        operation_manager* get_modes_manager()
            {
            return operations_manager;
            }

        int save_device( char *buff ) const override;

        virtual int set_cmd( const char *prop, u_int idx, double val );

        int save_params_as_Lua_str( char* str );

        int set_param( int par_id, int index, double value );

        /// @brief Установка последовательного номера объекта, начинается с 1.
        ///
        /// @param idx - последовательный номер, >= 1.
        void set_serial_idx( u_int idx );

        /// @brief Получение последовательного номера объекта (с 1).
        ///
        /// Это глобальный порядковый номер объекта - ключ [N] в
        /// main.objects.lua, он же суффикс имени OBJECTn.
        u_int get_serial_idx() const
            {
            return serial_idx;
            }

        std::string lastLoadedRecipeName = "Не выбран";
        int lastLoadedRecipeNmr = 0;

    protected:
        u_int serial_idx;           ///< Последовательный индекс объекта (с 1).

        u_int   number;             ///< Номер объекта.
        u_int   type;               ///< Тип объекта (для ошибок).
        u_int_4 cmd;                ///< Хранение команды объекта.
        u_int   operations_count;   ///< Количество операций.

        std::vector< u_int_4 >  state;          ///< Состояние объекта (битовое).
        std::vector< u_int_4 >  available;      ///< Доступность операций.

        char *name;        ///< Имя объекта.
        char* full_name;   ///< Имя объекта + номер.
        char *name_Lua;    ///< Имя объекта в Lua.

        mutable smart_ptr< operation_manager > operations_manager; ///< Шаги режимов.

        enum PARAMS_ID
            {
            ID_PAR_FLOAT = 1,
            ID_RT_PAR_FLOAT,
            ID_PAR_UINT,
            ID_RT_PAR_UINT,
            };

        // Работа с ошибками.
        enum ERRORS_CONSTANTS
            {
            E_MAX_ERRORS_SIZE = 10,
            };

        std::vector< err_info* > errors;
        // Работа с ошибками.

        //Проверка на ограничения операции.
        void check_availability( u_int operation_n );

        int set_extra_step( u_int operation, int step, int cmd );
    };
//-----------------------------------------------------------------------------
class tech_object_manager
    {
    public:
        /// @brief Инициализация сохраняемых параметров всех технологических
        /// объектов.
        int init_params();

        /// @brief Инициализация рабочих параметров всех технологических
        /// объектов.
        int init_runtime_params();

        /// @brief Получение единственного экземпляра класса.
        static tech_object_manager* get_instance();

        /// @brief Получение объекта с заданным активным режимом.
        int get_object_with_active_mode( u_int mode, u_int start_idx,
            u_int end_idx );

        /// @brief Получение объекта по номеру.
        tech_object* get_tech_objects( u_int idx );

        /// @brief Получение количества объектов.
        u_int get_count() const;

        /// @brief Выполнение итерации технологического процесса.
        int evaluate();

        /// @brief Инициализация объектов на основе скрипта описания Lua.
        virtual int init_objects();

        virtual ~tech_object_manager();

        /// @brief Добавление технологического объекта.
        void add_tech_object( tech_object* new_tech_object );

        /// @brief Горячая перезагрузка технологического объекта.
        ///
        /// Новый технологический объект строится Lua-функцией
        /// reload_tech_object (см. sys/sys.objects.lua), затем заменяется
        /// в менеджере, коммуникаторе устройств и реестре ошибок без
        /// остановки остальных объектов.
        ///
        /// @param serial_number - глобальный порядковый номер объекта (ключ [N]
        /// в main.objects.lua, он же суффикс имени OBJECTn).
        ///
        /// @return 0 - объект перезагружен.
        /// @return -1 - объект с номером не найден.
        /// @return -2 - объект не в простое, перезагрузка невозможна.
        /// @return -3 - ошибка построения нового объекта в Lua.
        int reload_object( u_int serial_number );

        int save_params_as_Lua_str( char* str );

        /// @brief Включен ли хотя бы один важный режим технологического объекта.
        bool is_any_important_mode()
            {
            for ( u_int i = 0; i < tech_objects.size(); i++ )
                {
                if ( tech_objects[ i ]->is_any_important_mode() )
                	{
                    return true;
                	}
                }

            return false;
            }

#ifdef __BORLANDC__
#pragma option -w-inl
#endif // __BORLANDC__

        /// @brief Отладочная печать объекта.
        void print()
            {
            printf( "Technological objects manager [%zu]:\n",
                tech_objects.size() );

            for ( u_int i = 0; i < tech_objects.size(); i++ )
                {
                tech_objects[ i ]->print();

#ifdef KEY_CONFIRM
                printf( "Press any key to continue..." );
                Getch();
                printf( "\n" );
#endif // KEY_CONFIRM
                }
            }

#ifdef __BORLANDC__
#pragma option -w.inl
#endif // __BORLANDC__

    protected:
        tech_object_manager();

    private:
                /// Единственный экземпляр класса.
        static auto_smart_ptr < tech_object_manager > instance;

        std::vector< tech_object* > tech_objects; ///< Технологические объекты.

        tech_object* stub;
    };
//-----------------------------------------------------------------------------
tech_object_manager* G_TECH_OBJECT_MNGR();
//-----------------------------------------------------------------------------
tech_object* G_TECH_OBJECTS( u_int idx );
//-----------------------------------------------------------------------------
#endif // TECH_DEFINITION_H
