/**
 * @file port_rgb_light.h
 * @brief Header for the portable functions to interact with the HW of the RGB light system. The functions must be implemented in the platform-specific code.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 07/04/2026
 */
#ifndef PORT_RGB_LIGHT_SYSTEM_H_
#define PORT_RGB_LIGHT_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include "rgb_colors.h"
/* Typedefs --------------------------------------------------------------------*/

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define PORT_RGB_LIGHT_ID 0
#define PORT_RGB_LIGHT_PWM_TIMEOUT 20 // PWM timeout in ms
/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given RGB light.
 * @param rgb_light_id
 */
void port_rgb_light_init(uint8_t rgb_light_id);

/**
 * @brief Set the Capture/Compare register values for each channel of the RGB LED given a color.
 * @param rgb_light_id The ID of the RGB light system.
 * @param color The color to set.
 */
void port_rgb_light_set_rgb(uint8_t rgb_light_id, rgb_color_t color);

#endif /* PORT_RGB_LIGHT_SYSTEM_H_ */