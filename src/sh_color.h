#ifndef __SH_COLOR_H__
#define __SH_COLOR_H__
/*******************************************************************************
 *  sh_color                                                                   *
 *  Wed Dec 18 CET 2013                                                        *
 *  Copyright Eduardo San Martin Morote                                        *
 *  eduardo.san-martin-morote@ensimag.fr                                       *
 *  http://posva.net                                                           *
 ******************************************************************************/

#include <stdint.h>

/**
 * @defgroup sh_color sh_color
 * Manipulation of pixels
 * @{
 */

/**
 * @brief Structure defining a color in RGB format
 */
typedef struct {
        uint8_t r, ///< Red
                g, ///< Green
                b, ///< Blue
                a; ///< Alpha
} color_t;

/**
 * @brief Structure defining a color in YUV format
 */
typedef struct {
        float y, ///< Y'
              u, ///< u
              v; ///< v
} color_yuv_t;

#define N_COLORS 247 ///< Number of colors in terminal
//uint32_t color_map[N_COLORS]; ///< palette of the terminal colors in RGB 0x00RRGGBB
// yuv equivalents
//color_yuv_t yuv_color_map[N_COLORS]; ///< palette of the terminal colors in YUV format


/**
 * @brief Convert a color from RGB to YUV
 *
 * @param rgb color in rgb
 * @param yuv returned conversion
 */
void rgb2yuv(const color_t *rgb, color_yuv_t *yuv);

/**
 * @brief Squared distance between 2 YUV colors
 *
 * @param a from color
 * @param b to color
 * @return the distance
 */
float col_yuv_distance(const color_yuv_t *a, const color_yuv_t *b);

/**
 * @brief Convert a color to its nearest equivalent in the terminal palette
 *
 * @param col color to convert
 * @param out returned color
 */
void convert_color(color_t *col, color_t *out);

/**
 * @brief Copy a color
 *
 * @param col color to copy
 * @param out color where to copy
 */
void col_cpy(const color_t *col, color_t *out);

/**
 * @brief Initialize the hastable for the colors
 */
void init_hash_colors();
/**
 * @brief Free the hashtables for the colors
 */
void free_hash_colors();

/// @}
#endif
