#include "platform_qmk.h"

#ifdef PLATFORM_QMK

void platform_register_code(platform_keycode_t keycode) {
    register_code(keycode);
}

void platform_unregister_code(platform_keycode_t keycode) {
    unregister_code(keycode);
}

void platform_tap_code(platform_keycode_t keycode) {
    tap_code(keycode);
}

bool platform_is_key_pressed(platform_keyrecord_t *record) {
    return record->event.pressed;
}

void platform_add_key(platform_keycode_t keycode) {
    add_key(keycode);
}

void platform_del_key(platform_keycode_t keycode) {
    del_key(keycode);
}

void platform_send_keyboard_report(void) {
    send_keyboard_report();
}

void platform_clear_keyboard(void) {
    clear_keyboard();
}

bool platform_is_modifier(platform_keycode_t keycode) {
    return IS_MODIFIER_KEYCODE(keycode);
}

platform_keycode_t platform_get_mods(void) {
    return get_mods();
}

void platform_set_mods(platform_keycode_t mods) {
    set_mods(mods);
}

void platform_add_mods(platform_keycode_t mods) {
    add_mods(mods);
}

void platform_del_mods(platform_keycode_t mods) {
    del_mods(mods);
}

#endif // PLATFORM_QMK
