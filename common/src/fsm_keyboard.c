/**
 * @file fsm_keyboard.c
 * @brief Keyboard sensor FSM main file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 16/03/2025
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>
/* HW dependent includes */
#include "port_system.h"
#include "port_keyboard.h"
#include "keyboards.h"
/* Project includes */
#include "fsm.h"
#include "fsm_keyboard.h"

/* Typedefs --------------------------------------------------------------------*/

/* Private functions -----------------------------------------------------------*/

/**
 * @brief Check if the timeout to activate a new row and scan columns has passed.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_keyboard_t.
 * @return true If the timeout has passed.
 * @return false If the timeout has not passed.
 */
static bool check_row_timeout(fsm_t *p_this)
{
    return port_keyboard_get_row_timeout_status(((fsm_keyboard_t *)p_this)->keyboard_id); /* Returns the value of flag_row_timeout of the keyboard with id keyboard_id */
}

/**
 * @brief Check if the keyboard has been pressed.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_keyboard_t.
 * @return true If the keyboard has been pressed.
 * @return false If the keyboard has not been pressed.
 */
static bool check_keyboard_pressed(fsm_t *p_this)
{
    uint8_t pressed_status = port_keyboard_get_key_pressed_status(((fsm_keyboard_t *)p_this)->keyboard_id);
    return pressed_status; /* Returns the value of flag_key_pressed of the keyboard with id keyboard_id */
}

/**
 * @brief Check if the keyboard has been released.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_keyboard_t.
 * @return true If the keyboard has been released.
 * @return false If the keyboard has not been released.
 */
static bool check_keyboard_released(fsm_t *p_this)
{
    uint8_t pressed_status = port_keyboard_get_key_pressed_status(((fsm_keyboard_t *)p_this)->keyboard_id);
    return !pressed_status; /* Returns the opposite value of flag_key_pressed of the keyboard with id keyboard_id */
}

/**
 * @brief Check if the debounce-time has passed.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_keyboard_t.
 * @return true If the debounce-time has passed.
 * @return false If the debounce-time has not passed.
 */
static bool check_timeout(fsm_t *p_this)
{
    uint32_t current_tick = port_system_get_millis();               /* Returns the current value of the system tick */
    return current_tick > ((fsm_keyboard_t *)p_this)->next_timeout; /* Returns true if the current value of the tick is greater than the value of next_timeout of the keyboard with id keyboard_id, false otherwise */
}

/**
 * @brief Store the system tick when the keyboard was pressed.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_keyboard_t.
 */
static void do_store_tick_pressed(fsm_t *p_this)
{
    fsm_keyboard_t *p_fsm = (fsm_keyboard_t *)p_this;
    uint32_t current_tick = port_system_get_millis();             /* Returns the current value of the system tick in milliseconds */
    p_fsm->tick_pressed = current_tick;                           /* Stores the current value of the tick in tick_pressed of the keyboard with id keyboard_id */
    p_fsm->next_timeout = current_tick + p_fsm->debounce_time_ms; /* Updates the value of next_timeout of the keyboard with id keyboard_id to the current tick plus the debounce time in milliseconds */
}

/**
 * @brief Store the key value of the keyboard press.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_keyboard_t.
 */
static void do_set_key_value(fsm_t *p_this)
{
    uint32_t current_tick = port_system_get_millis(); /* Returns the current value of the system tick in milliseconds */
    fsm_keyboard_t *p_fsm = (fsm_keyboard_t *)p_this;
    p_fsm->next_timeout = current_tick + p_fsm->debounce_time_ms; /* Updates the value of next_timeout of the keyboard with id keyboard_id to the current tick plus the debounce time in milliseconds */

    p_fsm->key_value = port_keyboard_get_key_value(p_fsm->keyboard_id);
}

/**
 * @brief Clean the row timeout flag and update the row to be excited.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_keyboard_t.
 */
void do_excite_next_row(fsm_t *p_this)
{
    fsm_keyboard_t *p_fsm = (fsm_keyboard_t *)p_this;
    port_keyboard_set_row_timeout_status(p_fsm->keyboard_id, false); /* Cleans the flag_row_timeout of the keyboard with id keyboard_id */
    port_keyboard_excite_next_row(p_fsm->keyboard_id);               /* Excites the next row of the keyboard with id keyboard_id */
}

/* Public functions -----------------------------------------------------------*/

