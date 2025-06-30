#pragma once

#include "keymacros.h"

void* pipeline_key_replacer_initialize_user_data(void);
void pipeline_key_replacer_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data);
