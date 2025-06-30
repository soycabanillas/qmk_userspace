#include "commons.h"
#include "keycode.h"
#include "abstractionsqmk.h"
#include "pipeline_key_replacer.h"

typedef struct {
    uint8_t modifiers;
} replacer_callback_input;

typedef struct {
    uint16_t keycode;
    uint8_t modifiers;
} replacer_callback_result;

typedef void (*replacer_callback)(replacer_callback_input*, replacer_callback_result*);

typedef struct {
    uint16_t keycode;
    replacer_callback callback;
    bool activated;
} pipeline_key_replacer_pair_t;

typedef struct {
    size_t length;
    pipeline_key_replacer_pair_t* modifier_pairs[];
} pipeline_key_replacer_pairs_array_t;

typedef struct {
    pipeline_key_replacer_pairs_array_t* pairs;
} pipeline_key_replacer_global_t;

pipeline_key_replacer_global_t key_replacer_global;

pipeline_key_replacer_pair_t* pipeline_key_replacer_create_pairs(uint16_t keycode, replacer_callback callback) {
    pipeline_key_replacer_pair_t* key_replacer_pairs = malloc(sizeof(pipeline_key_replacer_pair_t));
    key_replacer_pairs->keycode = keycode;
    key_replacer_pairs->callback = callback;
    key_replacer_pairs->activated = false;
    return key_replacer_pairs;
}

void callback_ckc_replace_shifted_2(replacer_callback_input* input, replacer_callback_result* result) {
    result->keycode = KC_2;
    result->modifiers = MACRO_KEY_MODIFIER_LEFT_SHIFT;
}

void callback_ckc_replace_shifted_3(replacer_callback_input* input, replacer_callback_result* result) {
    result->keycode = KC_3;
    result->modifiers = MACRO_KEY_MODIFIER_LEFT_SHIFT;
}

void callback_ckc_replace_shifted_4(replacer_callback_input* input, replacer_callback_result* result) {
    result->keycode = KC_4;
    result->modifiers = MACRO_KEY_MODIFIER_LEFT_SHIFT;
}

void callback_ckc_replace_shifted_5(replacer_callback_input* input, replacer_callback_result* result) {
    result->keycode = KC_5;
    result->modifiers = MACRO_KEY_MODIFIER_LEFT_SHIFT;
}

void callback_ckc_replace_shifted_6(replacer_callback_input* input, replacer_callback_result* result) {
    result->keycode = KC_6;
    result->modifiers = MACRO_KEY_MODIFIER_LEFT_SHIFT;
}

void callback_ckc_replace_shifted_9(replacer_callback_input* input, replacer_callback_result* result) {
    result->keycode = KC_9;
    result->modifiers = MACRO_KEY_MODIFIER_LEFT_SHIFT;
}

void callback_ckc_replace_shifted_0(replacer_callback_input* input, replacer_callback_result* result) {
    result->keycode = KC_0;
    result->modifiers = MACRO_KEY_MODIFIER_LEFT_SHIFT;
}


void* pipeline_key_replacer_initialize_user_data(void) {
    size_t n_modifiers = 8;
    key_replacer_global.pairs = malloc(sizeof(pipeline_key_replacer_pairs_array_t) + n_modifiers * sizeof(pipeline_key_replacer_pair_t*));
    key_replacer_global.pairs->length = n_modifiers;

    key_replacer_global.pairs->modifier_pairs[1] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_2, callback_ckc_replace_shifted_2);
    key_replacer_global.pairs->modifier_pairs[2] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_3, callback_ckc_replace_shifted_3);
    key_replacer_global.pairs->modifier_pairs[3] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_4, callback_ckc_replace_shifted_4);
    key_replacer_global.pairs->modifier_pairs[4] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_5, callback_ckc_replace_shifted_5);
    key_replacer_global.pairs->modifier_pairs[5] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_6, callback_ckc_replace_shifted_6);
    key_replacer_global.pairs->modifier_pairs[6] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_9, callback_ckc_replace_shifted_9);
    key_replacer_global.pairs->modifier_pairs[7] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_0, callback_ckc_replace_shifted_0);
    return &key_replacer_global;
}

