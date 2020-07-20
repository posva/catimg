#include "sh_color.h"
#include "sh_utils.h"
#include <math.h>
#include "khash.h"

uint32_t color_map[] = { 0x000000, 0x00005F, 0x000080, 0x000087, 0x0000AF, 0x0000D7, 0x0000FF, 0x005F5F, 0x005F87, 0x005FAF, 0x005FD7, 0x005FFF, 0x008000, 0x008080, 0x008700, 0x00875F, 0x008787, 0x0087AF, 0x0087D7, 0x0087FF, 0x00AF00, 0x00AF5F, 0x00AF87, 0x00AFAF, 0x00AFFF, 0x00D700, 0x00D75F, 0x00D787, 0x00D7AF, 0x00D7D7, 0x00D7FF, 0x00FF00, 0x00FF5F, 0x00FF87, 0x00FFAF, 0x00FFD7, 0x00FFFF, 0x121212, 0x1C1C1C, 0x262626, 0x303030, 0x3A3A3A, 0x444444, 0x4E4E4E, 0x585858, 0x005F00, 0x5F0000, 0x5F005F, 0x5F0087, 0x5F00AF, 0x5F00D7, 0x5F00FF, 0x5F5F00, 0x5F5F5F, 0x5F5F87, 0x5F5FAF, 0x5F5FD7, 0x5F5FFF, 0x5F8700, 0x5F875F, 0x5F8787, 0x5F87AF, 0x5F87D7, 0x5F87FF, 0x5FAF00, 0x5FAF5F, 0x5FAF87, 0x5FAFAF, 0x5FAFD7, 0x5FAFFF, 0x5FD700, 0x5FD75F, 0x5FD787, 0x5FD7AF, 0x5FD7D7, 0x5FD7FF, 0x5FFF00, 0x5FFF5F, 0x5FFF87, 0x5FFFAF, 0x5FFFD7, 0x5FFFFF, 0x626262, 0x767676, 0x800000, 0x800080, 0x808000, 0x080808, 0x808080, 0x870000, 0x87005F, 0x870087, 0x8700AF, 0x8700D7, 0x8700FF, 0x875F00, 0x875F5F, 0x875F87, 0x875FAF, 0x875FD7, 0x875FFF, 0x878700, 0x87875F, 0x878787, 0x8787AF, 0x8787D7, 0x8787FF, 0x87AF00, 0x87AF5F, 0x87AF87, 0x87AFAF, 0x87AFD7, 0x87AFFF, 0x87D700, 0x87D75F, 0x87D787, 0x87D7AF, 0x87D7D7, 0x87D7FF, 0x87FF00, 0x87FF5F, 0x87FF87, 0x87FFAF, 0x87FFD7, 0x87FFFF, 0x8A8A8A, 0x949494, 0x9E9E9E, 0xA8A8A8, 0xAF0000, 0xAF005F, 0xAF0087, 0xAF00AF, 0xAF00D7, 0xAF00FF, 0xAF5F00, 0xAF5F5F, 0xAF5F87, 0xAF5FAF, 0xAF5FD7, 0xAF5FFF, 0xAF8700, 0xAF875F, 0xAF8787, 0xAF87AF, 0xAF87D7, 0xAF87FF, 0xAFAF00, 0xAFAF5F, 0xAFAF87, 0xAFAFAF, 0xAFAFD7, 0xAFAFFF, 0x00AFD7, 0xAFD700, 0xAFD75F, 0xAFD787, 0xAFD7AF, 0xAFD7D7, 0xAFD7FF, 0xAFFF00, 0xAFFF5F, 0xAFFF87, 0xAFFFAF, 0xAFFFD7, 0xAFFFFF, 0xB2B2B2, 0xBCBCBC, 0xC0C0C0, 0xC6C6C6, 0xD0D0D0, 0xD70000, 0xD7005F, 0xD70087, 0xD700AF, 0xD700D7, 0xD700FF, 0xD75F00, 0xD75F5F, 0xD75F87, 0xD75FAF, 0xD75FD7, 0xD75FFF, 0xD78700, 0xD7875F, 0xD78787, 0xD787AF, 0xD787D7, 0xD787FF, 0xD7AF00, 0xD7AF5F, 0xD7AF87, 0xD7AFAF, 0xD7AFD7, 0xD7AFFF, 0xD7D700, 0xD7D75F, 0xD7D787, 0xD7D7AF, 0xD7D7D7, 0xD7D7FF, 0xD7FF00, 0xD7FF5F, 0xD7FF87, 0xD7FFAF, 0xD7FFD7, 0xD7FFFF, 0xDADADA, 0xE4E4E4, 0xEEEEEE, 0xFF0000, 0xFF005F, 0xFF0087, 0xFF00AF, 0xFF00D7, 0xFF00FF, 0xFF5F00, 0xFF5F5F, 0xFF5F87, 0xFF5FAF, 0xFF5FD7, 0xFF5FFF, 0xFF8700, 0xFF875F, 0xFF8787, 0xFF87AF, 0xFF87D7, 0xFF87FF, 0xFFAF00, 0xFFAF5F, 0xFFAF87, 0xFFAFAF, 0xFFAFD7, 0xFFAFFF, 0xFFD700, 0xFFD75F, 0xFFD787, 0xFFD7AF, 0xFFD7D7, 0xFFD7FF, 0xFFFF00, 0xFFFF5F, 0xFFFF87, 0xFFFFAF, 0xFFFFD7, 0xFFFFFF
};

