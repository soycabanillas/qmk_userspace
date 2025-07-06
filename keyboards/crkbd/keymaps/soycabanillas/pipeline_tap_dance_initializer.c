#include "pipeline_tap_dance_initializer.h"
#include <stdlib.h>
#include <string.h>

custom_action_custom_behaviour* createbehaviouraction(uint8_t repetitions, td_customlayer_action_t action, platform_keycode_t keycode, uint8_t layer) {
    custom_action_custom_behaviour behaviouraction = {
        .repetitions = repetitions,
        .action = action,
        .keycode = keycode,
        .layer = layer,
    };
    custom_action_custom_behaviour* allocation = (custom_action_custom_behaviour*)malloc(sizeof behaviouraction);
    memcpy(allocation, &behaviouraction, sizeof behaviouraction);
    return allocation;
}

custom_behaviour_config* createbehaviour(platform_keycode_t keycodemodifier, custom_action_custom_behaviour* actions[], size_t actionslength) {
    custom_switch_layer_custom_data userdata = {
        .sequence = _HOLD_OR_TAP_STATE_NOT_SET
    };
    userdata.actionslength = actionslength;
    custom_switch_layer_custom_data* allocationuserdata = (custom_switch_layer_custom_data*)malloc(sizeof(custom_switch_layer_custom_data) + actionslength * sizeof (custom_action_custom_behaviour*));
    memcpy(allocationuserdata, &userdata, sizeof userdata);
    for (size_t i = 0; i < actionslength; i++)
    {
        allocationuserdata->actions[i] = actions[i];
    }
    custom_behaviour_config behaviour = {
        .status = {
            .keycodemodifier = keycodemodifier
        },
        .user_data = (void*) allocationuserdata,
    };
    custom_behaviour_config* allocation = (custom_behaviour_config*)malloc(sizeof behaviour);
    memcpy(allocation, &behaviour, sizeof behaviour);
    return allocation;
}


