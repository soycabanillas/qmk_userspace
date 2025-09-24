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
#include "modules/soycabanillas/src/pipeline_combo.h"
#include "modules/soycabanillas/src/pipeline_combo_initializer.h"
#include "modules/soycabanillas/src/pipeline_tap_dance.h"
#include "modules/soycabanillas/src/pipeline_tap_dance_initializer.h"
#include "modules/soycabanillas/src/pipeline_oneshot_modifier.h"
#include "modules/soycabanillas/src/pipeline_oneshot_modifier_initializer.h"
#include "modules/soycabanillas/src/pipeline_executor.h"
#include "modules/soycabanillas/src/platform_interface.h"
#include QMK_KEYBOARD_H
// #include "print.h"
#include "commons.h"
#include "abstractionsqmk.h"
#include "keymap_initializer.h"

// // Layers
// const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// /* _NAME
//  * ,-----------------------------------------------------------------------------------.
//  * |      |      |      |      |      |      |      |      |      |      |      |      |
//  * |------+------+------+------+------+------+------+------+------+------+------+------|
//  * |      |      |      |      |      |      |      |      |      |      |      |      |
//  * |------+------+------+------+------+------+------+------+------+------+------+------|
//  * |      |      |      |      |      |      |      |      |      |      |      |      |
//  * |------+------+------+------+------+------+------+------+------+------+------+------|
//  * |      |      |      |      |      |      |      |      |      |      |      |      |
//  * `-----------------------------------------------------------------------------------'
//  */


