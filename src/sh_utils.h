#ifndef __SH_UTILS_H__
#define __SH_UTILS_H__
/*******************************************************************************
 *  sh_utils                                                                   *
 *  Wed Dec 18 CET 2013                                                        *
 *  Copyright Eduardo San Martin Morote                                        *
 *  eduardo.san-martin-morote@ensimag.fr                                       *
 *  http://posva.net                                                           *
 ******************************************************************************/

#include <stdint.h>

/**
 * @defgroup sh_utils sh_utils
 * Utilities and helpers for sh2d
 * @{
 */

#define SQUARED(x) ((x)*(x)) ///< x*x
#define RGB2X(r,g,b) (((r) << 16) | ((g) << 8) | (b)) ///< rgb in uint32_t as 0x00RRGGBB
#define X2R(x) (((x) & 0xff0000) >> 16) ///< Extract Red from uint32_t
#define X2G(x) (((x) & 0xff00) >> 8) ///< Extract Green from uint32_t
#define X2B(x) ((x) & 0xff) ///< Extract Blue from uint32_t

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define WINDOWS
#endif

/**
 * @brief Get the number of columns for the terminal
 * @return the number of columns
 */
uint32_t terminal_columns();

/**
 * @brief Get the number of rows for the terminal
 * @return the number of rows
 */
uint32_t terminal_rows();


/**
 * @brief Read binary data from stdin
 * @param ptr Address to the ptr which will be filled with the data
 * @return number of bytes read
 */
uint32_t read_stdin(unsigned char **ptr);

///@}
#endif
