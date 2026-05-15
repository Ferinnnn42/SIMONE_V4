# SIMONE

Access the project documentation and API at [this link](https://sdg2dieupm.github.io/simone/).

## Authors

* Fernando Ingelmo Ajejas - email: fernando.ingelmo@alumnos.upm.es
* Paula Rodríguez Hernández - email: paula.rodriguez.hernandez@alumnos.upm.es

**Project Description:**

This project implements a version of the classic memory game "Simone" using an STM32F446RE microcontroller. The system features a control button, a 4x4 matrix keypad for player inputs, and a PWM-controlled RGB LED to display color sequences. The game generates random sequences that the user must repeat, progressively increasing the difficulty and incorporating low-power modes (Sleep Mode) to optimize energy efficiency.

![Final assembly of the Simone project](docs/assets/imgs/portada.png)

[![Simone V5 Demonstration](docs/assets/imgs/portada.png)](https://youtu.be/TU_ENLACE_AQUI "Simone demonstration on YouTube.")

---

## Version 1

In this first version, the library and state machine (FSM) for controlling the **user button** have been developed.

* Implementation of the HW module (PORT) using the `EXTI13` external interrupt.
* Creation of a hardware-independent FSM (COMMON) to manage button presses.
* Integration of a **software debounce filter** using guard times.
* Functionality to measure the press duration, necessary for turning the system on/off.



## Version 2

This version adds support for reading a **4x4 matrix keypad**.

* Configuration of input pins (columns with *pull-down*) and output pins (rows).
* Use of the `TIM5` timer to perform a periodic and sequential excitation of the rows.
* Management of combined interrupts in the columns to detect the exact button pressed.
* State machine with self-transition and **debounce** control for saving and validating the key.



## Version 3

The **RGB light** is integrated to provide visual feedback to the user and display the game sequences.

* Use of the `TIM4` timer in **PWM** mode to generate the control signal.
* Manipulation of the *duty cycle* across three different channels to generate various colors (Red, Green, Blue, Yellow, Turquoise, White) by modifying their intensity.
* State machine that evaluates if the system is active and applies the colors or turns off the LED, allowing compatibility with low-power modes.
* These three images show the rgb colors:
![Color RED](Entornos/MatrixMCU/projects/simone/images/red.png)
![Color GREEN](Entornos/MatrixMCU/projects/simone/images/green.png)
![Color BLUE](Entornos/MatrixMCU/projects/simone/images/blue.png)


## Version 4

**Final integration** of the system and development of the core logic for the *Simone* game.

* Creation of the **central Simone FSM** (with 7 states) that orchestrates the button, keypad, and LED FSMs.
* Game rules management: random sequence generation, 3 increasing difficulty levels, player *timeout* periods, and real-time validation of hits/misses.
* Implementation of **low-power modes** (`Sleep Mode`). The `SysTick` is deactivated during inactivity and color playback, waking up the microcontroller only via interrupts to save energy.



## Version 5

*(Add a brief description here of the additional features you have implemented on your own. For example: real energy savings measurement, new game modes, melodies, etc.)*

* Implementation of extra feature 1.
* Implementation of extra feature 2.