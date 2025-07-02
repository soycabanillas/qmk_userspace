#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "mock_qmk.h"
#include "pipeline_tap_dance.h"
#include "commons.h"
}

using testing::_;
using testing::InSequence;

class TapDanceBasicTapTest : public ::testing::Test {
protected:
    void SetUp() override {
        reset_mock_state();

        // Initialize tap dance functionality
        custom_layers = (custom_layers_struct*)pipeline_tap_dance_initialize_user_data();
    }

    void TearDown() override {
        // Clean up any allocated memory
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

    // Helper function to simulate a key event
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

// Test single tap on CKC_LAY_MOUSE_Q should output KC_Q
TEST_F(TapDanceBasicTapTest, SingleTapOutputsCorrectKey) {
    // Reset call counters
    g_mock_state.tap_code_calls = 0;

    // Press and immediately release CKC_LAY_MOUSE_Q
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50); // Release after 50ms

    // Advance time past the tap timeout to trigger the tap action
    advance_time(250); // Total 300ms, past the 200ms timeout

    // Verify that KC_Q was tapped
    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_Q);
}

// Test single tap on CKC_LAY_NUMBERS_R should output KC_R
TEST_F(TapDanceBasicTapTest, SingleTapNumbers_R_OutputsCorrectKey) {
    g_mock_state.tap_code_calls = 0;

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_R);
}

// Test that quick tap doesn't activate layer
TEST_F(TapDanceBasicTapTest, QuickTapDoesNotActivateLayer) {
    g_mock_state.layer_on_calls = 0;

    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Layer should not be activated during the decision period
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    advance_time(250);

    // Layer should still not be activated after tap timeout
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}

// Test tap without any actions configured
TEST_F(TapDanceBasicTapTest, TapWithoutActionsDoesNothing) {
    // Use a keycode that doesn't have actions configured
    uint16_t unused_keycode = SAFE_RANGE + 100;

    g_mock_state.tap_code_calls = 0;
    g_mock_state.layer_on_calls = 0;

    simulate_key_event(unused_keycode, true);
    simulate_key_event(unused_keycode, false, 50);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 0);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);
}

// Test that exclamation mark single tap works
TEST_F(TapDanceBasicTapTest, ExclamationMarkSingleTap) {
    g_mock_state.tap_code_calls = 0;

    simulate_key_event(CKC_EXCL, true);
    simulate_key_event(CKC_EXCL, false, 50);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_EXLM);
}

// Test that question mark single tap works
TEST_F(TapDanceBasicTapTest, QuestionMarkSingleTap) {
    g_mock_state.tap_code_calls = 0;

    simulate_key_event(CKC_QUES, true);
    simulate_key_event(CKC_QUES, false, 50);
    advance_time(250);

    EXPECT_EQ(g_mock_state.tap_code_calls, 1);
    EXPECT_EQ(g_mock_state.last_tapped_code, KC_QUES);
}
