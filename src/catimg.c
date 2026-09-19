#define _DEFAULT_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sh_image.h"
#include "sh_utils.h"
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

#define USAGE "Usage: catimg [-hct] [-w width | -H height] [-l loops] [-r resolution] image-file\n\n" \
  "  -h: Displays this message\n"                                      \
  "  -w: Terminal width/columns by default\n"                           \
  "  -H: Terminal height/row by default\n"                           \
  "  -C: Maximum image width in terminal columns\n"                  \
  "  -R: Maximum image height in terminal rows\n"                    \
  "  -l: Loops are only useful with GIF files. A value of 1 means that the GIF will " \
  "be displayed twice because it loops once. A negative value means infinite " \
  "looping\n"                                                           \
  "  -r: Resolution must be 1 or 2. By default catimg checks for unicode support to " \
  "use higher resolution\n" \
  "  -c: Convert colors to a restricted palette\n" \
  "  -t: Disables true color (24-bit) support, falling back to 256 color\n" \
  "  -a: Auto-size the image to fit terminal dimensions, overriding -w, -H, -C, or -R\n"

#define ERR_WIDTH_OR_HEIGHT "[ERROR] '-w' and '-H' can't be used at the same time\n\n"
#define ERR_COL_NEGATIVE "[ERROR] '-C' can't be negative\n\n"
#define ERR_ROW_NEGATIVE "[ERROR] '-R' can't be negative\n\n"

// Transparency threshold -- all pixels with alpha below 25%.
#define TRANSP_ALPHA 64

#define EPSILON 0.00001

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

// For <C-C>
volatile int loops = -1, loop = -1;
volatile char stop = 0;

static void intHandler(int sig) {
    (void)sig;
    loops = loop;
    stop = 1;
}

static uint32_t pixelToInt(const color_t *pixel) {
    if (pixel->a == 0)
        return 0xffff;
    else if (pixel->r == pixel->g && pixel->g == pixel->b)
        return 232 + (pixel->r * 23)/255;
    else
        return (16 + ((pixel->r*5)/255)*36
                + ((pixel->g*5)/255)*6
                + (pixel->b*5)/255);
}

