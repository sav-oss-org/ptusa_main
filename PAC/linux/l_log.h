/// @file l_log.h
/// @brief Работа с протоколированием событий, ошибок и т.д. для ОС Linux.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev: 332 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-08-24 09:59:40#$.

#ifndef L_LOG_H
#define L_LOG_H

#include "log.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <dtime.h>

#ifndef SIMPLE_LOG
#include <syslog.h>
#endif

#include <stdio.h>

#include "l_console.h"
//-----------------------------------------------------------------------------
/// @brief Работа с журналом.
///
/// Базовый интерфейсный класс для журнала.
/// Содержит основные методы работы - запиь в лог, т.д.
///
class l_log: public i_log
    {
    public:

    virtual ~l_log()
        {
        }

    void virtual write_log( PRIORITIES priority )
        {
        forward_to_debugger( priority );
#ifdef SIMPLE_LOG
        printf( "%s\n", msg );
#else
        std::tm tm = get_time();

        printf( "%04d-%02d-%02d %02d.%02d.%02d ",
            1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );

        switch ( priority )
            {
            case i_log::P_ALERT:
                SetColor( RED_I );
                std::cout << "ALERT  (1) -> ";
                break;

            case i_log::P_CRIT:
                SetColor( RED );
                std::cout << "CRITIC (2) -> ";
                break;

            case i_log::P_ERR:
                SetColor( RED );
                std::cout << "ERROR  (3) -> ";
                break;

            case i_log::P_WARNING:
                SetColor( YELLOW );
                std::cout << "WARNING(4) -> ";
                break;

            case i_log::P_NOTICE:
                SetColor( GREEN );
                std::cout << "NOTICE (5) -> ";
                break;

            case i_log::P_INFO:
                SetColor( GREEN );
                std::cout << "INFO   (6) -> ";
                break;

            case i_log::P_DEBUG:
                SetColor( GRAY );
                std::cout << "DEBUG  (7) -> ";
                break;

            default:
                std::cout << "       (" << priority << ") -> ";
                break;
            }

        std::cout << msg << std::endl;
        SetColor( RESET );

        fflush( stdout );

        int len = strlen( msg ) - 1;
        for ( int i = 0; i < len; i++ )
            {
            if ( msg[ i ] == '\n' ) msg[ i ] = ';';
            }

        //Using openlog() because kbus functions also use it
        //(openlog( "kbus", ...)).
        openlog( "ptusa", LOG_PID, LOG_USER );

        syslog( (int) priority, "%s", msg );
        closelog();
#endif
        }

    l_log()
        {
        }
};

#endif // L_LOG_H
