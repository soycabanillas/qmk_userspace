#pragma once

#include <stddef.h>
#include "commons.h"

// typedef bool (*is_pressed)(uint16_t);
// typedef void (*remove_from_press_buffer)(uint16_t);

typedef enum {
    PIPELINE_CALLBACK_KEY_PRESS,
    PIPELINE_CALLBACK_KEY_RELEASE,
    PIPELINE_CALLBACK_TIMER
} pipeline_callback_type_t;

typedef bool (*is_pressed)(uint16_t);

typedef struct {
    is_pressed is_pressed;
} pipeline_info_t;

typedef struct {
    bool up;
    uint16_t keycode;
    keypos_t key;
    pipeline_callback_type_t callback_type;
    pipeline_info_t info;
} pipeline_callback_params_t;

typedef struct {
    uint16_t callback_time;
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
