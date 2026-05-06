/**
 * @file fsm_button.h
 * @brief Header for fsm_button.c file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 16/03/2025
 */

#ifndef FSM_BUTTON_H_
#define FSM_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
/* Enums */

/**
 * @brief Enumerator for the button finite state machine.
 * 
 */
enum FSM_BUTTON {
  BUTTON_RELEASED = 0, /* Starting state. Also comes here when the button has been released. */
  BUTTON_RELEASED_WAIT, /* State to perform the anti-debounce mechanism for a falling edge */
  BUTTON_PRESSED, /* State while the button is being pressed */
  BUTTON_PRESSED_WAIT /* State to perform the anti-debounce mechanism for a rising edge */
};

/* Typedefs --------------------------------------------------------------------*/

/**
 * @brief Structure of the Button FSM.
 * 
 */
typedef struct 
{
  fsm_t f; /* Button FSM */
  uint32_t debounce_time_ms; /* Button debounce time in ms */
  uint32_t next_timeout; /* Next timeout for the antidebounce in ms */
  uint32_t duration; /* How much time the button has been pressed */
  uint32_t tick_pressed; /* Number of ticks when the button was pressed */
  uint8_t button_id; /* Button ID. Must be unique for each button FSM in the system. It is used to identify the button in the port_button_get_pressed function. */
} fsm_button_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Create a new button FSM.
 * @param debounce_time_ms Debounce time in milliseconds.
 * @param button_id Button ID. Must be unique.
 * @return fsm_button_t* Pointer to the button FSM.
 */

fsm_button_t * 	fsm_button_new (uint32_t debounce_time_ms, uint8_t button_id);

/**
 * @brief Destroy a button FSM.
 * 
 * @param p_fsm Pointer to an fsm_button_t struct.
 */
void 	fsm_button_destroy (fsm_button_t *p_fsm);

/**
 * @brief Fire the button FSM.
 * 
 * @param p_fsm Pointer to an fsm_button_t struct.
 */
void 	fsm_button_fire (fsm_button_t *p_fsm);

/**
 * @brief Return the duration of the last button press.
 * 
 * @param p_fsm Pointer to an fsm_button_t struct.
 * @return uint32_t Duration of the last button press in milliseconds.
 */
uint32_t 	fsm_button_get_duration (fsm_button_t *p_fsm);

/**
 * @brief Reset the duration of the last button press.
 * 
 * @param p_fsm Pointer to an fsm_button_t struct.
 */
void 	fsm_button_reset_duration (fsm_button_t *p_fsm);

/**
 * @brief Get the debounce time of the button FSM.
 * 
 * @param p_fsm Pointer to an fsm_button_t struct.
 * @return uint32_t Debounce time in milliseconds.
 */
uint32_t 	fsm_button_get_debounce_time_ms (fsm_button_t *p_fsm); /* This function returns the debounce time of the button FSM. */

/**
 * @brief Check if the button FSM is active, or not.
 * 
 * @param p_fsm Pointer to an fsm_button_t struct.
 * @return true If the button FSM is active.
 * @return false If the button FSM is not active.
 */
bool 	fsm_button_check_activity (fsm_button_t *p_fsm); /* Check if the button FSM is active, or not. */
#endif