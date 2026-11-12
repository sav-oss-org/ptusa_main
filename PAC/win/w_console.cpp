#include "w_console.h"

HANDLE _hStdOut;
WORD wOldColorAttr;
CONSOLE_SCREEN_BUFFER_INFO csbiScreen;

void SetColor( WORD color )
    {
    if ( _hStdOut == NULL )
        {
        _hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );

        GetConsoleScreenBufferInfo( _hStdOut, &csbiScreen );
        wOldColorAttr = csbiScreen.wAttributes;
        }
    if ( 0 == color )
        {
        SetConsoleTextAttribute( _hStdOut, wOldColorAttr );
        }
    else
        {
        SetConsoleTextAttribute( _hStdOut, color );
        }
    }

int kb_hit()
    {
    return kbhit();
    }

int get_char()
    {
    //fflush( stdout );
    //getch();

    return 0; // В режиме эмуляции просто выводим на консоль сообщение.
    }
