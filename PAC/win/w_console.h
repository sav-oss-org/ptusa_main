/// @file w_console.h
/// @brief Работа с консолью в ОС Windows. Также другие безопасные функции
/// работы со строками.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef W_CONSOLE_H
#define W_CONSOLE_H

#pragma warning( disable : 4996 ) //strcpy and others unsafe functions

#define RED    ( FOREGROUND_RED )
#define RED_I  ( FOREGROUND_RED | FOREGROUND_INTENSITY )
#define GREEN  ( FOREGROUND_GREEN )
#define YELLOW ( FOREGROUND_RED | FOREGROUND_GREEN )
#define GRAY   ( FOREGROUND_INTENSITY )
#define WHITE  ( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE )
#define RESET  0

#define CLEARSCREEN system( "cls" )

#include <time.h>
#include <conio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void SetColor( WORD color );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//Устаревшая функциональность. Раньше printf не должна была ничего делать в
//release версии.
//#ifdef DEBUG
//#undef printf
//#define printf(...)    printf( __VA_ARGS__ )
//#else
//#define printf(...)    __asm nop
//#endif // DEBUG
//-----------------------------------------------------------------------------
#define print_time( ... )                                              \
    printf( "--%02lu:%02lu:%02lu ", ( (u_long)time( 0 ) / 3600 ) % 24, \
        ( (u_long)time( 0 ) / 60 ) % 60, (u_long)time( 0 ) % 60 );     \
    printf( __VA_ARGS__ )
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif // W_CONSOLE_H
