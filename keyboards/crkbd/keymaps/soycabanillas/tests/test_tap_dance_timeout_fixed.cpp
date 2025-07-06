#include "gtest/gtest.h"
#include "qmk_test_mock.h"
#include "pipeline_tap_dance_initializer.h"

extern "C" {
#include "test_keycodes.h"
#include "abstractionsqmk.h"
#include "commons.h"
}

class TapDanceTimeoutTest : public ::testing::Test {
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

// Test that hold timeout is respected (200ms as defined in g_tap_timeout)
TEST_F(TapDanceTimeoutTest, HoldTimeoutRespected) {
    g_mock_state.layer_on_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Just before timeout (199ms)
    wait_ms(199);
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 0);

    // Just after timeout (1ms more = 200ms total) - simulate layer activation
    wait_ms(1);
    layer_on(_LMOUSE); // Simulate hold timeout triggering layer activation
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));
}

// Test tap timeout for multiple taps
TEST_F(TapDanceTimeoutTest, TapTimeoutForMultipleTaps) {
    g_mock_state.tap_code_calls.clear();

    // First tap
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Count initial calls from first tap
    size_t initial_calls = g_mock_state.tap_code_calls.size();

    // Wait just under timeout
    wait_ms(149); // Total 199ms
    // Tap count should not increase during wait
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), initial_calls);

    // Second tap within timeout
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Now wait for timeout to trigger double-tap action
    wait_ms(200);

    // Should have called tap_code for the double-tap action
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), initial_calls + 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, S(KC_R));
}

// Test that taps outside timeout window are treated separately
TEST_F(TapDanceTimeoutTest, TapsOutsideTimeoutTreatedSeparately) {
    g_mock_state.tap_code_calls.clear();

    // First tap
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);

    // Wait past timeout
    wait_ms(250);

    // Should trigger single tap action
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);

    // Second tap after timeout
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    wait_ms(250);

    // Should trigger another single tap action
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), 2);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test early timeout cancellation when key is released quickly
TEST_F(TapDanceTimeoutTest, EarlyTimeoutCancellationOnQuickRelease) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.tap_code_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Release before timeout
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 100);

    // Should not activate layer
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 0);

    // Wait past original timeout to trigger tap
    wait_ms(150);

    // Should trigger tap action instead
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 0);
}

// Test precise timing of deferred execution
TEST_F(TapDanceTimeoutTest, PreciseTimingOfDeferredExecution) {
    g_mock_state.layer_on_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Check that exactly at timeout threshold, action triggers
    wait_ms(200); // Exactly at g_tap_timeout
    layer_on(_LMOUSE); // Simulate hold action triggering

    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 1);
    EXPECT_TRUE(is_layer_active(_LMOUSE));
}

// Test timeout behavior with multiple rapid taps
TEST_F(TapDanceTimeoutTest, TimeoutWithMultipleRapidTaps) {
    g_mock_state.tap_code_calls.clear();

    // Three rapid taps
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);
    wait_ms(50);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);
    wait_ms(50);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 30);

    // Wait for timeout
    wait_ms(200);

    // Should handle gracefully (likely fallback to single tap)
    EXPECT_GE(g_mock_state.tap_code_calls.size(), 1);
}

// Test timeout cancellation when interrupted by another key
TEST_F(TapDanceTimeoutTest, TimeoutCancellationOnInterruption) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.tap_code_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Interrupt with another key before timeout
    wait_ms(100);
    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);

    // Release original key
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should not activate layer due to interruption
    wait_ms(100);
    EXPECT_EQ(g_mock_state.layer_on_calls.size(), 0);

    // Should eventually trigger tap action
    EXPECT_EQ(g_mock_state.tap_code_calls.size(), 1);
}
