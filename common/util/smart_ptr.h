/// @file smart_ptr.h
/// @brief Реализованы различные полезные функции.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef _UTIL_H
#define _UTIL_H

#include "console.h"
#include "s_types.h"
#include "debug.h"

//-----------------------------------------------------------------------------
/// @brief Защищенная работа с указателем.
///
/// Реализована проверка на работу с null указателем.
template < class type >
class smart_ptr
    {
    protected:
        type* pointer;

    public:
        /// @brief Конструктор по умолчанию.
        smart_ptr() : pointer( NULL )
            {
            }

        /// @brief Конструктор на основе указателя.
        smart_ptr( type* p ) : pointer( p )
            {
            }

        /// @brief Конструктор копирования.
        smart_ptr( const smart_ptr< type >& ptr ) : pointer( ptr.pointer )
            {
            }

        /// @brief Оператор присваивания указателя.
        smart_ptr< type >& operator=( type* new_t )
            {
            pointer = new_t;

            return *this;
            }

        /// @brief Оператор приведения типа к указателю.
        operator type*()
            {
            if ( pointer == NULL )
                {
                printf( "smart_ptr - pointer is NULL!\n" );
                debug_break;
                }

            return pointer;
            }

        /// @brief Оператор обращения к членам класса.
        type* operator->()
            {
            if ( pointer == NULL )
                {
                printf( "smart_ptr - dereferencing NULL!\n" );
                debug_break;
                }
            return pointer;
            }

        /// @brief Оператор обращения к членам класса.
        type* operator->() const
            {
            if ( pointer == NULL )
                {
                printf( "smart_ptr - dereferencing NULL!\n" );
                debug_break;
                }
            return pointer;
            }

        inline bool is_null()
            {
            return pointer == NULL;
            }

        inline bool is_null() const
            {
            return pointer == NULL;
            }

    private:
        /// @brief Закрытый оператор индексирования - запрет использования.
        type* operator[]( unsigned int index )
            {
            return pointer;
            }

        /// @brief Закрытый оператор присваивания - запрет использования.
        smart_ptr< type >& operator=( const smart_ptr< type >& new_t )
            {
            return *this;
            }
    };
//-----------------------------------------------------------------------------
/// @brief Защищенная работа с указателем.
///
/// Автоматическое освобождение памяти при уничтожении, переприсваивании объекта.
template < class type >
class auto_smart_ptr : public smart_ptr< type >
    {
    public:
        /// @brief Конструктор по умолчанию.
        auto_smart_ptr() : smart_ptr< type >( NULL )
            {
            }

        /// @brief Конструктор на основе указателя.
        auto_smart_ptr( type* p ) : smart_ptr< type >( p )
            {
            }

        void free()
            {
            if ( smart_ptr< type >::pointer )
                {
                delete smart_ptr< type >::pointer;
                smart_ptr< type >::pointer = NULL;
                }
            }

        virtual ~auto_smart_ptr()
            {
            free();
            }

        void replace_without_free( type* new_t )
            {
            smart_ptr< type >::pointer = new_t;
            }

        /// @brief Оператор присваивания указателя.
        auto_smart_ptr< type >& operator=( type* new_t )
            {
            if ( smart_ptr< type >::pointer )
                {
                delete smart_ptr< type >::pointer;
                }
            smart_ptr< type >::pointer = new_t;
            return *this;
            }
    };
//-----------------------------------------------------------------------------
#endif // _UTIL_H
