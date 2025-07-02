#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "mock_qmk.h"
#include "pipeline_tap_dance.h"
#include "commons.h"
}

class TapDanceLayerSwitchingTest : public ::testing::Test {
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

// Test layer activation and deactivation cycle
TEST_F(TapDanceLayerSwitchingTest, LayerActivationDeactivationCycle) {
    // Test mouse layer
    EXPECT_FALSE(is_layer_active(_LMOUSE));

    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    advance_time(250); // Trigger hold

    EXPECT_TRUE(is_layer_active(_LMOUSE));
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOUSE);

    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    EXPECT_FALSE(is_layer_active(_LMOUSE));
    EXPECT_EQ(g_mock_state.last_layer_off, _LMOUSE);
}

// Test numbers layer activation
TEST_F(TapDanceLayerSwitchingTest, NumbersLayerActivation) {
    EXPECT_FALSE(is_layer_active(_LNUMBERS));

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    advance_time(250);

    EXPECT_TRUE(is_layer_active(_LNUMBERS));
    EXPECT_EQ(g_mock_state.last_layer_on, _LNUMBERS);

    simulate_key_event(CKC_LAY_NUMBERS_R, false);

    EXPECT_FALSE(is_layer_active(_LNUMBERS));
}

// Test movement layer activation
TEST_F(TapDanceLayerSwitchingTest, MovementLayerActivation) {
    EXPECT_FALSE(is_layer_active(_LMOVEMENT));

    simulate_key_event(CKC_LAY_MOVEMENT_F, true);
    advance_time(250);

    EXPECT_TRUE(is_layer_active(_LMOVEMENT));
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOVEMENT);

    simulate_key_event(CKC_LAY_MOVEMENT_F, false);

    EXPECT_FALSE(is_layer_active(_LMOVEMENT));
}

// Test right thumb layer activation
TEST_F(TapDanceLayerSwitchingTest, RightThumbLayerActivation) {
    EXPECT_FALSE(is_layer_active(_LRIGHT_THUMB));

    simulate_key_event(CKC_LAY_RIGHT_THUMB, true);
    advance_time(250);

    EXPECT_TRUE(is_layer_active(_LRIGHT_THUMB));
    EXPECT_EQ(g_mock_state.last_layer_on, _LRIGHT_THUMB);

    simulate_key_event(CKC_LAY_RIGHT_THUMB, false);

    EXPECT_FALSE(is_layer_active(_LRIGHT_THUMB));
}

// Test multiple layers don't interfere with each other
TEST_F(TapDanceLayerSwitchingTest, MultipleLayersIndependent) {
    // Activate mouse layer
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    advance_time(250);
    EXPECT_TRUE(is_layer_active(_LMOUSE));

    // Activate numbers layer with different key
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    advance_time(250);
    EXPECT_TRUE(is_layer_active(_LNUMBERS));
    EXPECT_TRUE(is_layer_active(_LMOUSE)); // Mouse should still be active

    // Release mouse layer
    simulate_key_event(CKC_LAY_MOUSE_Q, false);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
    EXPECT_TRUE(is_layer_active(_LNUMBERS)); // Numbers should still be active

    // Release numbers layer
    simulate_key_event(CKC_LAY_NUMBERS_R, false);
    EXPECT_FALSE(is_layer_active(_LNUMBERS));
}

// Test that keyboard is cleared when layer is deactivated
TEST_F(TapDanceLayerSwitchingTest, KeyboardClearedOnLayerDeactivation) {
    g_mock_state.clear_keyboard_calls = 0;

    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    advance_time(250);

    // Clear keyboard should not be called on activation
    EXPECT_EQ(g_mock_state.clear_keyboard_calls, 0);

    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Clear keyboard should be called on deactivation
    EXPECT_EQ(g_mock_state.clear_keyboard_calls, 1);
}

// Test layer switching with fast press/release doesn't activate layer
TEST_F(TapDanceLayerSwitchingTest, FastPressReleaseDoesNotActivateLayer) {
    g_mock_state.layer_on_calls = 0;

    // Quick press and release (under threshold)
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    simulate_key_event(CKC_LAY_MOUSE_Q, false, 50);

    // Should not activate layer even after some time
    advance_time(100);
    EXPECT_EQ(g_mock_state.layer_on_calls, 0);
    EXPECT_FALSE(is_layer_active(_LMOUSE));
}

// Test that layer state is consistent across multiple activations
TEST_F(TapDanceLayerSwitchingTest, LayerStateConsistentAcrossActivations) {
    for (int i = 0; i < 3; i++) {
        // Activate
        simulate_key_event(CKC_LAY_MOUSE_Q, true);
        advance_time(250);
        EXPECT_TRUE(is_layer_active(_LMOUSE));

        // Deactivate
        simulate_key_event(CKC_LAY_MOUSE_Q, false);
        EXPECT_FALSE(is_layer_active(_LMOUSE));

        // Wait a bit between cycles
        advance_time(100);
    }
}
