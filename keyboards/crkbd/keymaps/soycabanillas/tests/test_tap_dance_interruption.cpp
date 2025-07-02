#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "mock_qmk.h"
#include "pipeline_tap_dance.h"
#include "commons.h"
}

class TapDanceInterruptionTest : public ::testing::Test {
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

// Test interruption during hold decision period
TEST_F(TapDanceInterruptionTest, InterruptionDuringHoldDecision) {
    g_mock_state.layer_on_calls = 0;
    g_mock_state.tap_code_calls = 0;

    // Start hold decision
    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Interrupt with another key before timeout
    advance_time(100);
    simulate_key_event(KC_Q, true);

    // Hold decision should be interrupted, no layer activation yet
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    // Release interrupting key
    simulate_key_event(KC_Q, false, 50);

    // Release original key
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should eventually trigger tap action due to interruption
    advance_time(100);
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test interruption during multiple tap sequence
TEST_F(TapDanceInterruptionTest, InterruptionDuringMultipleTapSequence) {
    g_mock_state.tap_code_calls = 0;

    // Start double tap sequence
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(50);

    // Second tap
    simulate_key_event(CKC_LAY_NUMBERS_R, true);

    // Interrupt during second tap
    advance_time(50);
    simulate_key_event(KC_R, true);
    simulate_key_event(KC_R, false, 50);

    // Release second tap
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Should handle interruption gracefully
    advance_time(200);

    // Should eventually trigger some action
    EXPECT_GE(g_mock_state.tap_code_calls, 1);
}

// Test that other keys don't interfere when tap-dance key is not active
TEST_F(TapDanceInterruptionTest, OtherKeysDoNotInterfereWhenInactive) {
    g_mock_state.tap_code_calls = 0;
    g_mock_state.layer_on_calls = 0;

    // Press and release other keys when tap-dance is not active
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);
    advance_time(100);

    simulate_key_event(KC_R, true);
    simulate_key_event(KC_R, false, 50);
    advance_time(100);

    // Should not affect tap-dance state
    EXPECT_EQ(g_mock_state.tap_code_calls, 0);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    // Now use tap-dance key normally
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    advance_time(250);

    // Should work normally
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test interruption by multiple keys
TEST_F(TapDanceInterruptionTest, InterruptionByMultipleKeys) {
    g_mock_state.layer_on_calls = 0;
    g_mock_state.tap_code_calls = 0;

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Multiple interrupting keys
    advance_time(50);
    simulate_key_event(KC_Q, true);
    advance_time(20);
    simulate_key_event(KC_R, true);
    advance_time(30);
    simulate_key_event(KC_Q, false);
    advance_time(20);
    simulate_key_event(KC_R, false);

    // Release original key
    advance_time(30);
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should handle multiple interruptions gracefully
    advance_time(200);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0); // Should not activate layer
    EXPECT_GE(g_mock_state.tap_code_calls, 1); // Should trigger tap action
}

// Test interruption after hold has already been activated
TEST_F(TapDanceInterruptionTest, InterruptionAfterHoldActivated) {
    g_mock_state.layer_on_calls = 0;
    g_mock_state.layer_off_calls = 0;

    // Activate hold
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    advance_time(250); // Trigger hold

    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    // Now press other keys while layer is active
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);
    advance_time(50);

    // Layer should remain active
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    // Release original key
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Layer should be deactivated
    EXPECT_EQ(g_mock_state.layer_off_calls, 1);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}

// Test rapid interruption patterns
TEST_F(TapDanceInterruptionTest, RapidInterruptionPatterns) {
    g_mock_state.tap_code_calls = 0;

    // Start tap-dance
    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Rapid alternating keys
    for (int i = 0; i < 5; i++) {
        advance_time(10);
        simulate_key_event(KC_Q, true);
        advance_time(10);
        simulate_key_event(KC_Q, false);
        advance_time(10);
        simulate_key_event(KC_R, true);
        advance_time(10);
        simulate_key_event(KC_R, false);
    }

    // Release original key
    advance_time(50);
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should handle rapid interruptions without crashing
    advance_time(200);

    // Should eventually resolve to some action
    EXPECT_GE(g_mock_state.tap_code_calls, 1);
}

// Test that interruption doesn't affect subsequent tap-dance usage
TEST_F(TapDanceInterruptionTest, InterruptionDoesNotAffectSubsequentUsage) {
    // First interrupted sequence
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    advance_time(100);
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    advance_time(200);

    // Reset call counters
    g_mock_state.tap_code_calls = 0;
    g_mock_state.layer_on_calls = 0;

    // Second normal sequence should work fine
    advance_time(100);
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    advance_time(250);

    // Should activate layer normally
    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    simulate_key_event(CKC_LAY_MOUSE_Q, false);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}
