#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Platform detection
#ifdef UNIT_TEST
    #define PLATFORM_TEST
#elif defined(QMK_KEYBOARD_H)
    #define PLATFORM_QMK
#elif defined(ZMK_INCLUDE_KERNEL_H)
    #define PLATFORM_ZMK
#else
    #error "Unknown platform"
#endif

// Generic types (platform-agnostic definitions)
#if defined(PLATFORM_TEST) || defined(PLATFORM_QMK)
    typedef uint16_t platform_keycode_t;
    typedef uint16_t platform_time_t;
#elif defined(PLATFORM_ZMK)
    typedef uint32_t platform_keycode_t;
    typedef int64_t platform_time_t;
#else
    // Default to largest common types for unknown platforms
    typedef uint32_t platform_keycode_t;
    typedef int64_t platform_time_t;
#endif

#define MACRO_KEY_MODIFIER_LEFT_SHIFT  (1 << 0)
#define MACRO_KEY_MODIFIER_RIGHT_SHIFT (1 << 1)
#define MACRO_KEY_MODIFIER_LEFT_CTRL   (1 << 2)
#define MACRO_KEY_MODIFIER_RIGHT_CTRL  (1 << 3)
#define MACRO_KEY_MODIFIER_LEFT_ALT    (1 << 4)
#define MACRO_KEY_MODIFIER_RIGHT_ALT   (1 << 5)
#define MACRO_KEY_MODIFIER_LEFT_GUI    (1 << 6)
#define MACRO_KEY_MODIFIER_RIGHT_GUI   (1 << 7)

typedef enum {
    MODIFIER_LEFT_SHIFT  = (1 << 0),
    MODIFIER_RIGHT_SHIFT = (1 << 1),
    MODIFIER_LEFT_CTRL   = (1 << 2),
    MODIFIER_RIGHT_CTRL  = (1 << 3),
    MODIFIER_LEFT_ALT    = (1 << 4),
    MODIFIER_RIGHT_ALT   = (1 << 5),
    MODIFIER_LEFT_GUI    = (1 << 6),
    MODIFIER_RIGHT_GUI   = (1 << 7)
} modifier_t;


// Generic key position structure (platform-agnostic)
typedef struct {
    uint8_t col;
    uint8_t row;
} platform_keypos_t;

// Generic key record structure (platform-agnostic)
typedef struct {
    platform_keypos_t key;
    bool     pressed;
    platform_time_t time;
} abskeyevent_t;

// Timer functions
platform_time_t platform_timer_read(void);
platform_time_t platform_timer_elapsed(platform_time_t start);

// Platform-specific function declarations (mockable)
void platform_register_code(platform_keycode_t keycode);
void platform_unregister_code(platform_keycode_t keycode);
void platform_tap_code(platform_keycode_t keycode);
void platform_tap_code_delay(platform_keycode_t keycode, uint8_t delay);
void platform_add_key(platform_keycode_t keycode);
void platform_del_key(platform_keycode_t keycode);
void platform_send_keyboard_report(void);
void platform_clear_keyboard(void);
bool platform_is_modifier(platform_keycode_t keycode);
platform_keycode_t platform_get_mods(void);
void platform_set_mods(platform_keycode_t mods);
void platform_add_mods(platform_keycode_t mods);
void platform_del_mods(platform_keycode_t mods);
void platform_wait_ms(uint16_t ms);

// Layer management abstractions
void platform_layer_select(uint8_t layer);
uint8_t platform_layer_get_current(void);

// Deferred execution abstractions
typedef uint32_t platform_deferred_token;
typedef uint32_t (*platform_deferred_callback)(uint32_t trigger_time, void *cb_arg);
typedef void (*platform_deferred_callback_no_return)(uint32_t trigger_time, void *cb_arg);
platform_deferred_token platform_defer_exec(uint32_t delay_ms, platform_deferred_callback callback, void *cb_arg);
void platform_cancel_deferred_exec(platform_deferred_token token);

#ifdef PLATFORM_QMK
    // QMK-specific implementations will be in a separate .c file
#elif defined(PLATFORM_ZMK)
    // TODO: ZMK-specific implementations will be in a separate .c file
#endif

#ifdef __cplusplus
}
#endif
