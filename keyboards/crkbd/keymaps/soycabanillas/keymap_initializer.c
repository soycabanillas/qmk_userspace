#include "keymap_initializer.h"
#include <stdlib.h>
#include <string.h>
#include "abstractionsqmk.h"
#include "commons.h"
#include "keycodes.h"
#include "pipeline_tap_dance_initializer.h"

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
