/**
 * @file fsm_rgb_light.c
 * @brief RGB light system FSM main file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 16/04/2026
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>
/* HW dependent includes */
#include "port_rgb_light.h"
#include "port_system.h"
#include "fsm.h"
/* Project includes */
#include "fsm_rgb_light.h"
#include "rgb_colors.h"
/* Typedefs --------------------------------------------------------------------*/

/* Private functions -----------------------------------------------------------*/

/**
 * @brief This function takes a regular color and applies a reduction based on the given intensity.
 *
 * @param p_color Pointer to the color to be corrected.
 * @param intensity_perc Intensity percentage to apply to the color. Must be between 0 and 100.
 */
void _correct_rgb_light_levels(rgb_color_t *p_color, uint8_t intensity_perc)
{
    float intensity_factor = (float)intensity_perc / (float)MAX_LEVEL_INTENSITY; // Calculate the intensity factor as a value between 0 and 1
    /* Correct the RGB levels according to the intensity percentage */
    p_color->r = (uint8_t)((p_color->r * intensity_factor) + 0.5f); // Scale the red level according to the intensity percentage
    p_color->g = (uint8_t)((p_color->g * intensity_factor) + 0.5f); // Scale the green level according to the intensity percentage
    p_color->b = (uint8_t)((p_color->b * intensity_factor) + 0.5f); // Scale the blue level according to the intensity percentage
}

/* State machine input or transition functions */
/**
 * @brief Check if the RGB light is set to be active (ON), independently if it is idle or not.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_rgb_light_t.
 * @return true if the RGB light is set to be active, false otherwise.
 * @return false if the RGB light is not set to be active.
 */
static bool check_active(fsm_t *p_this)
{
    fsm_rgb_light_t *p_fsm_rgb_light = (fsm_rgb_light_t *)p_this; /* Cast the generic FSM pointer to the specific RGB light FSM pointer */
    return p_fsm_rgb_light->status;                               /* Return the status of the RGB light */
}

/**
 * @brief Check if a new color has to be set.
 * @param p_this Pointer to an fsm_t struct than contains an fsm_rgb_light_t.
 * @return true if a new color has to be set, false otherwise.
 * @return false if no new color has to be set.
 */
static bool check_set_new_color(fsm_t *p_this)
{
    fsm_rgb_light_t *p_fsm_rgb_light = (fsm_rgb_light_t *)p_this; /* Cast the generic FSM pointer to the specific RGB light FSM pointer */
    return p_fsm_rgb_light->new_color;                            /* Return the new_color flag of the RGB light */
}

/**
 * @brief Check if the RGB light is set to be inactive (OFF).
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_rgb_light_t.
 * @return true If the RGB light system has been indicated to be inactive.
 * @return false  If the RGB light system has been indicated to be active.
 */
static bool check_off(fsm_t *p_this)
{
    fsm_rgb_light_t *p_fsm_rgb_light = (fsm_rgb_light_t *)p_this; /* Cast the generic FSM pointer to the specific RGB light FSM pointer */
    return !p_fsm_rgb_light->status;                              /* Return the negation of the status of the RGB light */
}

/* State machine output or action functions */

/**
 * @brief Turn the RGB light system ON for the first time.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_rgb_light_t.

 */
static void do_set_on(fsm_t *p_this)
{
    fsm_rgb_light_t *p_fsm_rgb_light = (fsm_rgb_light_t *)p_this;     /* Cast the generic FSM pointer to the specific RGB light FSM pointer */
    port_rgb_light_set_rgb(p_fsm_rgb_light->rgb_light_id, color_off); /* Set the RGB light to off color to make sure it is off before setting the new color */
}

/**
 * @brief Set the color of the RGB LED according to the intensity measured by the ultrasound sensor.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_rgb_light_t.

 */
static void do_set_color(fsm_t *p_this)
{
    fsm_rgb_light_t *p_fsm_rgb_light = (fsm_rgb_light_t *)p_this;                        /* Cast the generic FSM pointer to the specific RGB light FSM pointer */
    _correct_rgb_light_levels(&p_fsm_rgb_light->color, p_fsm_rgb_light->intensity_perc); /* Correct the RGB levels according to the intensity percentage */
    port_rgb_light_set_rgb(p_fsm_rgb_light->rgb_light_id, p_fsm_rgb_light->color);       /* Set the RGB light color according to the corrected color */
    p_fsm_rgb_light->new_color = false;                                                  /* Clear the new_color flag */
    p_fsm_rgb_light->idle = true;                                                        /* Set the RGB light to idle until a new color is set */
}

/**
 * @brief Turn the RGB light system OFF.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_rgb_light_t.
 */
