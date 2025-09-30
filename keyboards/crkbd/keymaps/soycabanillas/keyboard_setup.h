#pragma once

#include "modules/soycabanillas/src/platform_types.h"

void initialize_keymap(void* my_keymaps, platform_keycode_t get_keycode_from_layer(uint8_t layer, platform_keypos_t position));
