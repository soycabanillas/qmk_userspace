/* Copyright 2015-2021 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "modules/soycabanillas/src/monkeyboard_deferred_callbacks.h"
#include "modules/soycabanillas/src/pipeline_executor.h"
#include "modules/soycabanillas/src/platform_types.h"
#include QMK_KEYBOARD_H
// #include "print.h"
#include "commons.h"
#include "keyboard_setup.h"





const uint16_t PROGMEM keymaps[][1][1] = {{{0}}};

const platform_keycode_t PROGMEM my_keymaps[][8][6] = {
/* _NAME
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
/* Qwerty
 * ,-----------------------------------------------------------------------------------.
 * | Esc  |   Q  |   W  |   E  | R/Num|   T  |   Y  |   U  |I/Medi|   O  |   P  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |R_Thum|A/Mous|   S  |   D  | F/Mov|   G  |   H  |   J  |   K  |   L  |;/Mov2|  '   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  | V/Fun|   B  |   N  |   M  |   ,  |   .  |   /  |Shift |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Gui  | Ctrl | Alt  | Shift|M_LTHU|Sh/ESC| Space|MO(RT)| Shift| Alt  | Ctrl | Caps |
 * `-----------------------------------------------------------------------------------'
 */
        [_LQWERTY] = LAYOUT_split_3x6_3(
            KC_ESC,  KC_Q, KC_W, KC_E, CKC_LAY_NUMBERS_R,      KC_T,               KC_Y,   KC_U,   KC_I,                KC_O,   KC_P,                        KC_BSPC,
            KC_NO,   KC_A, KC_S, KC_D, CKC_LAY_MOVEMENT_F,     KC_G,               KC_H,   KC_J,   KC_K,                KC_L,   CKC_LAY_MOVEMENT_RIGHT_SCLN, KC_QUOT,
            KC_LSFT, KC_Z, KC_X, KC_C, CKC_LAY_FUNCTIONKEYS_V, KC_B,               KC_N,   KC_M,   KC_COMM,             KC_DOT, KC_SLSH,                     KC_RSFT,
                                       KC_LSFT,                CKC_LAY_LEFT_THUMB, KC_ENT, KC_SPC, CKC_LAY_RIGHT_THUMB, KC_RCTL
        ),
/* _LLEFT_THUMB
 * ,-----------------------------------------------------------------------------------.
 * |      |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |OsLCTL|OsLALT|OsLSHI|OsLGUI|      |      |OsRGUI|OsRSHI|OsRALT|OsRCTL|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | TRAN | TRAN | TRAN | TRAN | TRAN |      | Enter|MO(BT)| TRAN | TRAN | TRAN | TRAN |
 * `-----------------------------------------------------------------------------------'
 */
        [_LLEFT_THUMB] = LAYOUT_split_3x6_3(
            KC_NO, KC_1                 , KC_2   , KC_3   , KC_4   , KC_5 , KC_6               , KC_7             , KC_8         , KC_9         , KC_0         , KC_BSPC,
            KC_NO, CKC_ONESHOT_MODIFIER_LEFT_CTRL, CKC_ONESHOT_MODIFIER_LEFT_ALT, CKC_ONESHOT_MODIFIER_LEFT_SHIFT, CKC_ONESHOT_MODIFIER_LEFT_GUI, KC_NO, KC_NO, CKC_ONESHOT_MODIFIER_RIGHT_GUI, CKC_ONESHOT_MODIFIER_RIGHT_SHIFT, CKC_ONESHOT_MODIFIER_RIGHT_ALT, CKC_ONESHOT_MODIFIER_RIGHT_CTRL, KC_NO,
            KC_NO, KC_NO, KC_NO  , KC_NO  , KC_NO  , KC_NO, KC_NO    , KC_NO            , KC_NO        , KC_NO        , KC_NO        , KC_NO,
            KC_NO, KC_NO, KC_NO, KC_ENT             , KC_NO, KC_TRNS
        ),
