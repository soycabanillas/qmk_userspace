#include "platform_qmk.h"
#include "qmk_test_mock.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>

// Global mock state
MockQMKState g_mock_state = {};

// Simplified deferred execution tracking - QMK style
struct DeferredExecution {
    uint32_t token;
    uint32_t trigger_time;
    uint32_t (*callback)(uint32_t, void*);
    void* cb_arg;
    bool active;
};

static DeferredExecution deferred_executions[5]; // Smaller array, more realistic
static uint32_t next_token = 1;

void reset_mock_state(void) {
    g_mock_state = {};
    next_token = 1;
    for (int i = 0; i < 5; i++) {
        deferred_executions[i].active = false;
        deferred_executions[i].token = 0;
        deferred_executions[i].callback = nullptr;
        deferred_executions[i].cb_arg = nullptr;
    }
}

void advance_time(uint16_t ms) {
    uint16_t old_time = g_mock_state.time;
    g_mock_state.time += ms;

    #ifdef DEBUG_VERBOSE
    printf("advance_time: %d -> %d (delta: %d)\n", old_time, g_mock_state.time, ms);
    #endif

    // Execute any deferred callbacks that should trigger
    for (int i = 0; i < 5; i++) {
        if (deferred_executions[i].active &&
            g_mock_state.time >= deferred_executions[i].trigger_time &&
            old_time < deferred_executions[i].trigger_time) {

            #ifdef DEBUG_VERBOSE
            printf("Executing deferred callback %d at time %d (trigger: %d)\n",
                   i, g_mock_state.time, deferred_executions[i].trigger_time);
            #endif

            if (deferred_executions[i].callback) {
                deferred_executions[i].callback(deferred_executions[i].trigger_time, deferred_executions[i].cb_arg);
            }
            deferred_executions[i].active = false;
        }
    }
}

uint16_t platform_timer_read(void) {
    return g_mock_state.time;
}

void platform_wait_ms(uint16_t ms) {
    advance_time(ms);
}

void platform_layer_select(uint8_t layer) {
    g_mock_state.layer_on_calls.push_back(layer);
    g_mock_state.last_layer_on = layer;
}

void platform_register_code(platform_keycode_t keycode) {
    g_mock_state.register_code_calls.push_back(keycode);
    g_mock_state.last_registered_code = keycode;
}

void platform_unregister_code(platform_keycode_t keycode) {
    g_mock_state.unregister_code_calls.push_back(keycode);
    g_mock_state.last_unregistered_code = keycode;
}


// QMK function implementations

uint32_t platform_defer_exec(uint32_t delay_ms, uint32_t (*callback)(uint32_t, void*), void* cb_arg) {
    // Find a free slot
    for (int i = 0; i < 5; i++) {
        if (!deferred_executions[i].active) {
            uint32_t token = next_token++;
            deferred_executions[i].token = token;
            deferred_executions[i].trigger_time = g_mock_state.time + delay_ms;
            deferred_executions[i].callback = callback;
            deferred_executions[i].cb_arg = cb_arg;
            deferred_executions[i].active = true;

            #ifdef DEBUG_VERBOSE
            printf("platform_defer_exec: token %d, delay %d, trigger_time %d, current_time %d\n",
                   token, delay_ms, deferred_executions[i].trigger_time, g_mock_state.time);
            #endif

            return token;
        }
    }
    return 0; // No free slots
}

void platform_cancel_deferred_exec(uint32_t token) {
    for (int i = 0; i < 5; i++) {
        if (deferred_executions[i].token == token && deferred_executions[i].active) {
            #ifdef DEBUG_VERBOSE
            printf("platform_cancel_deferred_exec: token %d cancelled\n", token);
            #endif
            deferred_executions[i].active = false;
            deferred_executions[i].token = 0;
            break;
        }
    }
}

// Test helper functions
void simulate_double_tap(uint16_t keycode, uint16_t delay) {
    platform_register_code(keycode);
    platform_wait_ms(delay);
    platform_unregister_code(keycode);
    platform_wait_ms(delay);
    platform_register_code(keycode);
    platform_wait_ms(delay);
    platform_unregister_code(keycode);
    platform_wait_ms(delay);
}

void simulate_triple_tap(uint16_t keycode, uint16_t delay) {
    platform_register_code(keycode);
    platform_wait_ms(delay);
    platform_unregister_code(keycode);
    platform_wait_ms(delay);
    platform_register_code(keycode);
    platform_wait_ms(delay);
    platform_unregister_code(keycode);
    platform_wait_ms(delay);
    platform_register_code(keycode);
    platform_wait_ms(delay);
    platform_unregister_code(keycode);
    platform_wait_ms(delay);
}
