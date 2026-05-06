/**
 * @file fsm_button.c
 * @brief Button FSM main file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 16/03/2025
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>

/* HW dependent includes */
#include "port_button.h"
#include "port_system.h"

/* Project includes */
#include "fsm_button.h"
#include "fsm.h"

/* State machine input or transition functions */

/**
 * @brief Check if the button has been pressed.
 *
 * @param p_this Pointer to an fsm_t struct that contains an fsm_button_t.
 * @return true If the button is pressed.
 * @return false If the button is not pressed.
 */

static bool check_button_pressed(fsm_t *p_this)
{
    return port_button_get_pressed(((fsm_button_t *)p_this)->button_id);
}

/**
 * @brief Check if the button has been released.
 *
 * @param p_this Pointer to an fsm_t struct that contains an fsm_button_t.
 * @return true If the button has been released.
 * @return false If the button has not been released.
 */

static bool check_button_released(fsm_t *p_this)
{
    return !port_button_get_pressed(((fsm_button_t *)p_this)->button_id);
}

/**
 * @brief Check if the debounce-time has passed.
 *
 * @param p_this Pointer to an fsm_t struct that contains an fsm_button_t.
 * @return true If the debounce-time has passed.
 * @return false If the debounce-time has not passed.
 */

static bool check_timeout(fsm_t *p_this)
{
    uint32_t current_time = port_system_get_millis();
    return current_time > ((fsm_button_t *)p_this)->next_timeout;
}

/**
 * @brief Store the system tick when the button was pressed.
 *
 * @param p_this Pointer to an fsm_t struct that contains an fsm_button_t.
 */

/* State machine output or action functions */

static void do_store_tick_pressed(fsm_t *p_this)
{
    uint32_t current_tick = port_system_get_millis();
    ((fsm_button_t *)p_this)->tick_pressed = current_tick;
    ((fsm_button_t *)p_this)->next_timeout = current_tick + ((fsm_button_t *)p_this)->debounce_time_ms;
}

/**
 * @brief Store the duration of the button press.
 *
 * @param p_this Pointer to an fsm_t struct that contains an fsm_button_t.
 */

static void do_set_duration(fsm_t *p_this)
{
    uint32_t now = port_system_get_millis();
    ((fsm_button_t *)p_this)->duration = now - ((fsm_button_t *)p_this)->tick_pressed;
    ((fsm_button_t *)p_this)->next_timeout = now + ((fsm_button_t *)p_this)->debounce_time_ms;
}

/**
 * @brief Array representing the transitions table of the FSM button.
 *
 */
static fsm_trans_t fsm_trans_button[] = {
    {BUTTON_RELEASED, check_button_pressed, BUTTON_PRESSED_WAIT, do_store_tick_pressed},
    {BUTTON_PRESSED_WAIT, check_timeout, BUTTON_PRESSED, NULL},
    {BUTTON_PRESSED, check_button_released, BUTTON_RELEASED_WAIT, do_set_duration},
    {BUTTON_RELEASED_WAIT, check_timeout, BUTTON_RELEASED, NULL},
    {-1, NULL, -1, NULL} /* End of the transition table */
};

/* Other auxiliary functions */

/**
 * @brief Initialize a button FSM.
 *
 * @param p_fsm_button Pointer to the button FSM.
 * @param debounce_time Anti-debounce time in milliseconds.
 * @param button_id Unique button identifier number.
 */
void fsm_button_init(fsm_button_t *p_fsm_button, uint32_t debounce_time, uint8_t button_id)
{
    fsm_init(&p_fsm_button->f, fsm_trans_button);
    p_fsm_button->debounce_time_ms = debounce_time;
    p_fsm_button->button_id = button_id;
    p_fsm_button->tick_pressed = 0;
    fsm_button_reset_duration(p_fsm_button);
    port_button_init(button_id);
}

/* Public functions -----------------------------------------------------------*/

/* This function creates a new button FSM with the given debounce time and button ID. */
fsm_button_t *fsm_button_new(uint32_t debounce_time, uint8_t button_id)
{
    fsm_button_t *p_fsm_button = malloc(sizeof(fsm_button_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_button_init(p_fsm_button, debounce_time, button_id);   /* Initialize the FSM */
    return p_fsm_button;                                       /* Composite pattern: return the fsm_t pointer as a fsm_button_t pointer */
}

/* FSM-interface functions. These functions are used to interact with the FSM */

uint32_t fsm_button_get_duration(fsm_button_t *p_fsm)
{
    return p_fsm->duration;
}

void fsm_button_reset_duration(fsm_button_t *p_fsm)
{
    p_fsm->duration = 0;
}

/* This function returns the debounce time of the button FSM. */
uint32_t fsm_button_get_debounce_time_ms(fsm_button_t *p_fsm)
{
    return p_fsm->debounce_time_ms;
}

/* This function is used to fire the button FSM. It is used to check the transitions and execute the actions of the button FSM.*/
void fsm_button_fire(fsm_button_t *p_fsm)
{
    fsm_fire(&p_fsm->f); // Is it also possible to do it in this way: fsm_fire((fsm_t *)p_fsm);
}

/* This function destroys a button FSM and frees the memory. */
void fsm_button_destroy(fsm_button_t *p_fsm)
{
    free(&p_fsm->f);
}

bool fsm_button_check_activity(fsm_button_t *p_fsm)
{
    return p_fsm->f.current_state == BUTTON_RELEASED ? false : true; //If the current state is not 0 (BUTTON_RELEASED), the button FSM is active. 
}