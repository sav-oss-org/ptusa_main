#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <ctime>

time_t current_time;
struct tm* time_info;

#include "l_console.h"
//-----------------------------------------------------------------------------
int kb_hit()
    {
    int tem = fcntl( 0, F_GETFL, 0 );
    fcntl( 0, F_SETFL, ( tem | O_NDELAY ) );
    int c = getchar();
    fcntl( 0, F_SETFL, tem );
    if ( c > 0 && c != 255 )
        {
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int get_char()
    {
    struct termios oldt;
    struct termios newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
    }
//-----------------------------------------------------------------------------