/* _LRIGHT_THUMB
* ,-----------------------------------------------------------------------------------.
* |      |   `  |   @  |   #  |   $  |   %  |   ^  |   [  |   ]  |   \  | Bksp |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |      |   <  |   >  |   {  |   }  |      |   _  |   (  |   )  |   :  |   =  |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |      |   &  |   |  |  !/¡ |  ?/¿ |   `  |   ~  |   /  |   *  |   -  |   +  |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* | TRAN | TRAN | TRAN | TRAN |MO(BT)|      |      | TRAN | TRAN | TRAN | TRAN | TRAN |
* `-----------------------------------------------------------------------------------'
*/
        [_LRIGHT_THUMB] = LAYOUT_split_3x6_3(
            KC_NO  , KC_GRV                , MONKEEB_LSFT(KC_2)        , MONKEEB_LSFT(KC_3)           , MONKEEB_LSFT(KC_4)            , MONKEEB_LSFT(KC_5)   , MONKEEB_LSFT(KC_6)    , KC_LBRC           , KC_RBRC           , KC_BSLS                   , KC_BSPC               , KC_NO,
            KC_NO  , MONKEEB_LSFT(KC_COMMA), MONKEEB_LSFT(KC_DOT)      , MONKEEB_LSFT(KC_LEFT_BRACKET), MONKEEB_LSFT(KC_RIGHT_BRACKET), KC_NO                , MONKEEB_LSFT(KC_MINUS), MONKEEB_LSFT(KC_9), MONKEEB_LSFT(KC_0), MONKEEB_LSFT(KC_SEMICOLON), KC_EQL                , KC_NO,
            KC_NO  , MONKEEB_LSFT(KC_7)    , MONKEEB_LSFT(KC_BACKSLASH), CKC_EXCL                     , CKC_QUES                      , KC_NO                , MONKEEB_LSFT(KC_GRAVE), KC_SLSH           , MONKEEB_LSFT(KC_8), KC_MINS                   , MONKEEB_LSFT(KC_EQUAL), KC_NO,
            KC_TRNS, KC_NO                 , KC_NO                     , KC_NO                        , KC_TRNS                       , KC_TRNS
        ),
/* _LMOVEMENT
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      | Home | PgUp | PgDo | End  | Bksp |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      | LCtrl| LAlt |LShift| TRAN |      | Left | Down |  Up  | Righ | Del  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |Ctrl C|Ctrl V|Ctrl X|      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |S(TAB)|  TAB | ESC  | Space| Enter|  APP |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
        [_LMOVEMENT] = LAYOUT_split_3x6_3(
            KC_NO, KC_NO             , KC_NO             , KC_NO             , KC_NO  , KC_NO , KC_HOME, KC_PGUP, KC_PGDN, KC_END ,  KC_BSPC, KC_NO,
            KC_NO, KC_LCTL           , KC_LALT           , KC_LSFT           , KC_TRNS, KC_NO , KC_LEFT , KC_DOWN, KC_UP  , KC_RGHT, KC_DEL , KC_NO,
            KC_NO, MONKEEB_LCTL(KC_C), MONKEEB_LCTL(KC_V), MONKEEB_LCTL(KC_X), KC_NO  , KC_NO , KC_NO   , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
            MONKEEB_LSFT(KC_TAB)    , KC_TAB , KC_ESC            , KC_SPC            , KC_ENT , KC_APP
        ),
        [_LNUMBERS] = LAYOUT_split_3x6_3(
            KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_PSLS, KC_7, KC_8, KC_9, KC_PMNS, KC_BSPC,
            KC_NO, KC_NO, KC_NO, KC_CALC, KC_NO, KC_NO, KC_PAST, KC_4, KC_5, KC_6, KC_PPLS, KC_DEL,
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_1, KC_2, KC_3, MONKEEB_LSFT(KC_9), MONKEEB_LSFT(KC_0),
            MONKEEB_LSFT(KC_TAB), KC_TAB, KC_ENT, KC_SPC, KC_0, KC_PDOT
        ),
        [_LFUNCTIONKEYS] = LAYOUT_split_3x6_3(
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F7, KC_F8, KC_F9, KC_F12, KC_BSPC,
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F4, KC_F5, KC_F6, KC_F11, KC_DEL,
            KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_NO, KC_F1, KC_F2, KC_F3, KC_F10, KC_NO,
            MONKEEB_LSFT(KC_TAB), KC_TAB, KC_NO, KC_NO, KC_NO, KC_NO
        ),
        [_LMOVEMENT_RIGHT_PINK] = LAYOUT_split_3x6_3(
            KC_NO, MONKEEB_LGUI(KC_TAB), MONKEEB_RCS(KC_T)   , MONKEEB_LCTL(KC_F4) , MONKEEB_LALT(KC_F4), KC_NO  , MONKEEB_LGUI(KC_M), MONKEEB_LGUI(KC_R), KC_NO, MONKEEB_LGUI(KC_E), KC_NO  , KC_NO,
            KC_NO, KC_LCTL     , KC_LALT     , KC_LSFT     , KC_LGUI    , KC_PSCR, KC_LEFT   , KC_DOWN   , KC_UP, KC_RGHT   , KC_TRNS, KC_NO,
            KC_NO, MONKEEB_RCS(KC_TAB) , MONKEEB_LCTL(KC_TAB), KC_NO       , KC_NO      , KC_NO  , KC_NO     , KC_NO     , KC_NO, KC_NO     , KC_NO  , KC_NO,
            MONKEEB_LSFT(KC_TAB), KC_TAB     , KC_ESC , KC_NO     , KC_APP    , KC_NO
        ),
// // /* _MEDIA
// //  * ,-----------------------------------------------------------------------------------.
// //  * |      |Search| Back |Forwar|Refres|      |      |MediaP| TRAN |Brig U|Brig D|      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      | Prev | Next |Rewind|FForwa|      |      |      |      |      |      |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      | Mute | Vol D| Vol U| Left | Right|      |      |      |      |      |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      |      |      |      | Play | Enter| Space| Stop |      |      |      |      |
// //  * `-----------------------------------------------------------------------------------'
// //  */
// // 	[_LMEDIA] = LAYOUT_split_3x6_3(
// //     KC_NO, KC_WSCH, KC_WBAK, KC_WFWD, KC_WREF, KC_NO  , KC_NO , KC_MSEL, KC_TRNS, KC_BRIU, KC_BRID, KC_NO,
// //     KC_NO, KC_MPRV, KC_MNXT, KC_MRWD, KC_MFFD, KC_NO  , KC_NO , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
// //     KC_NO, KC_MUTE, KC_VOLD, KC_VOLU, KC_LEFT, KC_RGHT, KC_NO , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
// //     KC_NO  , KC_MPLY, KC_ENT , KC_SPC, KC_MSTP, KC_NO
// //   ),
};



