#include "sh_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>
#include <assert.h>

typedef struct gif_result_t {
        int delay;
        unsigned char *data;
        struct gif_result_t *next;
} gif_result;

// stbi_xload was written by urraka and slighty modified by me (posva) to make
// it have a channels argument. Thanks to urraka for his help with this :)
STBIDEF unsigned char *stbi_xload(char const *filename, int *x, int *y, int *frames, int *channels)
{
        FILE *f;
        stbi__context s;
        unsigned char *result = 0;

        if (!(f = stbi__fopen(filename, "rb")))
                return stbi__errpuc("can't fopen", "Unable to open file");

        stbi__start_file(&s, f);

        if (stbi__gif_test(&s))
        {
                stbi__gif g;
                gif_result head;
                gif_result *prev = 0, *gr = &head;

                memset(&g, 0, sizeof(g));
                memset(&head, 0, sizeof(head));

                *frames = 0;

                while (gr->data = stbi__gif_load_next(&s, &g, channels, 4))
                {
                        if (gr->data == (unsigned char*)&s)
                        {
                                gr->data = 0;
                                break;
                        }

                        if (prev) prev->next = gr;
                        gr->delay = g.delay;
                        prev = gr;
                        gr = (gif_result*) stbi__malloc(sizeof(gif_result));
                        memset(gr, 0, sizeof(gif_result));
                        ++(*frames);
                }

                if (gr != &head)
                        STBI_FREE(gr);

                if (*frames > 0)
                {
                        *x = g.w;
                        *y = g.h;
                }

                result = head.data;

                if (*frames > 1)
                {
                        unsigned int size = 4 * g.w * g.h;
                        unsigned char *p = 0;

                        result = (unsigned char*)stbi__malloc(*frames * (size + 2));
                        gr = &head;
                        p = result;

                        while (gr)
                        {
                                prev = gr;
                                memcpy(p, gr->data, size);
                                p += size;
                                *p++ = gr->delay & 0xFF;
                                *p++ = (gr->delay & 0xFF00) >> 8;
                                gr = gr->next;

                                STBI_FREE(prev->data);
                                if (prev != &head) STBI_FREE(prev);
                        }
                }
        }
        else
        {
                result = stbi__load_main(&s, x, y, channels, 0);
                *frames = !!result;
        }

        fclose(f);
        return result;
}

void setPixelGray(color_t *pixel, unsigned char* ptr) {
        pixel->r = pixel->g = pixel->b = ptr[0];
        pixel->a = 255;
}

void setPixelGrayAlpha(color_t *pixel, unsigned char* ptr) {
        pixel->r = pixel->g = pixel->b = ptr[0];
        pixel->a = ptr[1];
}

void setPixelRGB(color_t *pixel, unsigned char* ptr) {
        pixel->r = ptr[0];
        pixel->g = ptr[1];
        pixel->b = ptr[2];
        pixel->a = 255;
}

void setPixelRGBAlpha(color_t *pixel, unsigned char* ptr) {
        pixel->r = ptr[0];
        pixel->g = ptr[1];
        pixel->b = ptr[2];
        pixel->a = ptr[3];
}

void img_load_from_file(image_t *img, const char* file)
{
        int channels, w, h, frames;
        /*unsigned char* ptr = stbi_load(file, &w, &h, &channels, 0);*/
        unsigned char* ptr = stbi_xload(file, &w, &h, &frames, &channels);

        if (ptr && w && h) {
                img->width = w;
                /*h *= 3;*/
                img->height = h;
                img->delays = NULL;
                img->frames = frames;

                if (!(img->pixels = malloc(sizeof(color_t)*w*h * frames))) {
                        perror("malloc error\n");
                        exit(1);
                }

                if (frames > 1 && !(img->delays = malloc(sizeof(uint16_t) * (frames - 1)))) {
                        perror("malloc error\n");
                        exit(1);
                }

                // fill the array
                void (*pixelSetter)(color_t *pixel, unsigned char* ptr);
                switch (channels) {
                        case 1:
                                pixelSetter = &setPixelGray;
                                break;
                        case 2:
                                pixelSetter = &setPixelGrayAlpha;
                                break;
                        case 3:
                                pixelSetter = &setPixelRGB;
                                break;
                        case 4:
                                pixelSetter = &setPixelRGBAlpha;
                                break;
                }
                for (int frame = 0; frame < frames; frame++) {
                        int offset = frame * (sizeof(unsigned char) * channels * w*h + 2);
                        int i = 0;
                        for (int j = 0; j < w*h; i += channels, j++)
                                pixelSetter(&img->pixels[j + frame*w*h], ptr + i * sizeof(unsigned char) + offset);
                        if (frames > 1) {
                                uint16_t delay = ptr[offset + (1 + i) * sizeof(unsigned char)] << 8;
                                delay += ptr[offset + i * sizeof(unsigned char)];
                                img->delays[frame] = delay;
                        }
                }

                stbi_image_free(ptr);
        } else {
                perror("stb_image error");
                exit(1);
        }
}

