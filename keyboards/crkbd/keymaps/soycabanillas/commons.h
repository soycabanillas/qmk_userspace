#pragma once

#include QMK_KEYBOARD_H

typedef enum  {
  _LQWERTY,
  _LLEFT_THUMB,
  _LRIGHT_THUMB,
  _LBOTH_THUMBS,
  _LCONTROL,
  _LMOVEMENT,
  _LMOVEMENT_RIGTH_PINK,
  _LMOUSE,
  _LNUMBERS,
  _LMEDIA,
  _LFUNCTIONKEYS
} planck_layers;

#define MACRO_KEY_MODIFIER_LEFT_SHIFT  (1 << 0)
#define MACRO_KEY_MODIFIER_RIGHT_SHIFT (1 << 1)
#define MACRO_KEY_MODIFIER_LEFT_CTRL   (1 << 2)
#define MACRO_KEY_MODIFIER_RIGHT_CTRL  (1 << 3)
#define MACRO_KEY_MODIFIER_LEFT_ALT    (1 << 4)
#define MACRO_KEY_MODIFIER_RIGHT_ALT   (1 << 5)
#define MACRO_KEY_MODIFIER_LEFT_GUI    (1 << 6)
#define MACRO_KEY_MODIFIER_RIGHT_GUI   (1 << 7)

// Layer declarations

/* key event */
typedef struct {
    keypos_t key;
    bool     pressed;
    uint16_t time;
} abskeyevent_t;
