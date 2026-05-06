/**
 * @file fsm_keyboard.h
 * @brief Header for fsm_keyboard.c file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 16/03/2025
 */

#ifndef FSM_KEYBOARD_H_
#define FSM_KEYBOARD_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/

/**
 * @brief Enumerator for the keyboard finite state machine.
 * 
 */
enum FSM_KEYBOARD
{
    KEYBOARD_RELEASED_WAIT_ROW = 0, 
    KEYBOARD_RELEASED_WAIT, 
    KEYBOARD_PRESSED, 
    KEYBOARD_PRESSED_WAIT 
};

/* Typedefs --------------------------------------------------------------------*/

/**
 * @brief Structure of the Keyboard FSM.
 * 
 */
typedef struct
{
    fsm_t f; 
    uint32_t debounce_time_ms; 
    char invalid_key; 
    char key_value; 
    uint8_t keyboard_id; 
    uint32_t next_timeout; 
    uint32_t tick_pressed; 
} fsm_keyboard_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Creates a new keyboard FSM with the given debounce time for all keys and keyboard ID.
 *
 * @param debounce_time_ms Debounce time in milliseconds for all keys of the keyboard.
 * @param keyboard_id Keyboard ID. Must be unique.
 * @return fsm_keyboard_t* Pointer to the keyboard FSM.
 */
fsm_keyboard_t *fsm_keyboard_new(uint32_t debounce_time_ms, uint8_t keyboard_id);

/**
 * @brief This function destroys a keyboard FSM and frees the memory.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 */
void fsm_keyboard_destroy(fsm_keyboard_t *p_fsm);

/**
 * @brief This function is used to fire the keyboard FSM. It is used to check the transitions and execute the actions of the keyboard FSM.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 */
void fsm_keyboard_fire(fsm_keyboard_t *p_fsm);

/**
 * @brief This function starts the keyboard scanning by calling the appropriate port function.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 */
void fsm_keyboard_start_scan(fsm_keyboard_t *p_fsm);

/**
 * @brief This function stops the keyboard scanning by calling the appropriate port function.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 */
void fsm_keyboard_stop_scan(fsm_keyboard_t *p_fsm);

/**
 * @brief Return the key pressed of the last keyboard press.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 * @return char Key pressed of the last keyboard press.
 */
char fsm_keyboard_get_key_value(fsm_keyboard_t *p_fsm);

/**
 * @brief This function checks if the last key pressed is valid by comparing it with the invalid key value.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 * @return true if the last key pressed is valid.
 * @return false if the last key pressed is invalid.
 */
bool fsm_keyboard_get_is_valid_key(fsm_keyboard_t *p_fsm);

/**
 * @brief Reset the key pressed of the last keyboard press.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 */
void fsm_keyboard_reset_key_value(fsm_keyboard_t *p_fsm);

/**
 * @brief Check if the keyboard FSM is active, or not. The keyboard is always inactive because it works with events.
 *
 * @param p_fsm Pointer to an fsm_keyboard_t struct.
 * @return true if the keyboard FSM is active.
 * @return false if the keyboard FSM is not active.
 */
bool fsm_keyboard_check_activity(fsm_keyboard_t *p_fsm);

#endif /* FSM_KEYBOARD_H_ */
