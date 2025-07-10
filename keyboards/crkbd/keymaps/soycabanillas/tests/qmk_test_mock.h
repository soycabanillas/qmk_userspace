#ifndef QMK_TEST_MOCK_H
#define QMK_TEST_MOCK_H

#include <stdbool.h>
#include <stddef.h>
#include <cstdint>

#ifdef __cplusplus
#include <vector>

// C++ part - Mock state structure
struct MockQMKState {
    uint16_t time;
    std::vector<uint16_t> register_code_calls;
    std::vector<uint16_t> unregister_code_calls;
    std::vector<uint16_t> tap_code_calls;
    std::vector<uint8_t> layer_on_calls;
    std::vector<uint16_t> keys_pressed;

    uint16_t last_registered_code;
    uint16_t last_unregistered_code;
    uint16_t last_tapped_code;
    uint8_t last_layer_on;
    uint8_t mods;

    int tap_code_calls_count() const { return tap_code_calls.size(); }
    int layer_on_calls_count() const { return layer_on_calls.size(); }
};

// Global mock state
extern struct MockQMKState g_mock_state;

#endif

// MockQMKState is defined above in the C++ section

#ifdef __cplusplus
void reset_mock_state(void);
void advance_time(uint16_t ms);

void simulate_double_tap(uint16_t keycode, uint16_t delay = 100);
void simulate_triple_tap(uint16_t keycode, uint16_t delay = 100);
#endif

#endif // QMK_TEST_MOCK_H
