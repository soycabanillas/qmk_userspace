#include "keymacros.h"
#include "abstractionsqmk.h"

typedef struct {
    bool first_time: 1;

    bool pressed: 1; //tap = true, untap = false

    uint8_t count; //number of times the key has been tapped in a row (within a time span between each tap)

    bool interrupted : 1;
    bool hasholdaction : 1;
    bool holdactionexecuted: 1;
    bool haskeyaction : 1;
    bool keyactionexecuted: 1;

    uint8_t oneshot_mods;
    uint8_t original_layer;

    bool layerchanged: 1; //
    uint8_t selected_layer;
    abskeyevent_t keybuffer[8];
    uint8_t keybuffer_length;
    bool duringholddecision : 1; //once the key has been tapped, it must be decided it the tap will transtion to a hold or not
    deferred_token hold_span_reached_token;

    uint16_t selected_keycode;
    bool duringkeydecision : 1;
    deferred_token key_repetition_span_exceeded_token;

    size_t actionslength;
    custom_action_custom_behaviour *actions[];
} custom_switch_layer_custom_data;

uint16_t lastKeyTapped = 0;
uint16_t lastKeyTappedTime = 0;
uint16_t lastKeyUntapped = 0;
uint16_t lastKeyUntappedTime = 0;

custom_layers_struct *custom_layers = NULL;

#define ABS_KEYBUFFER_MAX 256
abskeyevent_t abskeybuffer[ABS_KEYBUFFER_MAX];
uint8_t abskeybuffer_pos = 0;

uint32_t hold_span_reached_timer(uint32_t trigger_time, void *cb_arg) {
// #ifdef CONSOLE_ENABLE
//   uprintf("hold_span_reached_timer\n");
// #endif
    custom_switch_layer_custom_data *status = (custom_switch_layer_custom_data*)cb_arg;
    status->holdactionexecuted = true;
    layer_on(status->selected_layer);
    status->layerchanged = true;
    status->duringholddecision = false;
    status->hold_span_reached_token = 0;
    print_buffer(status->selected_layer, status->keybuffer, status->keybuffer_length);
    //print_buffer(status->selected_layer, &abskeybuffer[abskeybuffer_pos], ABS_KEYBUFFER_MAX - abskeybuffer_pos);
    return 0;
}

