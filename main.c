/**
 * @file main.c
 * @brief Main file.
 * @author Sistemas Digitales II
 * @date 2026-01-01
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */
#include <stdio.h> // printf

/* HW libraries */
#include "port_system.h"

/* Defines ------------------------------------------------------------------*/
#define SIMONE_ON_OFF_PRESS_TIME_MS 2000 // Time the button must be pressed to turn ON/OFF the Simone system
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* Init board */
    port_system_init();

    /* Infinite loop */
    while (1)
    {

    } // End of while(1)

    return 0;
}
