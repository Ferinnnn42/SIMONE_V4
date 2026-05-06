/**
 * @file stm32f4_rgb_light.h
 * @brief Header for stm32f4_rgb_light.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef STM32F4_RGB_LIGHT_SYSTEM_H_
#define STM32F4_RGB_LIGHT_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
/* HW dependent includes */
#include "stm32f4xx.h"
/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define STM32F4_RGB_LIGHT_R_GPIO GPIOB

#define STM32F4_RGB_LIGHT_R_PIN 6

#define STM32F4_RGB_LIGHT_G_GPIO GPIOB

#define STM32F4_RGB_LIGHT_G_PIN 8

#define STM32F4_RGB_LIGHT_B_GPIO GPIOB

#define STM32F4_RGB_LIGHT_B_PIN 9

/**
 * @brief Structure to define the HW dependencies of an RGB LED.
 * 
 */

typedef struct{
    GPIO_TypeDef *p_port_red;
    uint8_t pin_red;
    GPIO_TypeDef *p_port_green;
    uint8_t pin_green;
    GPIO_TypeDef *p_port_blue;
    uint8_t pin_blue;
} stm32f4_rgb_light_hw_t;

extern stm32f4_rgb_light_hw_t 	rgb_lights_arr [];
#endif /* STM32F4_RGB_LIGHT_SYSTEM_H_ */