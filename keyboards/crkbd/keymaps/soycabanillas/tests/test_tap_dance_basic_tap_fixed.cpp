#include "gtest/gtest.h"
#include "qmk_test_mock.h"
#include "pipeline_tap_dance_initializer.h"

extern "C" {
#include "test_keycodes.h"
#include "abstractionsqmk.h"
#include "commons.h"
}

class TapDanceBasicTapTest : public ::testing::Test {
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

// Test single tap on CKC_LAY_MOUSE_Q should output KC_Q
TEST_F(TapDanceBasicTapTest, SingleTapOutputsCorrectKey) {
    g_mock_state.tap_code_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    wait_ms(250); // Total 300ms, past the 200ms timeout

    EXPECT_EQ(g_mock_state.tap_code_calls_count(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test single tap on CKC_LAY_NUMBERS_R should output KC_R
TEST_F(TapDanceBasicTapTest, SingleTapNumbers_R_OutputsCorrectKey) {
    g_mock_state.tap_code_calls.clear();

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    wait_ms(250);

    EXPECT_EQ(g_mock_state.tap_code_calls_count(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test quick tap doesn't activate layer (only long hold should)
TEST_F(TapDanceBasicTapTest, QuickTapDoesNotActivateLayer) {
    g_mock_state.layer_on_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);
    wait_ms(250);

    EXPECT_EQ(g_mock_state.layer_on_calls_count(), 0);
    EXPECT_FALSE(is_layer_on(_LMOUSE));
}

// Test key without actions does nothing
TEST_F(TapDanceBasicTapTest, TapWithoutActionsDoesNothing) {
    g_mock_state.tap_code_calls.clear();
    g_mock_state.layer_on_calls.clear();

    simulate_key_event(KC_A, true);  // Regular key, not a tap dance key
    simulate_key_event(KC_A, false, 50);
    wait_ms(250);

    EXPECT_EQ(g_mock_state.tap_code_calls_count(), 0);
    EXPECT_EQ(g_mock_state.layer_on_calls_count(), 0);
}

// Test exclamation mark single tap
TEST_F(TapDanceBasicTapTest, ExclamationMarkSingleTap) {
    g_mock_state.tap_code_calls.clear();

    simulate_key_event(CKC_LSHIFT_EXCLAMATION_MARK, true);
    simulate_key_event(CKC_LSHIFT_EXCLAMATION_MARK, false, 50);
    wait_ms(250);

    EXPECT_EQ(g_mock_state.tap_code_calls_count(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_EXLM);
}

// Test question mark single tap
TEST_F(TapDanceBasicTapTest, QuestionMarkSingleTap) {
    g_mock_state.tap_code_calls.clear();

    simulate_key_event(CKC_RSHIFT_QUESTION_MARK, true);
    simulate_key_event(CKC_RSHIFT_QUESTION_MARK, false, 50);
    wait_ms(250);

    EXPECT_EQ(g_mock_state.tap_code_calls_count(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_QUES);
}
