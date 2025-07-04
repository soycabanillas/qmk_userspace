#include "pipeline_oneshot_modifier.h"
#include "keycodes.h"

 void pipeline_oneshot_modifier_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data) {
    // platform_log_debug("pipeline_oneshot_modifier_callback || up: %u || press: %u", params->up, params->callback_type);
    pipeline_oneshot_modifier_global_t* data = (pipeline_oneshot_modifier_global_t*)user_data;
    bool found_modifier = false;
    if (params->up == true && params->callback_type == PIPELINE_CALLBACK_KEY_PRESS) {
        for (size_t i = 0; i < data->pairs->length; i++)
        {
            if (data->pairs->modifier_pairs[i]->keycode == params->keycode) {
                data->modifiers = data->modifiers | data->pairs->modifier_pairs[i]->modifiers;
                found_modifier = true;
                break;
            }
        }
    }
    if (found_modifier == false) {
        if (data->modifiers != 0 && params->keycode <= 0xFF && params->up == true && params->callback_type == PIPELINE_CALLBACK_KEY_PRESS) {
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_SHIFT) {
                platform_register_code(KC_LEFT_SHIFT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_SHIFT) {
                platform_register_code(KC_RIGHT_SHIFT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_CTRL) {
                platform_register_code(KC_LEFT_CTRL);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_CTRL) {
                platform_register_code(KC_RIGHT_CTRL);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_ALT) {
                platform_register_code(KC_LEFT_ALT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_ALT) {
                platform_register_code(KC_RIGHT_ALT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_GUI) {
                platform_register_code(KC_LEFT_GUI);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_GUI) {
                platform_register_code(KC_RIGHT_GUI);
                platform_wait_ms(10);
            }
            data->executed_on_up = true;
        }
        else if (params->up == false && data->executed_on_up == true) {
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_SHIFT) {
                platform_unregister_code(KC_LEFT_SHIFT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_SHIFT) {
                platform_unregister_code(KC_RIGHT_SHIFT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_CTRL) {
                platform_unregister_code(KC_LEFT_CTRL);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_CTRL) {
                platform_unregister_code(KC_RIGHT_CTRL);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_ALT) {
                platform_unregister_code(KC_LEFT_ALT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_ALT) {
                platform_unregister_code(KC_RIGHT_ALT);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_GUI) {
                platform_unregister_code(KC_LEFT_GUI);
                platform_wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_GUI) {
                platform_unregister_code(KC_RIGHT_GUI);
                platform_wait_ms(10);
            }
            data->modifiers = 0;
            data->executed_on_up = false;
        }
    }
}
