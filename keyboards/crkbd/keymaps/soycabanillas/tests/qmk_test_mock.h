#ifndef QMK_TEST_MOCK_H
#define QMK_TEST_MOCK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
#include <vector>

// C++ part - Mock state structure
struct MockQMKState {
    uint16_t time;
    std::vector<uint16_t> register_code_calls;
    std::vector<uint16_t> unregister_code_calls;
    std::vector<uint16_t> tap_code_calls;
    std::vector<uint8_t> layer_on_calls;
    std::vector<uint8_t> layer_off_calls;
    std::vector<uint16_t> keys_pressed;

    uint16_t last_registered_code;
    uint16_t last_unregistered_code;
    uint16_t last_tapped_code;
    uint8_t last_layer_on;
    uint8_t last_layer_off;
    uint8_t mods;

    int clear_keyboard_calls;
    int tap_code_calls_count() const { return tap_code_calls.size(); }
    int layer_on_calls_count() const { return layer_on_calls.size(); }
    int layer_off_calls_count() const { return layer_off_calls.size(); }
};

// Global mock state
extern struct MockQMKState g_mock_state;

#endif

// MockQMKState is defined above in the C++ section

#ifdef __cplusplus
extern "C" {
#endif

// Platform function declarations for test environment
void platform_layer_on(uint8_t layer);
void platform_layer_off(uint8_t layer);
void platform_layer_select(uint8_t layer);
void platform_clear_keyboard(void);
void platform_tap_code_delay(uint16_t keycode, uint8_t delay);
uint32_t platform_defer_exec(uint32_t delay_ms, uint32_t (*callback)(uint32_t, void*), void* cb_arg);
void platform_cancel_deferred_exec(uint32_t token);
uint16_t platform_timer_read(void);
uint16_t platform_timer_elapsed(uint16_t start);

// QMK function mocks
void wait_ms(uint16_t ms);
uint16_t timer_read(void);
uint16_t timer_read16(void);
void clear_keyboard(void);
void register_code(uint16_t code);
void unregister_code(uint16_t code);
void tap_code(uint16_t code);
void tap_code16(uint16_t code);
void layer_on(uint8_t layer);
void layer_off(uint8_t layer);
bool layer_state_is(uint8_t layer);
bool is_layer_on(uint8_t layer);

// Test utility functions
void reset_mock_state(void);
void advance_time(uint16_t ms);
bool is_layer_active(uint8_t layer);

#ifdef __cplusplus
}

// Test helper functions (C++ only)
void simulate_double_tap(uint16_t keycode, uint16_t delay = 100);
void simulate_triple_tap(uint16_t keycode, uint16_t delay = 100);
void simulate_layer_activation(uint8_t layer);
void simulate_layer_deactivation(uint8_t layer);

#endif

#endif // QMK_TEST_MOCK_H
