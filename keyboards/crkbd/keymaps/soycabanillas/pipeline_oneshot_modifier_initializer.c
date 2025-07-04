#include "pipeline_oneshot_modifier_initializer.h"
#include <stdlib.h>
#include "abstractionsqmk.h"


pipeline_oneshot_modifier_pair_t* pipeline_oneshot_modifier_create_pairs(platform_keycode_t keycode, uint8_t modifiers) {
    pipeline_oneshot_modifier_pair_t* oneshot_modifier_pairs = malloc(sizeof(pipeline_oneshot_modifier_pair_t));
    oneshot_modifier_pairs->keycode = keycode;
    oneshot_modifier_pairs->modifiers = modifiers;
    return oneshot_modifier_pairs;
}

pipeline_oneshot_modifier_global_t* pipeline_oneshot_modifier_initialize_user_data(void) {
    size_t n_modifiers = 8;
    pipeline_oneshot_modifier_global_t* one_key_modifier_global = malloc(sizeof(pipeline_oneshot_modifier_global_t));
    if (!one_key_modifier_global) {
        return NULL;
    }
    one_key_modifier_global->modifiers = 0;
    one_key_modifier_global->executed_on_up = false;
    one_key_modifier_global->pairs = malloc(sizeof(pipeline_oneshot_modifier_pairs_array_t) + n_modifiers * sizeof(pipeline_oneshot_modifier_pair_t*));
    if (!one_key_modifier_global->pairs) {
        free(one_key_modifier_global);
        return NULL;
    }
    one_key_modifier_global->pairs->length = n_modifiers;

    one_key_modifier_global->pairs->modifier_pairs[0] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_CTRL, MACRO_KEY_MODIFIER_LEFT_CTRL);
    one_key_modifier_global->pairs->modifier_pairs[1] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_ALT, MACRO_KEY_MODIFIER_LEFT_ALT);
    one_key_modifier_global->pairs->modifier_pairs[2] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_SHIFT, MACRO_KEY_MODIFIER_LEFT_SHIFT);
    one_key_modifier_global->pairs->modifier_pairs[3] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_GUI, MACRO_KEY_MODIFIER_LEFT_GUI);
    one_key_modifier_global->pairs->modifier_pairs[4] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_CTRL, MACRO_KEY_MODIFIER_RIGHT_CTRL);
    one_key_modifier_global->pairs->modifier_pairs[5] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_ALT, MACRO_KEY_MODIFIER_RIGHT_ALT);
    one_key_modifier_global->pairs->modifier_pairs[6] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_SHIFT, MACRO_KEY_MODIFIER_RIGHT_SHIFT);
    one_key_modifier_global->pairs->modifier_pairs[7] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_GUI, MACRO_KEY_MODIFIER_RIGHT_GUI);
    return one_key_modifier_global;
}
