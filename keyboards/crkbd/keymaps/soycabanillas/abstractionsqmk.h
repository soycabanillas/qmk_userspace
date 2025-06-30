#pragma once

#include "commons.h"
#include "quantum_keycodes.h"

#define KEYS_ARE_EQUAL(keya, keyb) ((keya).row == (keyb).row && (keya).col == (keyb).col)

void print_buffer(uint8_t layer, abskeyevent_t keybuffer[8], uint8_t keybuffer_length);
uint8_t get_layer_topdown(keypos_t key);
bool flush_keybuffer_to_new_layer(uint8_t layer, keypos_t keyfromevent,abskeyevent_t keybuffer[8], uint8_t keybuffer_length);

// Custom Keycode Declarations
enum planck_keycodes {
  CKC_REMOVE_MODS = SAFE_RANGE,
  CKC_REMOVE_ONESHOTS,
  CKC_RESET_TAPPINGTERM,
  CKC_SET_TAPPINGTERM,
  CKC_LAY_MOUSE_Q,
  CKC_LAY_NUMBERS_R,
  CKC_LAY_MOVEMENT_F,
  CKC_EXCL,
  CKC_QUES,
  CKC_LAY_RIGHT_THUMB,

  CKC_ONESHOT_MODIFIER_LEFT_CTRL,
  CKC_ONESHOT_MODIFIER_RIGHT_CTRL,
  CKC_ONESHOT_MODIFIER_LEFT_ALT,
  CKC_ONESHOT_MODIFIER_RIGHT_ALT,
  CKC_ONESHOT_MODIFIER_LEFT_SHIFT,
  CKC_ONESHOT_MODIFIER_RIGHT_SHIFT,
  CKC_ONESHOT_MODIFIER_LEFT_GUI,
  CKC_ONESHOT_MODIFIER_RIGHT_GUI,

  CKC_REPLACE_SHIFTED_2,
  CKC_REPLACE_SHIFTED_3,
  CKC_REPLACE_SHIFTED_4,
  CKC_REPLACE_SHIFTED_5,
  CKC_REPLACE_SHIFTED_6,
  CKC_REPLACE_SHIFTED_9,
  CKC_REPLACE_SHIFTED_0,
};
