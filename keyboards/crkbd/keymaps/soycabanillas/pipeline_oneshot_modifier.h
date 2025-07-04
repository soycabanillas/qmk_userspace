#pragma once

#include "key_buffer.h"

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

void pipeline_oneshot_modifier_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data);
