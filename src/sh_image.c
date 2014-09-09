#include "sh_image.h"
#include "stb_image.h"
#include <stdlib.h>
#include <assert.h>


void img_load_from_file(image_t *img, const char* file)
{
        int channels, w, h;
        unsigned char* ptr = stbi_load(file, &w, &h, &channels, STBI_rgb_alpha);

        if (ptr && w && h) {
                img->width = w;
                img->height = h;

                if (!(img->pixels = malloc(sizeof(color_t)*w*h))) {
                        perror("malloc error\n");
                        exit(1);
                }

                // fill the array
                for (int i = 0, j = 0; j < w*h; i += 4, j++) {
                        img->pixels[j].r = ptr[i];
                        img->pixels[j].g = ptr[i+1];
                        img->pixels[j].b = ptr[i+2];
                        img->pixels[j].a = ptr[i+3];
                }

                stbi_image_free(ptr);
        } else {
                perror("stb_image error");
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
        for (uint32_t i = 0 ; i < img->width*img->height; i++)
                convert_color(&(img->pixels[i]), &(img->pixels[i]));
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

        if (!(pix = malloc(sizeof(color_t)*w*h))) {
                perror("malloc error\n");
                exit(1);
        }
        for (uint32_t y = 0; y < h; ++y) {
                for (uint32_t x = 0; x < w; ++x) {
                        uint32_t r = 0, g = 0, b = 0, a = 0,
                                 srcx = x*wsc,
                                 srcy = y*hsc;
                        for (int32_t yi = 0; yi < hsc_i; ++yi)
                                for (int32_t xi = 0; xi < wsc_i; ++xi) {
                                        r += img->pixels[srcx + xi +(srcy+yi)*img->width].r;
                                        g += img->pixels[srcx + xi +(srcy+yi)*img->width].g;
                                        b += img->pixels[srcx + xi +(srcy+yi)*img->width].b;
                                        a += img->pixels[srcx + xi +(srcy+yi)*img->width].a;
                                }
                        pix[x+y*w].r = r/wh;
                        pix[x+y*w].g = g/wh;
                        pix[x+y*w].b = b/wh;
                        pix[x+y*w].a = a/wh;
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
