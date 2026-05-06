/**
 * @file stm32f4_keyboard.c
 * @brief Portable functions to interact with the keyboard FSM library. All portable functions must be implemented in this file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 03/03/2026
 */

/* Standard C includes */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/* HW dependent includes */

#include "port_keyboard.h"
#include "port_system.h"
#include "keyboards.h"

/* Microcontroller dependent includes */

#include "stm32f4_system.h"
#include "stm32f4_keyboard.h"

/* Typedefs --------------------------------------------------------------------*/

/* Global variables */

/* Static arrays for main keyboard (pointed by the double pointers in the struct) */

/**
 * @brief Array of GPIO ports for the rows of the main keyboard.
 *
 */
static GPIO_TypeDef *keyboard_main_row_ports[] = {
    STM32F4_KEYBOARD_MAIN_ROW_0_GPIO,
    STM32F4_KEYBOARD_MAIN_ROW_1_GPIO,
    STM32F4_KEYBOARD_MAIN_ROW_2_GPIO,
    STM32F4_KEYBOARD_MAIN_ROW_3_GPIO};

/**
 * @brief Array of GPIO pins for the rows of the main keyboard.
 *
 */
static uint8_t keyboard_main_row_pins[] = {
    STM32F4_KEYBOARD_MAIN_ROW_0_PIN,
    STM32F4_KEYBOARD_MAIN_ROW_1_PIN,
    STM32F4_KEYBOARD_MAIN_ROW_2_PIN,
    STM32F4_KEYBOARD_MAIN_ROW_3_PIN};

/**
 * @brief Array of GPIO ports for the columns of the main keyboard.
 *
 */
static GPIO_TypeDef *keyboard_main_col_ports[] = {
    STM32F4_KEYBOARD_MAIN_COL_0_GPIO,
    STM32F4_KEYBOARD_MAIN_COL_1_GPIO,
    STM32F4_KEYBOARD_MAIN_COL_2_GPIO,
    STM32F4_KEYBOARD_MAIN_COL_3_GPIO};

/**
 * @brief Array of GPIO pins for the columns of the main keyboard.
 *
 */
static uint8_t keyboard_main_col_pins[] = {
    STM32F4_KEYBOARD_MAIN_COL_0_PIN,
    STM32F4_KEYBOARD_MAIN_COL_1_PIN,
    STM32F4_KEYBOARD_MAIN_COL_2_PIN,
    STM32F4_KEYBOARD_MAIN_COL_3_PIN};

stm32f4_keyboard_hw_t keyboards_arr[] = {
    {
        .p_row_ports = keyboard_main_row_ports,
        .p_row_pins = keyboard_main_row_pins,
        .p_col_ports = keyboard_main_col_ports,
        .p_col_pins = keyboard_main_col_pins,
        .p_keyboard = &standard_keyboard,
    }};

/* Private functions ----------------------------------------------------------*/

/**
 * @brief Get the keyboard struct with the given ID.
 *
 * @param keyboard_id
 * @return stm32f4_keyboard_hw_t*
 */

stm32f4_keyboard_hw_t *_stm32f4_keyboard_get(uint8_t keyboard_id)
{
    if (keyboard_id < sizeof(keyboards_arr) / sizeof(keyboards_arr[0]))
    {
        return &keyboards_arr[keyboard_id];
    }

    return NULL;
}

void _timer_scan_column_config(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; // Enable the clock for the timer used
    TIM5->CR1 &= ~TIM_CR1_CEN;          // Ensure the timer is stopped before configuring it
    TIM5->CR1 &= ~TIM_CR1_DIR;          // Configure the timer to count up
    TIM5->CR1 |= TIM_CR1_ARPE;          // Enable ARR update on the next update cycle
    TIM5->CNT = 0U;                     // Start counting from 0
    double f_clk = SystemCoreClock;
    double Max_ARR = 65535.0;                                                             // Maximum ARR value for a 16-bit timer
    double PSC = (f_clk * PORT_KEYBOARDS_TIMEOUT_MS / 1000.0) / Max_ARR - 1.0;            // Calculate PSC value to achieve the desired timeout with the maximum ARR value
    double ARR = (f_clk * PORT_KEYBOARDS_TIMEOUT_MS / 1000.0) / (round(PSC) + 1.0) - 1.0; // Calculate ARR value with rounded PSC
    double ARR_rounded = round(ARR);                                                      // Round ARR value to the nearest integer
    if (ARR_rounded > Max_ARR)
    { // If the rounded ARR value exceeds the maximum allowed, adjust it to the maximum
        PSC += 1.0;
        ARR_rounded = round((f_clk * PORT_KEYBOARDS_TIMEOUT_MS / 1000.0) / (round(PSC) + 1.0) - 1.0); // This is to ensure that the timeout remains as close as possible to the desired value (not exceeding Max_ARR), even if PSC has been incremented
    }
    TIM5->PSC = (uint32_t)round(PSC);                                                   // Configure the PSC value in the timer
    TIM5->ARR = (uint32_t)ARR_rounded;                                                  // Configure the value of ARR in the timer
    TIM5->EGR |= TIM_EGR_UG;                                                            // Generate an update event to load the new PSC and ARR values
    TIM5->SR &= ~TIM_SR_UIF;                                                            // Clear the update interrupt flag to avoid unwanted interrupts when starting the timer
    TIM5->DIER |= TIM_DIER_UIE;                                                         // Enable the update interrupt to generate an interrupt each time the timer reaches the ARR value
    NVIC_SetPriority(TIM5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0)); // Set the priority of the timer interrupt (can be adjusted according to system needs)
}
/* Public functions -----------------------------------------------------------*/

