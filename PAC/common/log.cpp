#include "log.h"
#include <stdarg.h>

#if defined  WIN_OS
#include "w_log.h"
#elif defined LINUX_OS
#include "l_log.h"
#endif

auto_smart_ptr < log_mngr > log_mngr::instance;

//-----------------------------------------------------------------------------
i_log* log_mngr::get_log()
    {
    if ( instance.is_null() )
        {
        instance = new log_mngr();
        }

    return instance->lg;
    }
//-----------------------------------------------------------------------------
log_mngr::log_mngr(): lg( nullptr )
    {
#if defined WIN_OS
    lg = new w_log();
#elif defined LINUX_OS
    lg = new l_log();
#endif
    }
//-----------------------------------------------------------------------------
log_mngr::~log_mngr()
    {
    delete lg;
    lg = nullptr;
    }
//-----------------------------------------------------------------------------
void i_log::write_log( PRIORITIES priority )
    {
    sanitize_msg();
    write_log_impl( priority );
    }
//-----------------------------------------------------------------------------
void i_log::sanitize_msg()
    {
    size_t escaped_len { 0 };
    size_t src_len { 0 };
    bool escaped { false };

    while ( msg[ src_len ] != 0 )
        {
        const char symbol = msg[ src_len ];
        const size_t width =
            ( symbol == '\n' || symbol == '\r' ) ? 2 : 1;

        if ( escaped_len + width > C_BUFF_SIZE - 1 )
            {
            break; // Экранированный текст не влез, хвост отброшен.
            }

        if ( width != 1 )
            {
            escaped = true;
            }

        escaped_len += width;
        ++src_len;
        }

    if ( !escaped && msg[ src_len ] == 0 )
        {
        return; // Экранирование и усечение не потребовались.
        }

    msg[ escaped_len ] = 0;

    // Заполняем буфер справа налево: позиция записи никогда не
    // отстаёт от позиции чтения, поэтому временный буфер не нужен.
    while ( src_len > 0 )
        {
        const char symbol = msg[ src_len - 1 ];

        switch ( symbol )
            {
            case '\n':
                msg[ --escaped_len ] = 'n';
                msg[ --escaped_len ] = '\\';
                break;

            case '\r':
                msg[ --escaped_len ] = 'r';
                msg[ --escaped_len ] = '\\';
                break;

            default:
                msg[ --escaped_len ] = symbol;
                break;
            }

        --src_len;
        }
    }
//-----------------------------------------------------------------------------
void i_log::write_log( PRIORITIES priority, const char* debug_message )
    {
    strncpy( msg, debug_message, C_BUFF_SIZE - 1 );
    msg[ C_BUFF_SIZE - 1 ] = 0;
    write_log( priority );
    }
//-----------------------------------------------------------------------------
void i_log::debug(const char* debug_message, ...)
	{
	if (G_DEBUG)
		{
		va_list params;
		va_start(params, debug_message);
		vsprintf(msg, debug_message, params);
		va_end(params);
		write_log(P_DEBUG);
		}
	}

void i_log::info(const char* info_message, ...)
	{
	va_list params;
	va_start(params, info_message);
	vsprintf(msg, info_message, params);
	va_end(params);
	write_log(P_INFO);
	}

void i_log::notice(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_NOTICE);
    }

void i_log::warning(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_WARNING);
    }

void i_log::error(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_ERR);
    }

void i_log::critical(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_CRIT);
    }

void i_log::alert(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_ALERT);
    }

void i_log::emergency(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_EMERG);
    }
//-----------------------------------------------------------------------------
i_log* G_SYS_LOG()
    {
    return log_mngr::get_log();
    }
//-----------------------------------------------------------------------------
