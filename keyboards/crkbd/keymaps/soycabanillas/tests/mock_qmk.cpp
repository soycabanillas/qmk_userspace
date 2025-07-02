#include "mock_qmk.h"
#include <cstdarg>
#include <cstdio>
#include <map>
#include <vector>

extern "C" {

// Global mock state
MockQMKState g_mock_state = {0};

// Deferred execution tracking
struct DeferredCallback {
    uint32_t trigger_time;
    uint32_t (*callback)(uint32_t, void*);
    void* arg;
    bool active;
};

static std::map<deferred_token, DeferredCallback> g_deferred_callbacks;

void reset_mock_state(void) {
    memset(&g_mock_state, 0, sizeof(g_mock_state));
    g_deferred_callbacks.clear();
}

// Timer functions
uint16_t timer_read(void) {
    return g_mock_state.time;
}

uint16_t timer_elapsed(uint16_t last) {
    return g_mock_state.time - last;
}

void wait_ms(uint16_t ms) {
    // In tests, we don't actually wait
}

void advance_time(uint16_t ms) {
    uint16_t old_time = g_mock_state.time;
    g_mock_state.time += ms;

    // Check if any deferred callbacks should trigger
    for (auto& pair : g_deferred_callbacks) {
        DeferredCallback& cb = pair.second;
        if (cb.active && cb.trigger_time <= g_mock_state.time && cb.trigger_time > old_time) {
            uint32_t result = cb.callback(cb.trigger_time, cb.arg);
            if (result == 0) {
                cb.active = false; // Callback indicates it should be removed
            } else {
                cb.trigger_time = g_mock_state.time + result; // Reschedule
            }
        }
    }
}

// Layer functions
void layer_on(uint8_t layer) {
    g_mock_state.layers[layer] = true;
    g_mock_state.layer_on_calls++;
    g_mock_state.last_layer_on = layer;
}

void layer_off(uint8_t layer) {
    g_mock_state.layers[layer] = false;
    g_mock_state.layer_off_calls++;
    g_mock_state.last_layer_off = layer;
}

void clear_keyboard(void) {
    memset(g_mock_state.keys_pressed, 0, sizeof(g_mock_state.keys_pressed));
    g_mock_state.clear_keyboard_calls++;
}

uint8_t get_layer_topdown(keypos_t key) {
    // Return the highest active layer
    for (int i = 15; i >= 0; i--) {
        if (g_mock_state.layers[i]) {
            return i;
        }
    }
    return 0; // Default layer
}

uint8_t layer_switch_get_layer(keypos_t key) {
    return get_layer_topdown(key);
}

uint16_t keymap_key_to_keycode(uint8_t layer, keypos_t key) {
    // Simple mock - return a predictable keycode based on position
    return (layer << 8) | (key.row << 4) | key.col;
}

// Key registration functions
void register_code(uint8_t code) {
    g_mock_state.keys_pressed[code] = true;
    g_mock_state.register_code_calls++;
    g_mock_state.last_registered_code = code;
}

void unregister_code(uint8_t code) {
    g_mock_state.keys_pressed[code] = false;
    g_mock_state.unregister_code_calls++;
    g_mock_state.last_unregistered_code = code;
}

void register_code16(uint16_t code) {
    register_code(code & 0xFF);
}

void unregister_code16(uint16_t code) {
    unregister_code(code & 0xFF);
}

void tap_code16_delay(uint16_t code, uint8_t delay) {
    register_code16(code);
    unregister_code16(code);
    g_mock_state.tap_code_calls++;
    g_mock_state.last_tapped_code = code;
}

void register_mods(uint8_t mods) {
    g_mock_state.mods |= mods;
}

void unregister_mods(uint8_t mods) {
    g_mock_state.mods &= ~mods;
}

void set_mods(uint8_t mods) {
    g_mock_state.mods = mods;
}

uint8_t get_mods(void) {
    return g_mock_state.mods;
}

uint8_t get_oneshot_mods(void) {
    return g_mock_state.oneshot_mods;
}

// Deferred execution
deferred_token defer_exec(uint32_t delay_ms, uint32_t (*callback)(uint32_t, void*), void* cb_arg) {
    deferred_token token = ++g_mock_state.next_token;

    DeferredCallback cb;
    cb.trigger_time = g_mock_state.time + delay_ms;
    cb.callback = callback;
    cb.arg = cb_arg;
    cb.active = true;

    g_deferred_callbacks[token] = cb;

    return token;
}

bool cancel_deferred_exec(deferred_token token) {
    auto it = g_deferred_callbacks.find(token);
    if (it != g_deferred_callbacks.end()) {
        it->second.active = false;
        return true;
    }
    return false;
}

// Debug functions
void uprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

// Test helper functions
bool is_layer_active(uint8_t layer) {
    return g_mock_state.layers[layer];
}

bool is_key_pressed(uint16_t keycode) {
    return g_mock_state.keys_pressed[keycode & 0xFF];
}

} // extern "C"
