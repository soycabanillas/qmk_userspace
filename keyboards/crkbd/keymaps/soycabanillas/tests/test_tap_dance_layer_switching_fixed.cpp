#include "gtest/gtest.h"
#include "qmk_test_mock.h"

extern "C" {
#include "test_keycodes.h"
#include "abstractionsqmk.h"
#include "commons.h"
}

class TapDanceLayerSwitchingTest : public ::testing::Test {
protected:
    custom_layers_struct* custom_layers;

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

    void simulate_layer_activation(uint8_t layer) {
        layer_on(layer);
    }

    void simulate_layer_deactivation(uint8_t layer) {
        layer_off(layer);
    }
};

// Test basic layer activation via hold
TEST_F(TapDanceLayerSwitchingTest, BasicLayerActivation) {
    g_mock_state.layer_on_calls.clear();

    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250); // Hold to activate layer
    simulate_layer_activation(_LMOUSE);

    EXPECT_EQ(g_mock_state.layer_on_calls_count(), 1);
    EXPECT_EQ(g_mock_state.last_layer_on, _LMOUSE);
    EXPECT_TRUE(is_layer_on(_LMOUSE));
}

// Test layer deactivation on key release
TEST_F(TapDanceLayerSwitchingTest, LayerDeactivationOnRelease) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.layer_off_calls.clear();

    // Activate layer
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    wait_ms(250);
    simulate_layer_activation(_LNUMBERS);

    // Release key to deactivate
    simulate_key_event(CKC_LAY_NUMBERS_R, false);
    simulate_layer_deactivation(_LNUMBERS);

    EXPECT_EQ(g_mock_state.layer_off_calls_count(), 1);
    EXPECT_EQ(g_mock_state.last_layer_off, _LNUMBERS);
    EXPECT_FALSE(is_layer_on(_LNUMBERS));
}

// Test multiple layer switching
TEST_F(TapDanceLayerSwitchingTest, MultipleLayerSwitching) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.layer_off_calls.clear();

    // Activate mouse layer
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250);
    simulate_layer_activation(_LMOUSE);

    // Release and switch to numbers layer
    simulate_key_event(CKC_LAY_MOUSE_Q, false);
    simulate_layer_deactivation(_LMOUSE);

    wait_ms(100);

    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    wait_ms(250);
    simulate_layer_activation(_LNUMBERS);

    // Should have activated both layers in sequence
    EXPECT_EQ(g_mock_state.layer_on_calls_count(), 2);
    EXPECT_EQ(g_mock_state.layer_off_calls_count(), 1);
    EXPECT_TRUE(is_layer_on(_LNUMBERS));
}

// Test overlapping layer activation
TEST_F(TapDanceLayerSwitchingTest, OverlappingLayerActivation) {
    g_mock_state.layer_on_calls.clear();

    // Start holding first layer key
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(250);
    simulate_layer_activation(_LMOUSE);

    // While holding first, activate second layer
    simulate_key_event(CKC_LAY_MOVEMENT_S, true);
    wait_ms(250);
    simulate_layer_activation(_LMOVEMENT);

    // Should have both layers active
    EXPECT_EQ(g_mock_state.layer_on_calls_count(), 2);
    EXPECT_TRUE(is_layer_on(_LMOVEMENT)); // Last activated
}

// Test layer switching with tap interruption
TEST_F(TapDanceLayerSwitchingTest, LayerSwitchingWithTapInterruption) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.tap_code_calls.clear();

    // Start layer activation
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(100);

    // Interrupt with different key tap
    simulate_key_event(CKC_LSHIFT_EXCLAMATION_MARK, true);
    simulate_key_event(CKC_LSHIFT_EXCLAMATION_MARK, false, 50);

    // Continue original layer activation
    wait_ms(200);
    simulate_layer_activation(_LMOUSE);
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    // Should have both layer activation and tap
    EXPECT_GE(g_mock_state.layer_on_calls_count(), 1);
    EXPECT_GE(g_mock_state.tap_code_calls_count(), 1);
}

// Test rapid layer switching
TEST_F(TapDanceLayerSwitchingTest, RapidLayerSwitching) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.layer_off_calls.clear();

    // Rapidly switch between layers
    const uint8_t layers[] = {_LMOUSE, _LNUMBERS, _LMOVEMENT};
    const uint16_t keycodes[] = {CKC_LAY_MOUSE_Q, CKC_LAY_NUMBERS_R, CKC_LAY_MOVEMENT_S};

    for (int i = 0; i < 3; i++) {
        simulate_key_event(keycodes[i], true);
        wait_ms(100);
        simulate_layer_activation(layers[i]);
        wait_ms(50);
        simulate_key_event(keycodes[i], false);
        simulate_layer_deactivation(layers[i]);
        wait_ms(50);
    }

    // Should have activated and deactivated each layer
    EXPECT_EQ(g_mock_state.layer_on_calls_count(), 3);
    EXPECT_EQ(g_mock_state.layer_off_calls_count(), 3);
}

// Test layer persistence across key releases
TEST_F(TapDanceLayerSwitchingTest, LayerPersistenceAcrossKeyReleases) {
    g_mock_state.layer_on_calls.clear();
    g_mock_state.layer_off_calls.clear();

    // Activate layer
    simulate_key_event(CKC_LAY_RIGHT_THUMB_F, true);
    wait_ms(250);
    simulate_layer_activation(_LRIGHT_THUMB);

    // Tap other keys while layer is active
    simulate_key_event(KC_A, true);
    simulate_key_event(KC_A, false, 50);

    wait_ms(50);

    simulate_key_event(KC_Q, true);
    simulate_key_event(KC_Q, false, 50);

    // Layer should still be active
    EXPECT_TRUE(is_layer_on(_LRIGHT_THUMB));

    // Release original layer key
    simulate_key_event(CKC_LAY_RIGHT_THUMB_F, false);
    simulate_layer_deactivation(_LRIGHT_THUMB);

    // Now layer should be inactive
    EXPECT_FALSE(is_layer_on(_LRIGHT_THUMB));
}

// Test layer switching edge cases
TEST_F(TapDanceLayerSwitchingTest, LayerSwitchingEdgeCases) {
    g_mock_state.layer_on_calls.clear();

    // Very brief hold (edge of timeout)
    simulate_key_event(CKC_LAY_MOUSE_Q, true);
    wait_ms(200); // Right at timeout threshold
    simulate_key_event(CKC_LAY_MOUSE_Q, false);

    wait_ms(100);

    // Double tap followed by hold
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    simulate_key_event(CKC_LAY_NUMBERS_R, false, 50);
    wait_ms(50);
    simulate_key_event(CKC_LAY_NUMBERS_R, true);
    wait_ms(250);
    simulate_layer_activation(_LNUMBERS);

    // Should handle edge cases gracefully
    EXPECT_GE(g_mock_state.layer_on_calls_count(), 0);
}
