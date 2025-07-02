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

#include QMK_KEYBOARD_H
#include "print.h"
#include "commons.h"
#include "abstractionsqmk.h"
#include "key_buffer.h"
#include "pipeline_key_replacer.h"
#include "pipeline_oneshot_modifier.h"
#include "pipeline_tap_dance.h"

// Layers
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

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
/*    1               2           3        4              5                   6                      7        8                        9           10                11                       12   */
    KC_ESC , KC_Q, KC_W   , KC_E   , LT(_LNUMBERS, KC_R)      , KC_T                  , KC_Y   , KC_U             , LT(_LMEDIA,KC_I), KC_O, KC_P                              , KC_BSPC,
    //KC_ESC , CKC_LAY_MOUSE_Q, KC_W   , KC_E   , CKC_LAY_NUMBERS_R      , KC_T                  , KC_Y   , KC_U             , LT(_LMEDIA,KC_I), KC_O, KC_P                              , KC_BSPC,
    CKC_LAY_RIGHT_THUMB, LT(_LMOUSE, KC_A)           , KC_S   , KC_D   , LT(_LMOVEMENT, KC_F)     , KC_G                  , KC_H   , KC_J             , KC_K            , KC_L, LT(_LMOVEMENT_RIGTH_PINK,KC_SCLN) , KC_QUOT,
    //CKC_LAY_RIGHT_THUMB , KC_A           , KC_S   , KC_D   , CKC_LAY_MOVEMENT_F     , KC_G                  , KC_H   , KC_J             , KC_K            , KC_L, LT(_LMOVEMENT_RIGTH_PINK,KC_SCLN) , KC_QUOT,
    KC_LSFT, KC_Z           , KC_X   , KC_C   , LT(_LFUNCTIONKEYS,KC_V), KC_B                  , KC_N   , KC_M             , KC_COMM, KC_DOT , KC_SLSH                                 , KC_RSFT,
    KC_LSFT, MO(_LLEFT_THUMB)       , KC_ENT                , KC_SPC , MO(_LRIGHT_THUMB), KC_RCTL
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
    KC_NO  , KC_1                 , KC_2   , KC_3   , KC_4   , KC_5 , KC_6               , KC_7             , KC_8         , KC_9         , KC_0         , KC_BSPC,
    KC_2   , CKC_ONESHOT_MODIFIER_LEFT_CTRL, CKC_ONESHOT_MODIFIER_LEFT_ALT, CKC_ONESHOT_MODIFIER_LEFT_SHIFT, CKC_ONESHOT_MODIFIER_LEFT_GUI, KC_NO, KC_NO, CKC_ONESHOT_MODIFIER_RIGHT_GUI, CKC_ONESHOT_MODIFIER_RIGHT_SHIFT, CKC_ONESHOT_MODIFIER_RIGHT_ALT, CKC_ONESHOT_MODIFIER_RIGHT_CTRL, KC_NO,
    KC_NO  , KC_NO, KC_NO  , KC_NO  , KC_NO  , KC_NO, KC_NO    , KC_NO            , KC_NO        , KC_NO        , KC_NO        , KC_NO,
    KC_TRNS, KC_TRNS, KC_NO, KC_ENT             , MO(_LBOTH_THUMBS), KC_TRNS
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
    KC_NO  , KC_GRV , KC_AT  , KC_HASH,  KC_DLR           , KC_PERC              , KC_CIRC, KC_LBRC, KC_RBRC, KC_BSLS, KC_BSPC, KC_NO,
    KC_NO  , KC_LT  , KC_GT  , KC_LCBR,  KC_RCBR          , KC_NO                , KC_UNDS, KC_LPRN, KC_RPRN, KC_COLN, KC_EQL , KC_NO,
    KC_NO  , KC_AMPR, KC_PIPE, CKC_EXCL, CKC_QUES         , CKC_REPLACE_SHIFTED_2, KC_TILD, KC_SLSH, KC_ASTR, KC_MINS, KC_PLUS, KC_NO,
    KC_TRNS,  MO(_LBOTH_THUMBS), KC_NO                , KC_NO  , KC_TRNS, KC_TRNS
  ),
  /* _LBOTH_THUMBS
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |TO(CT)| TRAN |      |      | TRAN |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
	[_LBOTH_THUMBS] = LAYOUT_split_3x6_3(
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    TO(_LCONTROL), KC_TRNS, KC_NO, KC_NO, KC_TRNS, KC_NO
  ),
/* _LCONTROL
 * ,-----------------------------------------------------------------------------------.
 * |      | BOOT |DB_TOG| KC_U |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |TO(QW)|      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      | TRAN |      |      |      |      |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
	[_LCONTROL] = LAYOUT_split_3x6_3(
    KC_NO       , QK_BOOT, DB_TOGG, KC_U , KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    TO(_LQWERTY), KC_NO  , KC_NO  , KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO       , KC_NO  , KC_NO  , KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO
  ),
/* _LLEFT_THUMB
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
    KC_NO, KC_NO     , KC_NO     , KC_NO           , KC_NO  , KC_NO , KC_HOME, KC_PGUP, KC_PGDN, KC_END,  KC_BSPC, KC_NO,
    KC_NO, KC_LCTL   , KC_LALT   , KC_LSFT         , KC_TRNS, KC_NO , KC_LEFT , KC_DOWN, KC_UP  , KC_RGHT, KC_DEL , KC_NO,
    KC_NO, LCTL(KC_C), LCTL(KC_V), LCTL(KC_X)      , KC_NO  , KC_NO , KC_NO   , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
    LSFT(KC_TAB)    , KC_TAB , KC_ESC, KC_SPC  , KC_ENT , KC_APP
  ),
	[_LMOVEMENT_RIGTH_PINK] = LAYOUT_split_3x6_3(
    KC_NO, LGUI(KC_TAB), RCS(KC_T)   , LCTL(KC_F4) , LALT(KC_F4), KC_NO  , LGUI(KC_M), LGUI(KC_R), KC_NO, LGUI(KC_E), KC_NO  , KC_NO,
    KC_NO, KC_LCTL     , KC_LALT     , KC_LSFT     , KC_LGUI    , KC_PSCR, KC_LEFT   , KC_DOWN   , KC_UP, KC_RGHT   , KC_TRNS, KC_NO,
    KC_NO, RCS(KC_TAB) , LCTL(KC_TAB), KC_NO       , KC_NO      , KC_NO  , KC_NO     , KC_NO     , KC_NO, KC_NO     , KC_NO  , KC_NO,
    LSFT(KC_TAB), KC_TAB     , KC_ESC , KC_NO     , KC_APP    , KC_NO
  ),
	[_LMOUSE] = LAYOUT_split_3x6_3(
    KC_NO, KC_TRNS , KC_ACL0, KC_ACL1, KC_ACL2, KC_NO  , KC_NO  , KC_ESC , KC_MS_U, KC_NO  , KC_NO, KC_NO,
    KC_NO, KC_LCTL, KC_LALT, KC_LSFT, KC_NO  , KC_NO  , KC_NO  , KC_MS_L, KC_MS_D, KC_MS_R, KC_NO, KC_NO,
    KC_NO, KC_NO   , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO, KC_NO,
    KC_WH_L, KC_BTN1, KC_WH_U, KC_WH_D, KC_BTN2, KC_WH_R
  ),
	[_LNUMBERS] = LAYOUT_split_3x6_3(
    KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_PSLS, KC_7, KC_8, KC_9, KC_PMNS, KC_BSPC,
    KC_NO, KC_NO, KC_NO, KC_CALC, KC_NO, KC_NO, KC_PAST, KC_4, KC_5, KC_6, KC_PPLS, KC_DEL,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_1, KC_2, KC_3, KC_LPRN, KC_RPRN,
    LSFT(KC_TAB), KC_TAB, KC_ENT, KC_SPC, KC_0, KC_PDOT
    ),
/* _MEDIA
 * ,-----------------------------------------------------------------------------------.
 * |      |Search| Back |Forwar|Refres|      |      |MediaP| TRAN |Brig U|Brig D|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      | Prev | Next |Rewind|FForwa|      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      | Mute | Vol D| Vol U| Left | Right|      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      | Play | Enter| Space| Stop |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
	[_LMEDIA] = LAYOUT_split_3x6_3(
    KC_NO, KC_WSCH, KC_WBAK, KC_WFWD, KC_WREF, KC_NO  , KC_NO , KC_MSEL, KC_TRNS, KC_BRIU, KC_BRID, KC_NO,
    KC_NO, KC_MPRV, KC_MNXT, KC_MRWD, KC_MFFD, KC_NO  , KC_NO , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
    KC_NO, KC_MUTE, KC_VOLD, KC_VOLU, KC_LEFT, KC_RGHT, KC_NO , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
    KC_NO  , KC_MPLY, KC_ENT , KC_SPC, KC_MSTP, KC_NO
  ),
	[_LFUNCTIONKEYS] = LAYOUT_split_3x6_3(
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F7, KC_F8, KC_F9, KC_F12, KC_BSPC,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F4, KC_F5, KC_F6, KC_F11, KC_DEL,
    KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_NO, KC_F1, KC_F2, KC_F3, KC_F10, KC_NO,
    LSFT(KC_TAB), KC_TAB, KC_NO, KC_NO, KC_NO, KC_NO
  )
};

void print_layer_status(void) {
// #ifdef CONSOLE_ENABLE
//   uprintf("print_layer_status: qwerty_pressed: %u, layerchanged: %u, layer: %u, keycode: %u\n", layer_status->pressed, layer_status->layerchanged, layer_status->layer, layer_status->keycode);
// #endif
}

void keyboard_post_init_user(void) {
  debug_enable=true;
  //debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;

    size_t n_pipelines = 3;
    pipeline_array = malloc(sizeof(pipeline_array_t) + n_pipelines * sizeof(pipeline_t*));
    pipeline_array->length = n_pipelines;

    pipeline_array->pipelines[0] = add_pipeline(&pipeline_oneshot_modifier_callback, pipeline_oneshot_modifier_initialize_user_data());
    pipeline_array->pipelines[1] = add_pipeline(&pipeline_key_replacer_callback, pipeline_key_replacer_initialize_user_data());
    pipeline_array->pipelines[2] = add_pipeline(&pipeline_tap_dance_callback, pipeline_tap_dance_initialize_user_data());
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    #ifdef CONSOLE_ENABLE
        uprintf("process_record_user     : kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u, mods: %u, oneshot_mods:%u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count, get_mods(), get_oneshot_mods());
    #endif

    abskeyevent_t abskeyevent = {
        .key = {
            .col = record->event.key.col,
            .row = record->event.key.row
        },
        .pressed = record->event.pressed,
        .time = record->event.time
    };

    //if (macros_process_key(keycode, abskeyevent) == false) return false;
    if (pipeline_process_key(keycode, abskeyevent) == false) return false;

    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
// #ifdef CONSOLE_ENABLE
//   uprintf("post_process_record_user: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u, mods: %u, oneshot_mods:%u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count, get_mods(), get_oneshot_mods());
// #endif
}
