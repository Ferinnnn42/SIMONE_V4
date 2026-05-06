#ifndef FSM_SIMONE_H_
#define FSM_SIMONE_H_

#include <stdint.h>
#include <stdbool.h>
#include "fsm.h"
#include "fsm_button.h"
#include "fsm_keyboard.h"
#include "fsm_rgb_light.h"

// Keys
#define KEY_RED '1'
#define KEY_GREEN '2'
#define KEY_BLUE '3'
#define KEY_YELLOW '5'
#define KEY_TURQUOISE '8'
#define KEY_WHITE '0'
#define KEY_INVALID_COLOR ' '

// Intensities
#define LEVEL_MAX_INTENSITY 100
#define LEVEL_EASY_MIN_INTENSITY 80
#define LEVEL_MEDIUM_MIN_INTENSITY 50
#define LEVEL_HARD_MIN_INTENSITY 20

// Sequence
#define SEQUENCE_LENGTH 5
#define NUMBER_OF_COLORS_GAME 6

// Timeouts of the game
#define SIMONE_TIME_OFF_BETWEEN_COLORS_MS 300
#define SIMONE_TIME_VISUAL_FEEDBACK_MS 300
#define SIMONE_TIME_ON_LEVEL_EASY_MS 3000
#define SIMONE_TIME_ON_LEVEL_MEDIUM_MS 2000
#define SIMONE_TIME_ON_LEVEL_HARD_MS 1000
#define SIMONE_TIME_WAIT_INPUT_MS 5000

/**
 * @brief Enumerator for the states of the Simone game.
 *
 */
enum FSM_SIMONE
{
    IDLE = 0,
    ADD_COLOR,
    PLAYBACK,
    WAIT_KEY,
    VERIFY_INPUT,
    SLEEP_WHILE_IDLE,
    SLEEP_WHILE_PLAYBACK,
};

/**
 * @brief Enumerator for the levels of the game.
 *
 */
enum LEVELS
{
    LEVEL_EASY = 0,
    LEVEL_MEDIUM,
    LEVEL_HARD
};

/**
 * @brief Structure to define the FSM of the Simone game.
 *
 */
typedef struct
{
    /*FSM elements*/
    fsm_t f;
    fsm_button_t *p_fsm_button;
    fsm_keyboard_t *p_fsm_keyboard;
    fsm_rgb_light_t *p_fsm_rgb_light;
    /*Data sequence*/
    rgb_color_t sequence[SEQUENCE_LENGTH];
    rgb_color_t seq_intensities[SEQUENCE_LENGTH];
    uint8_t level;
    /*Control Indexes*/
    uint8_t seq_idx;      // Index to add new colors to the sequence and to play it back
    uint8_t playback_idx; // Index to play back the sequence and to check the user input
    uint8_t player_idx;   // Index to check the user input. It is different from playback_idx because the player can make mistakes in the sequence, so we need to check the input with the sequence and not with the playback.
    /*Flags*/
    char player_key;
    bool playback_over;
    uint32_t on_off_press_time_ms;
} fsm_simone_t;

fsm_simone_t *fsm_simone_new(fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_keyboard_t *p_fsm_keyboard, fsm_rgb_light_t *p_fsm_rgb_light, uint8_t level);
void fsm_simone_fire(fsm_simone_t *p_fsm);
void fsm_simone_destroy(fsm_simone_t *p_fsm);

#endif /* FSM_SIMONE_H_ */