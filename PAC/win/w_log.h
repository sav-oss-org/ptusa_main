#ifndef w_log_h__
#define w_log_h__

#include "log.h"

#include <iostream>
#include <iomanip>
#include "ctime"

#include "w_console.h"
#include <dtime.h>
//-----------------------------------------------------------------------------
/// @brief Работа с журналом.
///
/// Базовый интерфейсный класс для журнала.
/// Содержит основные методы работы - запиь в лог, т.д.
///
class w_log: public i_log
	{
	public:

		virtual ~w_log()
			{
			}

#pragma warning( push )
#pragma warning( disable: 4996 ) //warning C4996: 'localtime': This function or variable may be unsafe.
		void virtual write_log(PRIORITIES priority)
			{
            forward_to_debugger( priority );
            std::tm tm = get_time();

            std::cout << std::put_time( &tm, "%Y-%m-%d %H.%M.%S " );

            switch ( priority )
                {
                case i_log::P_ALERT:
                    SetColor( RED_I );
                    std::cout << "ALERT  (1) -> ";
                    break;

                case i_log::P_CRIT:
                    SetColor( RED_I );
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
			}
#pragma warning( pop )

		w_log()
			{
			}
	};

#endif // w_log_h__
