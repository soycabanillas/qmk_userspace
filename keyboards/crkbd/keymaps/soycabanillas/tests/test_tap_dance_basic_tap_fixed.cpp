#include "gtest/gtest.h"
#include "qmk_test_mock.h"

extern "C" {
#include "test_keycodes.h"
#include "abstractionsqmk.h"
#include "commons.h"
}

class TapDanceBasicTapTest : public ::testing::Test {
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
            wait_ms(time_offset);
        }
        macros_process_key(keycode, event);
    }
};

// Test single tap on CKC_LAY_MOUSE_Q should output KC_Q
TEST_F(TapDanceBasicTapTest, SingleTapOutputsCorrectKey) {
    // Reset tap code calls
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
