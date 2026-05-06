/**
 * @file stm32f4_button.c
 * @brief Portable functions to interact with the button FSM library. All portable functions must be implemented in this file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 17/02/2026
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdio.h>
/* HW dependent includes */
#include "port_button.h"
#include "port_system.h"
#include "stm32f4_system.h"
#include "stm32f4_button.h"

/* Microcontroller dependent includes */

/* Global variables ------------------------------------------------------------*/

stm32f4_button_hw_t buttons_arr[] = {
    [PORT_USER_BUTTON_ID] = {.p_port = STM32F4_USER_BUTTON_GPIO, .pin = STM32F4_USER_BUTTON_PIN, .pupd_mode = STM32F4_GPIO_PUPDR_NOPULL},
};

/* Private functions ----------------------------------------------------------*/

/**
 * @brief Get the button status struct with the given ID.
 *
 * @param button_id Button ID.
 *
 * @return Pointer to the button state struct.
 * @return NULL If the button ID is not valid.
 */
stm32f4_button_hw_t *_stm32f4_button_get(uint8_t button_id)
{
    if (button_id < sizeof(buttons_arr) / sizeof(buttons_arr[0])) // Returnea el puntero al botón con el ID dado. Si el ID no es válido, retorna NULL.
    {
        return &buttons_arr[button_id];
    }
    return NULL;
}

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given button.
 *
 * @param button_id
 * @return * void
 */
void port_button_init(uint8_t button_id)
{
    // Obtiene la estructura del botón utilizando la función privada y el ID del botón
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);

    // Configura el modo y el pull de la GPIO, que en este caso son MODO ENTRADA y No push No pull.
    stm32f4_system_gpio_config(p_button->p_port, p_button->pin, STM32F4_GPIO_MODE_IN, STM32F4_GPIO_PUPDR_NOPULL);

    // Configura la interr externa o evento de la GPIO, que se encuentra en IM13.
    stm32f4_system_gpio_config_exti(p_button->p_port, p_button->pin, (STM32F4_TRIGGER_ENABLE_INTERR_REQ | STM32F4_TRIGGER_BOTH_EDGE));

    EXTI->PR = (1U << p_button->pin);

    // Habilita las interrupciones de una línea GPIO (pin).
    stm32f4_system_gpio_exti_enable(p_button->pin, 1, 0);
}

/**
 * @brief Return the status of the button (pressed or not).
 *
 * @param button_id
 * @return true
 * @return false
 */
bool port_button_get_pressed(uint8_t button_id)
{
    // hago un retrive de una estructura button para poder acceder mediante el get a su flag_pressed
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    return p_button->flag_pressed;
}
