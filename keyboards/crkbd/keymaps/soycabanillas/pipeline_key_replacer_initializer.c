#include "pipeline_key_replacer_initializer.h"
#include <stdlib.h>
#include "abstractionsqmk.h"

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

pipeline_key_replacer_pair_t* pipeline_key_replacer_create_pairs(platform_keycode_t keycode, replacer_callback callback) {
    pipeline_key_replacer_pair_t* key_replacer_pairs = malloc(sizeof(pipeline_key_replacer_pair_t));
    key_replacer_pairs->keycode = keycode;
    key_replacer_pairs->callback = callback;
    key_replacer_pairs->activated = false;
    return key_replacer_pairs;
}

pipeline_key_replacer_global_t* pipeline_key_replacer_initialize_user_data(void) {
    size_t n_modifiers = 7;
    pipeline_key_replacer_global_t* key_replacer_global = malloc(sizeof(pipeline_key_replacer_global_t));
    if (!key_replacer_global) {
        return NULL;
    }
    key_replacer_global->pairs = malloc(sizeof(pipeline_key_replacer_pairs_array_t) + n_modifiers * sizeof(pipeline_key_replacer_pair_t*));
    if (!key_replacer_global->pairs) {
        free(key_replacer_global);
        return NULL;
    }
    key_replacer_global->pairs->length = n_modifiers;

    key_replacer_global->pairs->modifier_pairs[0] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_2, callback_ckc_replace_shifted_2);
    key_replacer_global->pairs->modifier_pairs[1] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_3, callback_ckc_replace_shifted_3);
    key_replacer_global->pairs->modifier_pairs[2] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_4, callback_ckc_replace_shifted_4);
    key_replacer_global->pairs->modifier_pairs[3] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_5, callback_ckc_replace_shifted_5);
    key_replacer_global->pairs->modifier_pairs[4] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_6, callback_ckc_replace_shifted_6);
    key_replacer_global->pairs->modifier_pairs[5] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_9, callback_ckc_replace_shifted_9);
    key_replacer_global->pairs->modifier_pairs[6] = pipeline_key_replacer_create_pairs(CKC_REPLACE_SHIFTED_0, callback_ckc_replace_shifted_0);
    return key_replacer_global;
}
