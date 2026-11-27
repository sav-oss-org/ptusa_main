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

    /// @brief Однострочная копия @msg для приёмников журнала,
    /// читающих вывод построчно (syslog, файл). Заполняется
    /// методом sanitize_msg().
    char msg_journal[ C_BUFF_SIZE ];

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

    /// @brief Запись строки из @msg в журнал.
    ///
    /// Плоскостные приёмники (syslog, файл журнала) читают
    /// однострочную копию @msg_journal, поэтому один вызов даёт
    /// ровно одну физическую строку. Консоль печатает @msg через
    /// print_console_msg() без экранирования.
    ///
    /// @param priority - приоритет.
    void write_log( PRIORITIES priority );

    /// @brief Платформенная запись одной уже очищенной строки.
    void virtual write_log_impl( PRIORITIES priority ) = 0;

    void write_log( PRIORITIES priority, const char* debug_message );

    /// @brief Готовит однострочную копию @msg в @msg_journal:
    /// CR и LF заменяются видимыми двухсимвольными последовательностями.
    /// Не помещающийся в буфер хвост сообщения отбрасывается,
    /// @msg остаётся неизменным.
    void sanitize_msg();

    /// @brief Печатает @msg в консоль: LF даёт перенос строки
    /// с фиксированным отступом строк продолжения, одиночный CR
    /// выводится литералом и не перерисовывает строку терминала.
    void print_console_msg() const;

	void debug(const char* debug_message, ...);
	void info(const char* info_message, ...);
    void notice(const char* info_message, ...);
    void warning(const char* info_message, ...);
    void error(const char* info_message, ...);
    void critical(const char* info_message, ...);
    void alert(const char* info_message, ...);
    void emergency(const char* info_message, ...);

    protected:

    i_log()
        {
        memset( msg, 0, C_BUFF_SIZE );
        memset( msg_journal, 0, C_BUFF_SIZE );
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
