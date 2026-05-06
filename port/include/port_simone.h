/**
 * @file port_simone.h
 * @brief Header for the portable functions os the Simone board game.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 21/04/2026
 */
#ifndef PORT_SIMONE_H_
#define PORT_SIMONE_H_
/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Function prototypes and explanation -------------------------------------------------*/

void port_simone_init(void);

void port_simone_set_timer_timeout(uint32_t duration_ms);

void port_simone_stop_timer(void);

bool port_simone_get_timeout_status(void);

void port_simone_set_timeout_status(bool status);


#endif /* PORT_SIMONE_H_ */