// /* Qwerty
//  * ,-----------------------------------------------------------------------------------.
//  * | Esc  |   Q  |   W  |   E  | R/Num|   T  |   Y  |   U  |I/Medi|   O  |   P  | Bksp |
//  * |------+------+------+------+------+------+------+------+------+------+------+------|
//  * |R_Thum|A/Mous|   S  |   D  | F/Mov|   G  |   H  |   J  |   K  |   L  |;/Mov2|  '   |
//  * |------+------+------+------+------+------+------+------+------+------+------+------|
//  * | Shift|   Z  |   X  |   C  | V/Fun|   B  |   N  |   M  |   ,  |   .  |   /  |Shift |
//  * |------+------+------+------+------+------+------+------+------+------+------+------|
//  * | Gui  | Ctrl | Alt  | Shift|M_LTHU|Sh/ESC| Space|MO(RT)| Shift| Alt  | Ctrl | Caps |
//  * `-----------------------------------------------------------------------------------'
//  */
//         [_LQWERTY] = LAYOUT_split_3x6_3(
//             KC_ESC ,             KC_Q, KC_W, KC_E,    CKC_LAY_NUMBERS_R,      KC_T,               KC_Y,   KC_U,   KC_I,                KC_O,   KC_P,    KC_BSPC,
//             CKC_LAY_RIGHT_THUMB, KC_A, KC_S, KC_D,    CKC_LAY_MOVEMENT_F,     KC_G,               KC_H,   KC_J,   KC_K,                KC_L,   KC_SCLN, KC_QUOT,
//             KC_LSFT,             KC_Z, KC_X, KC_C,    CKC_LAY_FUNCTIONKEYS_V, KC_B,               KC_N,   KC_M,   KC_COMM,             KC_DOT, KC_SLSH, KC_RSFT,
//                                                                       KC_LSFT,                CKC_LAY_LEFT_THUMB, KC_ENT, KC_SPC, CKC_LAY_RIGHT_THUMB, KC_RCTL
//         ),
// // /* _LLEFT_THUMB
// //  * ,-----------------------------------------------------------------------------------.
// //  * |      |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      |OsLCTL|OsLALT|OsLSHI|OsLGUI|      |      |OsRGUI|OsRSHI|OsRALT|OsRCTL|      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      |      |      |      |      |      |      |      |      |      |      |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * | TRAN | TRAN | TRAN | TRAN | TRAN |      | Enter|MO(BT)| TRAN | TRAN | TRAN | TRAN |
// //  * `-----------------------------------------------------------------------------------'
// //  */
// // 	[_LLEFT_THUMB] = LAYOUT_split_3x6_3(
// //     KC_NO  , KC_1                 , KC_2   , KC_3   , KC_4   , KC_5 , KC_6               , KC_7             , KC_8         , KC_9         , KC_0         , KC_BSPC,
// //     KC_2   , CKC_ONESHOT_MODIFIER_LEFT_CTRL, CKC_ONESHOT_MODIFIER_LEFT_ALT, CKC_ONESHOT_MODIFIER_LEFT_SHIFT, CKC_ONESHOT_MODIFIER_LEFT_GUI, KC_NO, KC_NO, CKC_ONESHOT_MODIFIER_RIGHT_GUI, CKC_ONESHOT_MODIFIER_RIGHT_SHIFT, CKC_ONESHOT_MODIFIER_RIGHT_ALT, CKC_ONESHOT_MODIFIER_RIGHT_CTRL, KC_NO,
// //     KC_NO  , KC_NO, KC_NO  , KC_NO  , KC_NO  , KC_NO, KC_NO    , KC_NO            , KC_NO        , KC_NO        , KC_NO        , KC_NO,
// //     KC_TRNS, KC_TRNS, KC_NO, KC_ENT             , KC_NO, KC_TRNS
// //   ),
// // /* _LRIGHT_THUMB
// //  * ,-----------------------------------------------------------------------------------.
// //  * |      |   `  |   @  |   #  |   $  |   %  |   ^  |   [  |   ]  |   \  | Bksp |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      |   <  |   >  |   {  |   }  |      |   _  |   (  |   )  |   :  |   =  |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      |   &  |   |  |  !/¡ |  ?/¿ |   `  |   ~  |   /  |   *  |   -  |   +  |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * | TRAN | TRAN | TRAN | TRAN |MO(BT)|      |      | TRAN | TRAN | TRAN | TRAN | TRAN |
// //  * `-----------------------------------------------------------------------------------'
// //  */
// // 	[_LRIGHT_THUMB] = LAYOUT_split_3x6_3(
// //     KC_NO  , KC_GRV , KC_AT  , KC_HASH,  KC_DLR           , KC_PERC              , KC_CIRC, KC_LBRC, KC_RBRC, KC_BSLS, KC_BSPC, KC_NO,
// //     KC_NO  , KC_LT  , KC_GT  , KC_LCBR,  KC_RCBR          , KC_NO                , KC_UNDS, KC_LPRN, KC_RPRN, KC_COLN, KC_EQL , KC_NO,
// //     KC_NO  , KC_AMPR, KC_PIPE, CKC_EXCL, CKC_QUES         , CKC_REPLACE_SHIFTED_2, KC_TILD, KC_SLSH, KC_ASTR, KC_MINS, KC_PLUS, KC_NO,
// //     KC_TRNS,  KC_NO, KC_NO                , KC_NO  , KC_TRNS, KC_TRNS
// //   ),
// // /* _LMOVEMENT
// //  * ,-----------------------------------------------------------------------------------.
// //  * |      |      |      |      |      |      | Home | PgUp | PgDo | End  | Bksp |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      | LCtrl| LAlt |LShift| TRAN |      | Left | Down |  Up  | Righ | Del  |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      |Ctrl C|Ctrl V|Ctrl X|      |      |      |      |      |      |      |      |
// //  * |------+------+------+------+------+------+------+------+------+------+------+------|
// //  * |      |      |      |S(TAB)|  TAB | ESC  | Space| Enter|  APP |      |      |      |
// //  * `-----------------------------------------------------------------------------------'
// //  */
// // 	[_LMOVEMENT] = LAYOUT_split_3x6_3(
// //     KC_NO, KC_NO     , KC_NO     , KC_NO           , KC_NO  , KC_NO , KC_HOME, KC_PGUP, KC_PGDN, KC_END,  KC_BSPC, KC_NO,
// //     KC_NO, KC_LCTL   , KC_LALT   , KC_LSFT         , KC_TRNS, KC_NO , KC_LEFT , KC_DOWN, KC_UP  , KC_RGHT, KC_DEL , KC_NO,
// //     KC_NO, LCTL(KC_C), LCTL(KC_V), LCTL(KC_X)      , KC_NO  , KC_NO , KC_NO   , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
// //     LSFT(KC_TAB)    , KC_TAB , KC_ESC, KC_SPC  , KC_ENT , KC_APP
// //   ),
// // 	[_LMOVEMENT_RIGTH_PINK] = LAYOUT_split_3x6_3(
// //     KC_NO, LGUI(KC_TAB), RCS(KC_T)   , LCTL(KC_F4) , LALT(KC_F4), KC_NO  , LGUI(KC_M), LGUI(KC_R), KC_NO, LGUI(KC_E), KC_NO  , KC_NO,
// //     KC_NO, KC_LCTL     , KC_LALT     , KC_LSFT     , KC_LGUI    , KC_PSCR, KC_LEFT   , KC_DOWN   , KC_UP, KC_RGHT   , KC_TRNS, KC_NO,
// //     KC_NO, RCS(KC_TAB) , LCTL(KC_TAB), KC_NO       , KC_NO      , KC_NO  , KC_NO     , KC_NO     , KC_NO, KC_NO     , KC_NO  , KC_NO,
// //     LSFT(KC_TAB), KC_TAB     , KC_ESC , KC_NO     , KC_APP    , KC_NO
// //   ),
// // 	[_LMOUSE] = LAYOUT_split_3x6_3(
// //     KC_NO, KC_TRNS , KC_ACL0, KC_ACL1, KC_ACL2, KC_NO  , KC_NO  , KC_ESC , KC_MS_U, KC_NO  , KC_NO, KC_NO,
// //     KC_NO, KC_LCTL, KC_LALT, KC_LSFT, KC_NO  , KC_NO  , KC_NO  , KC_MS_L, KC_MS_D, KC_MS_R, KC_NO, KC_NO,
// //     KC_NO, KC_NO   , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO, KC_NO,
// //     KC_WH_L, KC_BTN1, KC_WH_U, KC_WH_D, KC_BTN2, KC_WH_R
// //   ),
// // 	[_LNUMBERS] = LAYOUT_split_3x6_3(
// //     KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_PSLS, KC_7, KC_8, KC_9, KC_PMNS, KC_BSPC,
// //     KC_NO, KC_NO, KC_NO, KC_CALC, KC_NO, KC_NO, KC_PAST, KC_4, KC_5, KC_6, KC_PPLS, KC_DEL,
// //     KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_1, KC_2, KC_3, KC_LPRN, KC_RPRN,
// //     LSFT(KC_TAB), KC_TAB, KC_ENT, KC_SPC, KC_0, KC_PDOT
// //     ),
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
// // 	[_LFUNCTIONKEYS] = LAYOUT_split_3x6_3(
// //     KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F7, KC_F8, KC_F9, KC_F12, KC_BSPC,
// //     KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F4, KC_F5, KC_F6, KC_F11, KC_DEL,
// //     KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_NO, KC_F1, KC_F2, KC_F3, KC_F10, KC_NO,
// //     LSFT(KC_TAB), KC_TAB, KC_NO, KC_NO, KC_NO, KC_NO
// //   )
// };

