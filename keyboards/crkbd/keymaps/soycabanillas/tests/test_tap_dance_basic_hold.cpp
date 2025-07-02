#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "mock_qmk.h"
#include "pipeline_tap_dance.h"
#include "commons.h"
}

class TapDanceBasicHoldTest : public ::testing::Test {
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

// Test hold on CKC_LAY_MOUSE_Q activates mouse layer
TEST_F(TapDanceBasicHoldTest, HoldActivatesMouseLayer) {
    g_mock_state.layer_on_calls = 0;

    // Press and hold CKC_LAY_MOUSE_Q
    simulate_key_event(CKC_LAY_MOUSE_Q, true);

    // Advance time past the tap timeout (200ms)
    advance_time(250);

    // Verify that the mouse layer was activated
    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOUSE);
    EXPECT_TRUE(is_layer_active(_LMOUSE));
}

// Test hold and release deactivates layer
TEST_F(TapDanceBasicHoldTest, HoldAndReleaseDeactivatesLayer) {
    g_mock_state.layer_off_calls = 0;
    g_mock_state.clear_keyboard_calls = 0;

    // Press and hold
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    advance_time(250); // Trigger hold

    // Verify layer is active
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    // Release the key
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Verify layer was deactivated and keyboard cleared
    EXPECT_EQ(g_mock_state.layer_off_calls, 1);
    EXPECT_EQ(g_mock_state.last_layer_off, _LMOUSE);
    EXPECT_EQ(g_mock_state.clear_keyboard_calls, 1);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}

// Test hold on CKC_LAY_NUMBERS_R activates numbers layer
TEST_F(TapDanceBasicHoldTest, HoldActivatesNumbersLayer) {
    g_mock_state.layer_on_calls = 0;

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    advance_time(250);

    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LNUMBERS);
    EXPECT_TRUE(is_layer_active(_LNUMBERS));
}

// Test hold on CKC_LAY_MOVEMENT_F activates movement layer
TEST_F(TapDanceBasicHoldTest, HoldActivatesMovementLayer) {
    g_mock_state.layer_on_calls = 0;

    simulate_key_event(CKC_LAY_MOVEMENT_F, true);
    advance_time(250);

    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOVEMENT);
    EXPECT_TRUE(is_layer_active(_LMOVEMENT));
}

// Test hold on CKC_LAY_RIGHT_THUMB activates thumb layer
TEST_F(TapDanceBasicHoldTest, HoldActivatesRightThumbLayer) {
    g_mock_state.layer_on_calls = 0;

    simulate_key_event(CKC_LAY_RIGHT_THUMB, true);
    advance_time(250);

    EXPECT_EQ(g_mock_state.layer_on_calls, 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LRIGHT_THUMB);
    EXPECT_TRUE(is_layer_active(_LRIGHT_THUMB));
}

// Test that short press doesn't activate layer
TEST_F(TapDanceBasicHoldTest, ShortPressDoesNotActivateLayer) {
    g_mock_state.layer_on_calls = 0;

    // Press and release quickly (before timeout)
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50); // Release after 50ms

    // Should not have activated layer
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}

// Test that keys without hold actions don't activate layers
TEST_F(TapDanceBasicHoldTest, KeysWithoutHoldActionsDoNotActivateLayer) {
    g_mock_state.layer_on_calls = 0;

    // CKC_EXCL and CKC_QUES have tap actions but no hold actions
    simulate_key_event(CKC_EXCL, true);
    advance_time(250);

    EXPECT_EQ(g_mock_state.layer_on_calls, 0);

    simulate_key_event(CKC_EXCL, false);

    // Try with question mark too
    simulate_key_event(CKC_QUES, true);
    advance_time(250);

    EXPECT_EQ(g_mock_state.layer_on_calls, 0);
}
