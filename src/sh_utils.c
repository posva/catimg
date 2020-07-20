#include "sh_utils.h"

#ifdef WINDOWS
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#define fileno _fileno
#define read _read
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint32_t terminal_columns()
{
#ifdef WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int ret;
        ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ),&csbi);
        return ret?csbi.dwSize.X:0;
#else
        struct winsize win;
        ioctl(1, TIOCGWINSZ, &win);
        return win.ws_col;
#endif
}

uint32_t terminal_rows()
{
#ifdef WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int ret;
        ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ),&csbi);
        return ret?csbi.dwSize.Y:0;
#else
        struct winsize win;
        ioctl(1, TIOCGWINSZ, &win);
        return win.ws_row;
#endif
}


#define READ_BUF_SIZE 4096
uint32_t read_stdin(unsigned char **ptr) {
#ifdef WINDOWS
    _setmode(_fileno(stdin), _O_BINARY);
#endif
    size_t size = 0;
    ssize_t count = 0;

    size_t cur_size = READ_BUF_SIZE;
    unsigned char* data = malloc(cur_size);

    unsigned char buf[READ_BUF_SIZE];

    int fd = fileno(stdin);

    while((count = read(fd, buf, READ_BUF_SIZE)) > 0) {
        size += count;
        if (size > cur_size) {
            cur_size *= 2;
            data = realloc(data, cur_size);
            if (!data) {
                printf("Error: Could not allocate memory\n");
                exit(1);
            }
        }
        memcpy(data+size-count, buf, count);
    }
    
    // If count is not zero, some error occured
    if (count < 0) {
        perror("read_stdin");
        exit(1);
    }

    *ptr = data;

    return size;
}

