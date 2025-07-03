#include "key_buffer.h"
#include "platform_qmk.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    platform_keypos_t key;
    platform_keycode_t keycode;
    uint8_t layer;
    bool release_on_buffer; //This flag marks that the key has been released, but the key is not erased until the buffer is processed. This way, info like the time the key was pressed, or the keycode can be obtained.
    platform_time_t time;
} only_press_buffer_item_t;

typedef struct {
    platform_keypos_t key;
    platform_keycode_t keycode;
    uint8_t layer;
    bool is_press;
    platform_time_t time;
} press_buffer_item_t;

#define ONLY_PRESS_BUFFER_MAX 5
only_press_buffer_item_t only_press_buffer[ONLY_PRESS_BUFFER_MAX];
uint8_t only_press_buffer_pos = 0;

#define PRESS_BUFFER_MAX 10
press_buffer_item_t press_buffer[PRESS_BUFFER_MAX];
uint8_t press_buffer_pos = 0;

uint8_t current_layer = 0;

bool info_is_pressed(platform_keycode_t keycode){
    for (size_t i = only_press_buffer_pos; i-- > 0;)
    {
        if (only_press_buffer[i].keycode == keycode) {
            return true;
        }
    }
    return false;
}

void print_press_buffers(int16_t num_keys) {
    uint8_t num_keys_to_print = num_keys;
    if (num_keys_to_print > ONLY_PRESS_BUFFER_MAX) num_keys_to_print = ONLY_PRESS_BUFFER_MAX;
    platform_log_debug("-only -  pos: %2u || ", only_press_buffer_pos);
    for (size_t i = 0; i < num_keys_to_print; i++)
    {
        platform_log_debug("col: %3u row: %2u key:0x%04X || ", only_press_buffer[i].key.col, only_press_buffer[i].key.row, only_press_buffer[i].keycode);
    }
    platform_log_debug("");

    if (num_keys_to_print > PRESS_BUFFER_MAX) num_keys_to_print = PRESS_BUFFER_MAX;
    platform_log_debug("-press-  pos: %2u || ", press_buffer_pos);
    for (size_t i = 0; i < num_keys_to_print; i++)
    {
        platform_log_debug("col: %3u row: %2u key:0x%04X || ", press_buffer[i].key.col, press_buffer[i].key.row, press_buffer[i].keycode);
    }
    platform_log_debug("");
}

// Add the key press or release to the press_buffer buffer.
// Add the key to the only_press_buffer when is a pressed or update the key to set the key as released.
// This buffers can be consumed after several key presses and releases. The same key (position) can be pressed and released and stored on this buffers. This is needed for example if a pipeline requires the information of repetition of a key.
bool add_to_press_buffer(platform_keycode_t keycode, platform_keypos_t key, platform_time_t time, bool is_press) {
    // Checks the available space in press_buffer.
    // If there is a press, ensure that the buffer has enough space to store both the current key press and a future key release. Otherwise, if the buffer becomes full, releasing keys will be impossible.
    // This only applies in basic situations, such as when too many keys are pressed simultaneously. If a pipeline fails to remove keys from the buffer, it can still become full.
    if ((is_press == true && press_buffer_pos + 1 < PRESS_BUFFER_MAX) || (is_press == false && press_buffer_pos < PRESS_BUFFER_MAX)) {
        // Before adding the key to the press_buffer, make sure there is space available in only_press_buffer.
        if (is_press == true) {
            if (only_press_buffer_pos < ONLY_PRESS_BUFFER_MAX) {
                only_press_buffer[only_press_buffer_pos].key.col = key.col;
                only_press_buffer[only_press_buffer_pos].key.row = key.row;
                only_press_buffer[only_press_buffer_pos].keycode = keycode;
                only_press_buffer[only_press_buffer_pos].layer = current_layer;
                only_press_buffer[only_press_buffer_pos].release_on_buffer = false;
                only_press_buffer[only_press_buffer_pos].time = time;
                only_press_buffer_pos = only_press_buffer_pos + 1;
            } else {
                return false;
            }
        } else {
            //Check if the new release has a press already buffered
            bool key_is_in_buffer = false;
            for (size_t i = only_press_buffer_pos; i-- > 0;)
            {
                if (only_press_buffer[i].key.col == key.col && only_press_buffer[i].key.row == key.row && only_press_buffer[i].release_on_buffer == false) {
                    only_press_buffer[i].release_on_buffer = true;
                    key_is_in_buffer = true;
                    break;
                }
            }
            if (key_is_in_buffer == false) return false;
        }
        press_buffer[press_buffer_pos].key.col = key.col;
        press_buffer[press_buffer_pos].key.row = key.row;
        press_buffer[press_buffer_pos].keycode = keycode;
        press_buffer[press_buffer_pos].layer = current_layer;
        press_buffer[press_buffer_pos].is_press = is_press;
        press_buffer[press_buffer_pos].time = time;
        press_buffer_pos = press_buffer_pos + 1;
        return true;
    }
    return false;
}