void img_create(image_t *img, uint32_t width, uint32_t height)
{
        if (!(img->pixels = malloc(sizeof(color_t)*width*height))) {
                perror("malloc error\n");
                exit(1);
        }

        // fill the array
        for (uint32_t i = 0, j = 0; j < width*height; i += 4, j++) {
                img->pixels[j].a = 0;
        }

}

void img_copy(image_t* img, image_t *cpy)
{
        img->width = cpy->width;
        img->height = cpy->height;
        img->pixels = NULL;
        if (!(img->pixels = malloc(sizeof(color_t)*img->width*img->height))) {
                perror("malloc error");
                exit(1);
        }
        for (uint32_t i = 0; i < img->width*img->height; ++i)
                img->pixels[i] = cpy->pixels[i];
}

void img_convert_colors(image_t *img)
{
        for (uint32_t frame = 0; frame < img->frames; frame++) {
                uint32_t offset = img->height*img->width * frame;
                for (uint32_t i = 0 ; i < img->width*img->height; i++)
                        convert_color(&(img->pixels[i+offset]), &(img->pixels[i+offset]));
        }
}

void img_free(image_t *img)
{
        free(img->pixels);
}

void img_resize(image_t *img, float wsc, float hsc)
{
        color_t *pix = NULL;
        uint32_t w = img->width*wsc,
                 h = img->height*hsc,
                 wh;
        wsc = 1.f/wsc;
        hsc = 1.f/hsc;
        int32_t wsc_i = wsc,
                hsc_i = hsc;
        wh = hsc_i*wsc_i;

        if (!(pix = malloc(sizeof(color_t)*w*h * img->frames))) {
                perror("malloc error\n");
                exit(1);
        }
        for (uint32_t frame = 0; frame < img->frames; frame++) {
                uint32_t src_offset = img->height*img->width * frame,
                         offset = w*h * frame;
                for (uint32_t y = 0; y < h; ++y) {
                        for (uint32_t x = 0; x < w; ++x) {
                                uint32_t r = 0, g = 0, b = 0, a = 0,
                                         srcx = x*wsc,
                                         srcy = y*hsc;
                                for (int32_t yi = 0; yi < hsc_i; ++yi)
                                        for (int32_t xi = 0; xi < wsc_i; ++xi) {
                                                r += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].r;
                                                g += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].g;
                                                b += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].b;
                                                a += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].a;
                                        }
                                pix[offset + x+y*w].r = r/wh;
                                pix[offset + x+y*w].g = g/wh;
                                pix[offset + x+y*w].b = b/wh;
                                pix[offset + x+y*w].a = a/wh;
                        }
                }
        }
        free(img->pixels);
        img->pixels = pix;
        img->width = w;
        img->height = h;
}

const color_t* img_get_pixel(image_t *img, uint32_t x, uint32_t y)
{
        assert(x<img->width && y<img->height);
        return &img->pixels[x+y*img->width];
}

void img_set_pixel(image_t *img, uint32_t x, uint32_t y, const color_t *col)
{
        assert(x<img->width && y<img->height);
        col_cpy(col, &img->pixels[x+y*img->width]);
}
