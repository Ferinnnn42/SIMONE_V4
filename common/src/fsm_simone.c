/**
 * @file fsm_simone.c
 * @brief Simone FSM main file.
 * @author Fernando Ingelmo Ajejas
 * @author Paula Rodríguez Hernández
 * @date 21/04/2026
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
/* HW dependent includes */
#include "port_system.h"
#include "port_button.h"
#include "port_simone.h"
#include "rgb_colors.h"
#include "fsm.h"
#include "fsm_simone.h"
/* Project includes */

const rgb_color_t *p_colors_library[] = {&color_red, &color_green, &color_blue, &color_yellow, &color_turquoise, &color_white};

#define PLAYBACK_FINISHED_MARKER 0xFF // Marker to indicate that the playback of the sequence has finished

/**
 * @brief 	Get the expected key from a given color.
 *
 * @param color
 * @return char
 */
static char _get_key_from_color(rgb_color_t color)
{
    if (color.r == color_red.r && color.g == color_red.g && color.b == color_red.b)
    {
        return KEY_RED;
    }
    else if (color.r == color_green.r && color.g == color_green.g && color.b == color_green.b)
    {
        return KEY_GREEN;
    }
    else if (color.r == color_blue.r && color.g == color_blue.g && color.b == color_blue.b)
    {
        return KEY_BLUE;
    }
    else if (color.r == color_yellow.r && color.g == color_yellow.g && color.b == color_yellow.b)
    {
        return KEY_YELLOW;
    }
    else if (color.r == color_turquoise.r && color.g == color_turquoise.g && color.b == color_turquoise.b)
    {
        return KEY_TURQUOISE;
    }
    else if (color.r == color_white.r && color.g == color_white.g && color.b == color_white.b)
    {
        return KEY_WHITE;
    }
    else
    {
        return KEY_INVALID_COLOR;
    }
}

/**
 * @brief 	Get the color corresponding to a given key.
 *
 * @param key
 * @return rgb_color_t
 */
static rgb_color_t _get_color_from_key(char key)
{
    switch (key)
    {
    case KEY_RED:
        return color_red;
    case KEY_GREEN:
        return color_green;
    case KEY_BLUE:
        return color_blue;
    case KEY_YELLOW:
        return color_yellow;
    case KEY_TURQUOISE:
        return color_turquoise;
    case KEY_WHITE:
        return color_white;
    default:
        return color_off;
    }
}

/**
 * @brief 	Add a new color and intensity to the sequence.
 *
 * @param p_fsm_simone
 */
static void _add_color(fsm_simone_t *p_fsm_simone)
{
    uint8_t color_idx = rand() % NUMBER_OF_COLORS_GAME; // Random index to select a color from the library
    uint8_t min = 0;

    // Set the minimum intensity according to the level

    if (p_fsm_simone->level == LEVEL_EASY)
    {
        min = LEVEL_EASY_MIN_INTENSITY;
    }
    else if (p_fsm_simone->level == LEVEL_MEDIUM)
    {
        min = LEVEL_MEDIUM_MIN_INTENSITY;
    }
    else if (p_fsm_simone->level == LEVEL_HARD)
    {
        min = LEVEL_HARD_MIN_INTENSITY;
    }

    uint8_t random_ints = (rand() % (LEVEL_MAX_INTENSITY - min + 1)) + min; // Random intensity between the minimum of the level and the maximum defined in the project

    if (p_fsm_simone->seq_idx == SEQUENCE_LENGTH) // If the sequence is full, reset the index
    {
        p_fsm_simone->seq_idx = 0;
    }
    else
    { // If the sequence is not full, add the new color and intensity to the sequence
        p_fsm_simone->seq_colors[p_fsm_simone->seq_idx] = *p_colors_library[color_idx];
        p_fsm_simone->seq_intensities[p_fsm_simone->seq_idx] = random_ints;
        p_fsm_simone->seq_idx++;
    }
}

/* State machine input or transition functions */