void remove_from_press_buffer(uint8_t pos) {
    if (press_buffer[pos].is_press == false) {
        for (size_t i = only_press_buffer_pos; i-- > 0;)
        {
            if (only_press_buffer[i].key.col == press_buffer[pos].key.col && only_press_buffer[i].key.row == press_buffer[pos].key.row) {
                if (i < only_press_buffer_pos - 1) {
                    memcpy(&only_press_buffer[i], &only_press_buffer[i + 1], sizeof(only_press_buffer_item_t) * (only_press_buffer_pos - 1 - i));
                }
                only_press_buffer_pos = only_press_buffer_pos - 1;
                break;
            }
        }
    }

    if (pos < press_buffer_pos - 1) {
        memcpy(&press_buffer[pos], &press_buffer[pos + 1], sizeof(press_buffer_item_t) * (press_buffer_pos - 1 - pos));
    }
    press_buffer_pos = press_buffer_pos - 1;
}

// bool check_if_release_is_before_press(abskeypos_t key_release, abskeypos_t key_press) {
//     //It just checks the order of the keys (if they exists) on the only_press_buffer.
//     //It will return true for key_release="A" and key_press="B" if called at the end of the sequence "press A" -> "press B" -> "release A".
//     //In this case, the only_press_buffer will have ["A", "B"] when the function is called
//     //It will return true for key_release="A" and key_press="B" if called at the end of the sequence "press B" -> "press A" -> "release A" because on the only_press_buffer there will be ["A", "B"]
//     //In this case, the only_press_buffer will have ["B", "A"] when the function is called
//     for (size_t i = 0; i < only_press_buffer_pos; i++)
//     {
//         if (only_press_buffer[i].key.col == key_press.col && only_press_buffer[i].key.row == key_press.row) return false;
//         if (only_press_buffer[i].key.col == key_release.col && only_press_buffer[i].key.row == key_release.row) return true;
//     }
//     return false;
// }

void execute_pipeline(bool up, uint16_t callback_time, uint8_t macro_buffer_pos, press_buffer_item_t* press_buffer_selected) {
    pipeline_callback_params_t callback_params;
    callback_params.up = up;
    if (callback_time == 0) {
        callback_params.keycode = press_buffer_selected->keycode;
        callback_params.key = press_buffer_selected->key;
        if (press_buffer_selected->is_press == true) {
            callback_params.callback_type = PIPELINE_CALLBACK_KEY_PRESS;
        } else {
            callback_params.callback_type = PIPELINE_CALLBACK_KEY_RELEASE;
        }
        callback_params.info.is_pressed = &info_is_pressed;
    } else {
        callback_params.callback_type = PIPELINE_CALLBACK_TIMER;
    }
    pipeline_config_t config;
    config.callback_time = 0;
    pipeline_array->pipelines[macro_buffer_pos]->definition->callback(&callback_params, &config, pipeline_array->pipelines[macro_buffer_pos]->definition->data);
}

//bool process_timer(void) {
//    execute_pipeline(true, i, 15, NULL);
//}

bool process_key_pool(void) {
    bool further_process_required = true;
    while (press_buffer_pos > 0) {
        press_buffer_item_t* press_buffer_selected = &press_buffer[0];
        //check first if there are pipelines waiting for more data
        for (size_t i = 0; i < pipeline_array->length; i++)
        {
            execute_pipeline(true, 0, i, press_buffer_selected);
        }

        if (press_buffer_selected->keycode <= 0xFF) {
            platform_keycode_t fallthrough = press_buffer_selected->keycode;
            if (press_buffer[0].is_press == true) {
                platform_register_code(fallthrough);
                platform_wait_ms(10);
            } else {
                platform_unregister_code(fallthrough);
                platform_wait_ms(10);
            }
            further_process_required = false;
        }

        for (size_t i = pipeline_array->length; i-- > 0;)
        {
            execute_pipeline(false, 0, i, press_buffer_selected);
        }

        remove_from_press_buffer(0);
    }
    print_press_buffers(10);
    return further_process_required;
}

bool pipeline_process_key(platform_keycode_t keycode, abskeyevent_t abskeyevent) {
    if (add_to_press_buffer(keycode, abskeyevent.key, abskeyevent.time, abskeyevent.pressed)) {
        return process_key_pool();
    }
    return true;
}

pipeline_t* add_pipeline(pipeline_callback callback, void* user_data) {
    pipeline_definition_t* definition = malloc(sizeof(pipeline_definition_t));
    definition->callback = callback;
    definition->data = user_data;

    pipeline_t* pipeline;
    pipeline = malloc(sizeof(pipeline_t));
    pipeline->definition = definition;

    return pipeline;
}
