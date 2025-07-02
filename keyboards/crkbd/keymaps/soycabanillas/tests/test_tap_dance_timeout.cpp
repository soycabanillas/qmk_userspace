#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "mock_qmk.h"
#include "pipeline_tap_dance.h"
#include "commons.h"
}

class TapDanceTimeoutTest : public ::testing::Test {
protected:
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
            advance_time(time_offset);
        }

        macros_process_key(keycode, event);
    }
};

// Test that hold timeout is respected (200ms as defined in g_tap_timeout)
TEST_F(TapDanceTimeoutTest, HoldTimeoutRespected) {
    g_mock_state.layer_on_calls = 0;

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Just before timeout (199ms)
    advance_time(199);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    // Just after timeout (1ms more = 200ms total)
    advance_time(1);
    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));
}

// Test tap timeout for multiple taps
TEST_F(TapDanceTimeoutTest, TapTimeoutForMultipleTaps) {
    g_mock_state.tap_code_calls = 0;

    // First tap
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Wait just under timeout
    advance_time(149); // Total 199ms
    EXPECT_EQ(g_mock_state.tap_code_calls, 0);

    // Second tap within timeout
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Now wait for timeout to trigger double-tap action
    advance_time(200);

    // Should trigger double-tap action (S(KC_R))
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, S(KC_R));
}

// Test that taps outside timeout window are treated separately
TEST_F(TapDanceTimeoutTest, TapsOutsideTimeoutTreatedSeparately) {
    g_mock_state.tap_code_calls = 0;

    // First tap
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Wait past timeout
    advance_time(250);

    // Should trigger single tap action
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);

    // Second tap after timeout
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(250);

    // Should trigger another single tap action
    EXPECT_EQ(g_mock_state.tap_code_calls, 2);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test early timeout cancellation when key is released quickly
TEST_F(TapDanceTimeoutTest, EarlyTimeoutCancellationOnQuickRelease) {
    g_mock_state.layer_on_calls = 0;
    g_mock_state.tap_code_calls = 0;

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Release before timeout
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 100);

    // Should not activate layer
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    // Wait past original timeout to trigger tap
    advance_time(150);

    // Should trigger tap action instead
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);
}

// Test precise timing of deferred execution
TEST_F(TapDanceTimeoutTest, PreciseTimingOfDeferredExecution) {
    g_mock_state.layer_on_calls = 0;

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Check that exactly at timeout threshold, action triggers
    advance_time(200); // Exactly at g_tap_timeout

    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));
}

// Test timeout behavior with multiple rapid taps
TEST_F(TapDanceTimeoutTest, TimeoutWithMultipleRapidTaps) {
    g_mock_state.tap_code_calls = 0;

    // Three rapid taps
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);
    advance_time(50);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);
    advance_time(50);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);

    // Wait for timeout
    advance_time(200);

    // Should handle gracefully (likely fallback to single tap)
    EXPECT_GE(g_mock_state.tap_code_calls, 1);
}

// Test timeout cancellation when interrupted by another key
TEST_F(TapDanceTimeoutTest, TimeoutCancellationOnInterruption) {
    g_mock_state.layer_on_calls = 0;
    g_mock_state.tap_code_calls = 0;

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Interrupt with another key before timeout
    advance_time(100);
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);

    // Release original key
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should not activate layer due to interruption
    advance_time(100);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    // Should eventually trigger tap action
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
}