/**
 * @brief 	Check if the button has been pressed for the required time to turn ON the Simone system.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_on(fsm_t *p_this)
{
    // Check if the button has been pressed for the required time to turn ON the Simone system
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    uint32_t duration = fsm_button_get_duration(p_fsm_simone->p_fsm_button);
    return (duration > 0 && duration >= p_fsm_simone->on_off_press_time_ms);
}

/**
 * @brief 	Check if any of the elements of the system is active.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_activity(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    return fsm_button_check_activity(p_fsm_simone->p_fsm_button) || fsm_keyboard_check_activity(p_fsm_simone->p_fsm_keyboard) || fsm_rgb_light_check_activity(p_fsm_simone->p_fsm_rgb_light);
}

/**
 * @brief 	Check if all the elements of the system are inactive.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_no_activity(fsm_t *p_this)
{
    return !check_activity(p_this);
}

/**
 * @brief 	Check if a new color has been added to the sequence.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_color_added(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    return p_fsm_simone->seq_idx != p_fsm_simone->player_idx;
}

/**
 * @brief Check if the button has been pressed for the required time to turn IDLE the system.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_off(fsm_t *p_this)
{
    // Check if the button has been pressed for the required time to turn OFF the Simone system
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    uint32_t duration = fsm_button_get_duration(p_fsm_simone->p_fsm_button);
    return (duration > 0 && duration >= p_fsm_simone->on_off_press_time_ms);
}

/**
 * @brief 	Check if the playback of the sequence is over and thus the player can start inputting the sequence.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_playback_over(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    bool result = (p_fsm_simone->playback_idx == PLAYBACK_FINISHED_MARKER && port_simone_get_timeout_status());
    return result;
}

/**
 * @brief Check if the player has won the game.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_winner(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    bool finished_seq = p_fsm_simone->player_idx >= p_fsm_simone->seq_idx; // Check if the player has input the same number of colors and intensities as the sequence has
    bool max_seq = p_fsm_simone->seq_idx >= SEQUENCE_LENGTH;               // Check if the sequence has reached the maximum length defined in the project
    bool check_level = (p_fsm_simone->level == LEVEL_HARD);                // Check if the player has reached the maximum level of the game
    return (finished_seq && max_seq && check_level);                       // true if the player has won the game, false otherwise
}

/**
 * @brief 	Check if the player key timeout has occurred. This is, the player has taken too long to press a key.
 *
 * @param p_this
 * @return true
 * @return false
 */

static bool check_player_key_timeout(fsm_t *p_this)
{
    bool val = port_simone_get_timeout_status();
    return val;
}

/**
 * @brief 	Check if the player has finished the current sequence.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_player_round_end(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    bool seq_correct = p_fsm_simone->player_idx == p_fsm_simone->seq_idx;
    bool seq_total = (p_fsm_simone->seq_idx < SEQUENCE_LENGTH || p_fsm_simone->level != LEVEL_HARD);
    return (seq_correct && seq_total) ? true : false; // Check if the player has input the same number of colors and intensities as the sequence has
}

/**
 * @brief 	Check if any key has been pressed by the player.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_any_key_pressed(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    char key = p_fsm_simone->p_fsm_keyboard->key_value;
    char inv = p_fsm_simone->p_fsm_keyboard->invalid_key;
    return key != inv; // Check if the value of key_value of the keyboard with id keyboard_id is different from the value of invalid_key
}

/**
 * @brief 	Check if the input feedback time has finished and if the pressed key is correct.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_input_valid(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    if (!port_simone_get_timeout_status())
    {
        return false; // If the input feedback time has not finished, return false
    }
    char expected_key = _get_key_from_color(p_fsm_simone->seq_colors[p_fsm_simone->player_idx]); // Get the expected key from the current color of the sequence that the player has to input
    printf("[SIMONE] GOOD JOB! Expected '%c' (%d), Pressed '%c' (%d). \n", expected_key, expected_key, p_fsm_simone->player_key, p_fsm_simone->player_key);
    return p_fsm_simone->player_key == expected_key; // Check if the key pressed by the player is the expected key for the current color of the sequence that the player has to
}

/**
 * @brief 	Check if the input feedback time has finished and if the pressed key is correct.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_input_invalid(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    if (!port_simone_get_timeout_status())
    {
        return false; // If the input feedback time has not finished, return false
    }
    char expected_key = _get_key_from_color(p_fsm_simone->seq_colors[p_fsm_simone->player_idx]); // Get the expected key from the current color of the sequence that the player has to input
    return p_fsm_simone->player_key != expected_key;                                             // Check if the key pressed by the player is not the expected key for the current color of the sequence that the player has to
}

/**
 * @brief 	Check if the playback color timeout has occurred.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_playback_color_timeout(fsm_t *p_this)
{
    return port_simone_get_timeout_status();
}

/* State machine output or action functions */

