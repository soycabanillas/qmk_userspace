#include "qmk_test_mock.h"
#include <cstring>
#include <cstdlib>

// Global mock state
MockQMKState g_mock_state = {};

// Global custom_layers variable (shared between test and implementation)
custom_layers_struct *custom_layers = nullptr;

void reset_mock_state(void) {
    g_mock_state = {};
}

void advance_time(uint16_t ms) {
    g_mock_state.time += ms;
}

bool is_layer_active(uint8_t layer) {
    return is_layer_on(layer);
}

// QMK function implementations
void wait_ms(uint16_t ms) {
    g_mock_state.time += ms;
}

uint16_t timer_read(void) {
    return g_mock_state.time;
}

uint16_t timer_read16(void) {
    return g_mock_state.time;
}

void clear_keyboard(void) {
    g_mock_state.clear_keyboard_calls++;
    g_mock_state.keys_pressed.clear();
    g_mock_state.mods = 0;
}

void register_code(uint16_t code) {
    g_mock_state.register_code_calls.push_back(code);
    g_mock_state.last_registered_code = code;
}

void unregister_code(uint16_t code) {
    g_mock_state.unregister_code_calls.push_back(code);
    g_mock_state.last_unregistered_code = code;
}

void tap_code(uint16_t code) {
    g_mock_state.tap_code_calls.push_back(code);
    g_mock_state.last_tapped_code = code;
}

void tap_code16(uint16_t code) {
    g_mock_state.tap_code_calls.push_back(code);
    g_mock_state.last_tapped_code = code;
}

void layer_on(uint8_t layer) {
    g_mock_state.layer_on_calls.push_back(layer);
    g_mock_state.last_layer_on = layer;
}

void layer_off(uint8_t layer) {
    g_mock_state.layer_off_calls.push_back(layer);
    g_mock_state.last_layer_off = layer;
}

bool layer_state_is(uint8_t layer) {
    // Check if the layer is currently active (not deactivated)
    if (g_mock_state.layer_off_calls.empty()) {
        return g_mock_state.last_layer_on == layer;
    }
    // If there have been layer_off calls, check if this layer was turned off after being turned on
    return g_mock_state.last_layer_on == layer && g_mock_state.last_layer_off != layer;
}

bool is_layer_on(uint8_t layer) {
    return layer_state_is(layer);
}

void platform_layer_on(uint8_t layer) {
    layer_on(layer);
}

void platform_layer_off(uint8_t layer) {
    layer_off(layer);
}

void platform_clear_keyboard(void) {
    g_mock_state.register_code_calls.clear();
    g_mock_state.keys_pressed.clear();
}

void platform_tap_code_delay(uint16_t keycode, uint8_t delay) {
    (void)delay; // Unused in mock
    tap_code(keycode);
}

uint32_t platform_defer_exec(uint32_t delay_ms, uint32_t (*callback)(uint32_t, void*), void* cb_arg) {
    // For testing, we immediately execute the callback
    (void)delay_ms;
    if (callback) {
        callback(0, cb_arg);
    }
    return 1; // Return a fake token
}

void platform_cancel_deferred_exec(uint32_t token) {
    (void)token; // Do nothing in the mock
}
