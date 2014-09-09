#include "sh_utils.h"

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

uint32_t terminal_columns()
{
#ifdef WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int ret;
        ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ),&csbi);
        return ret?csbi.dwSize.X:0;
#else
        struct winsize win;
        ioctl(0, TIOCGWINSZ, &win);
        return win.ws_col;
#endif
}

