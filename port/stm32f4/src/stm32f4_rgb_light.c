/**
 * @file stm32f4_rgb_light.c
 * @brief Portable functions to interact with the RGB light system FSM library. All portable functions must be implemented in this file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 21/04/2026
 */

/* Standard C includes */
#include <stdio.h>
/* HW dependent includes */
#include "port_rgb_light.h"
#include "port_system.h"
#include "stm32f4_system.h"
#include "stm32f4_rgb_light.h"
#include <math.h>
/* Microcontroller dependent includes */

/* Defines --------------------------------------------------------------------*/
#define PWM_FREQUENCY 50 // PWM frequency in Hz
#define f_clk 16000000.0 // Timer clock frequency in Hz
/* Typedefs --------------------------------------------------------------------*/

/* Global variables */

/**
 * @brief Array of elements that represents the HW characteristics of the RGB LED of the RGB light systems connected to the STM32F4 platform.
 *
 */
stm32f4_rgb_light_hw_t rgb_lights_arr[] = {
    {.p_port_red = STM32F4_RGB_LIGHT_R_GPIO,
     .pin_red = STM32F4_RGB_LIGHT_R_PIN,
     .p_port_green = STM32F4_RGB_LIGHT_G_GPIO,
     .pin_green = STM32F4_RGB_LIGHT_G_PIN,
     .p_port_blue = STM32F4_RGB_LIGHT_B_GPIO,
     .pin_blue = STM32F4_RGB_LIGHT_B_PIN}};
/* Private functions -----------------------------------------------------------*/

/**
 * @brief Get the RGB light struct with the given ID.
 *
 * @param rgb_light_id
 * @return stm32f4_rgb_light_hw_t*
 */
stm32f4_rgb_light_hw_t *_stm32f4_rgb_light_get(uint8_t rgb_light_id)
{
    if (rgb_light_id < sizeof(rgb_lights_arr) / sizeof(rgb_lights_arr[0]))
    {
        return &rgb_lights_arr[rgb_light_id];
    }

    return NULL;
}

/**
 * @brief Configure the timer that controls the PWM of each one of the RGB LEDs of the RGB light system.
 *
 * @param rgb_light_id
 */
void _timer_pwm_config(uint8_t rgb_light_id)
{
    double Max_ARR = 65535.0;                                                              // maximum ARR value for a 16-bit timer
    double PSC = (f_clk * PORT_RGB_LIGHT_PWM_TIMEOUT / 1000.0) / Max_ARR - 1.0;            // Calculation of the PSC value to achieve the desired timeout time with the maximum ARR value
    double ARR = (f_clk * PORT_RGB_LIGHT_PWM_TIMEOUT / 1000.0) / (round(PSC) + 1.0) - 1.0; // Calculation of the ARR value with the rounded PSC
    double ARR_rounded = round(ARR);                                                       // Round the ARR value to the nearest integer
    if (ARR_rounded > Max_ARR)
    { // If the rounded ARR value exceeds the maximum allowed, adjust it to the maximum
        PSC += 1.0;
        ARR_rounded = round((f_clk * PORT_RGB_LIGHT_PWM_TIMEOUT / 1000.0) / (round(PSC) + 1.0) - 1.0); // This is to ensure that the PWM timeout time remains as close as possible to the desired one (not exceeding Max_ARR), even if the PSC has been incremented
    }
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;                               // Configure the timer used for the PWM of the RGB LED
    TIM4->CR1 &= ~TIM_CR1_CEN;                                        // Make sure the timer is stopped before configuring it
    TIM4->CR1 |= TIM_CR1_ARPE;                                        // Enable ARR update on the next update event
    TIM4->CNT = 0U;                                                   // Start counting from 0
    TIM4->ARR = (uint32_t)ARR_rounded;                                // Configure the ARR value in the timer
    TIM4->PSC = (uint32_t)round(PSC);                                 // Configure the PSC value in the timer
    TIM4->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC3E | TIM_CCER_CC4E);   // Disable the output for all channels before configuring them
    TIM4->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC3P | TIM_CCER_CC4P);   // Configure the output polarity for all channels as active high
    TIM4->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC3NP | TIM_CCER_CC4P); // Configure the output polarity for all channels as active high (for complementary outputs, not used in this case)
    /* CH1 -> CCMR1 */
    TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;             // Clear the OC1M bits before setting them
    TIM4->CCMR1 |= (0x6 << TIM_CCMR1_OC1M_Pos); // In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive. In downcounting, channel 1 is inactive (OC1REF=‘0) as long as TIMx_CNT>TIMx_CCR1 else active (OC1REF=1).
    TIM4->CCMR1 |= TIM_CCMR1_OC1PE;             // Enable the preload for channel 1

    /* CH3 & CH4 -> CCMR2 */
    TIM4->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC4M);                        // Clear the OC3M and OC4M bits before setting them
    TIM4->CCMR2 |= (0x6 << TIM_CCMR2_OC3M_Pos) | (0x6 << TIM_CCMR2_OC4M_Pos); // Output compare 3 mode & Output compare 4 mode
    TIM4->CCMR2 |= (TIM_CCMR2_OC3PE | TIM_CCMR2_OC4PE);                       // Enable the preload for channels 3 and 4

    TIM4->EGR |= TIM_EGR_UG; // Generate an update event to load the new PSC and ARR values
}

