#pragma once

#include "pipeline_tap_dance.h"

#ifdef __cplusplus
extern "C" {
#endif

custom_action_custom_behaviour* createbehaviouraction(uint8_t repetitions, td_customlayer_action_t action, platform_keycode_t keycode, uint8_t layer);
custom_behaviour_config* createbehaviour(platform_keycode_t keycodemodifier, custom_action_custom_behaviour* actions[], size_t actionslength);

#ifdef __cplusplus
}
#endif
