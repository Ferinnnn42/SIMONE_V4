/**
 * @file stm32f4_simone.h
 * @brief Header for stm32f4_simone.c file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 21/04/2026
 */
#ifndef STM32F4_SIMONE_H_
#define STM32F4_SIMONE_H_
/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>

/* HW dependent includes */
#include "stm32f4xx.h"

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of a Simone's timer status.
 */
typedef struct
{
    bool flag_timer_timeout; /*!< Flag to indicate that the timer passed */
} stm32f4_simone_hw_t;

/* Global variables */
/**
 * @brief Element that represents the HW characteristics of the Simone game connected to the STM32F4 platform to manage the timers.
 *
 * This is an **extern** variable that is defined in `stm32f4_simone.c`. It represents the hardware of Simone.
 */
extern stm32f4_simone_hw_t simone_hw;

#endif /* STM32F4_SIMONE_H_ */