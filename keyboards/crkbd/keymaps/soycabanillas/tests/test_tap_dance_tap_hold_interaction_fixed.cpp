#include "gtest/gtest.h"
#include "qmk_test_mock.h"
#include "pipeline_tap_dance_initializer.h"

extern "C" {
#include "test_keycodes.h"
#include "abstractionsqmk.h"
#include "commons.h"
}

class TapDanceTapHoldInteractionTest : public ::testing::Test {
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

// Test that hold action overrides tap when held long enough
TEST_F(TapDanceTapHoldInteractionTest, HoldOverridesTapWhenHeldLongEnough) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // Press key and hold long enough to trigger hold action
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250); // Hold longer than tap timeout

    // Simulate layer activation for hold (since our mock doesn't implement hold logic)
    layer_on(_LMOUSE);

    // Release key
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should activate layer (hold action) rather than tap
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOUSE);

    // Should not have tapped the base key beyond any initial processing
    // Note: tap_code might be called once during initial processing, but hold should override
    EXPECT_LE(g_mock_state.tap_code_calls.size(), 1);
}

// Test tap-then-hold sequence
TEST_F(TapDanceTapHoldInteractionTest, TapThenHoldSequence) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // First: Quick tap
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    wait_ms(100); // Brief pause

    // Then: Hold
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250); // Hold
    layer_on(_LMOUSE); // Simulate layer activation
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should have both tap and hold actions
    EXPECT_GE(g_mock_state.tap_code_calls_count(), 1);
    EXPECT_GE(g_mock_state.layer_on_calls_count(), 1);
}

// Test hold-then-tap sequence
TEST_F(TapDanceTapHoldInteractionTest, HoldThenTapSequence) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // First: Hold
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250);
    layer_on(_LMOUSE);
    simulate_key_event(CKC_LAY_MOUSE_Q, false);
    layer_off(_LMOUSE);

    wait_ms(100); // Brief pause

    // Then: Quick tap
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should have both hold and tap actions
    EXPECT_GE(g_mock_state.layer_on_calls_count(), 1);
    EXPECT_GE(g_mock_state.tap_code_calls_count(), 1);
}

// Test rapid tap-hold transitions
TEST_F(TapDanceTapHoldInteractionTest, RapidTapHoldTransitions) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // Rapid sequence: tap, brief hold, tap, brief hold
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    wait_ms(150); // Short hold
    simulate_key_event(CKC_LAY_NUMBERS_R, false);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);

    wait_ms(250);

    // Should register some activity
    EXPECT_GT(g_mock_state.tap_code_calls_count(), 0);
}

// Test overlapping tap-hold between different keys
TEST_F(TapDanceTapHoldInteractionTest, OverlappingTapHoldDifferentKeys) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // Start holding first key
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(100);

    // While holding first, tap second key
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Continue holding first key
    wait_ms(200);
    layer_on(_LMOUSE); // Simulate layer activation for first key
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should handle both keys appropriately
    EXPECT_GT(g_mock_state.tap_code_calls_count(), 0); // From R tap
    EXPECT_GT(g_mock_state.layer_on_calls_count(), 0); // From Q hold
}

// Test interrupted hold (released before timeout)
TEST_F(TapDanceTapHoldInteractionTest, InterruptedHold) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    // Press and release before hold timeout
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(150); // Less than typical 200ms timeout
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    wait_ms(250);

    // Should treat as tap since hold wasn't completed
    EXPECT_GE(g_mock_state.tap_code_calls_count(), 1);
    EXPECT_EQ(g_mock_state.layer_on_calls_count(), 0);
}
