#ifdef UNIT_TEST
    // Test environment includes
    #include "key_buffer.h"
    #include "test_keycodes.h"
#else
    // Production environment includes
    #include "pipeline_tap_dance.h"
#endif
#include "commons.h"
#include "abstractionsqmk.h"
#include "platform_qmk.h"
#include <stdlib.h>
#include <string.h>

#define g_tap_timeout 200

typedef struct {
    platform_keycode_t keycodemodifier;
} t_layer_status;

typedef bool (*custom_behaviour_callback)(platform_keycode_t, abskeyevent_t, t_layer_status*, void*);

typedef enum {
    TDCL_TAP_KEY_SENDKEY,
    TDCL_HOLD_KEY_SENDKEY,
    TDCL_HOLD_KEY_CHANGELAYERTEMPO,
    // TDCL_TAP_KEY_CUSTOM,
    // TDCL_HOLD_KEY_CUSTOM,
} td_customlayer_action_t;

typedef struct {
    uint8_t repetitions;
    td_customlayer_action_t action;
    platform_keycode_t keycode;
    uint8_t layer;
} custom_action_custom_behaviour;

typedef struct {
    t_layer_status status;
    void *user_data;
    custom_behaviour_callback callback;
} custom_behaviour_config;

typedef struct {
    size_t length;
    custom_behaviour_config *layers[];
} custom_layers_struct;

#ifdef UNIT_TEST
// In test environment, custom_layers is defined in the test mock (qmk_test_mock.cpp)
extern custom_layers_struct *custom_layers;
#else
// In normal QMK environment, define it here
custom_layers_struct *custom_layers;
#endif

// --------------------------
typedef enum {
  _HOLD_STATE_NOT_SET,
  _DURING_HOLD_DECISION,
  _HOLD_INTERRUPTION_DETECTED,
  _HOLD_INTERRUPTION_AND_TRESHOLD_DETECTED,
  _HOLD_TRESHOLD_DETECTED,
} hold_state;

typedef enum {
  _HOLD_OR_TAP_STATE_NOT_SET,
  _DECIDING,
  _TAP,
  _HOLD,
  _NONE
} hold_or_tap_sequence;
// custom_layers_struct *custom_layers = NULL;

typedef enum {
  _PRESS_MAIN_KEY,
  _RELEASE_MAIN_KEY,
  _PRESS_OTHER_KEY,
  _RELEASE_OTHER_KEY,
} custom_key_event;

typedef struct {
    uint8_t count;
    bool hasholdaction : 1;
    bool haskeyaction : 1;
    uint8_t original_layer;
    uint8_t selected_layer;
    hold_or_tap_sequence sequence;
    hold_state state;
    uint8_t press_buffer_pos;
    uint8_t keybuffer_length;
    platform_deferred_token hold_span_reached_token;
    platform_keycode_t selected_keycode;
    platform_deferred_token key_repetition_span_exceeded_token;
    size_t actionslength;
    custom_action_custom_behaviour *actions[];
} custom_switch_layer_custom_data;

platform_keycode_t lastKeyTapped = 0;
platform_time_t lastKeyTappedTime = 0;
platform_keycode_t lastKeyUntapped = 0;
platform_time_t lastKeyUntappedTime = 0;

// Forward declarations
custom_action_custom_behaviour* get_action_tap_key_sendkey(uint8_t repetitions, custom_switch_layer_custom_data* custom_switch_layer_custom_data);

uint32_t hold_span_reached_timer(uint32_t trigger_time, void *cb_arg) {
    custom_switch_layer_custom_data *status = (custom_switch_layer_custom_data*)cb_arg;
    status->state = _HOLD_TRESHOLD_DETECTED;
    status->sequence = _HOLD;
    platform_layer_on(status->selected_layer);
    status->hold_span_reached_token = 0;
    return 0;
}

uint32_t key_repetition_span_exceeded_timer(uint32_t trigger_time, void *cb_arg) {
    custom_switch_layer_custom_data *status = (custom_switch_layer_custom_data*)cb_arg;
    status->key_repetition_span_exceeded_token = 0;
    // platform_log_debug("key_repetition_span_exceeded_timer - tap_code16_delay");
    platform_tap_code_delay(status->selected_keycode, 10);
    return 0;
}

custom_action_custom_behaviour* get_action_tap_key_sendkey(uint8_t repetitions, custom_switch_layer_custom_data* custom_switch_layer_custom_data) {
    custom_action_custom_behaviour* action;
    for (size_t i = 0; i < custom_switch_layer_custom_data->actionslength; i++)
    {
        action = custom_switch_layer_custom_data->actions[i];
        if (action->repetitions == repetitions && action->action == TDCL_TAP_KEY_SENDKEY) {
            return action;
        }
    }
    return NULL;
}

