#include "platform_qmk.h"

#ifdef PLATFORM_QMK
#include "_wait.h"
#include "action.h"
#include "action_layer.h"
#include "action_util.h"
#include "deferred_exec.h"
#include "quantum.h"
#include "timer.h"
#elif defined(PLATFORM_TEST)
// For test environment, we rely on the mock definitions
// All the necessary function definitions will be mocked
#include <stdint.h>
#include <stdbool.h>
#endif

#ifdef PLATFORM_QMK
// Timer implementations
platform_time_t platform_timer_read(void) {
    return timer_read();
}

platform_time_t platform_timer_elapsed(platform_time_t start) {
    return timer_elapsed(start);
}

// Key function implementations
void platform_register_code(platform_keycode_t keycode) {
    register_code(keycode);
}

void platform_unregister_code(platform_keycode_t keycode) {
    unregister_code(keycode);
}

void platform_tap_code(platform_keycode_t keycode) {
    tap_code(keycode);
}

void platform_tap_code_delay(platform_keycode_t keycode, uint8_t delay) {
    tap_code16_delay(keycode, delay);
}

void platform_add_key(platform_keycode_t keycode) {
    add_key(keycode);
}

void platform_del_key(platform_keycode_t keycode) {
    del_key(keycode);
}

void platform_send_keyboard_report(void) {
    send_keyboard_report();
}

void platform_clear_keyboard(void) {
    clear_keyboard();
}

bool platform_is_modifier(platform_keycode_t keycode) {
    return IS_MODIFIER_KEYCODE(keycode);
}

platform_keycode_t platform_get_mods(void) {
    return get_mods();
}

void platform_set_mods(platform_keycode_t mods) {
    set_mods(mods);
}

void platform_add_mods(platform_keycode_t mods) {
    add_mods(mods);
}

void platform_del_mods(platform_keycode_t mods) {
    del_mods(mods);
}

void platform_wait_ms(uint16_t ms) {
    wait_ms(ms);
}

// Layer management implementations
void platform_layer_on(uint8_t layer) {
    layer_on(layer);
}

void platform_layer_off(uint8_t layer) {
    layer_off(layer);
}

// Deferred execution implementations
platform_deferred_token platform_defer_exec(uint32_t delay_ms, platform_deferred_callback callback, void *cb_arg) {
    return defer_exec(delay_ms, callback, cb_arg);
}

void platform_cancel_deferred_exec(platform_deferred_token token) {
    cancel_deferred_exec(token);
}

#endif // PLATFORM_QMK

