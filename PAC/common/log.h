/// @file log.h
/// @brief Работа с протоколированием событий, ошибок и т.д.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev: 332 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-08-24 09:59:40#$.

#ifndef LOG_H
#define LOG_H

#include <string.h>

#include "smart_ptr.h"

extern int G_DEBUG;
//-----------------------------------------------------------------------------
/// @brief Работа с журналом.
///
/// Базовый интерфейсный класс для журнала.
/// Содержит основные методы работы - запиь в лог, т.д.
///
class i_log
    {
    public:
        enum CONSTANT
            {
            C_BUFF_SIZE = 10 * 1024,
            };

    virtual ~i_log()
        {
        }

    char msg[ C_BUFF_SIZE ];

    enum PRIORITIES
        {
        P_EMERG, 	// System is unusable
        P_ALERT,	// Action must be taken immediately
        P_CRIT,		// Critical conditions
        P_ERR,		// Error conditions
        P_WARNING,	// Warning conditions
        P_NOTICE,	// Normal but significant condition
        P_INFO,		// Informational
        P_DEBUG,	// Debug-level messages
        };

    /// @brief Запись строки в журнал. Строка должна быть в @msg.
    ///
    /// @param priority - приоритет.
    void virtual write_log( PRIORITIES priority ) = 0;

    void write_log( PRIORITIES priority, const char* debug_message );

	void debug(const char* debug_message, ...);
	void info(const char* info_message, ...);
    void notice(const char* info_message, ...);
    void warning(const char* info_message, ...);
    void error(const char* info_message, ...);
    void critical(const char* info_message, ...);
    void alert(const char* info_message, ...);
    void emergency(const char* info_message, ...);

    protected:

    /// Mirrors an already formatted log message to active debugger sessions.
    void forward_to_debugger( PRIORITIES priority ) const;

    i_log()
        {
        memset( msg, 0, C_BUFF_SIZE );
        }
    };
//-----------------------------------------------------------------------------
class log_mngr
    {
    public:
        static i_log* get_log();

        ~log_mngr();

    protected:
        /// Единственный экземпляр класса.
        static auto_smart_ptr < log_mngr > instance;

    private:
        log_mngr();

        i_log* lg;
    };
//-----------------------------------------------------------------------------
#define G_LOG log_mngr::get_log()

i_log* G_SYS_LOG();

#endif // LOG_H