/**
 * @brief Initialize the Simone game.
 *
 * @param p_this
 */
static void do_init_game(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    fsm_button_reset_duration(p_fsm_simone->p_fsm_button);
    p_fsm_simone->on_off_press_time_ms = 0;
    p_fsm_simone->seq_idx = 0;
    p_fsm_simone->playback_idx = 0;
    p_fsm_simone->player_idx = 0;
    p_fsm_simone->playback_over = false;
    p_fsm_simone->player_key = p_fsm_simone->p_fsm_keyboard->invalid_key;
    p_fsm_simone->level = LEVEL_EASY;
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++)
    {
        p_fsm_simone->seq_colors[i] = color_off;
        p_fsm_simone->seq_intensities[i] = 0;
    }
    _add_color(p_fsm_simone);
    fsm_rgb_light_set_status(p_fsm_simone->p_fsm_rgb_light, true); // This function activates the fsm rgb light system status
    printf("[SIMONE][%ld] Let's play Simone game \n", port_system_get_millis());
}

/**
 * @brief 	Start the low power mode while the Simone is IDLE.
 *
 * @param p_this
 */
static void do_sleep_idle(fsm_t *p_this)
{
    port_system_sleep();
}

/**
 * @brief 	Handle the playback of the current sequence to the player.
 *
 * @param p_this
 */
static void do_playback(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;

    if (p_fsm_simone->playback_idx == PLAYBACK_FINISHED_MARKER) // If the playback of the sequence has finished, do nothing
        return;
    else
    {
        p_fsm_simone->on_off_press_time_ms = 0;               // Reset the on_off_press_time_ms to avoid unintended timeouts while playing back the sequence
        fsm_keyboard_stop_scan(p_fsm_simone->p_fsm_keyboard); // Stop the keyboard scanning while playing back the sequence to avoid unintended key presses from the player

        if (p_fsm_simone->playback_over == true)
        {
            p_fsm_simone->p_fsm_rgb_light->color = color_off;                 // Set the RGB light color to off
            port_simone_set_timer_timeout(SIMONE_TIME_OFF_BETWEEN_COLORS_MS); // Set a timeout to enter the sleep mode after the playback of the sequence to save power while waiting for the player input
            p_fsm_simone->playback_idx++;
            p_fsm_simone->playback_over = false;

            if (p_fsm_simone->playback_idx >= p_fsm_simone->seq_idx)
            {
                p_fsm_simone->playback_idx = PLAYBACK_FINISHED_MARKER; // Set the playback index to the marker to indicate that the playback has finished
                return;
            }
            else
            {
                return;
            }
        }
        else
        {
            p_fsm_simone->p_fsm_rgb_light->color = p_fsm_simone->seq_colors[p_fsm_simone->playback_idx];               // Set the RGB light color to the current color of the sequence being played back
            p_fsm_simone->p_fsm_rgb_light->intensity_perc = p_fsm_simone->seq_intensities[p_fsm_simone->playback_idx]; // Set the RGB light intensity to the current intensity of the sequence being played back

            fsm_rgb_light_set_color_intensity(p_fsm_simone->p_fsm_rgb_light, p_fsm_simone->p_fsm_rgb_light->color, p_fsm_simone->p_fsm_rgb_light->intensity_perc); // Set the RGB light color and intensity to the current color and intensity of the sequence being played back

            int duration = 0;

            switch (p_fsm_simone->level) // Set the duration of the color being played back according to the level
            {
            case LEVEL_EASY:
                duration = SIMONE_TIME_ON_LEVEL_EASY_MS;
                break;
            case LEVEL_MEDIUM:
                duration = SIMONE_TIME_ON_LEVEL_MEDIUM_MS;
                break;
            case LEVEL_HARD:
                duration = SIMONE_TIME_ON_LEVEL_HARD_MS;
                break;
            default:
                duration = SIMONE_TIME_ON_LEVEL_EASY_MS;
                break;
            }

            port_simone_set_timer_timeout(duration); // Set a timeout to change to the next color of the sequence after a certain time depending on the level to give time to the player to see the color and intensity of each element of the sequence

            p_fsm_simone->playback_over = true; // Set the playback_over flag to true to indicate that the current color of the sequence has been played back
            return;
        }
    }
}

