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
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; // Habilitar el reloj para el timer utilizado
    TIM5->CR1 &= ~TIM_CR1_CEN;          // Asegurarse de que el timer esté detenido antes de configurarlo
    TIM5->CR1 &= ~TIM_CR1_DIR;          // Configurar el timer para contar hacia arriba
    TIM5->CR1 |= TIM_CR1_ARPE;          // Habilitar la actualización de ARR en el próximo ciclo de actualización
    TIM5->CNT = 0U;                     // Empiece a contar desde 0
    double f_clk = SystemCoreClock;
    double Max_ARR = 65535.0;                                                             // Valor máximo de ARR para un timer de 16 bits
    double PSC = (f_clk * PORT_KEYBOARDS_TIMEOUT_MS / 1000.0) / Max_ARR - 1.0;            // Cálculo del valor de PSC para lograr el tiempo de timeout deseado con el valor máximo de ARR
    double ARR = (f_clk * PORT_KEYBOARDS_TIMEOUT_MS / 1000.0) / (round(PSC) + 1.0) - 1.0; // Cálculo del valor de ARR con el PSC redondeado
    double ARR_rounded = round(ARR);                                                      // Redondear el valor de ARR al entero más cercano
    if (ARR_rounded > Max_ARR)
    { // Si el valor de ARR redondeado excede el máximo permitido, ajustarlo al máximo
        PSC += 1.0;
        ARR_rounded = round((f_clk * PORT_KEYBOARDS_TIMEOUT_MS / 1000.0) / (round(PSC) + 1.0) - 1.0); // Esto es para asegurar que el tiempo de timeout se mantenga lo más cercano posible al deseado (no me paso de Max_ARR), aunque el PSC haya sido incrementado
    }
    TIM5->PSC = (uint32_t)round(PSC);                                                   // Configurar el valor de PSC en el timer
    TIM5->ARR = (uint32_t)ARR_rounded;                                                  // Configurar el valor de ARR en el timer
    TIM5->EGR |= TIM_EGR_UG;                                                            // Generar un evento de actualización para cargar los nuevos valores de PSC y ARR
    TIM5->SR &= ~TIM_SR_UIF;                                                            // Limpiar la bandera de actualización (Update Interrupt Flag) para evitar interrupciones no deseadas al iniciar el timer
    TIM5->DIER |= TIM_DIER_UIE;                                                         // Habilitar la interrupción por actualización (Update Interrupt Enable) para que se genere una interrupción cada vez que el timer alcance el valor de ARR
    NVIC_SetPriority(TIM5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0)); // Configurar la prioridad de la interrupción del timer (puede ajustarse según las necesidades del sistema)
}
/* Public functions -----------------------------------------------------------*/

void port_keyboard_excite_row(uint8_t keyboard_id, uint8_t row_idx)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida

    for (uint8_t i = 0; i < teclado->p_keyboard->num_cols; i++) // Itero por todas las filas del teclado, pongo en HIGH la fila con índice row_idx y en LOW el resto de filas
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
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida

    teclado->current_excited_row = (teclado->current_excited_row + 1) % (teclado->p_keyboard->num_rows); // Actualizo el índice de la fila actual al siguiente, volviendo al inicio gracias a utilizar % (operador módulo)

    port_keyboard_excite_row(keyboard_id, teclado->current_excited_row); // Excito la siguiente fila utilizando la función anterior
}

bool port_keyboard_get_key_pressed_status(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida

    return teclado->flag_key_pressed; // De dicho teclado, devuelvo su valor de flag_key_pressed
}

void port_keyboard_set_key_pressed_status(uint8_t keyboard_id, bool status)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida
    teclado->flag_key_pressed = status;                                  // Actualizo el valor de flag_key_pressed del teclado con id keyboard_id al valor de status
}

bool port_keyboard_get_row_timeout_status(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida

    return teclado->flag_row_timeout; // Devuelvo el valor de flag_row_timeout del teclado con id keyboard_id
}

void port_keyboard_set_row_timeout_status(uint8_t keyboard_id, bool status)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida
    teclado->flag_row_timeout = status;                                  // Actualizo el valor de flag_row_timeout del teclado con id keyboard_id al valor de status
}