/* Public functions -----------------------------------------------------------*/

void port_rgb_light_init(uint8_t rgb_light_id)
{
    _stm32f4_rgb_light_get(rgb_light_id);
    stm32f4_system_gpio_config(rgb_lights_arr[rgb_light_id].p_port_red, rgb_lights_arr[rgb_light_id].pin_red, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
    stm32f4_system_gpio_config(rgb_lights_arr[rgb_light_id].p_port_green, rgb_lights_arr[rgb_light_id].pin_green, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
    stm32f4_system_gpio_config(rgb_lights_arr[rgb_light_id].p_port_blue, rgb_lights_arr[rgb_light_id].pin_blue, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
    stm32f4_system_gpio_config_alternate(rgb_lights_arr[rgb_light_id].p_port_red, rgb_lights_arr[rgb_light_id].pin_red, STM32F4_GPIO_MODE_AF);
    stm32f4_system_gpio_config_alternate(rgb_lights_arr[rgb_light_id].p_port_green, rgb_lights_arr[rgb_light_id].pin_green, STM32F4_GPIO_MODE_AF);
    stm32f4_system_gpio_config_alternate(rgb_lights_arr[rgb_light_id].p_port_blue, rgb_lights_arr[rgb_light_id].pin_blue, STM32F4_GPIO_MODE_AF);
    _timer_pwm_config(rgb_light_id);
    port_rgb_light_set_rgb(rgb_light_id, color_off);
}

void port_rgb_light_set_rgb(uint8_t rgb_light_id, rgb_color_t color)
{
    /* Write the code in a conditional statement to check the rgb_light_id. */
    if (rgb_light_id == PORT_RGB_LIGHT_ID)
    {
        /*1. Retrieve individual RGB levels (r, g, b) */
        uint8_t red = color.r;
        uint8_t green = color.g;
        uint8_t blue = color.b;

        /*2. Disable the timer */
        TIM4->CR1 &= ~TIM_CR1_CEN;

        /*3. Check if r=g=b=0 */
        if (red == 0 && green == 0 && blue == 0)
        {
            /* Disable the capture/compare register for all the channels */
            TIM4->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC3E | TIM_CCER_CC4E);
        }
        else
        {
            /*For the channel of the red LED (CH1): */
            if (red == 0)
            {
                TIM4->CCER &= ~TIM_CCER_CC1E; /* Disable CH1 */
            }
            else
            {
                /* Set the duty cycle in compare register */
                TIM4->CCR1 = (uint32_t)(red * (TIM4->ARR + 1)) / COLOR_RGB_MAX_VALUE;
                TIM4->CCER |= TIM_CCER_CC1E; /* Enable CH1 */
            }

            /*For the channel of the green LED (CH3): */
            if (green == 0)
            {
                TIM4->CCER &= ~TIM_CCER_CC3E; /* Disable CH3 */
            }
            else
            {
                /* Set the duty cycle in compare register */
                TIM4->CCR3 = (uint32_t)(green * (TIM4->ARR + 1)) / COLOR_RGB_MAX_VALUE;
                TIM4->CCER |= TIM_CCER_CC3E; /* Enable CH3 */
            }

            /*For the channel of the blue LED (CH4): */
            if (blue == 0)
            {
                TIM4->CCER &= ~TIM_CCER_CC4E; /* Disable CH4 */
            }
            else
            {
                /* Set the duty cycle in compare register */
                TIM4->CCR4 = (uint32_t)(blue * (TIM4->ARR + 1)) / COLOR_RGB_MAX_VALUE;
                TIM4->CCER |= TIM_CCER_CC4E; /* Enable CH4 */

                /*Set the UG bit in the EGR register */
                TIM4->EGR |= TIM_EGR_UG;

                /*Set the UG bit in the EGR register */
                TIM4->CR1 |= TIM_CR1_CEN;
            }
        }
    }
}