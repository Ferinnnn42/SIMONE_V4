/**
 * @file stm32f4_simone.c
 * @brief Portable functions to interact with the Simone FSM library. All portable functions must be implemented in this file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 21/04/2026
 */

/* Standard C includes */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h> // Used to compute the timer ARR and PSC values

/* HW dependent includes */
#include "port_simone.h"
#include "port_system.h"
#include "stm32f4_system.h"
#include "stm32f4_simone.h"

/* Microcontroller dependent includes */

/* Global variables */
stm32f4_simone_hw_t simone_hw;

/* Private functions ----------------------------------------------------------*/

/**
 * @brief Configure the timer that controls the duration of the note.
 *
 */
static void _timer_simone_setup()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;                                                 // Enabling the clock for the timer used
    TIM3->CR1 &= ~TIM_CR1_CEN;                                                          // Disable the counter to configure it
    TIM3->CR1 |= TIM_CR1_ARPE;                                                          // Enable the autoreload preaload
    TIM3->SR &= ~TIM_SR_UIF;                                                            // Clear the update interrupt flag
    TIM3->DIER |= TIM_DIER_UIE;                                                         // Enable the update interrupt
    NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0)); // Set the priority of the timer interrupt
}

void port_simone_set_timer_timeout(uint32_t duration_ms)
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->CNT = 0;

    double f_clk = SystemCoreClock;
    double Max_ARR = 65535.0;
    double PSC = (f_clk * duration_ms / 1000.0) / Max_ARR - 1.0;
    double ARR = (f_clk * duration_ms / 1000.0) / (round(PSC) + 1.0) - 1.0;
    double ARR_rounded = round(ARR);
    if (ARR_rounded > Max_ARR)
    {
        PSC += 1.0;
        ARR_rounded = round((f_clk * duration_ms / 1000.0) / (round(PSC) + 1.0) - 1.0);
    }

    TIM3->PSC = (uint32_t)round(PSC);
    TIM3->ARR = (uint32_t)ARR_rounded;
    TIM3->EGR  |= TIM_EGR_UG;
    TIM3->SR   &= ~TIM_SR_UIF;
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);
    simone_hw.flag_timer_timeout = false; 
    TIM3->CR1  |= TIM_CR1_CEN;
}
void port_simone_stop_timer(void)
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->DIER &= ~TIM_DIER_UIE;
}

bool port_simone_get_timeout_status(void)
{
    return simone_hw.flag_timer_timeout;
}

void port_simone_set_timeout_status(bool status)
{
    simone_hw.flag_timer_timeout = status;
}

void port_simone_init(void)
{
    // 1. Reset the flag of the timer timeout
    // 2. Configure the timer that controls the duration of the different aspects of the game
    simone_hw.flag_timer_timeout = false;
    _timer_simone_setup();
}