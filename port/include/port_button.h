/**
 * @file port_button.h
 * @brief Header for the portable functions to interact with the HW of the buttons. The functions must be implemented in the platform-specific code.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 17/02/2026
 */

#ifndef PORT_BUTTON_H_
#define PORT_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
// Define here all the button identifiers that are used in the system

#define PORT_USER_BUTTON_ID 0 /*!< id del boton azul*/
#define PORT_USER_BUTTON_DEBOUNCE_TIME_MS 150

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given button
 *
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 *  
 * @retval None
 */

void port_button_init (uint8_t button_id);

/**
 * @brief Return the status of the button (pressed or not)
 *
 * @param button_id Button ID. This index is used to get the correct button status struct
 * 
 * @retval bool Returns 'true' if the the button has been pressed,
 * 'false' if the button has not been pressed
 */
bool port_button_get_pressed(uint8_t button_id);


#endif