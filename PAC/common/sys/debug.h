/// @file debug.h
/// @brief Установка точки прерывания на заданной строке.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c WIN_OS - реализация для ОС Windows.@n@n
///
/// @par Текущая версия:
/// @$Rev: 223 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-17 09:39:32#$.

#ifndef DEBUG_H
#define DEBUG_H

///@brief Установка точки прерывания в отладочной версии. В финальной версии
/// ничего не происходит.
#define debug_break
//-----------------------------------------------------------------------------
#ifdef DEBUG_IDE

#if defined LINUX_OS && !defined PAC_PLCNEXT
#undef debug_break
#define debug_break \
    asm( "int"      \
         "3" )
#endif // defined LINUX_OS

#if defined WIN_OS
#undef debug_break
#define debug_break _asm { int 3 }
#endif // defined WIN_OS

#endif // DEBUG_IDE
//-----------------------------------------------------------------------------
#endif // DEBUG_H
