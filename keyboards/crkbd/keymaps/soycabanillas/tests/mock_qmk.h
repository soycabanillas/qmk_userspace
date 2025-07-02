#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmock/gmock.h>

extern "C" {

// Mock QMK types and constants
typedef struct {
    uint8_t col;
    uint8_t row;
} keypos_t;

typedef struct {
    keypos_t key;
    bool pressed;
    uint16_t time;
} keyrecord_t;

typedef uint32_t deferred_token;

// Mock keycodes
#define KC_Q 0x14
#define KC_R 0x15
#define KC_F 0x09
#define KC_EXLM 0x1E
#define KC_QUES 0x38
#define KC_2 0x1F
#define KC_3 0x20
#define KC_4 0x21
#define KC_5 0x22
#define KC_6 0x23
#define KC_9 0x26
#define KC_0 0x27
#define KC_SLSH 0x38
#define KC_1 0x1E
#define KC_LEFT_SHIFT 0xE1
#define KC_RIGHT_SHIFT 0xE5
#define KC_LEFT_CTRL 0xE0
#define KC_RIGHT_CTRL 0xE4
#define KC_LEFT_ALT 0xE2
#define KC_RIGHT_ALT 0xE6
#define KC_LEFT_GUI 0xE3
#define KC_RIGHT_GUI 0xE7

// Mock layer definitions
#define _LQWERTY 0
#define _LLEFT_THUMB 1
#define _LRIGHT_THUMB 2
#define _LBOTH_THUMBS 3
#define _LCONTROL 4
#define _LMOVEMENT 5
#define _LMOVEMENT_RIGTH_PINK 6
#define _LMOUSE 7
#define _LNUMBERS 8
#define _LMEDIA 9
#define _LFUNCTIONKEYS 10

// Mock custom keycodes
#define SAFE_RANGE 0x7E00
#define CKC_LAY_MOUSE_Q (SAFE_RANGE + 1)
#define CKC_LAY_NUMBERS_R (SAFE_RANGE + 2)
#define CKC_LAY_MOVEMENT_F (SAFE_RANGE + 3)
#define CKC_EXCL (SAFE_RANGE + 4)
#define CKC_QUES (SAFE_RANGE + 5)
#define CKC_LAY_RIGHT_THUMB (SAFE_RANGE + 6)

// Mock modifier macros
#define S(kc) (0x0100 | (kc))
#define RALT(kc) (0x0600 | (kc))

// Timer functions
uint16_t timer_read(void);
uint16_t timer_elapsed(uint16_t last);
void wait_ms(uint16_t ms);

// Layer functions
void layer_on(uint8_t layer);
void layer_off(uint8_t layer);
void clear_keyboard(void);
uint8_t get_layer_topdown(keypos_t key);
uint8_t layer_switch_get_layer(keypos_t key);
uint16_t keymap_key_to_keycode(uint8_t layer, keypos_t key);

// Key registration functions
void register_code(uint8_t code);
void unregister_code(uint8_t code);
void register_code16(uint16_t code);
void unregister_code16(uint16_t code);
void tap_code16_delay(uint16_t code, uint8_t delay);
void register_mods(uint8_t mods);
void unregister_mods(uint8_t mods);
void set_mods(uint8_t mods);
uint8_t get_mods(void);
uint8_t get_oneshot_mods(void);

// Deferred execution
deferred_token defer_exec(uint32_t delay_ms, uint32_t (*callback)(uint32_t, void*), void* cb_arg);
bool cancel_deferred_exec(deferred_token token);

// Debug functions
void uprintf(const char* fmt, ...);

// Mock global variables
extern uint16_t g_mock_time;
extern uint8_t g_mock_active_layer;
extern uint8_t g_mock_mods;
extern uint8_t g_mock_oneshot_mods;

// Mock function call tracking
struct MockQMKState {
    // Layer state
    bool layers[16];

    // Key state
    bool keys_pressed[256];

    // Modifiers
    uint8_t mods;
    uint8_t oneshot_mods;

    // Timer
    uint16_t time;

    // Deferred execution
    uint32_t next_token;

    // Function call counts
    int layer_on_calls;
    int layer_off_calls;
    int register_code_calls;
    int unregister_code_calls;
    int tap_code_calls;
    int clear_keyboard_calls;

    // Last called values
    uint8_t last_layer_on;
    uint8_t last_layer_off;
    uint16_t last_registered_code;
    uint16_t last_unregistered_code;
    uint16_t last_tapped_code;
};

extern MockQMKState g_mock_state;

// Reset mock state
void reset_mock_state(void);

// Test helper functions
void advance_time(uint16_t ms);
bool is_layer_active(uint8_t layer);
bool is_key_pressed(uint16_t keycode);

} // extern "C"
