#pragma once

#include <stdint.h>
#include <stdbool.h>

// Platform detection
#ifdef QMK_KEYBOARD_H
    #define PLATFORM_QMK
#elif defined(ZMK_INCLUDE_KERNEL_H)
    #define PLATFORM_ZMK
#else
    #error "Unknown platform"
#endif

// Logging abstraction
#ifdef PLATFORM_QMK
    #ifdef CONSOLE_ENABLE
        #define PLATFORM_LOGGING_ENABLED
    #endif
#elif defined(PLATFORM_ZMK)
    #ifdef CONFIG_LOG
        #define PLATFORM_LOGGING_ENABLED
    #endif
#endif

// Generic types (platform-agnostic definitions)
#ifdef PLATFORM_QMK
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

// Generic key position structure (platform-agnostic)
typedef struct {
    uint8_t col;
    uint8_t row;
} platform_keypos_t;

// Generic key record structure (platform-agnostic)
typedef struct {
    struct {
        platform_keypos_t key;
        bool pressed;
        platform_time_t time;
    } event;
} platform_keyrecord_t;

// Timer functions
platform_time_t platform_timer_read(void);
platform_time_t platform_timer_elapsed(platform_time_t start);

// Logging functions - simplified to avoid macro conflicts
#ifdef PLATFORM_LOGGING_ENABLED
    // For now, disable logging to avoid uprintf macro issues
    // TODO: Implement proper logging later
    #define platform_log_debug(fmt, ...)
    #define platform_log_info(fmt, ...)
    #define platform_log_error(fmt, ...)
#else
    #define platform_log_debug(fmt, ...)
    #define platform_log_info(fmt, ...)
    #define platform_log_error(fmt, ...)
#endif

// Platform-specific function declarations (mockable)
void platform_register_code(platform_keycode_t keycode);
void platform_unregister_code(platform_keycode_t keycode);
void platform_tap_code(platform_keycode_t keycode);
void platform_tap_code_delay(platform_keycode_t keycode, uint8_t delay);
bool platform_is_key_pressed(platform_keyrecord_t *record);
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
void platform_layer_on(uint8_t layer);
void platform_layer_off(uint8_t layer);

// Deferred execution abstractions
typedef uint32_t platform_deferred_token;
typedef uint32_t (*platform_deferred_callback)(uint32_t trigger_time, void *cb_arg);
platform_deferred_token platform_defer_exec(uint32_t delay_ms, platform_deferred_callback callback, void *cb_arg);
void platform_cancel_deferred_exec(platform_deferred_token token);

#ifdef PLATFORM_QMK
    // QMK-specific implementations will be in a separate .c file
#elif defined(PLATFORM_ZMK)
    // TODO: ZMK-specific implementations will be in a separate .c file
#endif
