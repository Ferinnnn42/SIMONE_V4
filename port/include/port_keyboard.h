/**
 * @file port_keyboard.h
 * @brief Header for the portable functions to interact with the HW of the keyboards. The functions must be implemented in the platform-specific code.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 03/03/2026
 */

#define PORT_KEYBOARD_MAIN_ID 0
#define PORT_KEYBOARDS_TIMEOUT_MS 25
#define PORT_KEYBOARD_MAIN_DEBOUNCE_TIME_MS 150
#ifndef PORT_KEYBOARD_H_
#define PORT_KEYBOARD_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */

#include <stdint.h>
#include <stdbool.h>

/* Defines and enums ----------------------------------------------------------*/

enum PORT_KEYBOARD_COL_IDS
{
    PORT_KEYBOARD_COL_0,
    PORT_KEYBOARD_COL_1,
    PORT_KEYBOARD_COL_2,
    PORT_KEYBOARD_COL_3
};

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given keyboard.
 * * Assuming we are using an STM32F4-based platform, this function must call
 * the necessary low-level initialization functions for GPIOs and timers.
 * * @param keyboard_id Identifier of the keyboard to be initialized.
 */
void port_keyboard_init(uint8_t keyboard_id);

/**
 * @brief Set the given row to HIGH and lower the others.
 * * This function handles the individual excitation of a specific row in the matrix
 * to allow the scanning logic to detect which column closes the circuit.
 * * @param keyboard_id Identifier of the keyboard.
 * @param row_idx Index of the row to be set to HIGH.
 */
void port_keyboard_excite_row(uint8_t keyboard_id, uint8_t row_idx);

/**
 * @brief Start the scanning of a keyboard.
 * * This function starts the scanning of a keyboard by enabling the timer that
 * controls the duration of the column scanning and setting the first row to HIGH.
 * * @param keyboard_id Identifier of the keyboard to start scanning.
 */
void port_keyboard_start_scan(uint8_t keyboard_id);

/**
 * @brief Stop the scanning of a keyboard.
 * * Disables the timers and clears the excitation of the rows to put the
 * keyboard interface in an idle state.
 * * @param keyboard_id Identifier of the keyboard to stop scanning.
 */
void port_keyboard_stop_scan(uint8_t keyboard_id);

/**
 * @brief Update the row to be excited.
 * * Moves the excitation to the next sequential row in the matrix.
 * * @param keyboard_id Identifier of the keyboard.
 */
void port_keyboard_excite_next_row(uint8_t keyboard_id);

/**
 * @brief Return the status of the keyboard (pressed or not).
 * * @param keyboard_id Identifier of the keyboard.
 * @return true If a key is currently detected as pressed.
 * @return false If no key is pressed.
 */
bool port_keyboard_get_key_pressed_status(uint8_t keyboard_id);

/**
 * @brief Set the status of the keyboard (pressed or not).
 * * This function is used to update the status of the keyboard when a key
 * press is detected or cleared.
 * * @param keyboard_id Identifier of the keyboard.
 * @param status New pressed status (true for pressed, false for released).
 */
void port_keyboard_set_key_pressed_status(uint8_t keyboard_id, bool status);

/**
 * @brief Return the status of the column timeout flag.
 * * @param keyboard_id Identifier of the keyboard.
 * @return true If the column scanning timeout has occurred.
 * @return false Otherwise.
 */
bool port_keyboard_get_row_timeout_status(uint8_t keyboard_id);

/**
 * @brief Set the status of the row timeout flag.
 * * This function is used to update the status of the row timeout flag when
 * a timeout occurs or is cleared.
 * * @param keyboard_id Identifier of the keyboard.
 * @param status New timeout flag status.
 */
void port_keyboard_set_row_timeout_status(uint8_t keyboard_id, bool status);

/**
 * @brief Return the char representing the key pressed of a given keyboard
 * based on its row that is being excited.
 * * This assumes that the matrix of chars is flattened (i.e., it is not a 2D array,
 * but all rows are in a single array), thus it is necessary to calculate only one index.
 * * @param keyboard_id Identifier of the keyboard.
 * @return char The character value of the pressed key.
 */
char port_keyboard_get_key_value(uint8_t keyboard_id);

/**
 * @brief Return the null key value of a given keyboard.
 * * This function retrieves the null key value defined in the keyboard layout.
 * * @param keyboard_id Identifier of the keyboard.
 * @return char The character representing an invalid or null key.
 */
char port_keyboard_get_invalid_key_value(uint8_t keyboard_id);

#endif /* PORT_KEYBOARD_H_ */
