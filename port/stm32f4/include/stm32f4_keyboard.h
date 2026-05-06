/**
 * @file stm32f4_keyboard.h
 * @brief Header for stm32f4_keyboard.c file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 03/03/2026
 */
#ifndef STM32F4_KEYBOARD_H_
#define STM32F4_KEYBOARD_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */

#include <stdint.h>
#include <stdbool.h>

/* HW dependent includes */

#include "stm32f4xx.h"
#include "keyboards.h"

/* Defines and enums ----------------------------------------------------------*/
/* Defines */

#define STM32F4_KEYBOARD_MAIN_ROW_0_GPIO GPIOA

#define STM32F4_KEYBOARD_MAIN_ROW_0_PIN 0

#define STM32F4_KEYBOARD_MAIN_ROW_1_GPIO GPIOA

#define STM32F4_KEYBOARD_MAIN_ROW_1_PIN 1

#define STM32F4_KEYBOARD_MAIN_ROW_2_GPIO GPIOA

#define STM32F4_KEYBOARD_MAIN_ROW_2_PIN 4

#define STM32F4_KEYBOARD_MAIN_ROW_3_GPIO GPIOB

#define STM32F4_KEYBOARD_MAIN_ROW_3_PIN 0

#define STM32F4_KEYBOARD_MAIN_COL_0_GPIO GPIOA

#define STM32F4_KEYBOARD_MAIN_COL_0_PIN 8

#define STM32F4_KEYBOARD_MAIN_COL_1_GPIO GPIOB

#define STM32F4_KEYBOARD_MAIN_COL_1_PIN 10

#define STM32F4_KEYBOARD_MAIN_COL_2_GPIO GPIOB

#define STM32F4_KEYBOARD_MAIN_COL_2_PIN 4

#define STM32F4_KEYBOARD_MAIN_COL_3_GPIO GPIOB

#define STM32F4_KEYBOARD_MAIN_COL_3_PIN 5

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief STM32F4 specific keyboard hardware configuration and state structure.
 */
typedef struct
{
    GPIO_TypeDef **p_row_ports;
    uint8_t *p_row_pins;
    GPIO_TypeDef **p_col_ports;
    uint8_t *p_col_pins;
    const keyboard_t *p_keyboard;
    uint8_t current_excited_row;
    uint8_t col_idx_interrupt;
    bool flag_key_pressed;
    bool flag_row_timeout;
} stm32f4_keyboard_hw_t;

/**
 * @brief Array of elements that represents the HW characteristics of the keyboards.
 *
 */
extern stm32f4_keyboard_hw_t keyboards_arr[];

#endif /* STM32F4_KEYBOARD_H_ */