/**
 * @brief Turn the Simone system IDLE.
 *
 * @param p_this
 */
static void do_stop_simone(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    fsm_button_reset_duration(p_fsm_simone->p_fsm_button);
    p_fsm_simone->on_off_press_time_ms = 0;
    fsm_rgb_light_set_status(p_fsm_simone->p_fsm_rgb_light, false); // This function deactivates the fsm rgb light system status
    p_fsm_simone->level = LEVEL_EASY;
    printf("[SIMONE][%ld]Game Over, press the button for 3 seconds to start a new game\n", port_system_get_millis());
}

/**
 * @brief Start the player input sequence. Prepare the system for player input.
 *
 * @param p_this
 */
static void do_start_player_sequence(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    p_fsm_simone->playback_over = false; // Reset playback_over flag
    p_fsm_simone->player_idx = 0;        // Reset player index to check the player input from the beginning of the sequence
    fsm_rgb_light_set_color_intensity(p_fsm_simone->p_fsm_rgb_light, color_off, 0);
    port_simone_set_timeout_status(false);                    // Reset the timeout status to avoid unintended timeouts while waiting for the player input
    port_simone_set_timer_timeout(SIMONE_TIME_WAIT_INPUT_MS); // Set a timeout to wait for the player input before entering the sleep mode to save power while waiting for the player input
    fsm_keyboard_start_scan(p_fsm_simone->p_fsm_keyboard);    // Start the keyboard scanning to check the player input
    printf("[SIMONE][%ld]YOUR TURN! You have %d seconds to input the sequence\n", port_system_get_millis(), SIMONE_TIME_WAIT_INPUT_MS / 1000);
}

/**
 * @brief 	Start the low power mode while the Simone is PLAYBACK.
 *
 * @param p_this
 */
static void do_sleep_playback(fsm_t *p_this)
{
    port_system_sleep();
}

/**
 * @brief 	Handle the winning status of the game.
 *
 * @param p_this
 */
static void do_winner(fsm_t *p_this)
{
    port_simone_stop_timer(); // Stop the timer
    printf("[SIMONE][%ld]CONGRATS! YOU WON THE GAME AND GOT CORRECT %d COLORS!!!\n", port_system_get_millis(), SEQUENCE_LENGTH);
}

/**
 * @brief Handle the game over status of the game.
 *
 * @param p_this
 */
static void do_game_over_timeout(fsm_t *p_this)
{
    port_simone_stop_timer(); // Stop the timer

    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    printf("[SIMONE][%ld]GAME OVER! You took too long to input the sequence, you got %d colors correct. press the blue button to play again!\n", port_system_get_millis(), p_fsm_simone->player_idx);
    p_fsm_simone->playback_idx = 0; // Reset playback index to the beginning of the sequence
    p_fsm_simone->player_idx = 0;   // Reset player index to the beginning of the sequence
    p_fsm_simone->playback_over = false;
    p_fsm_simone->seq_idx = 0;                            // Reset sequence index to the beginning of the sequence
    p_fsm_simone->level = LEVEL_EASY;                     // Reset level to the easy level
    p_fsm_simone->on_off_press_time_ms = 0;               // Reset to 0 the on_off_press_time_ms to avoid unintended timeouts
    fsm_keyboard_stop_scan(p_fsm_simone->p_fsm_keyboard); // Stop the keyboard scanning to avoid unintended key presses from the player
}

/**
 * @brief 	Add a new color to the current sequence.
 *
 * @param p_this
 */
void do_add_color(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    p_fsm_simone->player_idx = 0;
    p_fsm_simone->playback_idx = 0;
    p_fsm_simone->playback_over = false;

    if (p_fsm_simone->seq_idx >= SEQUENCE_LENGTH && p_fsm_simone->level < LEVEL_HARD) // If the player has not reached the maximum level, increase the level of the game
    {
        p_fsm_simone->level++;
        p_fsm_simone->seq_idx = 0; // Reset the sequence index to the beginning of the sequence to start a new level
        printf("[SIMONE][%ld]LEVEL UP! Level: %d\n", port_system_get_millis(), p_fsm_simone->level);
    }
    _add_color(p_fsm_simone); // Add a new color and intensity to the sequence
    return;
}