color_yuv_t yuv_color_map[N_COLORS]; ///< palette of the terminal colors in YUV format


void rgb2yuv(const color_t *rgb, color_yuv_t *yuv)
{
        float r = rgb->r/255.f, g = rgb->g/255.f, b = rgb->b/255.f;
        yuv->y = .299f*r + .587f*g + .114f*b;
        yuv->u = -.14173f*r + -.28886f*g + .436f*b;
        yuv->v = .615f*r + -.51499f*g + -.10001f*b;
}

float col_yuv_distance(const color_yuv_t *a, const color_yuv_t *b)
{
        return SQUARED(b->y - a->y) + SQUARED(b->u - a->u) + SQUARED(b->v - a->v);
}

// convert color to the terminal 256 colors
// if it have been already converted, fond it in the map
// First convert to XYZ, then to *L*a*b
// finnally find the closest color in the map and
// store it
KHASH_MAP_INIT_INT(uint32_t, uint32_t)
khash_t(uint32_t) *hash_colors;
void init_hash_colors()
{
        hash_colors = kh_init(uint32_t);
        int ret;
        khiter_t k;
        color_t rgb;
        for (int i = 0; i < N_COLORS; i++) {
                k = kh_put(uint32_t, hash_colors, color_map[i], &ret);
                /*if (!ret) kh_del(uint32_t, hash_colors, k);*/
                kh_value(hash_colors, k) = color_map[i];
                rgb.r = X2R(color_map[i]);
                rgb.g = X2G(color_map[i]);
                rgb.b = X2B(color_map[i]);
                rgb2yuv(&rgb, &yuv_color_map[i]);
        }
}

void free_hash_colors()
{
        kh_destroy(uint32_t, hash_colors);
}

uint32_t find_nearest_color(color_t *col)
{
        color_yuv_t yuv;
        rgb2yuv(col, &yuv);
        uint32_t b = 0;
        float dist = INFINITY, tmp;
        for (uint32_t i = 1; i < N_COLORS; ++i) {
                tmp = col_yuv_distance(&yuv, &yuv_color_map[i]);
                if (tmp < dist) {
                        dist = tmp;
                        b = i;
                }
        }
        return b;
}

void convert_color(color_t  *col, color_t *out)
{
        if (col->a == 0)
                return; // we don't care about this color
        /*int32_t r = col->r, g = col->g, b = col->b;*/
        /*double x,y,z;*/
        /*x = 0.412453*r + 0.357580*g + 0.180423*b;*/
        /*y = 0.212671*r + 0.715160*g + 0.072169*b;*/
        /*z = 0.019334*r + 0.119193*g + 0.950227*b;*/

        khiter_t k;
        k = kh_get(uint32_t, hash_colors, RGB2X(col->r, col->g, col->b));
        if (k == kh_end(hash_colors)) { // it doesn't exist
                uint32_t in = find_nearest_color(col);

                int ret;
                khiter_t k;
                k = kh_put(uint32_t, hash_colors, RGB2X(col->r, col->g, col->b), &ret);
                /*if (!ret) kh_del(uint32_t, hash_colors, k);*/
                kh_value(hash_colors, k) = color_map[in];
                out->r = X2R(color_map[in]);
                out->g = X2G(color_map[in]);
                out->b = X2B(color_map[in]);
        } else {
                uint32_t xcol = kh_value(hash_colors, k);
                out->r = X2R(xcol);
                out->g = X2G(xcol);
                out->b = X2B(xcol);
        }
}

void col_cpy(const color_t *col, color_t *out)
{
        out->r = col->r;
        out->g = col->g;
        out->b = col->b;
        out->a = col->a;
}
