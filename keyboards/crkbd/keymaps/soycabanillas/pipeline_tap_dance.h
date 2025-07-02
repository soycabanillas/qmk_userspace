#pragma once

#include "key_buffer.h"

void* pipeline_tap_dance_initialize_user_data(void);
void pipeline_tap_dance_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data);