/**
 * @brief Capture the player's key input and provide visual feedback.
 *
 * @param p_this
 */
static void do_capture_input(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    p_fsm_simone->player_key = fsm_keyboard_get_key_value(p_fsm_simone->p_fsm_keyboard); // Get the key value of the player input
    fsm_keyboard_reset_key_value(p_fsm_simone->p_fsm_keyboard);                          // Reset the key value of the keyboard to avoid unintended repeated inputs
    fsm_rgb_light_set_status(p_fsm_simone->p_fsm_rgb_light, true);                       // To make sure the RGB light system is active
    fsm_rgb_light_set_color_intensity(p_fsm_simone->p_fsm_rgb_light, _get_color_from_key(p_fsm_simone->player_key), LEVEL_MAX_INTENSITY);
    port_simone_set_timeout_status(false);
    port_simone_set_timer_timeout(SIMONE_TIME_VISUAL_FEEDBACK_MS); // Set a timeout to keep the visual feedback of the player input for a certain   time
}

/**
 * @brief 	Handle a valid key press from the player.
 *
 * @param p_this
 */
static void do_valid_key(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    fsm_rgb_light_set_color_intensity(p_fsm_simone->p_fsm_rgb_light, color_off, 0);
    p_fsm_simone->player_idx++;                                           // Increase the player index to check the next input of the player
    p_fsm_simone->player_key = p_fsm_simone->p_fsm_keyboard->invalid_key; // Reset the player key to the invalid key to avoid unintended repeated inputs
    port_simone_set_timeout_status(false);                                // Reset the timeout status to avoid unintended timeouts while waiting for the next player input
    port_simone_set_timer_timeout(SIMONE_TIME_WAIT_INPUT_MS);             // Set a timeout to wait for the next player input before entering the sleep mode to save power while waiting for the player
}

/**
 * @brief 	Handle an invalid key press from the player.
 *
 * @param p_this
 */
static void do_game_over_invalid_key(fsm_t *p_this)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)p_this;
    char expected_key = _get_key_from_color(p_fsm_simone->seq_colors[p_fsm_simone->player_idx]); // Get the expected key from the current color of the sequence that the player has to input
    printf("[SIMONE] Expected '%c' (%d), Pressed '%c' (%d). You got %d colors correct. If you want to play again, press the blue button!\n", expected_key, expected_key, p_fsm_simone->player_key, p_fsm_simone->player_key, p_fsm_simone->player_idx);
    fsm_rgb_light_set_color_intensity(p_fsm_simone->p_fsm_rgb_light, color_off, 0);
    p_fsm_simone->playback_idx = 0; // Reset playback index to the beginning of the sequence
    p_fsm_simone->player_idx = 0;   // Reset player index to the beginning of the sequence
    p_fsm_simone->playback_over = false;
    p_fsm_simone->seq_idx = 0;                            // Reset sequence index to the beginning of the sequence
    p_fsm_simone->level = LEVEL_EASY;                     // Reset level to the easy level
    p_fsm_simone->on_off_press_time_ms = 0;               // Reset to 0 the on_off_press_time_ms to avoid unintended timeouts
    port_simone_stop_timer();                             // Stop the timer
    fsm_keyboard_stop_scan(p_fsm_simone->p_fsm_keyboard); // Stop the keyboard scanning to avoid unintended key presses from the player
}
/**
 * @brief 	Array representing the transitions table of the FSM Simone.
 *
 */