// void print_layer_status(void) {
// // #ifdef CONSOLE_ENABLE
// //   uprintf("print_layer_status: qwerty_pressed: %u, layerchanged: %u, layer: %u, keycode: %u\n", layer_status->pressed, layer_status->layerchanged, layer_status->layer, layer_status->keycode);
// // #endif
// }

const uint16_t PROGMEM keymaps[][1][1] = {{{0}}};

void add_hold_tap(uint8_t pipeline_index) {
    pipeline_tap_dance_global_state_create();
    size_t n_elements = 6;
    pipeline_tap_dance_global_config_t* tap_dance_config = (pipeline_tap_dance_global_config_t*)malloc(sizeof(pipeline_tap_dance_global_config_t));
    tap_dance_config->length = n_elements;
    tap_dance_config->behaviours = (pipeline_tap_dance_behaviour_t**)malloc(n_elements * sizeof(pipeline_tap_dance_behaviour_t*));

    pipeline_executor_add_physical_pipeline(pipeline_index, &pipeline_tap_dance_callback_process_data_executor, &pipeline_tap_dance_callback_reset_executor, tap_dance_config);


    pipeline_tap_dance_action_config_t* actions_CKC_LAY_NUMBERS_R[] = {
        createbehaviouraction_tap(1, KC_R),
        createbehaviouraction_hold(1, _LNUMBERS, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_NUMBERS_R = createbehaviour(CKC_LAY_NUMBERS_R, actions_CKC_LAY_NUMBERS_R, 2);
    tap_dance_behavior_CKC_LAY_NUMBERS_R->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_NUMBERS_R->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[0] = tap_dance_behavior_CKC_LAY_NUMBERS_R;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_MOVEMENT_F[] = {
        createbehaviouraction_tap(1, KC_F),
        createbehaviouraction_hold(1, _LMOVEMENT, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t*  tap_dance_behavior_CKC_LAY_MOVEMENT_F = createbehaviour(CKC_LAY_MOVEMENT_F, actions_CKC_LAY_MOVEMENT_F, 2);
    tap_dance_behavior_CKC_LAY_MOVEMENT_F->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_MOVEMENT_F->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[1] = tap_dance_behavior_CKC_LAY_MOVEMENT_F;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_FUNCTIONKEYS_V[] = {
        createbehaviouraction_tap(1, KC_V),
        createbehaviouraction_hold(1, _LFUNCTIONKEYS, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V = createbehaviour(CKC_LAY_FUNCTIONKEYS_V, actions_CKC_LAY_FUNCTIONKEYS_V, 2);
    tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[2] = tap_dance_behavior_CKC_LAY_FUNCTIONKEYS_V;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_MOVEMENT_RIGHT_SCLN[] = {
        createbehaviouraction_tap(1, KC_SCLN),
        createbehaviouraction_hold(1, _LMOVEMENT_RIGHT_PINK, TAP_DANCE_BALANCED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN = createbehaviour(CKC_LAY_MOVEMENT_RIGHT_SCLN, actions_CKC_LAY_MOVEMENT_RIGHT_SCLN, 2);
    tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[3] = tap_dance_behavior_CKC_LAY_MOVEMENT_RIGHT_SCLN;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_LEFT_THUMB[] = {
        createbehaviouraction_hold(1, _LLEFT_THUMB, TAP_DANCE_HOLD_PREFERRED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_LEFT_THUMB = createbehaviour(CKC_LAY_LEFT_THUMB, actions_CKC_LAY_LEFT_THUMB, 1);
    tap_dance_behavior_CKC_LAY_LEFT_THUMB->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_LEFT_THUMB->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[4] = tap_dance_behavior_CKC_LAY_LEFT_THUMB;

    pipeline_tap_dance_action_config_t* actions_CKC_LAY_RIGHT_THUMB[] = {
        createbehaviouraction_hold(1, _LRIGHT_THUMB, TAP_DANCE_HOLD_PREFERRED)
    };
    pipeline_tap_dance_behaviour_t* tap_dance_behavior_CKC_LAY_RIGHT_THUMB = createbehaviour(CKC_LAY_RIGHT_THUMB, actions_CKC_LAY_RIGHT_THUMB, 1);
    tap_dance_behavior_CKC_LAY_RIGHT_THUMB->config->hold_timeout = 200; // Set hold timeout to 200ms
    tap_dance_behavior_CKC_LAY_RIGHT_THUMB->config->tap_timeout = 200; // Set tap timeout to 200ms
    tap_dance_config->behaviours[5] = tap_dance_behavior_CKC_LAY_RIGHT_THUMB;
}

void add_one_shot_modifiers(uint8_t pipeline_index) {
    size_t number_of_pairs = 8;
    pipeline_oneshot_modifier_global_status_t* global_status = pipeline_oneshot_modifier_global_state_create();
    pipeline_oneshot_modifier_global_config_t* global_config = (pipeline_oneshot_modifier_global_config_t*)(malloc(sizeof(*global_config)));
    global_config->length = number_of_pairs;
    global_config->modifier_pairs = (pipeline_oneshot_modifier_pair_t**)(malloc(sizeof(pipeline_oneshot_modifier_pair_t*) * number_of_pairs));
    global_config->modifier_pairs[0] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_CTRL, MACRO_KEY_MODIFIER_LEFT_CTRL);
    global_config->modifier_pairs[1] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_ALT, MACRO_KEY_MODIFIER_LEFT_ALT);
    global_config->modifier_pairs[2] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_SHIFT, MACRO_KEY_MODIFIER_LEFT_SHIFT);
    global_config->modifier_pairs[3] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_LEFT_GUI, MACRO_KEY_MODIFIER_LEFT_GUI);
    global_config->modifier_pairs[4] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_CTRL, MACRO_KEY_MODIFIER_RIGHT_CTRL);
    global_config->modifier_pairs[5] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_ALT, MACRO_KEY_MODIFIER_RIGHT_ALT);
    global_config->modifier_pairs[6] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_SHIFT, MACRO_KEY_MODIFIER_RIGHT_SHIFT);
    global_config->modifier_pairs[7] = pipeline_oneshot_modifier_create_pairs(CKC_ONESHOT_MODIFIER_RIGHT_GUI, MACRO_KEY_MODIFIER_RIGHT_GUI);
    pipeline_oneshot_modifier_global_t* global = malloc(sizeof(pipeline_oneshot_modifier_global_t));
    global->config = global_config;
    global->status = global_status;
    pipeline_executor_add_virtual_pipeline(pipeline_index, &pipeline_oneshot_modifier_callback_process_data_executor, &pipeline_oneshot_modifier_callback_reset_executor, global);
}

void add_combo(uint8_t pipeline_index) {
    pipeline_combo_global_state_create();
    size_t n_elements = 1;
    pipeline_combo_global_config_t* combo_config = (pipeline_combo_global_config_t*)malloc(sizeof(*combo_config));
    combo_config->length = n_elements;
    combo_config->combos = (pipeline_combo_config_t**)malloc(n_elements * sizeof(pipeline_combo_config_t*));
    combo_config->strategy = COMBO_STRATEGY_DISCARD_WHEN_ONE_PRESSED_IN_COMMON;

    pipeline_executor_add_physical_pipeline(pipeline_index, &pipeline_combo_callback_process_data_executor, &pipeline_combo_callback_reset_executor, combo_config);

    pipeline_combo_key_translation_t press_action_none = create_combo_key_action(COMBO_KEY_ACTION_NONE, 0);
    pipeline_combo_key_translation_t release_action_none = create_combo_key_action(COMBO_KEY_ACTION_NONE, 0);

    // Define a combo: pressing 'A' and 'S' together sends 'ESC'
    platform_keypos_t keypos_a = { .row = 0, .col = 1 }; // Example key position
    pipeline_combo_key_t* combo_key_a = create_combo_key(keypos_a, press_action_none, release_action_none);

    platform_keypos_t keypos_b = { .row = 0, .col = 2 }; // Example key position
    pipeline_combo_key_t* combo_key_b = create_combo_key(keypos_b, press_action_none, release_action_none);

    pipeline_combo_key_t* combo1_keys[] = { combo_key_a, combo_key_b };

    pipeline_combo_key_translation_t press_action_combo1 = create_combo_key_action(COMBO_KEY_ACTION_REGISTER, KC_AT);
    pipeline_combo_key_translation_t release_action_combo1 = create_combo_key_action(COMBO_KEY_ACTION_UNREGISTER, KC_AT);

    pipeline_combo_config_t* combo1 = create_combo(2, combo1_keys, press_action_combo1, release_action_combo1);
    combo_config->combos[0] = combo1;
}

void keyboard_post_init_user(void) {
  debug_enable=false;
  //debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;


    // size_t n_pipelines = 3;
    // pipeline_executor_config = (pipeline_executor_config_t*)malloc(sizeof(pipeline_executor_config_t) + n_pipelines * sizeof(pipeline_t*));
    // pipeline_executor_config->length = n_pipelines;
    // pipeline_executor_global_state_create();

    // pipeline_oneshot_modifier_global_config_t* oneshot_modifier_global_config = pipeline_oneshot_modifier_global_config_create();
    // pipeline_oneshot_modifier_global_status_t* oneshot_modifier_global_status = pipeline_oneshot_modifier_global_state_create();
    // pipeline_oneshot_modifier_global_t* oneshot_modifier_global = malloc(sizeof(pipeline_oneshot_modifier_global_t));
    // oneshot_modifier_global->config = oneshot_modifier_global_config;
    // oneshot_modifier_global->status = oneshot_modifier_global_status;
    // pipeline_executor_config->pipelines[0] = add_pipeline(&pipeline_oneshot_modifier_callback, pipeline_oneshot_modifier_global_config_create());

    // pipeline_key_replacer_global_config_t* key_replacer_global_config = pipeline_key_replacer_global_config_create();
    // pipeline_key_replacer_global_t* key_replacer_global = malloc(sizeof(pipeline_key_replacer_global_t));
    // key_replacer_global->config = key_replacer_global_config;
    // pipeline_executor_config->pipelines[1] = add_pipeline(&pipeline_key_replacer_callback, pipeline_key_replacer_global_config_create());

    // pipeline_tap_dance_global_config_t* tap_dance_global_config = pipeline_tap_dance_global_config_create();
    // pipeline_tap_dance_global_state_create();
    // pipeline_executor_config->pipelines[2] = add_pipeline(&pipeline_tap_dance_callback, tap_dance_global_config);

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
    static const platform_keycode_t keymaps[][8][6] = {
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
            KC_NO  , KC_GRV , KC_AT  , KC_HASH,  KC_DLR           , KC_PERC              , KC_CIRC, KC_LBRC, KC_RBRC, KC_BSLS, KC_BSPC, KC_NO,
            KC_NO  , KC_LT  , KC_GT  , KC_LCBR,  KC_RCBR          , KC_NO                , KC_UNDS, KC_LPRN, KC_RPRN, KC_COLN, KC_EQL , KC_NO,
            KC_NO  , KC_AMPR, KC_PIPE, CKC_EXCL, CKC_QUES         , CKC_REPLACE_SHIFTED_2, KC_TILD, KC_SLSH, KC_ASTR, KC_MINS, KC_PLUS, KC_NO,
            KC_TRNS,  KC_NO, KC_NO                , KC_NO  , KC_TRNS, KC_TRNS
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
            KC_NO, KC_NO     , KC_NO     , KC_NO           , KC_NO  , KC_NO , KC_HOME, KC_PGUP, KC_PGDN, KC_END,  KC_BSPC, KC_NO,
            KC_NO, KC_LCTL   , KC_LALT   , KC_LSFT         , KC_TRNS, KC_NO , KC_LEFT , KC_DOWN, KC_UP  , KC_RGHT, KC_DEL , KC_NO,
            KC_NO, LCTL(KC_C), LCTL(KC_V), LCTL(KC_X)      , KC_NO  , KC_NO , KC_NO   , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO,
            LSFT(KC_TAB)    , KC_TAB , KC_ESC, KC_SPC  , KC_ENT , KC_APP
        ),
        [_LNUMBERS] = LAYOUT_split_3x6_3(
            KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_PSLS, KC_7, KC_8, KC_9, KC_PMNS, KC_BSPC,
            KC_NO, KC_NO, KC_NO, KC_CALC, KC_NO, KC_NO, KC_PAST, KC_4, KC_5, KC_6, KC_PPLS, KC_DEL,
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_1, KC_2, KC_3, KC_LPRN, KC_RPRN,
            LSFT(KC_TAB), KC_TAB, KC_ENT, KC_SPC, KC_0, KC_PDOT
        ),
        [_LFUNCTIONKEYS] = LAYOUT_split_3x6_3(
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F7, KC_F8, KC_F9, KC_F12, KC_BSPC,
            KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_F4, KC_F5, KC_F6, KC_F11, KC_DEL,
            KC_LGUI, KC_LCTL, KC_LALT, KC_LSFT, KC_TRNS, KC_NO, KC_NO, KC_F1, KC_F2, KC_F3, KC_F10, KC_NO,
            LSFT(KC_TAB), KC_TAB, KC_NO, KC_NO, KC_NO, KC_NO
        ),
        [_LMOVEMENT_RIGHT_PINK] = LAYOUT_split_3x6_3(
            KC_NO, LGUI(KC_TAB), RCS(KC_T)   , LCTL(KC_F4) , LALT(KC_F4), KC_NO  , LGUI(KC_M), LGUI(KC_R), KC_NO, LGUI(KC_E), KC_NO  , KC_NO,
            KC_NO, KC_LCTL     , KC_LALT     , KC_LSFT     , KC_LGUI    , KC_PSCR, KC_LEFT   , KC_DOWN   , KC_UP, KC_RGHT   , KC_TRNS, KC_NO,
            KC_NO, RCS(KC_TAB) , LCTL(KC_TAB), KC_NO       , KC_NO      , KC_NO  , KC_NO     , KC_NO     , KC_NO, KC_NO     , KC_NO  , KC_NO,
            LSFT(KC_TAB), KC_TAB     , KC_ESC , KC_NO     , KC_APP    , KC_NO
        ),
    };
    platform_layout_init_2D_keymap((const uint16_t*)keymaps, 7, 8, 6);
    pipeline_executor_create_config(2, 1);

    add_combo(0);
    add_hold_tap(1);
    add_one_shot_modifiers(0);

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