void fsm_keyboard_start_scan(fsm_keyboard_t *p_fsm)
{
    port_keyboard_start_scan(((fsm_keyboard_t *)p_fsm)->keyboard_id); /* Starts the keyboard scanning by calling the appropriate port function for the keyboard with id keyboard_id */
}

void fsm_keyboard_stop_scan(fsm_keyboard_t *p_fsm)
{
    port_keyboard_stop_scan(((fsm_keyboard_t *)p_fsm)->keyboard_id); /* Stops the keyboard scanning by calling the appropriate port function for the keyboard with id keyboard_id */
}

char fsm_keyboard_get_key_value(fsm_keyboard_t *p_fsm)
{
    return p_fsm->key_value; /* Returns the value of key_value of the keyboard with id keyboard_id, which is the key pressed of the last keyboard press */
}

bool fsm_keyboard_get_is_valid_key(fsm_keyboard_t *p_fsm)
{
    return p_fsm->keyboard_id != p_fsm->invalid_key; /* Returns true if the value of key_value of the keyboard with id keyboard_id is different from the value of invalid_key of the keyboard with id keyboard_id, false otherwise */
}

void fsm_keyboard_reset_key_value(fsm_keyboard_t *p_fsm)
{
    ((fsm_keyboard_t *)p_fsm)->key_value = ((fsm_keyboard_t *)p_fsm)->invalid_key; /* Resets the value of key_value of the keyboard with id keyboard_id to the value of invalid_key of the keyboard with id keyboard_id */
}

bool fsm_keyboard_check_activity(fsm_keyboard_t *p_fsm)
{
    return false; //The keyboard is always inactive because it works with events.
}

/* State machine output or action functions */

static fsm_trans_t fsm_trans_keyboard[] = {
    // 1. First: If there is a key pressed, we will wait for the debounce
    {KEYBOARD_RELEASED_WAIT_ROW, check_keyboard_pressed, KEYBOARD_PRESSED_WAIT, do_store_tick_pressed},

    // 2. Second: If there is no key pressed, we continue scanning rows
    {KEYBOARD_RELEASED_WAIT_ROW, check_row_timeout, KEYBOARD_RELEASED_WAIT_ROW, do_excite_next_row},

    // 3. Debouncing: Wait for the signal to be stable
    {KEYBOARD_PRESSED_WAIT, check_timeout, KEYBOARD_PRESSED, NULL},

    // 4. Wait for the key to be released
    {KEYBOARD_PRESSED, check_keyboard_released, KEYBOARD_RELEASED_WAIT, do_set_key_value},

    // 5. Timeout to allow rescan (Prevents infinite repetitions)
    {KEYBOARD_RELEASED_WAIT, check_timeout, KEYBOARD_RELEASED_WAIT_ROW, NULL},

    {-1, NULL, -1, NULL}};

/**
 * @brief Initialize a keyboard FSM.
 *
 * @param p_fsm_keyboard Pointer to the keyboard FSM.
 * @param debounce_time Anti-debounce time in milliseconds.
 * @param keyboard_id Unique keyboard identifier number.
 */
static void fsm_keyboard_init(fsm_keyboard_t *p_fsm_keyboard, uint32_t debounce_time, uint8_t keyboard_id)
{
    fsm_init(&p_fsm_keyboard->f, fsm_trans_keyboard);

    p_fsm_keyboard->debounce_time_ms = debounce_time;
    p_fsm_keyboard->keyboard_id = keyboard_id;

    p_fsm_keyboard->tick_pressed = 0;
    p_fsm_keyboard->invalid_key = '\0'; /* Value of an invalid key, can be changed to another value if desired */

    port_keyboard_init(keyboard_id);
    p_fsm_keyboard->invalid_key = port_keyboard_get_invalid_key_value(keyboard_id);
    p_fsm_keyboard->key_value = port_keyboard_get_invalid_key_value(keyboard_id);

    fsm_keyboard_reset_key_value(p_fsm_keyboard);
}

fsm_keyboard_t *fsm_keyboard_new(uint32_t debounce_time_ms, uint8_t keyboard_id)
{
    fsm_keyboard_t *p_fsm_keyboard = malloc(sizeof(fsm_keyboard_t));  /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_keyboard_init(p_fsm_keyboard, debounce_time_ms, keyboard_id); /* Initialize the FSM */
    return p_fsm_keyboard;
}

void fsm_keyboard_fire(fsm_keyboard_t *p_fsm)
{
    fsm_fire(&p_fsm->f); /* Fire the FSM by calling the fsm_fire function with the fsm element of the keyboard FSM */
}
