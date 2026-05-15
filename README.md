# SIMONE

Acceso a la documentación y API del proyecto en [este enlace](https://sdg2dieupm.github.io/simone/).

## Authors

* Fernando Ingelmo Ajejas - email: fernando.ingelmo@alumnos.upm.es
* Paula Rodríguez Hernández - email: paula.rodriguez.hernandez@alumnos.upm.es

**Descripción del proyecto / Project Description:**

Este proyecto implementa una versión del clásico juego de memoria "Simone" utilizando un microcontrolador STM32F446RE. El sistema cuenta con un botón de control, un teclado matricial 4x4 para las entradas del jugador, y un LED RGB controlado por PWM para mostrar secuencias de colores. El juego genera secuencias aleatorias que el usuario debe repetir, aumentando la dificultad progresivamente e incorporando modos de bajo consumo (Sleep Mode) para optimizar la eficiencia energética.

*This project implements a version of the classic memory game "Simone" using an STM32F446RE microcontroller. The system features a control button, a 4x4 matrix keypad for player inputs, and a PWM-controlled RGB LED to display color sequences. The game generates random sequences that the user must repeat, progressively increasing the difficulty and incorporating low-power modes (Sleep Mode) to optimize energy efficiency.*

![Montaje final del proyecto Simone](docs/assets/imgs/portada.png)

[![Demostración de Simone V5](docs/assets/imgs/portada.png)](https://youtu.be/TU_ENLACE_AQUI "Demostración de Simone en YouTube.")

---

## Version 1

En esta primera versión se ha desarrollado la librería y la máquina de estados (FSM) para el control del **botón de usuario**. 

* Implementación del módulo HW (PORT) usando la interrupción externa `EXTI13`.
* Creación de una FSM (COMMON) independiente del hardware para gestionar las pulsaciones.
* Integración de un **filtro anti-rebotes por software** mediante tiempos de guarda.
* Funcionalidad para medir la duración de la pulsación, necesaria para el encendido/apagado del sistema.

Enlace a la [FSM de Version 1](fsm__button_8c.html).

## Version 2

Esta versión incorpora el soporte para la lectura de un **teclado matricial 4x4**.

* Configuración de pines de entrada (columnas con *pull-down*) y salida (filas).
* Uso del temporizador `TIM5` para realizar una excitación periódica y secuencial de las filas.
* Gestión de interrupciones combinadas en las columnas para detectar qué botón exacto se ha pulsado.
* Máquina de estados con autotransición y control **anti-rebotes** para el guardado y validación de la tecla.

Enlace a la [FSM de Version 2](fsm__keyboard_8c.html).

## Version 3

Se integra el **RGB light** para proporcionar feedback visual al usuario y mostrar las secuencias del juego.

* Uso del temporizador `TIM4` en modo **PWM** para generar la señal de control.
* Manipulación del ciclo de trabajo (*duty cycle*) en tres canales diferentes para generar diversos colores (Rojo, Verde, Azul, Amarillo, Turquesa, Blanco) modificando su intensidad.
* Máquina de estados que evalúa si el sistema está activo y aplica los colores o apaga el LED, permitiendo su compatibilidad con modos de bajo consumo.

Enlace a la [FSM de Version 3](fsm__rgb__light_8c.html).

## Version 4

**Integración final** del sistema y desarrollo de la lógica central del juego *Simone*.

* Creación de la **FSM central de Simone** (con 7 estados) que orquesta las FSM del botón, el teclado y el LED.
* Gestión de reglas del juego: generación de secuencias aleatorias, 3 niveles de dificultad crecientes, tiempos de *timeout* del jugador y validación de aciertos/errores en tiempo real.
* Implementación de **modos de bajo consumo** (`Sleep Mode`). Se desactiva el `SysTick` durante la inactividad y la reproducción de colores, despertando el microcontrolador únicamente mediante interrupciones para ahorrar energía.

Enlace a la [FSM de Version 4](fsm__simone_8c.html).

## Version 5

*(Añade aquí una breve descripción de las características adicionales que hayáis implementado por vuestra cuenta. Por ejemplo: medición real del ahorro de energía, nuevos modos de juego, melodías, etc.)*

* Implementación de funcionalidad extra 1.
* Implementación de funcionalidad extra 2.