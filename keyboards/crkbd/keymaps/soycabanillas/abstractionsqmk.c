#include "abstractionsqmk.h"
#include "commons.h"
#include "limits.h"
#include "quantum.h"

void print_buffer(uint8_t layer_num, abskeyevent_t keybuffer[8], uint8_t keybuffer_length) {
    #ifdef CONSOLE_ENABLE
        uprintf("print_buffer: length:%u", keybuffer_length);
    #endif
    for (uint8_t i = 0; i < keybuffer_length; i++)
    {
        uint16_t fallthrough = keymap_key_to_keycode(layer_num, keybuffer[i].key);
        #ifdef CONSOLE_ENABLE
            uprintf(" [%u]:0x%04X-%u", i, fallthrough, keybuffer[i].pressed);
        #endif
        if (keybuffer[i].pressed) {
            register_code16(fallthrough);
            // register_code(fallthrough);
            wait_ms(10);
        } else {
            unregister_code16(fallthrough);
            // unregister_code(fallthrough);
            wait_ms(10);
        }
    }
    #ifdef CONSOLE_ENABLE
        uprintf("\n");
    #endif
}

uint8_t get_layer_topdown(keypos_t key) {
    return layer_switch_get_layer(key);
}

bool flush_keybuffer_to_new_layer(uint8_t layer_num, keypos_t keyfromevent, abskeyevent_t keybuffer[8], uint8_t keybuffer_length) {
    for (uint8_t di = keybuffer_length; di > 0; di--)
    {
        uint8_t i = di - 1;
        if (KEYS_ARE_EQUAL(keybuffer[i].key, keyfromevent) && keybuffer[i].pressed) {
            // keypos_t key = {
            //     .col = keybuffer[i].key.col,
            //     .row = keybuffer[i].key.row
            // };
            // uint16_t fallthrough = keymap_key_to_keycode(layer, key);
            uint16_t fallthrough = keymap_key_to_keycode(layer_num, keybuffer[i].key);
            #ifdef CONSOLE_ENABLE
            uprintf("post_process unregister code:%u\n", fallthrough);
            #endif
            unregister_code16(fallthrough);
            //unregister_code(fallthrough);
            wait_ms(10);
            // send_keyboard_report();
            keybuffer[i].key.col = UCHAR_MAX;
            keybuffer[i].key.row = UCHAR_MAX;
            return false;
        } else if (KEYS_ARE_EQUAL(keybuffer[i].key, keyfromevent) && keybuffer[i].pressed == false) {
            break;
        }
    }
    return true;
}