void pipeline_key_replacer_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data) {
    #ifdef CONSOLE_ENABLE
        uprintf("pipeline_key_replacer_callback || up: %u || press: %u\n", params->up, params->is_press);
    #endif
    pipeline_key_replacer_global_t* data = (pipeline_key_replacer_global_t*)user_data;
    if (params->up == true && params->is_press == true) {
        for (size_t i = 0; i < data->pairs->length; i++)
        {
            if (data->pairs->modifier_pairs[i]->keycode == params->keycode) {
                replacer_callback_input input = {
                    .modifiers = 0
                };
                replacer_callback_result result = {
                    .keycode = 0,
                    .modifiers = 0
                };
                data->pairs->modifier_pairs[i]->callback(&input, &result);
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_SHIFT && params->info.is_pressed(KC_LEFT_SHIFT) == false) {
                    register_code(KC_LEFT_SHIFT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_SHIFT && params->info.is_pressed(KC_RIGHT_SHIFT) == false) {
                    register_code(KC_RIGHT_SHIFT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_CTRL && params->info.is_pressed(KC_LEFT_CTRL) == false) {
                    register_code(KC_LEFT_CTRL);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_CTRL && params->info.is_pressed(KC_RIGHT_CTRL) == false) {
                    register_code(KC_RIGHT_CTRL);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_ALT && params->info.is_pressed(KC_LEFT_ALT) == false) {
                    register_code(KC_LEFT_ALT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_ALT && params->info.is_pressed(KC_RIGHT_ALT) == false) {
                    register_code(KC_RIGHT_ALT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_GUI && params->info.is_pressed(KC_LEFT_GUI) == false) {
                    register_code(KC_LEFT_GUI);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_GUI && params->info.is_pressed(KC_RIGHT_GUI) == false) {
                    register_code(KC_RIGHT_GUI);
                    wait_ms(10);
                }
                // tap_code(result.keycode);
                register_code(result.keycode);
                data->pairs->modifier_pairs[i]->activated = true;
                break;
            }
        }
    } else if (params->up == true && params->is_press == false) {
        for (size_t i = 0; i < data->pairs->length; i++)
        {
            if (data->pairs->modifier_pairs[i]->keycode == params->keycode && data->pairs->modifier_pairs[i]->activated) {
                replacer_callback_input input = {
                    .modifiers = 0
                };
                replacer_callback_result result = {
                    .keycode = 0,
                    .modifiers = 0
                };
                data->pairs->modifier_pairs[i]->callback(&input, &result);
                unregister_code(result.keycode);
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_SHIFT && params->info.is_pressed(KC_LEFT_SHIFT) == false) {
                    unregister_code(KC_LEFT_SHIFT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_SHIFT && params->info.is_pressed(KC_RIGHT_SHIFT) == false) {
                    unregister_code(KC_RIGHT_SHIFT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_CTRL && params->info.is_pressed(KC_LEFT_CTRL) == false) {
                    unregister_code(KC_LEFT_CTRL);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_CTRL && params->info.is_pressed(KC_RIGHT_CTRL) == false) {
                    unregister_code(KC_RIGHT_CTRL);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_ALT && params->info.is_pressed(KC_LEFT_ALT) == false) {
                    unregister_code(KC_LEFT_ALT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_ALT && params->info.is_pressed(KC_RIGHT_ALT) == false) {
                    unregister_code(KC_RIGHT_ALT);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_LEFT_GUI && params->info.is_pressed(KC_LEFT_GUI) == false) {
                    unregister_code(KC_LEFT_GUI);
                    wait_ms(10);
                }
                if (result.modifiers & MACRO_KEY_MODIFIER_RIGHT_GUI && params->info.is_pressed(KC_RIGHT_GUI) == false) {
                    unregister_code(KC_RIGHT_GUI);
                    wait_ms(10);
                }
                data->pairs->modifier_pairs[i]->activated = false;
                break;
            }
        }
    }
}
