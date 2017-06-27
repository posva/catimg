#ifndef __SH_IMAGE_H__
#define __SH_IMAGE_H__
/*******************************************************************************
 *  sh_image                                                                   *
 *  Wed Dec 18 CET 2013                                                        *
 *  Copyright Eduardo San Martin Morote                                        *
 *  eduardo.san-martin-morote@ensimag.fr                                       *
 *  http://posva.net                                                           *
 ******************************************************************************/

#include <stdint.h>
#include "sh_color.h"

/**
 * @defgroup sh_image sh_image
 * This module allows to load pictures from these formats:
 *
 *    <ul>
 *    <li>JPEG baseline (no JPEG progressive)</li>
 *
 *    <li>PNG 8-bit only</li>
 *
 *    <li>TGA (not sure what subset, if a subset)</li>
 *
 *    <li>BMP non-1bpp, non-RLE</li>
 *
 *    <li>PSD (composited view only, no extra channels)</li>
 *
 *    <li>GIF (*comp always reports as 4-channel)</li>
 *
 *    <li>HDR (radiance rgbE format)</li>
 *
 *    <li>PIC (Softimage PIC)</li>
 *    </ul>
 *
 * It also implements the needed functions to resize, copy and convert the
 * colors of an image
 * @{
 */

/**
 * @brief Structure to handle images
 */
typedef struct {
        color_t *pixels; ///< 1 dim pixels pixels[x+y*w]
        uint32_t width, ///< Width of the image
                 height, ///< Height of the image
                 frames; ///< Number of frames
        uint16_t *delays; ///< Array of delays. Length = frames - 1
} image_t;

/**
 * @brief Load an image from a file
 *
 * @param img instance
 * @param file file to load
 */
void img_load_from_file(image_t *img, const char* file);

/**
 * @brief Load an image from stdin
 *
 * @param img instance
 */
void img_load_from_stdin(image_t *img);

/**
 * @brief Init a transparent picture
 *
 * @param img instance
 */
void img_create(image_t *img, uint32_t width, uint32_t height);

/**
 * @brief Copy a loaded image into an unitialzed image
 *
 * @param img instance affected
 * @param cpy image we want to copy
 */
void img_copy(image_t* img, image_t *cpy);

/**
 * @brief Free the ressources for an image
 *
 * @param img instance affected
 */
void img_free(image_t *img);

/**
 * @brief Resize an image using the nearest color
 *
 * @param img instance affected
 * @param wsc horizontal scale
 * @param hsc vertical scale
 */
void img_resize(image_t *img, float wsc, float hsc);

/**
 * @brief Convert the colors from an image to the terminal palette
 *
 * @param img instance affected
 */
void img_convert_colors(image_t *img);

/**
 * @brief get the color of a pixel
 *
 * @param img instance affected
 * @return a const pointer to the color
 */
const color_t* img_get_pixel(image_t *img, uint32_t x, uint32_t y);

/**
 * @brief change the color of a pixel
 *
 * @param img instance affected
 * @return a const pointer to the color
 */
void img_set_pixel(image_t *img, uint32_t x, uint32_t y, const color_t *col);

///@}
#endif
