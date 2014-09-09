#include <stdio.h>
#include <stdlib.h>
#include "sh_image.h"
#include "sh_utils.h"
#include <unistd.h>
#define USAGE printf("Usage catimg [-h] [-w width] img\nBy default w is the terminal width.\n")

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

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
        uint32_t w = 0;
        for (uint32_t i = 0; i < img.width*img.height; i++) {
                if (img.pixels[i].a == 0) {
                        printf("\e[m  ");
                } else {
                        uint32_t col;
                        if (img.pixels[i].r == img.pixels[i].g && img.pixels[i].g == img.pixels[i].b)
                                col = 232 + (img.pixels[i].r*23)/255;
                        else
                                col = (16 + ((img.pixels[i].r*5)/255)*36
                                                + ((img.pixels[i].g*5)/255)*6
                                                + (img.pixels[i].b*5)/255);

                        /*printf("(%u,%u,%u)", img.pixels[i].r, img.pixels[i].g, img.pixels[i].b);*/
                        printf("\e[48;5;%um  ", col);
                }
                w++;
                if (w >= img.width) {
                        w = 0;
                        printf("\e[m\n");
                }
        }
        img_free(&img);
        free_hash_colors();
        return 0;
}

