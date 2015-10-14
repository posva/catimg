#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sh_image.h"
#include "sh_utils.h"
#include <unistd.h>
#include <signal.h>
#define USAGE printf("Usage catimg [-h] [-w width] [-l loops] [-r resolution] img\nBy default w is the terminal width.\nLoops are only useful with GIF. A value of 1 means that the GIF will be displayed twice. A negative value means infinite looping.\nResolution must be 1 or 2. By default catimg checks for unicode support to use higher resolution\n")

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

// For <C-C>
volatile int loops = -1, loop = -1;
volatile char stop = 0;

void intHandler() {
        loops = loop;
        stop = 1;
}

int getopt(int argc, char * const argv[], const char *optstring);

uint32_t pixelToInt(const color_t *pixel) {
        if (pixel->a == 0)
                return 0xffff;
        else if (pixel->r == pixel->g && pixel->g == pixel->b)
                return 232 + (pixel->r * 23)/255;
        else
                return (16 + ((pixel->r*5)/255)*36
                                + ((pixel->g*5)/255)*6
                                + (pixel->b*5)/255);
}

char supportsUTF8() {
        const char* LC_ALL = getenv("LC_ALL");
        const char* LANG = getenv("LANG");
        const char* LC_CTYPE = getenv("LC_CTYPE");
        const char* UTF = "UTF-8";
        return (LC_ALL && strstr(LC_ALL, UTF))
                || (LANG && strstr(LANG, UTF))
                || (LC_CTYPE && strstr(LC_CTYPE, UTF));
}

int main(int argc, char *argv[])
{
        init_hash_colors();
        image_t img;
        char *file;
        char *num;
        int c;
        opterr = 0;

        uint32_t cols = 0, precision = 0;
        while ((c = getopt (argc, argv, "w:l:r:h")) != -1)
                switch (c) {
                        case 'w':
                                cols = strtol(optarg, &num, 0) >> 1;
                                break;
                        case 'l':
                                loops = strtol(optarg, &num, 0);
                                break;
                        case 'r':
                                precision = strtol(optarg, &num, 0);
                                break;
                        case 'h':
                                USAGE;
                                exit(0);
                        default:
                                USAGE;
                                exit(1);
                                break;
                }

        if (argc > 1)
                file = argv[argc-1];
        else {
                USAGE;
                exit(1);
        }

        if (precision == 0 || precision > 2) {
               if (supportsUTF8())
                       precision = 2;
               else
                       precision = 1;
        }

        if (cols < 1) // if precision is 2 we can use the terminal full width. Otherwise we can only use half
                cols = terminal_columns() / (2 / precision);
        img_load_from_file(&img, file);
        if (cols < img.width) {
                float sc = cols/(float)img.width;
                img_resize(&img, sc, sc);
        }
        img_convert_colors(&img);
        /*printf("Loaded %s: %ux%u. Console width: %u\n", file, img.width, img.height, cols);*/
        // For GIF
        if (img.frames > 1) {
                system("clear");
                signal(SIGINT, intHandler);
        } else {
                loops = 0;
        }
        // Save the cursor position and hide it
        printf("\e[s\e[?25l");
        while (loop++ < loops || loops < 0) {
                uint32_t offset = 0;
                for (uint32_t frame = 0; frame < img.frames; frame++) {
                        if (frame > 0 || loop > 0) {
                                if (frame > 0)
                                        usleep(img.delays[frame - 1] * 10000);
                                else
                                        usleep(img.delays[img.frames - 1] * 10000);
                                printf("\e[u");
                        }
                        uint32_t index, x, y;
                        for (y = 0; y < img.height; y += precision) {
                                for (x = 0; x < img.width; x++) {
                                        index = y * img.width + x + offset;
                                        uint32_t fgCol = pixelToInt(&img.pixels[index]);
                                        if (precision == 2) {
                                                if (y < img.height - 1) {
                                                        uint32_t bgCol = pixelToInt(&img.pixels[index + img.width]);
                                                        if (fgCol ==0xffff) { // first pixel is transparent
                                                                if (bgCol ==0xffff)
                                                                        printf("\e[m ");
                                                                else
                                                                        printf("\e[0;38;5;%um\u2584", bgCol);
                                                        } else {
                                                                if (bgCol ==0xffff)
                                                                        printf("\e[0;38;5;%um\u2580", fgCol);
                                                                else
                                                                        printf("\e[38;5;%u;48;5;%um\u2580", fgCol, bgCol);
                                                        }
                                                } else { // this is the last line
                                                        if (fgCol ==0xffff)
                                                                printf("\e[m ");
                                                        else
                                                                printf("\e[38;5;%um\u2580", fgCol);
                                                }
                                        } else {
                                                if (fgCol == 0xffff)
                                                        printf("\e[m  ");
                                                else
                                                        printf("\e[48;5;%um  ", fgCol);
                                        }
                                }
                                printf("\e[m\n");
                        }
                        offset += img.width * img.height;
                        if (stop) frame = img.frames;
                }
        }
        // Display the cursor again
        printf("\e[?25h");

        img_free(&img);
        free_hash_colors();
        return 0;
}

