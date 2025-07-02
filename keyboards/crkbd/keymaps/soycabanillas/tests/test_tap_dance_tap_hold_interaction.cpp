#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "mock_qmk.h"
#include "pipeline_tap_dance.h"
#include "commons.h"
}

class TapDanceTapHoldInteractionTest : public ::testing::Test {
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

// Test that hold action overrides tap when held long enough
TEST_F(TapDanceTapHoldInteractionTest, HoldOverridesTapWhenHeldLongEnough) {
    g_mock_state.tap_code_calls = 0;
    g_mock_state.layer_on_calls = 0;

    // Press key
    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Hold past timeout threshold
    advance_time(250);

    // Should activate layer, not tap key
    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOUSE);
    EXPECT_EQ(g_mock_state.tap_code_calls, 0);

    // Release
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should deactivate layer
    EXPECT_EQ(g_mock_state.layer_off_calls, 1);
}

// Test that quick release triggers tap action instead of hold
TEST_F(TapDanceTapHoldInteractionTest, QuickReleaseTriggersTapNotHold) {
    g_mock_state.tap_code_calls = 0;
    g_mock_state.layer_on_calls = 0;

    // Press and quickly release
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should not activate layer during decision period
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    // Advance past timeout to trigger tap action
    advance_time(250);

    // Should tap key, not activate layer
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);
}

// Test interruption by other key during hold decision
TEST_F(TapDanceTapHoldInteractionTest, InterruptionDuringHoldDecision) {
    g_mock_state.tap_code_calls = 0;
    g_mock_state.layer_on_calls = 0;

    // Press tap-dance key
    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Press another key before timeout (interruption)
    advance_time(100);
    simulate_key_event(KC_Q, true);

    // Original behavior: should still be in decision mode
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    // Release the interrupting key
    simulate_key_event(KC_Q, false, 50);

    // Release the tap-dance key
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should trigger tap action due to interruption
    advance_time(100);
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test CKC_LAY_MOVEMENT_F complex behavior (tap F, hold for movement, double-hold for mouse)
TEST_F(TapDanceTapHoldInteractionTest, MovementFComplexBehavior) {
    // Test single tap outputs F
    g_mock_state.tap_code_calls = 0;
    simulate_key_event(CKC_LAY_MOVEMENT_F, true);
    simulate_key_event(CKC_LAY_MOVEMENT_F, false, 50);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_F);

    // Reset for hold test
    reset_mock_state();
    custom_layers = (custom_layers_struct*)pipeline_tap_dance_initialize_user_data();

    // Test hold activates movement layer
    g_mock_state.layer_on_calls = 0;
    simulate_key_event(CKC_LAY_MOVEMENT_F, true);
    advance_time(250);

    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOVEMENT);
}

// Test that only keys with both tap and hold actions exhibit this behavior
TEST_F(TapDanceTapHoldInteractionTest, KeysWithOnlyTapActionsIgnoreHold) {
    g_mock_state.tap_code_calls = 0;
    g_mock_state.layer_on_calls = 0;

    // CKC_EXCL only has tap actions, no hold actions
    simulate_key_event(CKC_EXCL, true);
    advance_time(300); // Well past timeout

    // Should not activate any layer
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    simulate_key_event(CKC_EXCL, false);
    advance_time(100);

    // Should eventually tap the key
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_EXLM);
}

// Test sequence: tap -> wait -> tap should trigger two separate tap actions
TEST_F(TapDanceTapHoldInteractionTest, SeparatedTapsAreIndependent) {
    g_mock_state.tap_code_calls = 0;

    // First tap
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    advance_time(250); // Trigger first tap

    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);

    // Second tap after timeout
    advance_time(100);
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 2);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}
