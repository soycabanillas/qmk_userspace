#include "pipeline_tap_dance_initializer.h"
#include <stdlib.h>
#include <string.h>
#include "abstractionsqmk.h"
#include "commons.h"
#include "keycodes.h"

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
        .callback = &custom_switch_layer_custom_function
    };
    custom_behaviour_config* allocation = (custom_behaviour_config*)malloc(sizeof behaviour);
    memcpy(allocation, &behaviour, sizeof behaviour);
    return allocation;
}

custom_layers_struct* pipeline_tap_dance_initialize_user_data(void) {
    size_t nelements = 6;
    custom_layers_struct* custom_layers = (custom_layers_struct*)malloc(sizeof *custom_layers + nelements * sizeof *custom_layers->layers);
    custom_layers->length = nelements;

    custom_action_custom_behaviour* custom_actions[] =
         {
             createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_Q, _LMOUSE),
             createbehaviouraction(0, TDCL_HOLD_KEY_CHANGELAYERTEMPO, KC_Q, _LMOUSE)
         };
    custom_layers->layers[0] = createbehaviour(CKC_LAY_MOUSE_Q, custom_actions, 2);

    custom_action_custom_behaviour* custom_actions2[] =
         {
             createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_R, _LNUMBERS),
             createbehaviouraction(1, TDCL_TAP_KEY_SENDKEY, S(KC_R), _LNUMBERS),
             createbehaviouraction(0, TDCL_HOLD_KEY_CHANGELAYERTEMPO, KC_R, _LNUMBERS)
         };
    custom_layers->layers[1] = createbehaviour(CKC_LAY_NUMBERS_R, custom_actions2, 3);

    custom_action_custom_behaviour* custom_actions3[] =
         {
             createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_F, _LNUMBERS),
             createbehaviouraction(0, TDCL_HOLD_KEY_CHANGELAYERTEMPO, KC_F, _LMOVEMENT),
             createbehaviouraction(1, TDCL_HOLD_KEY_CHANGELAYERTEMPO, KC_F, _LMOUSE)
         };
    custom_layers->layers[2] = createbehaviour(CKC_LAY_MOVEMENT_F, custom_actions3, 3);

    custom_action_custom_behaviour* custom_actions4[] =
         {
             createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_EXLM, _LMOUSE),
             createbehaviouraction(1, TDCL_TAP_KEY_SENDKEY, RALT(KC_1), _LNUMBERS),
         };
    custom_layers->layers[3] = createbehaviour(CKC_EXCL, custom_actions4, 2);

    custom_action_custom_behaviour* custom_actions5[] =
         {
             createbehaviouraction(0, TDCL_TAP_KEY_SENDKEY, KC_QUES, _LNUMBERS),
             createbehaviouraction(1, TDCL_TAP_KEY_SENDKEY, RALT(KC_SLSH), _LNUMBERS),
         };
    custom_layers->layers[4] = createbehaviour(CKC_QUES, custom_actions5, 2);

    custom_action_custom_behaviour* custom_actions6[] =
         {
             createbehaviouraction(0, TDCL_HOLD_KEY_CHANGELAYERTEMPO, KC_QUES, _LRIGHT_THUMB),
         };
    custom_layers->layers[5] = createbehaviour(CKC_LAY_RIGHT_THUMB, custom_actions6, 1);
    return custom_layers;
}
