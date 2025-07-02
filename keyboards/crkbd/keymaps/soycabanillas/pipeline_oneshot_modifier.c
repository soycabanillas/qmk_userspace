#include "commons.h"
#include "abstractionsqmk.h"
#include "key_buffer.h"
#include "platform_qmk.h"
#include "pipeline_oneshot_modifier.h"
#include <stdlib.h>

typedef struct {
    platform_keycode_t keycode;
    uint8_t modifiers;
} pipeline_oneshot_modifier_pair_t;

typedef struct {
    size_t length;
    pipeline_oneshot_modifier_pair_t* modifier_pairs[];
} pipeline_oneshot_modifier_pairs_array_t;

typedef struct {
    uint8_t modifiers;
    bool executed_on_up;
    pipeline_oneshot_modifier_pairs_array_t* pairs;
} pipeline_oneshot_modifier_global_t;

pipeline_oneshot_modifier_global_t one_key_modifier_global;

pipeline_oneshot_modifier_pair_t* pipeline_oneshot_modifier_create_pairs(platform_keycode_t keycode, uint8_t modifiers) {
    pipeline_oneshot_modifier_pair_t* oneshot_modifier_pairs = malloc(sizeof(pipeline_oneshot_modifier_pair_t));
    oneshot_modifier_pairs->keycode = keycode;
    oneshot_modifier_pairs->modifiers = modifiers;
    return oneshot_modifier_pairs;
}

void* pipeline_oneshot_modifier_initialize_user_data(void) {
    size_t n_modifiers = 8;
    one_key_modifier_global.modifiers = 0;
    one_key_modifier_global.executed_on_up = false;
    one_key_modifier_global.pairs = malloc(sizeof(pipeline_oneshot_modifier_pairs_array_t) + n_modifiers * sizeof(pipeline_oneshot_modifier_pair_t*));
    one_key_modifier_global.pairs->length = n_modifiers;

    one_key_modifier_global.pairs->modifier_pairs[0] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_CTRL, MACRO_KEY_MODIFIER_LEFT_CTRL);
    one_key_modifier_global.pairs->modifier_pairs[1] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_ALT, MACRO_KEY_MODIFIER_LEFT_ALT);
    one_key_modifier_global.pairs->modifier_pairs[2] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_SHIFT, MACRO_KEY_MODIFIER_LEFT_SHIFT);
    one_key_modifier_global.pairs->modifier_pairs[3] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_GUI, MACRO_KEY_MODIFIER_LEFT_GUI);
    one_key_modifier_global.pairs->modifier_pairs[4] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_CTRL, MACRO_KEY_MODIFIER_RIGHT_CTRL);
    one_key_modifier_global.pairs->modifier_pairs[5] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_ALT, MACRO_KEY_MODIFIER_RIGHT_ALT);
    one_key_modifier_global.pairs->modifier_pairs[6] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_SHIFT, MACRO_KEY_MODIFIER_RIGHT_SHIFT);
    one_key_modifier_global.pairs->modifier_pairs[7] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_GUI, MACRO_KEY_MODIFIER_RIGHT_GUI);
    return &one_key_modifier_global;
}

void pipeline_oneshot_modifier_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data) {
    platform_log_debug("pipeline_oneshot_modifier_callback || up: %u || press: %u", params->up, params->callback_type);
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
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_SHIFT) {
                platform_register_code(KC_RIGHT_SHIFT);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_CTRL) {
                platform_register_code(KC_LEFT_CTRL);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_CTRL) {
                platform_register_code(KC_RIGHT_CTRL);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_ALT) {
                platform_register_code(KC_LEFT_ALT);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_ALT) {
                platform_register_code(KC_RIGHT_ALT);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_GUI) {
                platform_register_code(KC_LEFT_GUI);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_GUI) {
                platform_register_code(KC_RIGHT_GUI);
                wait_ms(10);
            }
            data->executed_on_up = true;
        }
        else if (params->up == false && data->executed_on_up == true) {
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_SHIFT) {
                platform_unregister_code(KC_LEFT_SHIFT);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_SHIFT) {
                platform_unregister_code(KC_RIGHT_SHIFT);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_CTRL) {
                platform_unregister_code(KC_LEFT_CTRL);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_CTRL) {
                platform_unregister_code(KC_RIGHT_CTRL);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_ALT) {
                platform_unregister_code(KC_LEFT_ALT);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_ALT) {
                platform_unregister_code(KC_RIGHT_ALT);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_LEFT_GUI) {
                platform_unregister_code(KC_LEFT_GUI);
                wait_ms(10);
            }
            if (data->modifiers & MACRO_KEY_MODIFIER_RIGHT_GUI) {
                platform_unregister_code(KC_RIGHT_GUI);
                wait_ms(10);
            }
            data->modifiers = 0;
            data->executed_on_up = false;
        }
    }
}
