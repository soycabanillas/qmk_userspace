#include "action.h"
#include "action_util.h"
#include "modules/soycabanillas/src/monkeyboard_deferred_callbacks.h"
#include "modules/soycabanillas/src/platform_interface.h"
#include "modules/soycabanillas/src/platform_types.h"
#include "modules/soycabanillas/src/platform_layout.h"


// Key operations

void platform_tap_keycode(platform_keycode_t keycode) {
    platform_register_keycode(keycode);
    platform_unregister_keycode(keycode);
}

void platform_register_keycode(platform_keycode_t keycode) {
    // if (MOD32_RGUI & keycode) c(MODIFIER_RIGHT_GUI);
    // if (MOD32_RALT & keycode) add_weak_mods(MODIFIER_RIGHT_ALT);
    // if (MOD32_RSFT & keycode) add_weak_mods(MODIFIER_RIGHT_SHIFT);
    // if (MOD32_RCTL & keycode) add_weak_mods(MODIFIER_RIGHT_CTRL);
    // if (MOD32_LGUI & keycode) add_weak_mods(MODIFIER_LEFT_GUI);
    // if (MOD32_LALT & keycode) add_weak_mods(MODIFIER_LEFT_ALT);
    // if (MOD32_LSFT & keycode) add_weak_mods(MODIFIER_LEFT_SHIFT);
    // if (MOD32_LCTL & keycode) add_weak_mods(MODIFIER_LEFT_CTRL);
    add_weak_mods((keycode & 0x0000FF00) >> 8); // Add modifier bits
    register_code(keycode);
}

void platform_unregister_keycode(platform_keycode_t keycode) {
    unregister_code(keycode);
    // if (MOD32_RGUI  & keycode) del_weak_mods(KC_RGUI);
    // if (MOD32_RALT  & keycode) del_weak_mods(KC_RALT);
    // if (MOD32_RSFT  & keycode) del_weak_mods(KC_RSFT);
    // if (MOD32_RCTL  & keycode) del_weak_mods(KC_RCTL);
    // if (MOD32_LGUI  & keycode) del_weak_mods(KC_LGUI);
    // if (MOD32_LALT  & keycode) del_weak_mods(KC_LALT);
    // if (MOD32_LSFT  & keycode) del_weak_mods(KC_LSFT);
    // if (MOD32_LCTL  & keycode) del_weak_mods(KC_LCTL);
}

void platform_add_key(platform_keycode_t keycode) {
    add_key(keycode);
}

void platform_del_key(platform_keycode_t keycode) {
    del_key(keycode);
}

void platform_send_report(void) {
    send_keyboard_report();
}

bool platform_compare_keyposition(platform_keypos_t key1, platform_keypos_t key2) {
    return (key1.row == key2.row && key1.col == key2.col);
}

// Layer operations

#if defined(FRAMEWORK_QMK)
void platform_layout_init_qmk_keymap(const platform_keycode_t layers[][MATRIX_ROWS][MATRIX_COLS], uint8_t num_layers) {
    platform_layout_init_qmk_keymap_impl(layers, num_layers);
}
#elif defined(FRAMEWORK_ZMK)
void platform_layout_init_zmk_keymap(platform_keycode_t **layers, uint8_t num_layers, uint16_t num_keys) {
    platform_layout_init_zmk_keymap_impl(layers, num_layers, num_keys);
}
#endif
#if defined(AGNOSTIC_USE_1D_ARRAY)
void platform_layout_init_custom_1D_keymap(void* layers, uint8_t num_layers, uint16_t numkeys, get_keycode_from_layer_def get_keycode_from_layer_fn) {
    platform_layout_init_custom_1D_keymap_impl(layers, num_layers, numkeys, get_keycode_from_layer_fn);
}

void platform_layout_init_1d_keymap(platform_keycode_t **layers, uint8_t num_layers, uint16_t num_keys) {
    platform_layout_init_1d_keymap_impl(layers, num_layers, num_keys);
}
#elif defined(AGNOSTIC_USE_2D_ARRAY)
void platform_layout_init_custom_2D_keymap(void* layers, uint8_t num_layers, uint8_t rows, uint8_t cols, get_keycode_from_layer_def get_keycode_from_layer_fn) {
    platform_layout_init_custom_2D_keymap_impl(layers, num_layers, rows, cols, get_keycode_from_layer_fn);
}

void platform_layout_init_2D_keymap(const platform_keycode_t* layers, uint8_t num_layers, uint8_t rows, uint8_t cols) {
    platform_layout_init_2d_keymap_impl(layers,  num_layers, rows, cols);
}
#endif

bool platform_layout_is_valid_layer(uint8_t layer) {
    return platform_layout_is_valid_layer_impl(layer);
}

void platform_layout_set_layer(uint8_t layer) {
    platform_layout_set_layer_impl(layer);
}

uint8_t platform_layout_get_current_layer(void) {
    return platform_layout_get_current_layer_impl();
}

platform_keycode_t platform_layout_get_keycode(platform_keypos_t position) {
    return platform_layout_get_keycode_impl(position);
}

platform_keycode_t platform_layout_get_keycode_from_layer(uint8_t layer, platform_keypos_t position) {
    return platform_layout_get_keycode_from_layer_impl(layer, position);
}

// Deferred execution

platform_deferred_token platform_defer_exec(uint32_t delay_ms, void (*callback)(void*), void* data) {
    return schedule_deferred_callback(delay_ms, callback, data);
}

bool platform_cancel_deferred_exec(platform_deferred_token token) {
    return cancel_deferred_callback(token);
}

// Timer

platform_time_t monkeyboard_get_time_32(void) {
    return timer_read32();
}