char port_keyboard_get_key_value(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id);                            // Recupero el teclado con id keyboard_id que sigue la estructura definida
    uint8_t excited_row = teclado->current_excited_row;                                             // Recupero la fila actualmente excitada del teclado con id keyboard_id
    uint8_t key_index = (excited_row * teclado->p_keyboard->num_cols) + teclado->col_idx_interrupt; // Calculo el índice del char correspondiente a la tecla presionada en función de la fila excitada y la columna detectada en la interrupción
    return teclado->p_keyboard->keys[key_index];                                                    // keys es la matriz de valores ascii del teclado
}

char port_keyboard_get_invalid_key_value(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida
    return teclado->p_keyboard->null_key;                                // Devuelvo el valor de null_key del teclado con id keyboard_id, que representa un valor de tecla inválida o nula
}

void port_keyboard_start_scan(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id); // Recupero el teclado con id keyboard_id que sigue la estructura definida
    teclado->flag_row_timeout = false;                                   // Limpiar la bandera de timeout antes de iniciar el escaneo
    TIM5->CNT = 0;                                                       // Reiniciar el contador del timer para empezar a contar desde 0
    port_keyboard_excite_row(keyboard_id, 0);                            // Excitar la primera fila para iniciar el proceso de escaneo
    NVIC_EnableIRQ(TIM5_IRQn);                                           // Habilitar la interrupción del timer para que se generen interrupciones de timeout durante el escaneo
    TIM5->CR1 |= TIM_CR1_CEN;                                            // Iniciar el timer para que comience a contar y generar interrupciones de timeout
}

void port_keyboard_stop_scan(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *teclado = _stm32f4_keyboard_get(keyboard_id);
    TIM5->CR1 &= ~TIM_CR1_CEN;  // Detener el timer para que deje de contar y generar interrupciones de timeout
    NVIC_DisableIRQ(TIM5_IRQn); // Deshabilitar la interrupción del timer para evitar interrupciones no deseadas después de detener el escaneo
    for (uint8_t i = 0; i < teclado->p_keyboard->num_rows; i++)
    {
        stm32f4_system_gpio_write(teclado->p_row_ports[i], teclado->p_row_pins[i], false); // Asegurarse de que todas las filas estén desactivadas al detener el escaneo
    }
}

void port_keyboard_init(uint8_t keyboard_id)
{
    // 1. Recupero el teclado con id keyboard_id que sigue la estructura definida
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    // 2. Limpiar/Setear campos de estado a valores iniciales
    p_keyboard->current_excited_row = (uint8_t)-1; // Indica que ninguna fila está activa al iniciar, ya que el proceso de escaneo no ha comenzado aún
    p_keyboard->col_idx_interrupt = (uint8_t)-1;   // Ninguna interrupción de columna detectada
    p_keyboard->flag_key_pressed = false;
    p_keyboard->flag_row_timeout = false;

    // 3. Configurar FILAS como salidas (No Pull y en nivel bajo inicialmente)
    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_rows; i++)
    {
        stm32f4_system_gpio_config(
            p_keyboard->p_row_ports[i],
            p_keyboard->p_row_pins[i],
            STM32F4_GPIO_MODE_OUT,
            STM32F4_GPIO_PUPDR_NOPULL);
    }

    // 4. Configurar COLUMNAS como entradas (Pull Down))
    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_cols; i++)
    {
        // Configuración de modo e interrupción EXTI para cada pin de columna
        stm32f4_system_gpio_config(
            p_keyboard->p_col_ports[i],
            p_keyboard->p_col_pins[i],
            STM32F4_GPIO_MODE_IN,
            STM32F4_GPIO_PUPDR_PULLDOWN);

        stm32f4_system_gpio_config_exti(
            p_keyboard->p_col_ports[i],
            p_keyboard->p_col_pins[i],
            (STM32F4_TRIGGER_ENABLE_INTERR_REQ | STM32F4_TRIGGER_BOTH_EDGE));

        // Habilitar interrupción en el NVIC para el pin correspondiente
        stm32f4_system_gpio_exti_enable(p_keyboard->p_col_pins[i], 1, 1);
    }

    // 5. Configurar el timer de escaneo
    _timer_scan_column_config();
}
