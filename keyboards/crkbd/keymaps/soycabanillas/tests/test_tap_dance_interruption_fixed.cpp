#include "gtest/gtest.h"
#include "qmk_test_mock.h"
#include "pipeline_tap_dance_initializer.h"

extern "C" {
#include "test_keycodes.h"
#include "abstractionsqmk.h"
#include "commons.h"
}

class TapDanceInterruptionTest : public ::testing::Test {
protected:

    custom_layers_struct* test_layers;

    void SetUp() override {
        reset_mock_state();
        // Create minimal test configuration for the tap dance keys
        size_t nelements = 4;
        test_layers = (custom_layers_struct*)malloc(sizeof *test_layers + nelements * sizeof *test_layers->layers);
        test_layers->length = nelements;

        // CKC_LAY_MOUSE_Q -> KC_Q
        custom_action_custom_behaviour* mouse_q_action = createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_Q, _LMOUSE);
        custom_action_custom_behaviour* mouse_q_actions[] = { mouse_q_action };
        test_layers->layers[0] = createbehaviour(CKC_LAY_MOUSE_Q, mouse_q_actions, 1);

        // CKC_LAY_NUMBERS_R -> KC_R
        custom_action_custom_behaviour* numbers_r_action = createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_R, _LNUMBERS);
        custom_action_custom_behaviour* numbers_r_actions[] = { numbers_r_action };
        test_layers->layers[1] = createbehaviour(CKC_LAY_NUMBERS_R, numbers_r_actions, 1);

        // CKC_LSHIFT_EXCLAMATION_MARK -> KC_EXLM
        custom_action_custom_behaviour* exlm_action = createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_EXLM, 0);
        custom_action_custom_behaviour* exlm_actions[] = { exlm_action };
        test_layers->layers[2] = createbehaviour(CKC_LSHIFT_EXCLAMATION_MARK, exlm_actions, 1);

        // CKC_RSHIFT_QUESTION_MARK -> KC_QUES
        custom_action_custom_behaviour* ques_action = createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_QUES, 0);
        custom_action_custom_behaviour* ques_actions[] = { ques_action };
        test_layers->layers[3] = createbehaviour(CKC_RSHIFT_QUESTION_MARK, ques_actions, 1);
    }

    void TearDown() override {
        if (test_layers) {
            for (size_t i = 0; i < test_layers->length; i++) {
                if (test_layers->layers[i]) {
                    free(test_layers->layers[i]->user_data);
                    free(test_layers->layers[i]);
                }
            }
            free(test_layers);
            test_layers = nullptr;
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
        macros_process_key(keycode, event, test_layers);
    }
};

// Test interruption during hold decision period
TEST_F(TapDanceInterruptionTest, InterruptionDuringHoldDecision) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.tap_code_calls.clear();

    // Start hold decision
    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Interrupt with another key before timeout
    wait_ms(100);
    simulate_key_event(KC_Q, true);

    // Hold decision should be interrupted, no layer activation yet
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 0);

    // Release interrupting key
    simulate_key_event(KC_Q, false, 50);

    // Release original key
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should eventually trigger tap action due to interruption
    wait_ms(100);
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test interruption during multiple tap sequence
TEST_F(TapDanceInterruptionTest, InterruptionDuringMultipleTapSequence) {
    g_mock_state.tap_code_calls.clear();

    // Start double tap sequence
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    wait_ms(50);

    // Second tap
    simulate_key_event(CKC_LAY_NUMBERS_R, true);

    // Interrupt during second tap
    wait_ms(50);
    simulate_key_event(KC_R, true);
    simulate_key_event(KC_R, false, 50);

    // Release second tap
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Should handle interruption gracefully
    wait_ms(200);

    // Should eventually trigger some action
    EXPECT_GE(g_mock_state.tap_code_calls.size(), 1);
}

// Test that other keys don't interfere when tap-dance key is not active
TEST_F(TapDanceInterruptionTest, OtherKeysDoNotInterfereWhenInactive) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // Press and release other keys when tap-dance is not active
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);
    wait_ms(100);

    simulate_key_event(KC_R, true);
    simulate_key_event(KC_R, false, 50);
    wait_ms(100);

    // Should not affect tap-dance state
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), 0);
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 0);

    // Now use tap-dance key normally
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    wait_ms(250);

    // Should work normally
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test interruption by multiple keys
TEST_F(TapDanceInterruptionTest, InterruptionByMultipleKeys) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.tap_code_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Multiple interrupting keys
    wait_ms(50);
    simulate_key_event(KC_Q, true);
    wait_ms(20);
    simulate_key_event(KC_R, true);
    wait_ms(30);
    simulate_key_event(KC_Q, false);
    wait_ms(20);
    simulate_key_event(KC_R, false);

    // Release original key
    wait_ms(30);
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should handle multiple interruptions gracefully
    wait_ms(200);
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 0); // Should not activate layer
    EXPECT_GE(g_mock_state.tap_code_calls.size(), 1); // Should trigger tap action
}

// Test interruption after hold has already been activated
TEST_F(TapDanceInterruptionTest, InterruptionAfterHoldActivated) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.layer_off_calls.clear();

    // Activate hold
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250); // Trigger hold

    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    // Now press other keys while layer is active
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);
    wait_ms(50);

    // Layer should remain active
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    // Release original key
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Layer should be deactivated
    EXPECT_EQ(g_mock_state.layer_off_calls.size(), 1);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}

// Test rapid interruption patterns
TEST_F(TapDanceInterruptionTest, RapidInterruptionPatterns) {
    g_mock_state.tap_code_calls.clear();

    // Start tap-dance
    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Rapid alternating keys
    for (int i = 0; i < 5; i++) {
        wait_ms(10);
        simulate_key_event(KC_Q, true);
        wait_ms(10);
        simulate_key_event(KC_Q, false);
        wait_ms(10);
        simulate_key_event(KC_R, true);
        wait_ms(10);
        simulate_key_event(KC_R, false);
    }

    // Release original key
    wait_ms(50);
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should handle rapid interruptions without crashing
    wait_ms(200);

    // Should eventually resolve to some action
    EXPECT_GE(g_mock_state.tap_code_calls.size(), 1);
}

// Test that interruption doesn't affect subsequent tap-dance usage
TEST_F(TapDanceInterruptionTest, InterruptionDoesNotAffectSubsequentUsage) {
    // First interrupted sequence
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(100);
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    wait_ms(200);

    // Reset call counters
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // Second normal sequence should work fine
    wait_ms(100);
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250);

    // Should activate layer normally
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    simulate_key_event(CKC_LAY_MOUSE_Q, false);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}
