#include "action.h"
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
    register_code(keycode);
}

void platform_unregister_keycode(platform_keycode_t keycode) {
    unregister_code(keycode);
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
void platform_layout_init_qmk_keymap(const uint16_t layers[][MATRIX_ROWS][MATRIX_COLS], uint8_t num_layers) {
    platform_layout_init_qmk_keymap_impl(layers, num_layers);
}
#elif defined(FRAMEWORK_ZMK)
void platform_layout_init_zmk_keymap(platform_keycode_t **layers, uint8_t num_layers, matrix_pos_t* key_map, uint16_t num_keys) {
    platform_layout_init_zmk_keymap_impl(layers, num_layers, key_map, num_keys);
}
#endif
#if defined(AGNOSTIC_USE_1D_ARRAY)
void platform_layout_init_1d_keymap(platform_keycode_t **layers, uint8_t num_layers, matrix_pos_t* key_map, uint16_t num_keys) {
    platform_layout_init_1d_keymap_impl(layers, num_layers, key_map, num_keys);
}
#elif defined(AGNOSTIC_USE_2D_ARRAY)
void platform_layout_init_2D_keymap(const uint16_t* layers, uint8_t num_layers, uint8_t rows, uint8_t cols) {
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
