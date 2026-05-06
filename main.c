/**
 * @file main.c
 * @brief Main file.
 * @author Sistemas Digitales II
 * @date 2026-01-01
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */
#include <stdio.h> // printf
#include <stdlib.h>
#include <stdint.h>
/* HW libraries */
#include "port_system.h"
#include "port_button.h"
#include "port_keyboard.h"
#include "port_rgb_light.h"
#include "fsm.h"
#include "fsm_button.h"
#include "fsm_keyboard.h"
#include "fsm_rgb_light.h"
#include "fsm_simone.h"
/* Defines ------------------------------------------------------------------*/
#define SIMONE_ON_OFF_PRESS_TIME_MS 1500 // Time the button must be pressed to turn ON/OFF the Simone system
#define BUTTON_DEBOUNCE_MS 50
#define KEYBOARD_DEBOUNCE_MS 150
#define MY_BUTTON_ID 0
#define MY_KEYBOARD_ID 0
#define MY_RGB_ID 0

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* Init board */
    port_system_init();
    /* Create FSMs */
    fsm_button_t *p_fsm_button = fsm_button_new(BUTTON_DEBOUNCE_MS, MY_BUTTON_ID);
    fsm_keyboard_t *p_fsm_keyboard = fsm_keyboard_new(KEYBOARD_DEBOUNCE_MS, MY_KEYBOARD_ID);
    fsm_rgb_light_t *p_fsm_rgb_light = fsm_rgb_light_new(MY_RGB_ID);
    fsm_simone_t *p_fsm_simone = fsm_simone_new(p_fsm_button, SIMONE_ON_OFF_PRESS_TIME_MS, p_fsm_keyboard, p_fsm_rgb_light, LEVEL_EASY);

    /* Infinite loop */
    while (1)
    {
        fsm_button_fire(p_fsm_button);       // Fire the button FSM to check for button events and update its state
        fsm_keyboard_fire(p_fsm_keyboard);   // Fire the keyboard FSM to check for keyboard events and update its state
        fsm_rgb_light_fire(p_fsm_rgb_light); // Fire the RGB light FSM to check for RGB light events and update its state
        fsm_simone_fire(p_fsm_simone);       // Fire the Simone FSM to check for Simone game events and update its state
    } // End of while(1)

    fsm_button_destroy(p_fsm_button);       // Destroy the button FSM and free the memory
    fsm_keyboard_destroy(p_fsm_keyboard);   // Destroy the keyboard FSM and free the memory
    fsm_rgb_light_destroy(p_fsm_rgb_light); // Destroy the RGB light FSM and free the memory
    fsm_simone_destroy(p_fsm_simone);       // Destroy the Simone FSM and free the memory

    return 0;
}
