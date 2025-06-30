#include "keymacros.h"
#include "abstractionsqmk.h"

typedef struct {
    keypos_t key;
    uint16_t keycode;
    bool release_on_buffer;
    uint16_t time;
} only_press_buffer_item_t;

typedef struct {
    keypos_t key;
    uint16_t keycode;
    bool is_press;
    uint16_t time;
} press_buffer_item_t;

#define ONLY_PRESS_BUFFER_MAX 5
only_press_buffer_item_t only_press_buffer[ONLY_PRESS_BUFFER_MAX];
uint8_t only_press_buffer_pos = 0;

#define PRESS_BUFFER_MAX 10
press_buffer_item_t press_buffer[PRESS_BUFFER_MAX];
uint8_t press_buffer_pos = 0;

uint8_t current_layer = 0;

bool info_is_pressed(uint16_t keycode){
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
    uprintf("-only -  pos: %2u || ", only_press_buffer_pos);
    for (size_t i = 0; i < num_keys_to_print; i++)
    {
        uprintf("col: %3u row: %2u key:0x%04X || ", only_press_buffer[i].key.col, only_press_buffer[i].key.row, only_press_buffer[i].keycode);
    }
    uprintf("\n");

    if (num_keys_to_print > PRESS_BUFFER_MAX) num_keys_to_print = PRESS_BUFFER_MAX;
    uprintf("-press-  pos: %2u || ", press_buffer_pos);
    for (size_t i = 0; i < num_keys_to_print; i++)
    {
        uprintf("col: %3u row: %2u key:0x%04X || ", press_buffer[i].key.col, press_buffer[i].key.row, press_buffer[i].keycode);
    }
    uprintf("\n");
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

void execute_pipeline(bool up, uint8_t macro_buffer_pos, press_buffer_item_t* press_buffer_selected) {
    // if (macro_selected.macro_config.ignore_releases_from_presses_before_macro_activation == true && check_if_release_is_before_press(press_buffer_selected->key, macro_selected->key)) return;
    pipeline_callback_params_t callback_params;
    callback_params.up = up;
    callback_params.keycode = press_buffer_selected->keycode;
    callback_params.key = press_buffer_selected->key;
    callback_params.is_press = press_buffer_selected->is_press;
    callback_params.info.is_pressed = &info_is_pressed;
    pipeline_config_t config;
    pipeline_array->pipelines[macro_buffer_pos]->definition->callback(&callback_params, &config, pipeline_array->pipelines[macro_buffer_pos]->definition->data);
    // if (macro_selected.macro_config.remove_key_capture) remove_from_macro_buffer_by_pos(macro_buffer_pos);
}

bool process_key_pool(void) {
    bool further_process_required = true;
    while (press_buffer_pos > 0) {
        press_buffer_item_t* press_buffer_selected = &press_buffer[0];
        for (size_t i = 0; i < pipeline_array->length; i++)
        {
            execute_pipeline(true, i, press_buffer_selected);
        }

        if (press_buffer_selected->keycode <= 0xFF) {
            #ifdef CONSOLE_ENABLE
                uprintf("basic_key 0x%04X\n", press_buffer_selected->keycode);
            #endif
            // keypos_t key = {
            //     .col = press_buffer_selected->key.col,
            //     .row = press_buffer_selected->key.row
            // };
            // uint16_t fallthrough = keymap_key_to_keycode(10, key);
            #ifdef CONSOLE_ENABLE
                uint16_t fallthrough2 = keymap_key_to_keycode(0, press_buffer[0].key);
                uprintf("SHOULD BE : 0x%04X\n", fallthrough2);
            #endif
            uint16_t fallthrough = press_buffer_selected->keycode;
            if (press_buffer[0].is_press == true) {
                register_code(fallthrough);
                wait_ms(10);
            } else {
                unregister_code(fallthrough);
                wait_ms(10);
            }
            further_process_required = false;
        }

        for (size_t i = pipeline_array->length; i-- > 0;)
        {
            execute_pipeline(false, i, press_buffer_selected);
        }

        remove_from_press_buffer(0);
    }
    #ifdef CONSOLE_ENABLE
        print_press_buffers(10);
    #endif
    return further_process_required;
}

bool add_to_only_press_buffer(uint16_t keycode, keypos_t key, uint16_t time, bool is_press) {
    if (is_press == true) {
        if (only_press_buffer_pos < ONLY_PRESS_BUFFER_MAX) {
            only_press_buffer[only_press_buffer_pos].key.col = key.col;
            only_press_buffer[only_press_buffer_pos].key.row = key.row;
            only_press_buffer[only_press_buffer_pos].keycode = keycode;
            only_press_buffer[only_press_buffer_pos].release_on_buffer = false;
            only_press_buffer[only_press_buffer_pos].time = time;
            only_press_buffer_pos = only_press_buffer_pos + 1;
            return true;
        } else {
            return false;
        }
    } else {
        //Check if the new release has a press already buffered
        for (size_t i = only_press_buffer_pos; i-- > 0;)
        {
            if (only_press_buffer[i].key.col == key.col && only_press_buffer[i].key.row == key.row && only_press_buffer[i].release_on_buffer == false) {
                only_press_buffer[i].release_on_buffer = true;
                return true;
            }
        }
        return false;
    }
}

bool add_to_press_buffer(uint16_t keycode, keypos_t key, uint16_t time, bool is_press) {
    if ((is_press == true && press_buffer_pos + 1 < PRESS_BUFFER_MAX) || (is_press == false && press_buffer_pos < PRESS_BUFFER_MAX)) {
        if (add_to_only_press_buffer(keycode, key, time, is_press)) {
            press_buffer[press_buffer_pos].key.col = key.col;
            press_buffer[press_buffer_pos].key.row = key.row;
            press_buffer[press_buffer_pos].keycode = keycode;
            press_buffer[press_buffer_pos].is_press = is_press;
            press_buffer[press_buffer_pos].time = time;
            press_buffer_pos = press_buffer_pos + 1;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool pipeline_process_key(uint16_t keycode, abskeyevent_t abskeyevent) {
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
