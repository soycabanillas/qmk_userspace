#pragma once

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

// Generic key code type
#ifdef PLATFORM_QMK
    #include QMK_KEYBOARD_H
    typedef uint16_t platform_keycode_t;
    typedef keyrecord_t platform_keyrecord_t;
    typedef keypos_t platform_keypos_t;
    #define PLATFORM_KC_NO KC_NO
    #define PLATFORM_KC_TRANSPARENT KC_TRANSPARENT
#elif defined(PLATFORM_ZMK)
    // ZMK types will be defined when we add ZMK support
    typedef uint32_t platform_keycode_t;
    // TODO: Define ZMK equivalents
#endif

// Generic time type
#ifdef PLATFORM_QMK
    typedef uint16_t platform_time_t;
    #define platform_timer_read() timer_read()
    #define platform_timer_elapsed(start) timer_elapsed(start)
#elif defined(PLATFORM_ZMK)
    typedef uint32_t platform_time_t;
    // TODO: Define ZMK timer functions
#endif

// Logging functions
#ifdef PLATFORM_LOGGING_ENABLED
    #ifdef PLATFORM_QMK
        #include "print.h"
        #define platform_log_debug(fmt, ...) uprintf(fmt "\n", ##__VA_ARGS__)
        #define platform_log_info(fmt, ...) uprintf(fmt "\n", ##__VA_ARGS__)
        #define platform_log_error(fmt, ...) uprintf("ERROR: " fmt "\n", ##__VA_ARGS__)
    #elif defined(PLATFORM_ZMK)
        // TODO: Define ZMK logging functions
        #define platform_log_debug(fmt, ...)
        #define platform_log_info(fmt, ...)
        #define platform_log_error(fmt, ...)
    #endif
#else
    #define platform_log_debug(fmt, ...)
    #define platform_log_info(fmt, ...)
    #define platform_log_error(fmt, ...)
#endif

// Platform-specific function declarations (mockable)
void platform_register_code(platform_keycode_t keycode);
void platform_unregister_code(platform_keycode_t keycode);
void platform_tap_code(platform_keycode_t keycode);
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

#ifdef PLATFORM_QMK
    // QMK-specific implementations will be in a separate .c file
#elif defined(PLATFORM_ZMK)
    // TODO: ZMK-specific implementations will be in a separate .c file
#endif
