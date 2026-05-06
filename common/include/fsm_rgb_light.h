/**
 * @file fsm_rgb_light.h
 * @brief Header for fsm_rgb_light.c file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 14/04/2026
 */

#ifndef FSM_RGB_LIGHT_SYSTEM_H_
#define FSM_RGB_LIGHT_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "fsm.h"
#include "rgb_colors.h"
/* Defines and enums ----------------------------------------------------------*/

#define MAX_LEVEL_INTENSITY 100
/* Enums */
/**
 * @brief Enumerator for the RGB light system finite state machine.
 *
 */
enum FSM_RGB_LIGHT_SYSTEM
{
    IDLE_RGB = 0,
    SET_COLOR
};
/* Typedefs --------------------------------------------------------------------*/

/**
 * @brief Structure to define the FSM of the RGB light system.
 *
 */
typedef struct
{
    fsm_t f;
    rgb_color_t color;
    uint8_t intensity_perc;
    bool new_color;
    bool status;
    bool idle;
    uint8_t rgb_light_id;
} fsm_rgb_light_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief This function creates a new RGB light FSM with the given RGB light ID.
 *
 * @param rgb_light_id RGB light ID. Must be unique.
 * @return fsm_rgb_light_t*
 */
fsm_rgb_light_t *fsm_rgb_light_new(uint8_t rgb_light_id);

/**
 * @brief This function destroys an RGB light FSM and frees the memory.
 *
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 */
void fsm_rgb_light_destroy(fsm_rgb_light_t *p_fsm);

/**
 * @brief This function sets the color and intensity of the RGB light.
 *
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 * @param color RGB color to set.
 * @param intensity_perc Intensity percentage to set (0-100).
 */
void fsm_rgb_light_set_color_intensity(fsm_rgb_light_t *p_fsm, rgb_color_t color, uint8_t intensity_perc);

/**
 * @brief This function is used to fire the RGB light FSM. It is used to check the transitions and execute the actions of the RGB light FSM.
 *
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 */
void fsm_rgb_light_fire(fsm_rgb_light_t *p_fsm);

/**
 * @brief This function returns the status of the RGB light system. This function might be used for testing and debugging purposes.
 *
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 * @return bool True if the RGB light system is active, false otherwise.
 */
bool fsm_rgb_light_get_status(fsm_rgb_light_t *p_fsm);

/**
 * @brief This function is used to set the status of the RGB light system. Indicating if the RGB light system is active or paused.
 *
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 * @param pause True if the RGB light system is paused, false otherwise.
 */
void fsm_rgb_light_set_status(fsm_rgb_light_t *p_fsm, bool pause);

/**
 * @brief This function checks if the RGB light system is active.
 * 
 * @param p_fsm Pointer to an fsm_rgb_light_t struct.
 * @return true if the RGB light system is active,
 * @return false if the RGB light system is paused.
 */
bool fsm_rgb_light_check_activity(fsm_rgb_light_t *p_fsm);

#endif /* FSM_RGB_LIGHT_SYSTEM_H_ */