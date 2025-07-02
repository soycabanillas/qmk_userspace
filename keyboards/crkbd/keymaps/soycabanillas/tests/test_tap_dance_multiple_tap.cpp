#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "mock_qmk.h"
#include "pipeline_tap_dance.h"
#include "commons.h"
}

class TapDanceMultipleTapTest : public ::testing::Test {
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

    void simulate_double_tap(uint16_t keycode, uint16_t interval = 100) {
        // First tap
        simulate_key_event(keycode, true);
        simulate_key_event(keycode, false, 50);

        // Small interval between taps
        advance_time(interval);

        // Second tap
        simulate_key_event(keycode, true);
        simulate_key_event(keycode, false, 50);
    }
};

// Test double tap on CKC_LAY_NUMBERS_R outputs shifted R
TEST_F(TapDanceMultipleTapTest, DoubleTapNumbersROutputsShiftedR) {
    g_mock_state.tap_code_calls = 0;

    simulate_double_tap(CKC_LAY_NUMBERS_R);

    // Advance time past tap timeout to trigger action
    advance_time(250);

    // Should output S(KC_R) which is shifted R
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, S(KC_R));
}

// Test double tap on CKC_EXCL outputs RALT(KC_1)
TEST_F(TapDanceMultipleTapTest, DoubleTapExclOutputsRAltOne) {
    g_mock_state.tap_code_calls = 0;

    simulate_double_tap(CKC_EXCL);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, RALT(KC_1));
}

// Test double tap on CKC_QUES outputs RALT(KC_SLSH)
TEST_F(TapDanceMultipleTapTest, DoubleTapQuesOutputsRAltSlash) {
    g_mock_state.tap_code_calls = 0;

    simulate_double_tap(CKC_QUES);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, RALT(KC_SLSH));
}

// Test that single tap still works after configuring double tap
TEST_F(TapDanceMultipleTapTest, SingleTapStillWorksWithDoubleTapConfigured) {
    g_mock_state.tap_code_calls = 0;

    // Single tap CKC_LAY_NUMBERS_R
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(250);

    // Should output regular KC_R, not shifted
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test that taps must be within timeout to count as multiple
TEST_F(TapDanceMultipleTapTest, TapsOutsideTimeoutCountAsSeparate) {
    g_mock_state.tap_code_calls = 0;

    // First tap
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Wait past timeout before second tap
    advance_time(300);

    // Should trigger first tap action (KC_R)
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);

    // Second tap after timeout
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(250);

    // Should trigger second tap action (another KC_R)
    EXPECT_EQ(g_mock_state.tap_code_calls, 2);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test triple tap beyond configured actions falls back to single tap
TEST_F(TapDanceMultipleTapTest, TripleTapFallsBackToSingleTap) {
    g_mock_state.tap_code_calls = 0;

    // Three quick taps
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(50);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(50);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(250);

    // Since there's no triple-tap action configured, should fall back to single tap
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test rapid taps that exceed configured repetitions
TEST_F(TapDanceMultipleTapTest, ExcessiveTapsHandledGracefully) {
    g_mock_state.tap_code_calls = 0;

    // Simulate 5 rapid taps on a key that only has single and double tap actions
    for (int i = 0; i < 5; i++) {
        simulate_key_event(CKC_EXCL, true);
        simulate_key_event(CKC_EXCL, false, 20);
        advance_time(30);
    }

    advance_time(250);

    // Should handle gracefully and trigger an action (likely single tap fallback)
    EXPECT_GE(g_mock_state.tap_code_calls, 1);
}