void port_keyboard_excite_row(uint8_t keyboard_id, uint8_t row_idx)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure

    for (uint8_t i = 0; i < teclado->p_keyboard->num_cols; i++) // Iterate through all the rows of the keyboard, set the row with index row_idx to HIGH and the rest to LOW
    {
        if (i == row_idx)
        {
            stm32f4_system_gpio_write(teclado->p_row_ports[i], teclado->p_row_pins[i], true);
        }
        else
        {
            stm32f4_system_gpio_write(teclado->p_row_ports[i], teclado->p_row_pins[i], false);
        }
    }
};

void port_keyboard_excite_next_row(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure

    teclado->current_excited_row = (teclado->current_excited_row + 1) % (teclado->p_keyboard->num_rows); // Update the index of the current row to the next one, wrapping around using % (modulo operator)

    port_keyboard_excite_row(keyboard_id, teclado->current_excited_row); // Excite the next row using the previous function
}

bool port_keyboard_get_key_pressed_status(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure

    return teclado->flag_key_pressed; // Return the value of flag_key_pressed for the given keyboard
}

void port_keyboard_set_key_pressed_status(uint8_t keyboard_id, bool status)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure
    teclado->flag_key_pressed = status;                                  // Update the value of flag_key_pressed for the given keyboard
}

bool port_keyboard_get_row_timeout_status(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure

    return teclado->flag_row_timeout; // Return the value of flag_row_timeout for the given keyboard
}

void port_keyboard_set_row_timeout_status(uint8_t keyboard_id, bool status)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure
    teclado->flag_row_timeout = status;                                  // Update the value of flag_row_timeout for the given keyboard
}

char port_keyboard_get_key_value(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id);                            // Retrieve the keyboard with the given ID following the defined structure
    uint8_t excited_row = teclado->current_excited_row;                                             // Retrieve the currently excited row of the keyboard with the given ID
    uint8_t key_index = (excited_row * teclado->p_keyboard->num_cols) + teclado->col_idx_interrupt; // Calculate the index of the char corresponding to the pressed key based on the excited row and the column detected in the interrupt
    return teclado->p_keyboard->keys[key_index];                                                    // keys is the matrix of ASCII values of the keyboard
}

char port_keyboard_get_invalid_key_value(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure
    return teclado->p_keyboard->null_key;                                // Return the value of null_key for the given keyboard, representing an invalid or null key value
}

void port_keyboard_start_scan(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Retrieve the keyboard with the given ID following the defined structure
    teclado->flag_row_timeout = false;                                   // Clear the row timeout flag before starting the scan
    TIM5->CNT = 0;                                                       // Reset the timer counter to start counting from 0
    port_keyboard_excite_row(keyboard_id, 0);                            // Excite the first row to start the scanning process
    NVIC_EnableIRQ(TIM5_IRQn);                                           // Enable the timer interrupt to generate timeout interrupts during the scan
    TIM5->CR1 |= TIM_CR1_CEN;                                            // Start the timer to begin counting and generating timeout interrupts
}

void port_keyboard_stop_scan(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id);
    TIM5->CR1 &= ~TIM_CR1_CEN;  // Stop the timer to stop counting and generating timeout interrupts
    NVIC_DisableIRQ(TIM5_IRQn); // Disable the timer interrupt to avoid unwanted interrupts after stopping the scan
    for (uint8_t i = 0; i < teclado->p_keyboard->num_rows; i++)
    {
        stm32f4_system_gpio_write(teclado->p_row_ports[i], teclado->p_row_pins[i], false); // Ensure all rows are deactivated when stopping the scan
    }
}

void port_keyboard_init(uint8_t keyboard_id)
{
    // 1. Retrieve the keyboard with the given ID following the defined structure
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    // 2. Clear/Set state fields to initial values
    p_keyboard->current_excited_row = (uint8_t)-1; // Indicates that no row is active at startup, as the scanning process has not yet begun
    p_keyboard->col_idx_interrupt = (uint8_t)-1;   // No column interrupt detected
    p_keyboard->flag_key_pressed = false;
    p_keyboard->flag_row_timeout = false;

    // 3. Configure ROWS as outputs (No Pull and initially low)
    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_rows; i++)
    {
        stm32f4_system_gpio_config(
            p_keyboard->p_row_ports[i],
            p_keyboard->p_row_pins[i],
            STM32F4_GPIO_MODE_OUT,
            STM32F4_GPIO_PUPDR_NOPULL);
    }

    // 4. Configure COLUMNS as inputs (Pull Down)
    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_cols; i++)
    {
        // Configure mode and EXTI interrupt for each column pin
        stm32f4_system_gpio_config(
            p_keyboard->p_col_ports[i],
            p_keyboard->p_col_pins[i],
            STM32F4_GPIO_MODE_IN,
            STM32F4_GPIO_PUPDR_PULLDOWN);

        stm32f4_system_gpio_config_exti(
            p_keyboard->p_col_ports[i],
            p_keyboard->p_col_pins[i],
            (STM32F4_TRIGGER_ENABLE_INTERR_REQ | STM32F4_TRIGGER_BOTH_EDGE));

        // Enable interrupt in the NVIC for the corresponding pin
        stm32f4_system_gpio_exti_enable(p_keyboard->p_col_pins[i], 1, 1);
    }

    // 5. Configure the scan timer
    _timer_scan_column_config();
}