custom_action_custom_behaviour* get_action_hold_key_changelayertempo(uint8_t repetitions, custom_switch_layer_custom_data* custom_switch_layer_custom_data) {
    custom_action_custom_behaviour* action;
    for (size_t i = 0; i < custom_switch_layer_custom_data->actionslength; i++)
    {
        action = custom_switch_layer_custom_data->actions[i];
        if (action->repetitions == repetitions && action->action == TDCL_HOLD_KEY_CHANGELAYERTEMPO) {
            return action;
        }
    }
    return NULL;
}

bool is_there_actions_after(custom_switch_layer_custom_data* custom_switch_layer_custom_data, uint8_t repetitions) {
    custom_action_custom_behaviour* action;
    for (size_t i = 0; i < custom_switch_layer_custom_data->actionslength; i++)
    {
        action = custom_switch_layer_custom_data->actions[i];
        if (action->repetitions > repetitions) {
            return true;
        }
    }
    return false;
}

bool custom_switch_layer_custom_function (platform_keycode_t keycode, abskeyevent_t event, t_layer_status *status, void *user_data) {
    custom_switch_layer_custom_data *layer_status = (custom_switch_layer_custom_data *)user_data;
    if (layer_status->actionslength == 0) {
        //platform_log_debug("cslcd - exit because actionslength == 0");
        return false;
    }
    if (keycode == (status->keycodemodifier)) {
        if (event.pressed) {
            layer_status->state = _HOLD_STATE_NOT_SET;

            if (layer_status->sequence == _HOLD_OR_TAP_STATE_NOT_SET || layer_status->sequence == _NONE) {
                layer_status->count = 1;
            } else if (layer_status->sequence == _HOLD) {
                layer_status->count = 1;
            } else if (layer_status->sequence == _TAP) {
                if (is_there_actions_after(layer_status, layer_status->count - 1) == true) {
                    platform_cancel_deferred_exec(layer_status->key_repetition_span_exceeded_token);
                    layer_status->key_repetition_span_exceeded_token = 0;
                    layer_status->count = layer_status->count + 1;
                } else {
                    layer_status->count = 1;
                }
            } else if (layer_status->sequence == _DECIDING) {
                //platform_log_debug("Invalid state: layer_status->hold_or_tap_sequence == _DECIDING");
            }
            if (layer_status->count == 1) {
                layer_status->original_layer = 0;//get_layer_topdown(event.key);
            }
            layer_status->sequence = _DECIDING;

            custom_action_custom_behaviour* hold_action = get_action_hold_key_changelayertempo(layer_status->count - 1, layer_status);
            if (hold_action != NULL) {
                layer_status->state = _DURING_HOLD_DECISION;

                layer_status->hasholdaction = true;
                layer_status->selected_layer = hold_action->layer;
                layer_status->keybuffer_length = 0;
                layer_status->hold_span_reached_token = platform_defer_exec(g_tap_timeout, hold_span_reached_timer, layer_status);
            } else {
                layer_status->hasholdaction = false;
                layer_status->selected_layer = 0;
                layer_status->keybuffer_length = 0;
                layer_status->hold_span_reached_token = 0;
            }
            custom_action_custom_behaviour* tap_key_action = get_action_tap_key_sendkey(layer_status->count - 1, layer_status);
            if (tap_key_action != NULL) {
                layer_status->haskeyaction = true;
                layer_status->selected_keycode = tap_key_action->keycode;
            } else {
                layer_status->haskeyaction = false;
                layer_status->selected_keycode = 0;
            }
            // platform_log_debug("cslcd - pressed  : count: %u, hasholdaction: %u, haskeyaction %u, g_tap_timeout %u", layer_status->count, layer_status->hasholdaction, layer_status->haskeyaction, g_tap_timeout);
        } else {
            if (layer_status->state == _DURING_HOLD_DECISION) {
                platform_cancel_deferred_exec(layer_status->hold_span_reached_token);
                layer_status->hold_span_reached_token = 0;

                if (layer_status->haskeyaction) {
                    layer_status->sequence = _TAP;
                } else {
                    layer_status->sequence = _NONE;
                }
            } else if (layer_status->state == _HOLD_TRESHOLD_DETECTED) {
                platform_layer_off(layer_status->selected_layer);
                platform_clear_keyboard();
                // The value of layer_status->sequence has been set on the deferred execution to _HOLD
            } else if (layer_status->haskeyaction) {
                layer_status->sequence = _TAP;
            } else {
                layer_status->sequence = _NONE;
            }

            if (layer_status->sequence == _TAP && is_there_actions_after(layer_status, layer_status->count - 1) == true) {
                layer_status->key_repetition_span_exceeded_token = platform_defer_exec(g_tap_timeout, key_repetition_span_exceeded_timer, layer_status);
            }
            //platform_log_debug("cslcd - unpressed: count: %u, hasholdaction: %u, haskeyaction: %u, selected_keycode: %u", layer_status->count, layer_status->hasholdaction, layer_status->haskeyaction, layer_status->selected_keycode);
        }
        return false;
    } else {
        if (layer_status->sequence == _DECIDING) {
            // if (layer_status->keybuffer_length < 8)
            // {
            //     layer_status->keybuffer[layer_status->keybuffer_length].key = event.key;
            //     layer_status->keybuffer[layer_status->keybuffer_length].pressed = event.pressed;
            //     layer_status->keybuffer[layer_status->keybuffer_length].time = event.time;
            //     layer_status->keybuffer_length += 1;
            // }
            return false;
        } else if (layer_status->sequence == _HOLD) {
            //decide_over_keypressed(keycode, layer_status, false);
        } else if (layer_status->sequence == _TAP) {
            if (is_there_actions_after(layer_status, layer_status->count - 1) == true) {
                platform_cancel_deferred_exec(layer_status->key_repetition_span_exceeded_token);
                layer_status->key_repetition_span_exceeded_token = 0;
            }
            layer_status->sequence = _HOLD_OR_TAP_STATE_NOT_SET;
        }
    }
    return true;
}

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

