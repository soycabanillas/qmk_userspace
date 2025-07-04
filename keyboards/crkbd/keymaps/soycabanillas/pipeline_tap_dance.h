#pragma once

#include "key_buffer.h"

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

bool custom_switch_layer_custom_function (platform_keycode_t keycode, abskeyevent_t event, t_layer_status *status, void *user_data);
void pipeline_tap_dance_callback(pipeline_callback_params_t* params, pipeline_config_t* config, void* user_data);
