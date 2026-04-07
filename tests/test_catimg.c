#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "../src/sh_color.h"
#include "../src/sh_image.h"
#include "../src/sh_utils.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) static void name(void)
#define RUN_TEST(name) do { \
    printf("  %-50s", #name); \
    tests_run++; \
    name(); \
    tests_passed++; \
    printf("PASS\n"); \
} while (0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        printf("FAIL\n    %s:%d: %d != %d\n", __FILE__, __LINE__, (int)(a), (int)(b)); \
        return; \
    } \
} while (0)

#define ASSERT_FLOAT_EQ(a, b, eps) do { \
    if (fabsf((a) - (b)) > (eps)) { \
        printf("FAIL\n    %s:%d: %.6f != %.6f\n", __FILE__, __LINE__, (float)(a), (float)(b)); \
        return; \
    } \
} while (0)

#define ASSERT_TRUE(cond) do { \
    if (!(cond)) { \
        printf("FAIL\n    %s:%d: condition false\n", __FILE__, __LINE__); \
        return; \
    } \
} while (0)

// ============================================================================
// sh_color tests
// ============================================================================

TEST(test_rgb2yuv_black) {
    color_t rgb = {0, 0, 0, 255};
    color_yuv_t yuv;
    rgb2yuv(&rgb, &yuv);
    ASSERT_FLOAT_EQ(yuv.y, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(yuv.u, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(yuv.v, 0.0f, 0.001f);
}

TEST(test_rgb2yuv_white) {
    color_t rgb = {255, 255, 255, 255};
    color_yuv_t yuv;
    rgb2yuv(&rgb, &yuv);
    ASSERT_FLOAT_EQ(yuv.y, 1.0f, 0.01f);
    ASSERT_FLOAT_EQ(yuv.u, 0.0f, 0.02f);
    ASSERT_FLOAT_EQ(yuv.v, 0.0f, 0.02f);
}

TEST(test_rgb2yuv_red) {
    color_t rgb = {255, 0, 0, 255};
    color_yuv_t yuv;
    rgb2yuv(&rgb, &yuv);
    ASSERT_FLOAT_EQ(yuv.y, 0.299f, 0.01f);
    ASSERT_TRUE(yuv.v > 0.0f);
}

TEST(test_rgb2yuv_green) {
    color_t rgb = {0, 255, 0, 255};
    color_yuv_t yuv;
    rgb2yuv(&rgb, &yuv);
    ASSERT_FLOAT_EQ(yuv.y, 0.587f, 0.01f);
}

TEST(test_rgb2yuv_blue) {
    color_t rgb = {0, 0, 255, 255};
    color_yuv_t yuv;
    rgb2yuv(&rgb, &yuv);
    ASSERT_FLOAT_EQ(yuv.y, 0.114f, 0.01f);
    ASSERT_TRUE(yuv.u > 0.0f);
}

TEST(test_col_yuv_distance_same) {
    color_yuv_t a = {0.5f, 0.1f, 0.2f};
    color_yuv_t b = {0.5f, 0.1f, 0.2f};
    ASSERT_FLOAT_EQ(col_yuv_distance(&a, &b), 0.0f, 0.0001f);
}

TEST(test_col_yuv_distance_different) {
    color_yuv_t a = {0.0f, 0.0f, 0.0f};
    color_yuv_t b = {1.0f, 0.0f, 0.0f};
    ASSERT_FLOAT_EQ(col_yuv_distance(&a, &b), 1.0f, 0.0001f);
}

TEST(test_col_yuv_distance_symmetric) {
    color_yuv_t a = {0.3f, 0.1f, 0.2f};
    color_yuv_t b = {0.7f, 0.4f, 0.5f};
    float d1 = col_yuv_distance(&a, &b);
    float d2 = col_yuv_distance(&b, &a);
    ASSERT_FLOAT_EQ(d1, d2, 0.0001f);
}

TEST(test_col_cpy) {
    color_t src = {100, 150, 200, 128};
    color_t dst = {0, 0, 0, 0};
    col_cpy(&src, &dst);
    ASSERT_EQ(dst.r, 100);
    ASSERT_EQ(dst.g, 150);
    ASSERT_EQ(dst.b, 200);
    ASSERT_EQ(dst.a, 128);
}

TEST(test_col_cpy_preserves_source) {
    color_t src = {10, 20, 30, 40};
    color_t dst;
    col_cpy(&src, &dst);
    ASSERT_EQ(src.r, 10);
    ASSERT_EQ(src.g, 20);
    ASSERT_EQ(src.b, 30);
    ASSERT_EQ(src.a, 40);
}

TEST(test_init_free_hash_colors) {
    free_hash_colors();
    init_hash_colors();
}

TEST(test_convert_color_exact_palette) {
    color_t col = {255, 0, 0, 255};
    color_t out = {0, 0, 0, 255};
    convert_color(&col, &out);
    ASSERT_EQ(out.r, 255);
    ASSERT_EQ(out.g, 0);
    ASSERT_EQ(out.b, 0);
}

TEST(test_convert_color_transparent_noop) {
    color_t col = {123, 45, 67, 0};
    color_t out = {0, 0, 0, 0};
    convert_color(&col, &out);
    ASSERT_EQ(out.r, 0);
    ASSERT_EQ(out.g, 0);
    ASSERT_EQ(out.b, 0);
}

TEST(test_convert_color_near_white) {
    color_t col = {254, 254, 254, 255};
    color_t out = {0, 0, 0, 255};
    convert_color(&col, &out);
    ASSERT_EQ(out.r, 255);
    ASSERT_EQ(out.g, 255);
    ASSERT_EQ(out.b, 255);
}

TEST(test_convert_color_near_black) {
    color_t col = {1, 1, 1, 255};
    color_t out = {0, 0, 0, 255};
    convert_color(&col, &out);
    ASSERT_TRUE(out.r <= 8);
    ASSERT_TRUE(out.g <= 8);
    ASSERT_TRUE(out.b <= 8);
}

// ============================================================================
// sh_utils tests
// ============================================================================

TEST(test_rgb2x_macro) {
    uint32_t x = RGB2X(0xFF, 0x80, 0x40);
    ASSERT_EQ(x, 0xFF8040);
}

TEST(test_x2r_macro) {
    ASSERT_EQ(X2R(0xFF8040), 0xFF);
}

TEST(test_x2g_macro) {
    ASSERT_EQ(X2G(0xFF8040), 0x80);
}

TEST(test_x2b_macro) {
    ASSERT_EQ(X2B(0xFF8040), 0x40);
}

TEST(test_rgb2x_roundtrip) {
    uint8_t r = 123, g = 45, b = 67;
    uint32_t x = RGB2X(r, g, b);
    ASSERT_EQ(X2R(x), r);
    ASSERT_EQ(X2G(x), g);
    ASSERT_EQ(X2B(x), b);
}

TEST(test_rgb2x_black) {
    ASSERT_EQ(RGB2X(0, 0, 0), 0x000000);
}

TEST(test_rgb2x_white) {
    ASSERT_EQ(RGB2X(255, 255, 255), 0xFFFFFF);
}

TEST(test_squared_macro) {
    ASSERT_EQ(SQUARED(5), 25);
    ASSERT_EQ(SQUARED(0), 0);
    ASSERT_EQ(SQUARED(1), 1);
    ASSERT_EQ(SQUARED(-3), 9);
}

// ============================================================================
// sh_image tests
// ============================================================================

TEST(test_img_create) {
    image_t img;
    img_create(&img, 10, 10);
    for (uint32_t i = 0; i < 100; i++) {
        ASSERT_EQ(img.pixels[i].a, 0);
    }
    free(img.pixels);
}

TEST(test_img_create_dimensions) {
    image_t img;
    img.width = 0;
    img.height = 0;
    img_create(&img, 32, 16);
    img.width = 32;
    img.height = 16;
    img.pixels[0].r = 42;
    ASSERT_EQ(img.pixels[0].r, 42);
    free(img.pixels);
}

TEST(test_img_set_get_pixel) {
    image_t img;
    img_create(&img, 10, 10);
    img.width = 10;
    img.height = 10;

    color_t col = {255, 128, 64, 200};
    img_set_pixel(&img, 5, 5, &col);

    const color_t *got = img_get_pixel(&img, 5, 5);
    ASSERT_EQ(got->r, 255);
    ASSERT_EQ(got->g, 128);
    ASSERT_EQ(got->b, 64);
    ASSERT_EQ(got->a, 200);

    free(img.pixels);
}

TEST(test_img_set_pixel_corners) {
    image_t img;
    img_create(&img, 8, 8);
    img.width = 8;
    img.height = 8;

    color_t red = {255, 0, 0, 255};
    color_t green = {0, 255, 0, 255};
    color_t blue = {0, 0, 255, 255};
    color_t white = {255, 255, 255, 255};

    img_set_pixel(&img, 0, 0, &red);
    img_set_pixel(&img, 7, 0, &green);
    img_set_pixel(&img, 0, 7, &blue);
    img_set_pixel(&img, 7, 7, &white);

    ASSERT_EQ(img_get_pixel(&img, 0, 0)->r, 255);
    ASSERT_EQ(img_get_pixel(&img, 7, 0)->g, 255);
    ASSERT_EQ(img_get_pixel(&img, 0, 7)->b, 255);
    ASSERT_EQ(img_get_pixel(&img, 7, 7)->r, 255);
    ASSERT_EQ(img_get_pixel(&img, 7, 7)->g, 255);
    ASSERT_EQ(img_get_pixel(&img, 7, 7)->b, 255);

    free(img.pixels);
}

TEST(test_img_copy) {
    image_t src, dst;
    img_create(&src, 4, 4);
    src.width = 4;
    src.height = 4;
    src.frames = 1;

    color_t col = {10, 20, 30, 255};
    for (uint32_t y = 0; y < 4; y++)
        for (uint32_t x = 0; x < 4; x++)
            img_set_pixel(&src, x, y, &col);

    img_copy(&dst, &src);

    ASSERT_EQ(dst.width, 4);
    ASSERT_EQ(dst.height, 4);

    for (uint32_t i = 0; i < 16; i++) {
        ASSERT_EQ(dst.pixels[i].r, 10);
        ASSERT_EQ(dst.pixels[i].g, 20);
        ASSERT_EQ(dst.pixels[i].b, 30);
        ASSERT_EQ(dst.pixels[i].a, 255);
    }

    dst.pixels[0].r = 99;
    ASSERT_EQ(src.pixels[0].r, 10);

    free(src.pixels);
    free(dst.pixels);
}

TEST(test_img_resize_downscale) {
    image_t img;
    img_create(&img, 8, 8);
    img.width = 8;
    img.height = 8;
    img.frames = 1;
    img.delays = NULL;

    for (uint32_t i = 0; i < 64; i++) {
        img.pixels[i] = (color_t){255, 0, 0, 255};
    }

    img_resize(&img, 0.5f, 0.5f);
    ASSERT_EQ(img.width, 4);
    ASSERT_EQ(img.height, 4);

    for (uint32_t i = 0; i < 16; i++) {
        ASSERT_EQ(img.pixels[i].r, 255);
        ASSERT_EQ(img.pixels[i].g, 0);
        ASSERT_EQ(img.pixels[i].b, 0);
    }

    free(img.pixels);
}

TEST(test_img_convert_colors) {
    image_t img;
    img_create(&img, 2, 2);
    img.width = 2;
    img.height = 2;
    img.frames = 1;
    img.delays = NULL;

    for (uint32_t i = 0; i < 4; i++) {
        img.pixels[i] = (color_t){0, 128, 0, 255};
    }

    img_convert_colors(&img);

    for (uint32_t i = 0; i < 4; i++) {
        ASSERT_TRUE(img.pixels[i].a == 255);
    }

    free(img.pixels);
}

TEST(test_img_free) {
    image_t img;
    img_create(&img, 4, 4);
    img.width = 4;
    img.height = 4;
    img.frames = 1;
    img.delays = NULL;
    img_free(&img);
}

TEST(test_img_free_with_delays) {
    image_t img;
    img_create(&img, 4, 4);
    img.width = 4;
    img.height = 4;
    img.frames = 2;
    img.delays = malloc(sizeof(uint16_t) * 2);
    img.delays[0] = 100;
    img.delays[1] = 200;
    img_free(&img);
}

TEST(test_img_load_from_file_png) {
    image_t img;
    img_load_from_file(&img, "test-images/mewtwo-front.png");
    ASSERT_TRUE(img.width > 0);
    ASSERT_TRUE(img.height > 0);
    ASSERT_TRUE(img.pixels != NULL);
    ASSERT_EQ(img.frames, 1);
    img_free(&img);
}

TEST(test_img_load_from_file_ico) {
    image_t img;
    img_load_from_file(&img, "test-images/google.ico");
    ASSERT_TRUE(img.width > 0);
    ASSERT_TRUE(img.height > 0);
    ASSERT_TRUE(img.pixels != NULL);
    img_free(&img);
}

TEST(test_img_resize_preserves_frames) {
    image_t img;
    img_create(&img, 16, 16);
    img.width = 16;
    img.height = 16;
    img.frames = 1;
    img.delays = NULL;

    for (uint32_t i = 0; i < 256; i++) {
        img.pixels[i] = (color_t){128, 128, 128, 255};
    }

    img_resize(&img, 0.25f, 0.25f);
    ASSERT_EQ(img.width, 4);
    ASSERT_EQ(img.height, 4);
    ASSERT_EQ(img.frames, 1);

    free(img.pixels);
}

// ============================================================================
// Main
// ============================================================================

int main(void) {
    init_hash_colors();

    printf("\n=== sh_color tests ===\n");
    RUN_TEST(test_rgb2yuv_black);
    RUN_TEST(test_rgb2yuv_white);
    RUN_TEST(test_rgb2yuv_red);
    RUN_TEST(test_rgb2yuv_green);
    RUN_TEST(test_rgb2yuv_blue);
    RUN_TEST(test_col_yuv_distance_same);
    RUN_TEST(test_col_yuv_distance_different);
    RUN_TEST(test_col_yuv_distance_symmetric);
    RUN_TEST(test_col_cpy);
    RUN_TEST(test_col_cpy_preserves_source);
    RUN_TEST(test_init_free_hash_colors);
    RUN_TEST(test_convert_color_exact_palette);
    RUN_TEST(test_convert_color_transparent_noop);
    RUN_TEST(test_convert_color_near_white);
    RUN_TEST(test_convert_color_near_black);

    printf("\n=== sh_utils tests ===\n");
    RUN_TEST(test_rgb2x_macro);
    RUN_TEST(test_x2r_macro);
    RUN_TEST(test_x2g_macro);
    RUN_TEST(test_x2b_macro);
    RUN_TEST(test_rgb2x_roundtrip);
    RUN_TEST(test_rgb2x_black);
    RUN_TEST(test_rgb2x_white);
    RUN_TEST(test_squared_macro);

    printf("\n=== sh_image tests ===\n");
    RUN_TEST(test_img_create);
    RUN_TEST(test_img_create_dimensions);
    RUN_TEST(test_img_set_get_pixel);
    RUN_TEST(test_img_set_pixel_corners);
    RUN_TEST(test_img_copy);
    RUN_TEST(test_img_resize_downscale);
    RUN_TEST(test_img_convert_colors);
    RUN_TEST(test_img_free);
    RUN_TEST(test_img_free_with_delays);
    RUN_TEST(test_img_load_from_file_png);
    RUN_TEST(test_img_load_from_file_ico);
    RUN_TEST(test_img_resize_preserves_frames);

    free_hash_colors();

    printf("\n%d/%d tests passed\n\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
