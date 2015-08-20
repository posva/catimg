#include <stdio.h>
#include <stdlib.h>
#include "sh_image.h"
#include "sh_utils.h"
#include <unistd.h>
#include <signal.h>
#define USAGE printf("Usage catimg [-h] [-w width] img\nBy default w is the terminal width.\n")

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

int main(int argc, char *argv[])
{
        init_hash_colors();
        image_t img;
        char *file;
        char *num;
        int c;
        opterr = 0;
        uint32_t cols = terminal_columns() >> 1; // divided by 2 because of 2 chars

        while ((c = getopt (argc, argv, "w:h")) != -1)
                switch (c) {
                        case 'w':
                                cols = strtol(optarg, &num, 0) >> 1;
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
                uint32_t w = 0, offset = 0;
                for (uint32_t frame = 0; frame < img.frames; frame++) {
                        if (frame > 0 || loop > 0) {
                                if (frame > 0)
                                        usleep(img.delays[frame - 1] * 10000);
                                else
                                        usleep(img.delays[img.frames - 1] * 10000);
                                printf("\e[u");
                        }
                        uint32_t i, index;
                        for (i = 0; i < img.width*img.height; i++) {
                                index = i + offset;
                                if (img.pixels[index].a == 0) {
                                        printf("\e[m  ");
                                } else {
                                        uint32_t col;
                                        if (img.pixels[index].r == img.pixels[index].g && img.pixels[index].g == img.pixels[index].b)
                                                col = 232 + (img.pixels[index].r*23)/255;
                                        else
                                                col = (16 + ((img.pixels[index].r*5)/255)*36
                                                                + ((img.pixels[index].g*5)/255)*6
                                                                + (img.pixels[index].b*5)/255);

                                        /*printf("(%u,%u,%u)", img.pixels[index].r, img.pixels[index].g, img.pixels[index].b);*/
                                        printf("\e[48;5;%um  ", col);
                                }
                                w++;
                                if (w >= img.width) {
                                        w = 0;
                                        printf("\e[m\n");
                                }
                        }
                        offset += i;
                        if (w != 0 && w < img.width) {
                                w = 0;
                                printf("\e[m\n");
                        }
                        if (stop) frame = img.frames;
                }
        }
        // Display the cursor again
        printf("\e[?25h");

        img_free(&img);
        free_hash_colors();
        return 0;
}

