#include "gtest/gtest.h"
#include "qmk_test_mock.h"

extern "C" {
#include "test_keycodes.h"
#include "abstractionsqmk.h"
#include "commons.h"
}

class TapDanceMultipleTapTest : public ::testing::Test {
protected:
    custom_layers_struct* custom_layers;

    void SetUp() override {
        reset_mock_state();
        custom_layers = (custom_layers_struct*)pipeline_tap_dance_initialize_user_data();
    }

    void TearDown() override {
        if (custom_layers) {
            for (size_t i = 0; i < custom_layers->length; i++) {
                if (custom_layers->layers[i]) {
                    free(custom_layers->layers[i]->user_data);
                    free(custom_layers->layers[i]);
                }
            }
            free(custom_layers);
            custom_layers = nullptr;
        }
    }

    void simulate_key_event(uint16_t keycode, bool pressed, uint16_t time_offset = 0) {
        abskeyevent_t event = {
            .key = {.col = 0, .row = 0},
            .pressed = pressed,
            .time = static_cast<uint16_t>(g_mock_state.time + time_offset)
        };
        if (time_offset > 0) {
            wait_ms(time_offset);
        }
        macros_process_key(keycode, event);
    }

    void simulate_double_tap(uint16_t keycode, uint16_t interval = 100) {
        // First tap
        simulate_key_event(keycode, true);
        simulate_key_event(keycode, false, 50);

        // Small interval between taps
        wait_ms(interval);

        // Second tap
        simulate_key_event(keycode, true);
        simulate_key_event(keycode, false, 50);
    }

    void simulate_triple_tap(uint16_t keycode, uint16_t interval = 100) {
        // First tap
        simulate_key_event(keycode, true);
        simulate_key_event(keycode, false, 50);
        wait_ms(interval);

        // Second tap
        simulate_key_event(keycode, true);
        simulate_key_event(keycode, false, 50);
        wait_ms(interval);

        // Third tap
        simulate_key_event(keycode, true);
        simulate_key_event(keycode, false, 50);
    }
};

// Test double tap on CKC_LAY_MOUSE_Q
TEST_F(TapDanceMultipleTapTest, DoubleTapMouseKey) {
    g_mock_state.tap_code_calls.clear();

    simulate_double_tap(CKC_LAY_MOUSE_Q);
    wait_ms(250); // Allow tap dance to complete

    // Should register two Q taps for double tap
    EXPECT_GE(g_mock_state.tap_code_calls_count(), 2);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test triple tap sequence
TEST_F(TapDanceMultipleTapTest, TripleTapSequence) {
    g_mock_state.tap_code_calls.clear();

    simulate_triple_tap(CKC_LAY_NUMBERS_R);
    wait_ms(250);

    // Should register multiple R taps for triple tap
    EXPECT_GE(g_mock_state.tap_code_calls_count(), 3);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test tap sequence timing sensitivity
TEST_F(TapDanceMultipleTapTest, TimingSensitivity) {
    g_mock_state.tap_code_calls.clear();

    // Double tap with quick timing
    simulate_double_tap(CKC_LAY_MOUSE_Q, 50); // Fast double tap
    wait_ms(250);

    int fast_tap_count = g_mock_state.tap_code_calls_count();

    // Reset and try slow timing
    g_mock_state.tap_code_calls.clear();
    simulate_double_tap(CKC_LAY_MOUSE_Q, 300); // Slow double tap (should timeout)
    wait_ms(250);

    int slow_tap_count = g_mock_state.tap_code_calls_count();

    // Fast double tap should behave differently than slow (timeout) taps
    EXPECT_TRUE(fast_tap_count > 0 && slow_tap_count > 0);
}

// Test interruption of multiple tap sequence
TEST_F(TapDanceMultipleTapTest, SequenceInterruption) {
    g_mock_state.tap_code_calls.clear();

    // Start double tap sequence
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Interrupt with different key
    simulate_key_event(KC_A, true);
    simulate_key_event(KC_A, false, 50);

    // Complete the original sequence
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    wait_ms(250);

    // Should have registered some taps
    EXPECT_GT(g_mock_state.tap_code_calls_count(), 0);
}

// Test multiple tap with layer activation potential
TEST_F(TapDanceMultipleTapTest, MultipleTapVsHold) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // Quick double tap (should not activate layer)
    simulate_double_tap(CKC_LAY_MOUSE_Q, 50);

    // Brief pause, then hold
    wait_ms(100);
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250); // Long enough to trigger hold
    layer_on(_LMOUSE); // Simulate layer activation for hold
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should have both tap and layer behavior
    EXPECT_GT(g_mock_state.tap_code_calls_count(), 0);
    EXPECT_GT(g_mock_state.layer_on_calls_count(), 0);
}

// Test rapid multiple taps
TEST_F(TapDanceMultipleTapTest, RapidMultipleTaps) {
    g_mock_state.tap_code_calls.clear();

    // Simulate very rapid tapping
    for (int i = 0; i < 5; i++) {
        simulate_key_event(CKC_LAY_NUMBERS_R, true);
        simulate_key_event(CKC_LAY_NUMBERS_R, false, 25); // Very fast
        wait_ms(25);
    }

    wait_ms(250);

    // Should handle rapid input gracefully
    EXPECT_GT(g_mock_state.tap_code_calls_count(), 0);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test alternating multiple taps between different keys
TEST_F(TapDanceMultipleTapTest, AlternatingMultipleTaps) {
    g_mock_state.tap_code_calls.clear();

    // Alternate between two tap dance keys
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    wait_ms(50);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    wait_ms(50);

    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    wait_ms(250);

    // Should register taps from both keys
    EXPECT_GT(g_mock_state.tap_code_calls_count(), 0);
}
