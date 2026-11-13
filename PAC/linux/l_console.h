/// @file l_console.h
/// @brief Работа с консолью в ОС Linux. Также другие безопасные функции
/// работы со строками.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG    - компиляция c выводом отладочной информации в консоль.@n@n
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef L_CONSOLE_H
#define L_CONSOLE_H

#include <time.h>
//-----------------------------------------------------------------------------
#ifdef DEBUG
#undef Print
#define Print printf
#else
#define Print( ... )
#endif // DEBUG

#define RED    "\e[31m"
#define RED_I  "\e[1;31m"
#define GREEN  "\e[32m"
#define YELLOW "\e[33m"
#define GRAY   "\e[37m"
#define WHITE  "\e[1;37m"
#define RESET  "\e[0m"

#define CLEARSCREEN   printf( "\e[2J\e[H" )
#define SetColor( x ) printf( x )
//-----------------------------------------------------------------------------
extern time_t current_time;
extern struct tm* time_info;

#define print_time                                                     \
    current_time = time( nullptr );                                    \
    time_info = localtime( &current_time );                            \
    printf( "%02d.%02d.%02d %02d:%02d:%02d ", time_info->tm_mday,      \
        time_info->tm_mon + 1, time_info->tm_year, time_info->tm_hour, \
        time_info->tm_min, time_info->tm_sec );                        \
    printf
//-----------------------------------------------------------------------------
#endif // L_CONSOLE_H
