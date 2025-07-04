#include "pipeline_tap_dance.h"

#define g_tap_timeout 200

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

bool macros_process_key(platform_keycode_t keycode, abskeyevent_t abskeyevent, custom_layers_struct *custom_layers) {
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


#ifdef __cplusplus
extern "C" {
#endif
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