static platform_keycode_t get_keycode_from_layer(uint8_t layer, platform_keypos_t position) {
    if (layer >= sizeof(my_keymaps) / sizeof(my_keymaps[0]) || position.row >= MATRIX_ROWS || position.col >= MATRIX_COLS) {
        return 0;
    }
    return (platform_keycode_t) pgm_read_dword(&my_keymaps[layer][position.row][position.col]);
}

void keyboard_post_init_user(void) {
    debug_enable=false;
    //debug_matrix=true;
    //debug_keyboard=true;
    //debug_mouse=true;
    initialize_keymap((void*)my_keymaps, get_keycode_from_layer);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // #ifdef CONSOLE_ENABLE
    //     uprintf("process_record_user     : kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u, mods: %u, oneshot_mods:%u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count, get_mods(), get_oneshot_mods());
    // #endif

    // #if defined(FRAMEWORK_UNIT_TEST)
    //     uprintf("FRAMEWORK_UNIT_TEST is defined\n");
    // #else
    //     uprintf("FRAMEWORK_UNIT_TEST is not defined\n");
    // #endif

    // #if defined(FRAMEWORK_QMK)
    //     uprintf("FRAMEWORK_QMK is defined\n");
    // #else
    //     uprintf("FRAMEWORK_QMK is not defined\n");
    // #endif

    // #if defined(QMK_KEYBOARD)
    //     uprintf("QMK_KEYBOARD is defined\n");
    // #else
    //     uprintf("QMK_KEYBOARD is not defined\n");
    // #endif

    // #if defined(QUANTUM_H)
    //     uprintf("QUANTUM_H is defined\n");
    // #else
    //     uprintf("QUANTUM_H is not defined\n");
    // #endif

    // #if defined(DEBUG)
    //     uprintf("DEBUG is defined\n");
    // #else
    //     uprintf("DEBUG is not defined\n");
    // #endif

    // #if defined(CONSOLE_ENABLE)
    //     uprintf("CONSOLE_ENABLE is defined\n");
    // #else
    //     uprintf("CONSOLE_ENABLE is not defined\n");
    // #endif

    // #if defined(MONKEYBOARD_DEBUG)
    //     uprintf("MONKEYBOARD_DEBUG is defined\n");
    // #else
    //     uprintf("MONKEYBOARD_DEBUG is not defined\n");
    // #endif
    // abskeyevent_t abskeyevent = {
    //     .keypos = {
    //         .col = record->event.key.col,
    //         .row = record->event.key.row
    //     },
    //     .pressed = record->event.pressed,
    //     .time = record->event.time
    // };

    // pipeline_process_key(abskeyevent);

    return false; // Prevent default key processing
}

// void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
// // #ifdef CONSOLE_ENABLE
// //   uprintf("post_process_record_user: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u, mods: %u, oneshot_mods:%u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count, get_mods(), get_oneshot_mods());
// // #endif
// }

void check_matrix_changes(void) {
    static matrix_row_t previous_matrix[MATRIX_ROWS] = {0};

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        matrix_row_t current_row = matrix_get_row(row);
        matrix_row_t changed = current_row ^ previous_matrix[row];

        if (changed) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                if (changed & (1UL << col)) {
                    bool is_pressed = current_row & (1UL << col);
                    // Handle key change at (row, col, is_pressed)
                    abskeyevent_t abskeyevent = {
                        .keypos = {
                            .col = col,
                            .row = row
                        },
                        .pressed = is_pressed,
                        .time = timer_read32()
                    };

                    pipeline_process_key(abskeyevent);
                }
            }
            previous_matrix[row] = current_row;
        }
    }
}

void matrix_scan_user(void) {
    // Your library code here
    check_matrix_changes();
}

void housekeeping_task_user(void) {
    execute_deferred_executions();
}