#ifdef __cplusplus
extern "C" {
#endif

bool macros_process_key(platform_keycode_t keycode, abskeyevent_t abskeyevent) {
    if (abskeyevent.pressed) {
        lastKeyTapped = keycode;
        lastKeyTappedTime = abskeyevent.time;
    }
    else {
        lastKeyUntapped = keycode;
        lastKeyUntappedTime = abskeyevent.time;
    }
    //print_layer_status();
    //Esc and Enter should remove sticky keys... but what about other selections that use numbers or letters?
    //platform_log_debug("sizeof *custom_layers: %u, nelements: %u, sizeof *custom_layers->layers: %u, modifier1: %u, modifier2: %u", sizeof *custom_layers, nelements, sizeof *custom_layers->layers, CKC_LAY_MOUSE_Q, CKC_LAY_NUMBERS_R);
    for (uint8_t i = 0; i < custom_layers->length; i++)
    {
        //platform_log_debug("custom_layers_length: %u, keycodemodifier: %u", custom_layers->length, custom_layers->layers[i]->status.keycodemodifier);
        custom_behaviour_config *layer_status = custom_layers->layers[i];
        //platform_log_debug("keycodemodifier: %u", layer_status->status.keycodemodifier);
        if (layer_status->callback(keycode, abskeyevent, &layer_status->status, layer_status->user_data) == false) {
            return false;
        }
    }
    return true;
}

void* pipeline_tap_dance_initialize_user_data(void) {
    size_t nelements = 6;
    custom_layers = (custom_layers_struct*)malloc(sizeof *custom_layers + nelements * sizeof *custom_layers->layers);
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

void pipeline_tap_dance_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data) {
    // if (abskeyevent.pressed) {
    //     lastKeyTapped = keycode;
    //     lastKeyTappedTime = abskeyevent.time;
    // }
    // else {
    //     lastKeyUntapped = keycode;
    //     lastKeyUntappedTime = abskeyevent.time;
    // }
    // if (params->more_data == true) {
    //     if (params->up == true && params->is_press == true) {
    //         for (uint8_t i = 0; i < custom_layers->length; i++)
    //         {
    //             custom_behaviour_config *layer_status = custom_layers->layers[i];
    //             if (layer_status->status.keycodemodifier == params->keycode) {
    //                 config->needs_more_data = true;
    //             }
    //             // if (layer_status->callback(keycode, abskeyevent, &layer_status->status, layer_status->user_data) == false) return false;
    //         }
    //     }
    // } else {
    //     if (params->up == true && params->is_press == true) {
    //         #ifdef CONSOLE_ENABLE
    //             uprintf("MORE_DATA : 0x%04X\n", params->keycode);
    //         #endif
    //     }
    // }
}

#ifdef __cplusplus
}
#endif
