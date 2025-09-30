
#include <stdlib.h>
#include "commons.h"
#include "keycodes.h"
#include "modules/soycabanillas/src/pipeline_combo.h"
#include "modules/soycabanillas/src/pipeline_combo_initializer.h"
#include "modules/soycabanillas/src/pipeline_tap_dance.h"
#include "modules/soycabanillas/src/pipeline_tap_dance_initializer.h"
#include "modules/soycabanillas/src/pipeline_oneshot_modifier.h"
#include "modules/soycabanillas/src/pipeline_oneshot_modifier_initializer.h"
#include "modules/soycabanillas/src/platform_interface.h"

static void add_hold_tap(uint8_t pipeline_index) {
    pipeline_tap_dance_global_state_create();
    size_t n_elements = 8;
    pipeline_tap_dance_global_config_t* tap_dance_config = (pipeline_tap_dance_global_config_t*)malloc(sizeof(pipeline_tap_dance_global_config_t));
    tap_dance_config->length = n_elements;
    tap_dance_config->behaviours = (pipeline_tap_dance_behaviour_t**)malloc(n_elements * sizeof(pipeline_tap_dance_behaviour_t*));

    pipeline_executor_add_physical_pipeline(pipeline_index, &pipeline_tap_dance_callback_process_data_executor, &pipeline_tap_dance_callback_reset_executor, tap_dance_config);


    pipeline_tap_dance_action_config_t* actions_CKC_LAY_NUMBERS_R[] = {
        createbehaviouraction_tap(1, KC_R),
        createbehaviouraction_hold(1, _LNUMBERS, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_NUMBERS_R = createbehaviour(CKC_LAY_NUMBERS_R, actions_CKC_LAY_NUMBERS_R, 2);
    tap_dance_behavior_CKC_LAY_NUMBERS_R->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_NUMBERS_R->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[0] = tap_dance_behavior_CKC_LAY_NUMBERS_R;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_MOVEMENT_F[] = {
        createbehaviouraction_tap(1, KC_F),
        createbehaviouraction_hold(1, _LMOVEMENT, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t*  tap_dance_behavior_CKC_LAY_MOVEMENT_F = createbehaviour(CKC_LAY_MOVEMENT_F, actions_CKC_LAY_MOVEMENT_F, 2);
    tap_dance_behavior_CKC_LAY_MOVEMENT_F->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_MOVEMENT_F->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[1] = tap_dance_behavior_CKC_LAY_MOVEMENT_F;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_FUNCTIONKEYS_V[] = {
        createbehaviouraction_tap(1, KC_V),
        createbehaviouraction_hold(1, _LFUNCTIONKEYS, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V = createbehaviour(CKC_LAY_FUNCTIONKEYS_V, actions_CKC_LAY_FUNCTIONKEYS_V, 2);
    tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[2] = tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_MOVEMENT_RIGHT_SCLN[] = {
        createbehaviouraction_tap(1, KC_SCLN),
        createbehaviouraction_hold(1, _LMOVEMENT_RIGHT_PINK, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN = createbehaviour(CKC_LAY_MOVEMENT_RIGHT_SCLN, actions_CKC_LAY_MOVEMENT_RIGHT_SCLN, 2);
    tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[3] = tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_LEFT_THUMB[] = {
        createbehaviouraction_hold(1, _LLEFT_THUMB, TAP_DANCE_HOLD_PREFERRED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_LEFT_THUMB = createbehaviour(CKC_LAY_LEFT_THUMB, actions_CKC_LAY_LEFT_THUMB, 1);
    tap_dance_behavior_CKC_LAY_LEFT_THUMB->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_LEFT_THUMB->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[4] = tap_dance_behavior_CKC_LAY_LEFT_THUMB;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_RIGHT_THUMB[] = {
        createbehaviouraction_hold(1, _LRIGHT_THUMB, TAP_DANCE_HOLD_PREFERRED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_RIGHT_THUMB = createbehaviour(CKC_LAY_RIGHT_THUMB, actions_CKC_LAY_RIGHT_THUMB, 1);
    tap_dance_behavior_CKC_LAY_RIGHT_THUMB->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_RIGHT_THUMB->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[5] = tap_dance_behavior_CKC_LAY_RIGHT_THUMB;

    pipeline_tap_dance_action_config_t* actions_CKC_EXCL[] = {
        createbehaviouraction_tap(1, MONKEEB_LSFT(KC_1)),
        createbehaviouraction_tap(2, MONKEEB_RALT(KC_1))
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_EXCL = createbehaviour(CKC_EXCL, actions_CKC_EXCL, 2);
    tap_dance_behavior_CKC_EXCL->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_EXCL->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[6] = tap_dance_behavior_CKC_EXCL;

    pipeline_tap_dance_action_config_t* actions_CKC_QUES[] = {
        createbehaviouraction_tap(1, MONKEEB_LSFT(KC_SLASH)),
        createbehaviouraction_tap(2, MONKEEB_RALT(KC_SLASH))
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_QUES = createbehaviour(CKC_QUES, actions_CKC_QUES, 2);
    tap_dance_behavior_CKC_QUES->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_QUES->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[7] = tap_dance_behavior_CKC_QUES;
}

static void add_one_shot_modifiers(uint8_t pipeline_index) {
    size_t number_of_pairs = 8;
    pipeline_oneshot_modifier_global_status_t* global_status = pipeline_oneshot_modifier_global_state_create();
    pipeline_oneshot_modifier_global_config_t* global_config = (pipeline_oneshot_modifier_global_config_t*)(malloc(sizeof(*global_config)));
    global_config->length = number_of_pairs;
    global_config->modifier_pairs = (pipeline_oneshot_modifier_pair_t**)(malloc(sizeof(pipeline_oneshot_modifier_pair_t*) * number_of_pairs));
    global_config->modifier_pairs[0] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_CTRL, MACRO_KEY_MODIFIER_LEFT_CTRL);
    global_config->modifier_pairs[1] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_ALT, MACRO_KEY_MODIFIER_LEFT_ALT);
    global_config->modifier_pairs[2] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_SHIFT, MACRO_KEY_MODIFIER_LEFT_SHIFT);
    global_config->modifier_pairs[3] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_GUI, MACRO_KEY_MODIFIER_LEFT_GUI);
    global_config->modifier_pairs[4] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_CTRL, MACRO_KEY_MODIFIER_RIGHT_CTRL);
    global_config->modifier_pairs[5] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_ALT, MACRO_KEY_MODIFIER_RIGHT_ALT);
    global_config->modifier_pairs[6] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_SHIFT, MACRO_KEY_MODIFIER_RIGHT_SHIFT);
    global_config->modifier_pairs[7] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_GUI, MACRO_KEY_MODIFIER_RIGHT_GUI);
    pipeline_oneshot_modifier_global_t* global = malloc(sizeof(pipeline_oneshot_modifier_global_t));
    global->config = global_config;
    global->status = global_status;
    pipeline_executor_add_virtual_pipeline(pipeline_index, &pipeline_oneshot_modifier_callback_process_data_executor, &pipeline_oneshot_modifier_callback_reset_executor, global);
}

static void add_combo(uint8_t pipeline_index) {
    pipeline_combo_global_state_create();
    size_t n_elements = 1;
    pipeline_combo_global_config_t* combo_config = (pipeline_combo_global_config_t*)malloc(sizeof(*combo_config));
    combo_config->length = n_elements;
    combo_config->combos = (pipeline_combo_config_t**)malloc(n_elements * sizeof(pipeline_combo_config_t*));
    combo_config->strategy = COMBO_STRATEGY_DISCARD_WHEN_ONE_PRESSED_IN_COMMON;

    pipeline_executor_add_physical_pipeline(pipeline_index, &pipeline_combo_callback_process_data_executor, &pipeline_combo_callback_reset_executor, combo_config);

    pipeline_combo_key_translation_t press_action_none = create_combo_key_action(COMBO_KEY_ACTION_NONE, 0);
    pipeline_combo_key_translation_t release_action_none = create_combo_key_action(COMBO_KEY_ACTION_NONE, 0);

    // Define a combo: pressing 'A' and 'S' together sends 'ESC'
    platform_keypos_t keypos_a = { .row = 0, .col = 1 }; // Example key position
    pipeline_combo_key_t* combo_key_a = create_combo_key(keypos_a, press_action_none, release_action_none);

    platform_keypos_t keypos_b = { .row = 0, .col = 2 }; // Example key position
    pipeline_combo_key_t* combo_key_b = create_combo_key(keypos_b, press_action_none, release_action_none);

    pipeline_combo_key_t* combo1_keys[] = { combo_key_a, combo_key_b };

    pipeline_combo_key_translation_t press_action_combo1 = create_combo_key_action(COMBO_KEY_ACTION_REGISTER, MONKEEB_LSFT(KC_2));
    pipeline_combo_key_translation_t release_action_combo1 = create_combo_key_action(COMBO_KEY_ACTION_UNREGISTER, MONKEEB_LSFT(KC_2));

    pipeline_combo_config_t* combo1 = create_combo(2, combo1_keys, press_action_combo1, release_action_combo1);
    combo_config->combos[0] = combo1;
}

void initialize_keymap(void* my_keymaps, platform_keycode_t get_keycode_from_layer(uint8_t layer, platform_keypos_t position)) {
    platform_layout_init_custom_2D_keymap((void*)my_keymaps, 7, 8, 6, get_keycode_from_layer);
    pipeline_executor_create_config(2, 1);
    add_combo(0);
    add_hold_tap(1);
    add_one_shot_modifiers(0);
}
