#pragma once

#include QMK_KEYBOARD_H
#include "commons.h"

typedef bool (*is_pressed)(uint16_t);

typedef struct {
    is_pressed is_pressed;
} pipeline_info_t;

typedef struct {
    bool up;
    uint16_t keycode;
    keypos_t key;
    bool is_press;
    pipeline_info_t info;
} pipeline_callback_params_t;

typedef struct {
    // bool remove_key_capture;
    // bool ignore_releases_from_presses_before_macro_activation;
    // bool ignore_presses_after_macro_key_release;
} pipeline_config_t;

typedef void (*pipeline_callback)(pipeline_callback_params_t*, pipeline_config_t*, void*);

typedef struct {
    pipeline_callback callback;
    void* data;
} pipeline_definition_t;

typedef struct {
    pipeline_definition_t* definition;
} pipeline_t;

typedef struct {
    size_t length;
    pipeline_t *pipelines[];
} pipeline_array_t;


pipeline_array_t *pipeline_array;
bool pipeline_process_key(uint16_t keycode, abskeyevent_t abskeyevent);
pipeline_t* add_pipeline(pipeline_callback callback, void* user_data);