static void do_set_off(fsm_t *p_this)
{
    fsm_rgb_light_t *p_fsm_rgb_light = (fsm_rgb_light_t *)p_this;     /* Cast the generic FSM pointer to the specific RGB light FSM pointer */
    port_rgb_light_set_rgb(p_fsm_rgb_light->rgb_light_id, color_off); /* Set the RGB light to off color to turn it off */
    p_fsm_rgb_light->idle = false;                                    /* Reset the flag idle to indicate that the RGB light system is not idle */
}

/* Other auxiliary functions */

/**
 * @brief This function is used to fire the RGB light FSM. It is used to check the transitions and execute the actions of the RGB light FSM.
 *
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 */
void fsm_rgb_light_fire(fsm_rgb_light_t *p_fsm)
{
    fsm_fire(&p_fsm->f); /* Call the generic FSM fire function with the pointer to the FSM struct */
}

/**
 * @brief This function destroys an RGB light FSM and frees the memory.
 *
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 */
void fsm_rgb_light_destroy(fsm_rgb_light_t *p_fsm)
{
    fsm_destroy(&p_fsm->f); /* Call the generic FSM destroy function with the pointer to the FSM struct */
}

/**
 * @brief Array representing the transitions table of the FSM RGB light.
 *
 */
static fsm_trans_t fsm_trans_rgb_light[] = {

    {IDLE_RGB, check_active, SET_COLOR, do_set_on},            /* If the RGB light is idle and it is set to be active, turn it on for the first time */
    {SET_COLOR, check_set_new_color, SET_COLOR, do_set_color}, /* If the RGB light is active and a new color has to be set, set the new color */
    {SET_COLOR, check_off, IDLE_RGB, do_set_off},              /* If the RGB light is active and it is set to be inactive, turn it off */
    {-1, NULL, -1, NULL}                                       /* End of the transition table */
};

/**
 * @brief Initialize an RGB light system FSM.
 * This function initializes the default values of the FSM struct and calls to the port to initialize the associated HW given the ID.
 * The FSM stores the RGB light level of the RGB light system. The user should set it using the function fsm_rgb_light_set_color_intensity().
 *
 * @param p_fsm_rgb_light
 * @param rgb_light_id
 */
static void fsm_rgb_light_init(fsm_rgb_light_t *p_fsm_rgb_light, uint8_t rgb_light_id)
{
    fsm_init(&p_fsm_rgb_light->f, fsm_trans_rgb_light); /* Initialize the FSM struct with the transition table */

    p_fsm_rgb_light->rgb_light_id = rgb_light_id; /* Initialize the RGB light ID */

    p_fsm_rgb_light->intensity_perc = MAX_LEVEL_INTENSITY; /* Initialize the intensity percentage to the maximum level */
    p_fsm_rgb_light->color = color_off;                    /* Initialize the color to OFF */

    p_fsm_rgb_light->new_color = false; /* Initialize the new_color flag to false */
    p_fsm_rgb_light->status = false;    /* Initialize the status of the RGB light to false (OFF) */
    p_fsm_rgb_light->idle = false;      /* Initialize the idle flag to false */

    port_rgb_light_init(rgb_light_id); /* Initialize the RGB light HW with the given ID */
}
/* Public functions -----------------------------------------------------------*/
fsm_rgb_light_t *fsm_rgb_light_new(uint8_t rgb_light_id)
{
    fsm_rgb_light_t *p_fsm_rgb_light = malloc(sizeof(fsm_rgb_light_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_rgb_light_init(p_fsm_rgb_light, rgb_light_id);                  /* Initialize the FSM */
    return p_fsm_rgb_light;
}

void fsm_rgb_light_set_color_intensity(fsm_rgb_light_t *p_fsm, rgb_color_t color, uint8_t intensity_perc)
{
    p_fsm->color = color;                   /* Set the new color in the FSM struct */
    p_fsm->intensity_perc = intensity_perc; /* Set the new intensity percentage in the FSM struct */
    p_fsm->new_color = true;                /* Set the new_color flag to true to indicate that a new color has to be set */
    p_fsm->idle = false;                   /* Clear the idle flag to indicate that the RGB light system is not idle */
}

bool fsm_rgb_light_get_status(fsm_rgb_light_t *p_fsm)
{
    return p_fsm->status; /* Return the status of the RGB light system */
}

void fsm_rgb_light_set_status(fsm_rgb_light_t *p_fsm, bool pause)
{
    p_fsm->status = pause;
}

bool fsm_rgb_light_check_activity(fsm_rgb_light_t *p_fsm)
{
    return p_fsm->status && !p_fsm->idle; /* Return true if the RGB light system is active and is not idle, false otherwise */
}