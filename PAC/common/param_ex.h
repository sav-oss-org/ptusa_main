/// @file param_ex.h
/// @brief Содержит описания классов, которые реализуют работу с параметрами.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c KEY_CONFIRM - переход к следующему отладочному сообщению по нажатии
/// клавиши.@n@n
/// @c USE_SIMPLE_DEV_ERRORS   - компиляция с модулем ошибок устройств.@n
/// @c USE_NO_TANK_COMB_DEVICE - компиляция без объектов Танк и Гребенка. Для
/// уменьшения размера программы при отсутствии таковых технологических
/// объектов.
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PARAMS_EX_H
#define PARAMS_EX_H

#ifdef WIN_OS
#include <locale.h>
#endif //WIN_OS

#include <array>
#include <cstddef>
#include <math.h>
#include <string.h>

#include "base_mem.h"
#include "g_device.h"
#include "log.h"

#ifdef __BORLANDC__
#pragma option -w-inl
#pragma option -w-ccc
#pragma option -w-rch
#endif // __BORLANDC__

class saved_params_u_int_4;
//-----------------------------------------------------------------------------
class i_params_owner
    {
    public:
        virtual const char* get_full_name() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами.
///
/// Служит для создания объектов для работы с параметрами. Проверяется
/// корректность данных путем подсчета контрольной суммы.
class params_manager
    {
    // Friendly класс предназначен только для тестирования
    // и не должен использоваться в других целях
#ifdef PTUSA_TEST
    friend class test_params_manager;
#endif
    public:
        enum class CONSTANTS
            {
            C_TOTAL_PARAMS_SIZE =
#ifdef PTUSA_TEST
                1024 * 100,     ///< Общий размер памяти параметров для тестов.
#else
                1024 * 30,      ///< Общий размер памяти параметров.
#endif

            C_SYS_MEM_SIZE = 10 ///< Память для хранения CRC и т.д.
            };

        /// @brief Возвращает единственный экземпляр класса для работы с
        /// параметрами.
        ///
        /// @return - указатель на единственный объект класса @ref
        /// params_manager.
        static params_manager* get_instance();

        /// @brief Инициализация значений параметров.
        ///
        /// Считывание параметров из EEPROM в массив параметров, проверка
        /// контрольной суммы. Контрольная сумма зависит от
        /// параметра @ref C_TOTAL_PARAMS_SIZE.
        ///
        /// @param project_id - номер проекта. Для привязки параметров к
        /// конкретной управляющей программе.
        ///
        /// @return 0 - ОК.
        /// @return 1 - Ошибка контрольной суммы.
        virtual int init( unsigned int project_id );

        /// @brief Окончательная инициализация значений параметров.
        ///
        /// Если при считывании параметров из EEPROM произошла ошибка (метод
        /// @ref init), параметры задаются значениями по умолчанию
        /// путем вызова заданных функций.
        ///
        /// @param auto_init_params - вызывать ли функцию init_params классов
        /// tank и comb.
        /// @param auto_init_work_params - вызывать ли функцию init_work_params
        /// классов tank и comb.
        /// @param custom_init_params_function - пользовательская функция
        /// инициализации параметров.
        virtual void final_init( int auto_init_params = 1,
            int auto_init_work_params = 1,
            void ( *custom_init_params_function )() = 0 );

        /// @brief Запись параметров в EEPROM.
        ///
        /// Устанавливается флаг изменения параметров и обновляется время
        /// последней записи.
        ///
        void save();

        /// @brief Получение указателя на блок данных параметров.
        ///
        /// @param size - размер блока данных в байтах.
        /// @param [out] start_pos - стартовый индекс в глобальном массиве
        /// параметров. Необходим для дальнейшей записи параметров в память.
        ///
        /// @return 0 - ОК.
        /// @return 1 - Ошибка контрольной суммы.
        virtual std::byte* reserve_params_region( int size, int &start_pos );

        virtual ~params_manager();

        int save_params();

        int evaluate();

        enum PARAMS
            {
            P_COUNT = 1,

            P_IS_RESET_PARAMS = 1,
            };

        saved_params_u_int_4 *par;

        int save_params_as_Lua_str( char* str );

        int restore_params_from_server_backup( char *backup_str );

        // Высчитывание контрольной суммы.
        u_int_2 solve_CRC();

        void reset_CRC_mem();

        int get_params_change_counter() const;

        int get_params_save_counter() const;

        /// @brief Результат инициализации параметров.
        ///
        /// @return true - инициализация прошла успешно.
        /// @return false - инициализация завершилась с ошибкой.
        bool was_successful_init() const;

#ifndef PTUSA_TEST
    protected:
#endif // !PTUSA_TEST

        static char is_init;

        /// @brief Закрытый конструктор.
        ///
        /// Для вызова методов используется статический метод @ref get_instance.
        params_manager();

	private:
        void reset_to_default( void( *custom_init_params_function )( ),
            int auto_init_params, int auto_init_work_params );

        /// Статический экземпляр класса для вызова методов.
        static auto_smart_ptr< params_manager > instance;

        /// Номер последнего выделенного параметра. Используется при создании
        /// экземпляра класса @ref parameters.
        u_int last_idx;

        u_int project_id;   ///< Номер проекта (для уникальности параметров).

        i_memory* params_mem; ///< Память параметров.
        i_memory* CRC_mem;    ///< Память контрольной суммы.

        /// Инициализируем начальное время - запись параметров произойдет
        /// через заданное время после запуска программы (при наличии
        /// изменений).
        uint32_t last_change_ms{ get_millisec() };
        uint32_t last_save_ms{ get_millisec() };
        bool is_changed{ false };

        int params_change_counter{ 0 };
        int params_save_counter{ 0 };

        ///< Флаг успешной инициализации параметров.
        bool successful_init{ false };
    };
//-----------------------------------------------------------------------------
/// @brief Работа с массивом параметров.
///
/// Служит для создания конкретных типов параметров. Реализованы операции
/// доступа через индекс ( [] ).

template < class type, bool is_float > class parameters
    {
    private:
        char name[ 20 ];
        i_params_owner* owner;

    public:
        /// @brief Получение элемента через операцию индексирования.
        ///
        /// @param index - индекс элемента.
        ///
        /// @return - значение элемента с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// stub ( значение 0 ).
        type& operator[] ( unsigned int index )
            {
            if ( index <= count && index > 0 )
                {
                return values[ index - 1 ];
                }

            print_msg_out_of_range( index, "access" );
            return stub;
            }

        /// @brief Получение элемента через операцию индексирования.
        ///
        /// @param index - индекс элемента.
        ///
        /// @return - значение элемента с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// stub ( значение 0 ).
        const type& operator[] ( unsigned int index ) const
            {
            if ( index <= count && index > 0 )
                {
                return values[ index - 1 ];
                }

            print_msg_out_of_range( index, "access" );
            return stub;
            }
        /// @brief Получение элемента через индекс.
        ///
        /// @param idx - индекс элемента.
        ///
        /// @return - значение элемента с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// stub ( значение 0 ).
        type get_val( int idx ) const
            {
            return this->operator []( idx );
            }

        /// @brief Установка всех параметров в нулевое значение.
        virtual void reset_to_0()
            {
            for ( u_int i = 1; i <= count; i++ )
                {
                values[ i - 1 ] = 0;
                }
            }

        /// @brief Получение количества параметров.
        ///
        /// @return - количество параметров.
        u_int get_count() const
            {
            return count;
            }

        parameters( int count, const char* name, i_params_owner* owner ) :
            parameters( count, name, 0, owner )
            {
            }

        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param name  - имя объекта.
        /// @param value - указатель на буфер для хранения значений параметров.
        parameters( int count, const char *name, type *value = 0,
            i_params_owner* owner = 0 ): count( count ),
            values( value ), owner( owner )
            {
            stub = 0;
            this->name[ 0 ] = 0;
            strcpy( this->name, name );

#ifdef DEBUG_IDE
            if ( 0 == count )
                {
                printf( "\"%s\" parameters(...) - error: count = 0!\n",
                    owner ? owner->get_full_name() : "?" );
                }
#endif // DEBUG_IDE

            is_delete = 0;
            if ( count > 0 && 0 == values )
                {
                is_delete = 1;
                values = new type[ count ];
                memset( values, 0, count * sizeof( type ) );
                }
            }

        virtual ~parameters()
            {
            if ( 1 == is_delete && count && values )
                {
                delete [] values;
                values = nullptr;
                count = 0;
                }
            }

        virtual void print() const
            {
            printf( "\"%s\"\t - ", name );
            for ( u_int i = 1; i <= count; i++ )
                {
                printf( "[%u]=", i );
                if ( is_float )
                	{
                    printf( "%.2f,", ( float ) values[ i - 1 ] );
                	}
                else
                    {
                    printf( "%d,", ( int ) values[ i - 1 ] );
                    }
                }
            printf( "\n" );
            }

        int save_device_ex( char *buff, const char *prefix, const char *new_name ) const
            {
            int res = sprintf( buff, "%s%s=\n", prefix, new_name );
            res += save_dev( buff + res, prefix );

            return res;
            }

        int save_device( char *buff, const char *prefix ) const
            {
            int res = sprintf( buff, "%s%s=\n", prefix, name );
            res += save_dev( buff + res, prefix );

            return res;
            }

        const char* get_name()
            {
            return name;
            }

    protected:
        char is_delete; ///< Признак удаления буфера при удалении объекта.

        /// @brief Получение указателя на буфер для хранения значений
        /// параметров.
        ///
        /// @return - указатель на буфер для хранения значений параметров.
        type* get_values()
            {
            return values;
            }

        /// Заглушка для обращения через индекс с выходом за диапазон.
        type         stub;

        unsigned int count;     ///< Количество элементов.
        type         *values;   ///< Указатель на массив значений элементов.

        void print_msg_out_of_range( unsigned int index, const char* action ) const
            {
            if ( G_DEBUG )
                {
                if ( 0 == index )
                    {
                    G_LOG->warning( "\"%s\" \"%s\" parameters %s error: "
                        "index = %u",
                        owner ? owner->get_full_name() : "?",
                        name, action, index );
                    }
                else
                    {
                    G_LOG->warning( "\"%s\" \"%s\" parameters %s error: "
                        "index > size (%u > %u)",
                        owner ? owner->get_full_name() : "?",
                        name, action, index, count );
                    }
                }
            }

    private:
        int save_dev( char *buff, const char *prefix ) const
            {
            int answer_size = sprintf( buff, "%s\t{\n%s\t",
                prefix, prefix );

            for ( u_int i = 1; i <= count; i++ )
                {
                if ( is_float )
                    {
                    float val  = ( float ) get_val( i );
                    if ( 0. == val )
                        {
                        answer_size += sprintf( buff + answer_size, "0, " );
                        }
                    else
                        {
                        double tmp;
                        if ( modf( val, &tmp ) == 0 )
                            {
                            answer_size += sprintf( buff + answer_size, "%d, ",
                                ( int ) val );
                            }
                        else
                            {
                            answer_size += sprintf( buff + answer_size, "%.2f, ",
                                val );
                            }
                        }
                    }
                else
                    {
                    answer_size += sprintf( buff + answer_size, "%u, ",
                        ( u_int ) get_val( i ) );
                    }
                }

            answer_size += sprintf( buff + answer_size, "\n%s\t},\n", prefix );
            return answer_size;
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами времени выполнения типа float.
///
/// Данные параметры передаются на сервер через соответствующие теги.
class run_time_params_float: public parameters < float, true >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        run_time_params_float( int count, i_params_owner* owner = 0 ):parameters < float, true >( count, "RT_PAR_F", owner )
            {
            }

    protected:

        float get_val( int idx ) const
            {
            return parameters< float, true >::get_val( idx );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами времени выполнения типа @ref u_int_4.
///
/// Данные параметры передаются на сервер через соответствующие теги.
class run_time_params_u_int_4: public parameters < u_int_4, false >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param name  - имя параметров.
        run_time_params_u_int_4( int count,
            const char *name = "RT_PAR_UI", i_params_owner* owner = 0 ) :
            parameters < u_int_4, false >( count, name, owner )
            {
            }

        virtual ~run_time_params_u_int_4() = default;

    protected:
        u_int_4 get_val( int idx ) const
            {
            return parameters< u_int_4, false >::get_val( idx );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами.
///
/// Данные параметры передаются на сервер через соответствующие теги. При
/// перезагрузке PAC их значения сохраняются.
template < class type, bool is_float > class saved_params:
public parameters < type, is_float >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param name  - имя объекта.
        saved_params( int count, const char *name, i_params_owner* owner = 0 ) : parameters < type, is_float >(
            count, name,
            ( type* ) params_manager::get_instance()->reserve_params_region(
            count * sizeof( type ), start_pos ), owner )
            {
            }

        virtual ~saved_params() = default;

        /// @brief Сохранение значения параметра в энергонезависимой памяти.
        ///
        /// Операция доступа через индекс сохраняет значение параметра только
        /// в буфере, так что для сохранения его в энергонезависимой памяти надо
        /// использовать данный метод.
        type save( u_int idx, type value )
            {
            if ( idx > 0 && idx <= parameters< type, is_float >::get_count() )
                {
                idx--;
                parameters< type, is_float >::get_values()[ idx ] = value;

                params_manager::get_instance()->save();
                }
            else
                {
                parameters< type, is_float >::print_msg_out_of_range( idx, "save" );
                }

            return value;
            }

        /// @brief Сохранение значения всех параметров в энергонезависимой памяти.
        ///
        /// Операция доступа через индекс сохраняет значение параметра только
        /// в буфере, так что для сохранения его в энергонезависимой памяти надо
        /// использовать данный метод.
        int save_all()
            {
            params_manager::get_instance()->save();

            return 0;
            }

        /// @brief Установка всех параметров в нулевое значение и сохранение
        /// в энергонезависимой памяти.
        void reset_to_0()
            {
        	u_int size = parameters< type, is_float >::get_count();
            for ( u_int i = 0; i < size; i++ )
                {
                parameters< type, is_float >::get_values()[ i ] = 0;
                }

            params_manager::get_instance()->save();
            }

    private:
        /// Индекс начала значений в общем массиве, для сохранения значения
        /// параметра в энергонезависимой памяти (@ref save).
        int start_pos;
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами типа @ref u_int_4.
class saved_params_u_int_4: public saved_params < u_int_4, false >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        saved_params_u_int_4( int count, i_params_owner* owner = 0 ) :
          saved_params < u_int_4, false >( count, "S_PAR_UI", owner )
              {
              }

        virtual ~saved_params_u_int_4() = default;

    protected:
        u_int_4 get_val( int idx ) const
            {
            return saved_params< u_int_4, false >::get_val( idx );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами типа float.
class saved_params_float: public saved_params < float, true >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        saved_params_float( int count, i_params_owner* owner = 0 ):
          saved_params < float, true >( count, "S_PAR_F", owner )
              {
              }

        virtual ~saved_params_float() = default;

    protected:
        float get_val( int idx ) const
            {
            return saved_params< float, true >::get_val( idx );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Класс для тестирования классов работы с параметрами.
class params_test
    {
    public:
        /// @brief Тестирование классов работы с параметрами.
        ///
        /// @return 0 - Ок.
        /// @return 1 - Ошибка.
        static int make_test();
    };
//-----------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma option -w.inl
#pragma option -w.ccc
#pragma option -w.rch
#endif // __BORLANDC__

#endif // PARAMS_EX_H