uint32_t key_repetition_span_exceeded_timer(uint32_t trigger_time, void *cb_arg) {
// #ifdef CONSOLE_ENABLE
//   uprintf("key_repetition_span_exceeded_timer\n");
// #endif
    custom_switch_layer_custom_data *status = (custom_switch_layer_custom_data*)cb_arg;
    status->keyactionexecuted = true;
    status->duringkeydecision = false;
    status->key_repetition_span_exceeded_token = 0;
    uint8_t mods = get_mods();
    register_mods(status->oneshot_mods);
    uprintf("key_repetition_span_exceeded_timer - tap_code16_delay\n");
    tap_code16_delay(status->selected_keycode, 10);
    //tap_code16(status->selected_keycode);
    set_mods(mods);
    print_buffer(status->original_layer, status->keybuffer, status->keybuffer_length);
    //print_buffer(status->original_layer, &abskeybuffer[abskeybuffer_pos], ABS_KEYBUFFER_MAX - abskeybuffer_pos);
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

void decide_over_keypressed(custom_switch_layer_custom_data* status, bool check_next_action) {
    if (status->duringkeydecision) {
        status->duringkeydecision = false;
        uprintf("decide_over_keypressed - cancel_deferred_exec\n");
        cancel_deferred_exec(status->key_repetition_span_exceeded_token);
        status->key_repetition_span_exceeded_token = 0;
    }
    if (check_next_action && is_there_actions_after(status, status->count - 1) == true){
        status->duringkeydecision = true;
        uprintf("decide_over_keypressed - defer_exec\n");
        status->key_repetition_span_exceeded_token = defer_exec(g_tapping_term, key_repetition_span_exceeded_timer, status);
    } else {
        status->keyactionexecuted = true;
        uint8_t mods = get_mods();
        register_mods(status->oneshot_mods);
        uprintf("decide_over_keypressed - tap_code16_delay\n");
        tap_code16_delay(status->selected_keycode, 10);
        //tap_code16(layer_status->selected_keycode);
        set_mods(mods);
        print_buffer(status->original_layer, status->keybuffer, status->keybuffer_length);
        //print_buffer(status->original_layer, &abskeybuffer[abskeybuffer_pos], ABS_KEYBUFFER_MAX - abskeybuffer_pos);
    }
}

bool custom_switch_layer_custom_function (uint16_t keycode, abskeyevent_t event, t_layer_status *status, void *user_data) {
    custom_switch_layer_custom_data *layer_status = (custom_switch_layer_custom_data *)user_data;
    //uprintf("keycode: %u, keycodemodifier: %u\n", keycode, status->keycodemodifier);
    if (layer_status->actionslength == 0) {
        uprintf("cslcd - exit becase actionslength == 0");
        return false;
    }
    if (keycode == (status->keycodemodifier)) {
        if (event.pressed) {
            layer_status->pressed = true;
            uint16_t time_since_last_untapped = timer_elapsed(lastKeyUntappedTime);
            if (layer_status->first_time == false &&
                keycode == lastKeyTapped &&
                keycode == lastKeyUntapped &&
                layer_status->holdactionexecuted == false &&
                layer_status->keyactionexecuted == false &&
                time_since_last_untapped < g_tapping_term &&
                is_there_actions_after(layer_status, layer_status->count - 1))
            {
                layer_status->count = layer_status->count + 1;
            } else {
                layer_status->first_time = false;

                layer_status->count = 1;

                layer_status->interrupted = false;
                layer_status->holdactionexecuted = false;
                layer_status->keyactionexecuted = false;

                layer_status->oneshot_mods = get_oneshot_mods();
                layer_status->original_layer = get_layer_topdown(event.key);

                layer_status->duringkeydecision = false;
            }
            custom_action_custom_behaviour* hold_action = get_action_hold_key_changelayertempo(layer_status->count - 1, layer_status);
            if (hold_action != NULL) {
                layer_status->hasholdaction = true;
                layer_status->layerchanged = false;
                layer_status->selected_layer = hold_action->layer;
                layer_status->keybuffer_length = 0;
                layer_status->duringholddecision = true;
                layer_status->hold_span_reached_token = defer_exec(g_tapping_term, hold_span_reached_timer, layer_status);
            } else {
                layer_status->hasholdaction = false;
                layer_status->layerchanged = false;
                layer_status->selected_layer = 0;
                layer_status->keybuffer_length = 0;
                layer_status->duringholddecision = false;
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
            uprintf("cslcd - pressed  : count: %u, hasholdaction: %u, haskeyaction %u, time_since_last_untapped %u, g_tapping_term %u\n", layer_status->count, layer_status->hasholdaction, layer_status->haskeyaction, time_since_last_untapped, g_tapping_term);
        } else {
            layer_status->pressed = false;
            if (layer_status->hasholdaction) {
                if (layer_status->layerchanged) {
                    layer_off(layer_status->selected_layer);
                    clear_keyboard();
                    layer_status->layerchanged = false;
                } else {
                    if (layer_status->duringholddecision) {
                        layer_status->layerchanged = false;
                        cancel_deferred_exec(layer_status->hold_span_reached_token);
                        layer_status->duringholddecision = false;
                        layer_status->hold_span_reached_token = 0;
                        if (layer_status->haskeyaction && layer_status->keyactionexecuted == false) {
                            uprintf("decide_over_keypressed: unpressed: hasholdaction and layer not changed and duringholddecision\n");
                            decide_over_keypressed(layer_status, true);
                        }
                    }
                }
            } else if (layer_status->haskeyaction && layer_status->keyactionexecuted == false) {
                uprintf("decide_over_keypressed: unpressed: haskeyaction\n");
                decide_over_keypressed(layer_status, true);
            }
            uprintf("cslcd - unpressed: count: %u, hasholdaction: %u, haskeyaction: %u, layerchanged: %u, duringholddecision: %u, duringkeydecision: %u, selected_keycode: %u\n", layer_status->count, layer_status->hasholdaction, layer_status->haskeyaction, layer_status->layerchanged, layer_status->duringholddecision, layer_status->duringkeydecision, layer_status->selected_keycode);
        }
        return false;
    } else {
        if (layer_status->pressed) {
            uprintf("cslcd - layer pressed: count: %u, hasholdaction: %u, haskeyaction: %u, layerchanged: %u, duringholddecision: %u, duringkeydecision: %u\n", layer_status->count, layer_status->hasholdaction, layer_status->haskeyaction, layer_status->layerchanged, layer_status->duringholddecision, layer_status->duringkeydecision);
            layer_status->interrupted = true;
            if (layer_status->hasholdaction) {
                if (layer_status->duringholddecision) {
                    if (layer_status->keybuffer_length < 8)
                    {
                        layer_status->keybuffer[layer_status->keybuffer_length].key = event.key;
                        layer_status->keybuffer[layer_status->keybuffer_length].pressed = event.pressed;
                        layer_status->keybuffer[layer_status->keybuffer_length].time = event.time;
                        layer_status->keybuffer_length += 1;
                    }
                    return false;
                }
                if (layer_status->layerchanged && event.pressed == false) {
                    // This is used to unregister a keycode after the layer switching keycode as been released.
                    // It should be check if a holding has been detected and another key was pressed. If so,
                    // the release of that key or keys should be directed to the new layer, not the original one
                    return flush_keybuffer_to_new_layer(layer_status->selected_layer, event.key, layer_status->keybuffer, layer_status->keybuffer_length);
                }
            } else if (layer_status->haskeyaction && layer_status->keyactionexecuted == false) {
                #ifdef CONSOLE_ENABLE
                uprintf("decide_over_keypressed: interrupted\n");
                #endif
                decide_over_keypressed(layer_status, false);
                return true;
            }
        }
        if (layer_status->duringkeydecision) {
            if (layer_status->keybuffer_length < 8)
            {
                layer_status->keybuffer[layer_status->keybuffer_length].key = event.key;
                layer_status->keybuffer[layer_status->keybuffer_length].pressed = event.pressed;
                layer_status->keybuffer[layer_status->keybuffer_length].time = event.time;
                layer_status->keybuffer_length += 1;
            }
            return false;
        }
    }
    return true;
}

custom_action_custom_behaviour* createbehaviouraction(uint8_t repetitions, td_customlayer_action_t action, uint16_t keycode, uint8_t layer) {
    custom_action_custom_behaviour behaviouraction = {
        .repetitions = repetitions,
        .action = action,
        .keycode = keycode,
        .layer = layer
    };
    custom_action_custom_behaviour* allocation = malloc(sizeof behaviouraction);
    memcpy(allocation, &behaviouraction, sizeof behaviouraction);
    return allocation;
}

custom_behaviour_config* createbehaviour(uint16_t keycodemodifier, custom_action_custom_behaviour* actions[], size_t actionslength) {
    custom_switch_layer_custom_data userdata = {
        .first_time = true
    };
    userdata.actionslength = actionslength;
    custom_switch_layer_custom_data* allocationuserdata = malloc(sizeof(custom_switch_layer_custom_data) + actionslength * sizeof (custom_action_custom_behaviour*));
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
    custom_behaviour_config* allocation = malloc(sizeof behaviour);
    memcpy(allocation, &behaviour, sizeof behaviour);
    return allocation;
}

bool macros_process_key(uint16_t keycode, abskeyevent_t abskeyevent) {
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
    //uprintf("sizeof *custom_layers: %u, nelements: %u, sizeof *custom_layers->layers: %u, modifier1: %u, modifier2: %u\n", sizeof *custom_layers, nelements, sizeof *custom_layers->layers, CKC_LAY_MOUSE_Q, CKC_LAY_NUMBERS_R);
    for (uint8_t i = 0; i < custom_layers->length; i++)
    {
        //uprintf("custom_layers_length: %u, keycodemodifier: %u\n", custom_layers->length, custom_layers->layers[i]->status.keycodemodifier);
        custom_behaviour_config *layer_status = custom_layers->layers[i];
        //uprintf("keycodemodifier: %u\n", layer_status->status.keycodemodifier);
        if (layer_status->callback(keycode, abskeyevent, &layer_status->status, layer_status->user_data) == false) return false;
    }
    return true;
}