static fsm_trans_t fsm_trans_simone[] = {

    // ================== ESTADO: IDLE ==================
    // 1. Por encendido del usuario
    {IDLE, check_on, ADD_COLOR, do_init_game},
    // 2. Por inactividad
    {IDLE, check_no_activity, SLEEP_WHILE_IDLE, do_sleep_idle},

    // ================== ESTADO: ADD_COLOR ==================
    // 1. Secuencia actualizada
    {ADD_COLOR, check_color_added, PLAYBACK, do_playback},

    // ================== ESTADO: PLAYBACK ==================
    // 1. Apagado manual
    {PLAYBACK, check_off, IDLE, do_stop_simone},
    // 2. Turno del jugador
    {PLAYBACK, check_playback_over, WAIT_KEY, do_start_player_sequence},
    // 3. Por inactividad (durante esperas de luz)
    {PLAYBACK, check_no_activity, SLEEP_WHILE_PLAYBACK, do_sleep_playback},

    // ================== ESTADO: WAIT_KEY ==================
    // 1. Apagado manual
    {WAIT_KEY, check_off, IDLE, do_stop_simone},
    // 2. Victoria final
    {WAIT_KEY, check_winner, IDLE, do_winner},
    // 3. Fin de ronda (superada pero sin victoria final)
    {WAIT_KEY, check_player_round_end, ADD_COLOR, do_add_color},
    // 4. Pulsación de una tecla
    {WAIT_KEY, check_any_key_pressed, VERIFY_INPUT, do_capture_input},
    // 5. Derrota por tiempo
    {WAIT_KEY, check_player_key_timeout, IDLE, do_game_over_timeout},

    // ================== ESTADO: VERIFY_INPUT ==================
    // 1. Tecla correcta
    {VERIFY_INPUT, check_input_valid, WAIT_KEY, do_valid_key},
    // 2. Tecla incorrecta
    {VERIFY_INPUT, check_input_invalid, IDLE, do_game_over_invalid_key},

    // ================== ESTADO: SLEEP_WHILE_IDLE ==================
    // 1. Detección de actividad (despertar)
    {SLEEP_WHILE_IDLE, check_activity, IDLE, NULL},
    // 2. Sin actividad (autotransición)
    {SLEEP_WHILE_IDLE, check_no_activity, SLEEP_WHILE_IDLE, do_sleep_idle},

    // ================== ESTADO: SLEEP_WHILE_PLAYBACK ==================
    // 1. Timeout del color/pausa (despertar para cambiar LED)
    {SLEEP_WHILE_PLAYBACK, check_playback_color_timeout, PLAYBACK, do_playback},
    // 2. Sin actividad (autotransición)
    {SLEEP_WHILE_PLAYBACK, check_no_activity, SLEEP_WHILE_PLAYBACK, do_sleep_playback},

    // ================== MARCADOR DE FIN DE TABLA ==================
    {-1, NULL, -1, NULL}};

static void fsm_simone_init(fsm_simone_t *p_fsm_simone, fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_keyboard_t *p_fsm_keyboard, fsm_rgb_light_t *p_fsm_rgb_light, uint8_t level)
{
    fsm_init(&p_fsm_simone->f, fsm_trans_simone);
    port_simone_init(); // Initialize the Simone system (timer, random generator, etc.)
    p_fsm_simone->p_fsm_button = p_fsm_button;
    p_fsm_simone->p_fsm_keyboard = p_fsm_keyboard;
    p_fsm_simone->p_fsm_rgb_light = p_fsm_rgb_light;
    p_fsm_simone->on_off_press_time_ms = on_off_press_time_ms;
    p_fsm_simone->level = level;
    srand(time(NULL)); // Initialize the random number generator with the current time as seed
    printf("[SIMONE][%ld] Simone FSM initialized. Press the blue button to play!\n", port_system_get_millis());
}

fsm_simone_t *fsm_simone_new(fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_keyboard_t *p_fsm_keyboard, fsm_rgb_light_t *p_fsm_rgb_light, uint8_t level)
{
    fsm_simone_t *p_fsm_simone = (fsm_simone_t *)malloc(sizeof(fsm_simone_t)); // Allocate memory for the FSM Simone struct
    if (p_fsm_simone != NULL)
    {
        fsm_simone_init(p_fsm_simone, p_fsm_button, on_off_press_time_ms, p_fsm_keyboard, p_fsm_rgb_light, level);
    }
    return p_fsm_simone;
}

void fsm_simone_fire(fsm_simone_t *p_fsm_simone)
{
    fsm_fire(&p_fsm_simone->f); // Call the generic FSM fire function with the pointer to the FSM struct */
}

void fsm_simone_destroy(fsm_simone_t *p_fsm_simone)
{
    fsm_destroy(&p_fsm_simone->f); // Call the generic FSM destroy function with the pointer to the FSM struct
}
