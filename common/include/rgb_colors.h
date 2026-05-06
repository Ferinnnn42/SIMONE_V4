/**
 * @file rgb_colors.h
 * @brief Header to define RGB colors and related constants.
 * @author Sistemas Digitales II
 * @date 2026-01-01
 */

#ifndef RGB_COLORS_H_
#define RGB_COLORS_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define COLOR_RGB_MAX_VALUE 255 /*!< Maximum value for the RGB LED @hideinitializer */
#define NUMBER_OF_COLORS_GAME 6 /*!< Number of defined colors in rgb_colors.c*/
#define SIMONE_TIMEOFF_BETWEEN_COLORS_MS 300 /*!< Time in milliseconds that the RGB LED will be off between two consecutive colors in the game mode.*/
#define SIMONE_TIME_WAIT_INPUT_MS 5000 /*!< Time in milliseconds that the system will wait for the user input in the game mode. */
#define SIMONE_TIME_VISUAL_FEEDBACK_MS 300 /*!< Time in milliseconds that the RGB LED will be on to provide visual feedback of a correct or incorrect user input in the game mode. */ 
/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define an RGB color.
 */
typedef struct
{
    uint8_t r; /*!< Red color value */
    uint8_t g; /*!< Green color value */
    uint8_t b; /*!< Blue color value */
} rgb_color_t;

// Colors must be defined in rgb_colors.c, and declared here as extern
// Check other colors: https://www.downtownuplighting.com/rgb-color-chart
extern const rgb_color_t color_red;
extern const rgb_color_t color_green;
extern const rgb_color_t color_blue;
extern const rgb_color_t color_yellow;
extern const rgb_color_t color_white;
extern const rgb_color_t color_turquoise;
extern const rgb_color_t color_off;

#endif /* RGB_COLORS_H_ */