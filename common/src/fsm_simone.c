/**
 * @file fsm_simone.c
 * @brief Simone FSM main file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 21/04/2026
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
/* HW dependent includes */
#include "port_system.h"
#include "port_button.h"
#include "port_simone.h"
#include "rgb_colors.h"
#include "fsm.h"
#include "fsm_simone.h"
/* Project includes */

const rgb_color_t *p_colors_library[] = {&color_red, &color_green, &color_blue, &color_yellow, &color_turquoise, &color_white};

/**
 * @brief 	Get the expected key from a given color.
 * 
 * @param color 
 * @return char 
 */
static char _get_key_from_color(rgb_color_t color)
{
    if (color.r == color_red.r && color.g == color_red.g && color.b == color_red.b)
    {
        return KEY_RED;
    }
    else if (color.r == color_green.r && color.g == color_green.g && color.b == color_green.b)
    {
        return KEY_GREEN;
    }
    else if (color.r == color_blue.r && color.g == color_blue.g && color.b == color_blue.b)
    {
        return KEY_BLUE;
    }
    else if (color.r == color_yellow.r && color.g == color_yellow.g && color.b == color_yellow.b)
    {
        return KEY_YELLOW;
    }
    else if (color.r == color_turquoise.r && color.g == color_turquoise.g && color.b == color_turquoise.b)
    {
        return KEY_TURQUOISE;
    }
    else if (color.r == color_white.r && color.g == color_white.g && color.b == color_white.b)
    {
        return KEY_WHITE;
    }
    else
    {
        return KEY_INVALID_COLOR;
    }
}

/**
 * @brief 	Get the color corresponding to a given key.
 * 
 * @param key 
 * @return rgb_color_t 
 */
static rgb_color_t _get_color_from_key(char key)
{
    switch (key)
    {
    case KEY_RED:
        return color_red;
    case KEY_GREEN:
        return color_green;
    case KEY_BLUE:
        return color_blue;
    case KEY_YELLOW:
        return color_yellow;
    case KEY_TURQUOISE:
        return color_turquoise;
    case KEY_WHITE:
        return color_white;
    default:
        return color_off;
    }
}

/**
 * @brief 	Check if the button has been pressed for the required time to turn ON the Simone system.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_on(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    return (fsm_button_check_activity(p_fsm_simone->p_fsm_button) && p_fsm_simone->on_off_press_time_ms >= fsm_button_get_duration(p_fsm_simone->p_fsm_button)) ? true : false;
}

/**
 * @brief 	Add a new color and intensity to the sequence.
 * 
 * @param p_fsm_simone 
 */
static void _add_color(fsm_simone_t *p_fsm_simone)
{
    /* TODO students */
}

/* State machine input or transition functions */

/* State machine output or action functions */

static void fsm_simone_init(fsm_simone_t *p_fsm_simone, fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_keyboard_t *p_fsm_keyboard, fsm_rgb_light_t *p_fsm_rgb_light, uint8_t level)
{
    /* TODO students */
}

fsm_simone_t *fsm_simone_new(fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_keyboard_t *p_fsm_keyboard, fsm_rgb_light_t *p_fsm_rgb_light, uint8_t level)
{
    /* TODO students */
}