static char supportsUTF8() {
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

    uint32_t cols = 0, rows = 0, precision = 0;
    uint32_t tcols = 0, trows = 0;
    uint32_t max_cols = 0, max_rows = 0;
    int32_t desired_cols = -1, desired_rows = -1;
    uint8_t convert = 0;
    uint8_t true_color = 1;
    uint8_t adjust_to_height = 0, adjust_to_width = 0;
    uint8_t auto_size = 0;
    float scale = 1.0, scale_cols = 0, scale_rows = 0;

    while ((c = getopt (argc, argv, "H:w:l:r:hctaC:R:")) != -1)
        switch (c) {
            case 'H':
                rows = strtol(optarg, &num, 0);
                if (adjust_to_width) {
                    // only either adjust to width or adjust to height is allowed, but not both.
                    printf(ERR_WIDTH_OR_HEIGHT);
                    printf(USAGE);
                    exit(1);
                }
                adjust_to_height = 1;
                break;
            case 'w':
                cols = strtol(optarg, &num, 0) >> 1;
                if (adjust_to_height) {
                    // only either adjust to width or adjust to height is allowed, but not both.
                    printf(ERR_WIDTH_OR_HEIGHT);
                    printf(USAGE);
                    exit(1);
                }
                adjust_to_width = 1;
                break;
            case 'l':
                loops = strtol(optarg, &num, 0);
                break;
            case 'r':
                precision = strtol(optarg, &num, 0);
                break;
            case 'h':
                printf(USAGE);
                exit(0);
                break;
            case 'c':
                convert = 1;
                break;
            case 't':
                true_color = 0;
                break;
            case 'C':
                desired_cols = strtol(optarg, &num, 0);
                if (desired_cols < 0) {
                    fprintf(stderr, ERR_COL_NEGATIVE);
                    exit(1);
                }
                break;
            case 'R':
                desired_rows = strtol(optarg, &num, 0);
                if (desired_rows < 0) {
                    fprintf(stderr, ERR_ROW_NEGATIVE);
                    exit(1);
                }
                break;
            case 'a':
                auto_size = 1;
                break;
            default:
                printf(USAGE);
                exit(1);
                break;
        }

    if (argc > 1)
        file = argv[argc-1];
    else {
        printf(USAGE);
        exit(1);
    }

    if (precision == 0 || precision > 2) {
        if (supportsUTF8())
            precision = 2;
        else
            precision = 1;
    }

    // if precision is 2 we can use the terminal full width/height. Otherwise we can only use half
    tcols = terminal_columns();
    trows = terminal_rows();
    max_cols = (desired_cols < 0 || (auto_size && desired_cols > tcols))
        ? tcols
        : desired_cols;
    max_rows = (desired_rows < 0 || (auto_size && desired_rows > trows))
        ? trows
        : desired_rows;
    tcols /= (2 / precision);
    trows  *= precision;
    max_cols /= (2 / precision);
    max_rows *= precision;

    if (strcmp(file, "-") == 0) {
        img_load_from_stdin(&img);
    } else {
        img_load_from_file(&img, file);
    }

    if (desired_cols >= 0 && desired_rows >= 0) {
        scale_cols = max_cols / (float)img.width;
        scale_rows = max_rows / (float)img.height;
        if (scale_rows < scale_cols)
            scale = scale_rows;
        else
            scale = scale_cols;
    } else if (desired_cols >= 0) {
        scale = max_cols / (float)img.width;
    } else if (desired_rows >= 0) {
        scale = max_rows / (float)img.height;
    } else if (cols == 0 && rows == 0) {
        scale_cols = tcols / (float)img.width;
        scale_rows = trows / (float)img.height;
        if (adjust_to_height && scale_rows < scale_cols && trows < img.height)
            // rows == 0 and adjust_to_height > adjust to height instead of width
            scale = scale_rows;
        else if (tcols < img.width)
            scale = scale_cols;
    } else if (cols > 0 && cols < img.width) {
        scale = cols / (float)img.width;
    } else if (rows > 0 && rows < img.height) {
        scale = rows / (float)img.height;
    }

    if (auto_size) {
        scale_cols = tcols / (float)img.width;
        scale_rows = trows / (float)img.height;
        scale = fmin(fmin(scale, scale_cols), scale_rows);
    }

    if (fabs(scale - 1.0) > EPSILON)
        img_resize(&img, scale, scale);

    if (convert)
        img_convert_colors(&img);
    /*printf("Loaded %s: %ux%u. Console width: %u\n", file, img.width, img.height, cols);*/
    // For GIF
    if (img.frames > 1) {
        // switch to the alternate screen and place cursor at top left
        printf("\e[?1049h\e[H");
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
                    const color_t* upperPixel = &img.pixels[index];
                    uint32_t fgCol = pixelToInt(upperPixel);
                    if (precision == 2) {
                        if (y < img.height - 1) {
                            const color_t* lowerPixel = &img.pixels[index + img.width];
                            uint32_t bgCol = pixelToInt(&img.pixels[index + img.width]);
                            if (upperPixel->a < TRANSP_ALPHA) { // first pixel is transparent
                                if (lowerPixel->a < TRANSP_ALPHA)
                                    printf("\e[m ");
                                else
                                    if (true_color)
                                        printf("\x1b[0;38;2;%d;%d;%dm\u2584",
                                               lowerPixel->r, lowerPixel->g, lowerPixel->b);
                                    else
                                        printf("\e[0;38;5;%um\u2584", bgCol);
                            } else {
                                if (lowerPixel->a < TRANSP_ALPHA)
                                    if (true_color)
                                        printf("\x1b[0;38;2;%d;%d;%dm\u2580",
                                               upperPixel->r, upperPixel->g, upperPixel->b);
                                    else
                                        printf("\e[0;38;5;%um\u2580", fgCol);
                                else
                                    if (true_color)
                                        printf("\x1b[48;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm\u2584",
                                               upperPixel->r, upperPixel->g, upperPixel->b,
                                               lowerPixel->r, lowerPixel->g, lowerPixel->b);
                                    else
                                         printf("\e[38;5;%u;48;5;%um\u2580", fgCol, bgCol);
                            }
                        } else { // this is the last line
                            if (upperPixel->a < TRANSP_ALPHA)
                                printf("\e[m ");
                            else
                                if (true_color)
                                    printf("\x1b[0;38;2;%d;%d;%dm\u2580",
                                           upperPixel->r, upperPixel->g, upperPixel->b);
                                else
                                    printf("\e[38;5;%um\u2580", fgCol);
                        }
                    } else {
                        if (img.pixels[index].a < TRANSP_ALPHA)
                            printf("\e[m  ");
                        else
                            if (true_color)
                                printf("\x1b[0;48;2;%d;%d;%dm  ",
                                       img.pixels[index].r, img.pixels[index].g, img.pixels[index].b);
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
    if (img.frames > 1) {
        // switch from the alternate screen
        printf("\e[?1049l");
    }
    // Display the cursor again
    printf("\e[?25h");

    img_free(&img);
    free_hash_colors();
    return 0;
}
