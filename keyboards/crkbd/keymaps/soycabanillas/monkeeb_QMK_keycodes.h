#ifndef KEYCODES_H
#define KEYCODES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// QMK keycode ranges (from the documents you shared)
#define QK_BASIC                0x0000
#define QK_BASIC_MAX            0x00FF
#define QK_MODS                 0x0100
#define QK_MODS_MAX             0x1FFF

// QMK modifier bits (from quantum.h)
#define QK_LCTL                 0x0100
#define QK_LSFT                 0x0200
#define QK_LALT                 0x0400
#define QK_LGUI                 0x0800
#define QK_RCTL                 0x1100
#define QK_RSFT                 0x1200
#define QK_RALT                 0x1400
#define QK_RGUI                 0x1800

// Our 32-bit keycode ranges
#define KC32_BASIC_MAX          0x000000FF
#define KC32_MODIFIED_MAX       0x0000FFFF

// Modifier bit positions in our 32-bit format
#define MOD32_LCTL              (1 << 15)
#define MOD32_LSFT              (1 << 14)
#define MOD32_LALT              (1 << 13)
#define MOD32_LGUI              (1 << 12)
#define MOD32_RCTL              (1 << 11)
#define MOD32_RSFT              (1 << 10)
#define MOD32_RALT              (1 << 9)
#define MOD32_RGUI              (1 << 8)

// =============================================================================
// TYPE DEFINITIONS
// =============================================================================

typedef struct {
    uint8_t basic_key;
    uint8_t modifiers;
    bool is_modified;
} keycode_32bit_info_t;

typedef struct {
    size_t converted_count;
    size_t failed_count;
    size_t total_count;
} conversion_stats_t;

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

bool is_qmk_basic_with_mods(uint16_t qmk_keycode);
uint32_t qmk_to_32bit_keycode(uint16_t qmk_keycode);
uint32_t qmk_mods_to_32bit_mods(uint16_t qmk_keycode);
uint32_t qmk_to_32bit_robust(uint16_t qmk_keycode);
void test_converter(void);
keycode_32bit_info_t decode_32bit_keycode(uint32_t keycode);
void convert_qmk_keymap(const uint16_t* qmk_keymap, uint32_t* output_keymap, size_t count);
conversion_stats_t convert_qmk_keymap_with_stats(const uint16_t* qmk_keymap,
                                                uint32_t* output_keymap,
                                                size_t count);

// =============================================================================
// CONVERSION MACROS - Transform QMK keycodes to 32-bit format
// =============================================================================

/**
 * Main macro to convert QMK keycode to 32-bit format
 * Handles basic keys, modified keys, and returns 0 for unsupported types
 */
#define QMK_TO_32BIT(qmk_keycode) \
    ((qmk_keycode) <= QK_BASIC_MAX ? (qmk_keycode) : \
     (((qmk_keycode) >= QK_MODS && (qmk_keycode) <= QK_MODS_MAX) ? \
      qmk_to_32bit_robust(qmk_keycode) : 0))

/**
 * Compile-time macro for common QMK basic keys (if keycode is known at compile time)
 */
#define QMK_BASIC_TO_32BIT(qmk_basic) (qmk_basic)

/**
 * Compile-time macro for QMK modified keys (if components are known at compile time)
 * Usage: QMK_MOD_TO_32BIT(QK_LCTL, KC_A) for Ctrl+A
 */
#define QMK_MOD_TO_32BIT(qmk_mod, basic_key) \
    qmk_to_32bit_robust((qmk_mod) | (basic_key))

// =============================================================================
// CONVENIENCE MACROS - Common QMK keycode patterns
// =============================================================================

/**
 * Convert QMK modifier+key macros to 32-bit format
 */
#define MONKEEB_LCTL(key) (QK_LCTL | (key))
#define MONKEEB_LSFT(key) (QK_LSFT | (key))
#define MONKEEB_LALT(key) (QK_LALT | (key))
#define MONKEEB_LGUI(key) (QK_LGUI | (key))

#define MONKEEB_RCTL(key) (QK_RCTL | (key))
#define MONKEEB_RSFT(key) (QK_RSFT | (key))
#define MONKEEB_RALT(key) (QK_RALT | (key))
#define MONKEEB_RGUI(key) (QK_RGUI | (key))


#define MONKEEB_LCS(kc) (QK_LCTL | QK_LSFT | (kc))
#define MONKEEB_LCA(kc) (QK_LCTL | QK_LALT | (kc))
#define MONKEEB_LCG(kc) (QK_LCTL | QK_LGUI | (kc))
#define MONKEEB_LSA(kc) (QK_LSFT | QK_LALT | (kc))
#define MONKEEB_LSG(kc) (QK_LSFT | QK_LGUI | (kc))
#define MONKEEB_LAG(kc) (QK_LALT | QK_LGUI | (kc))
#define MONKEEB_LCSG(kc) (QK_LCTL | QK_LSFT | QK_LGUI | (kc))
#define MONKEEB_LCAG(kc) (QK_LCTL | QK_LALT | QK_LGUI | (kc))
#define MONKEEB_LSAG(kc) (QK_LSFT | QK_LALT | QK_LGUI | (kc))

#define MONKEEB_RCA(kc) (QK_RCTL | QK_RALT | (kc))
#define MONKEEB_RCS(kc) (QK_RCTL | QK_RSFT | (kc))
#define MONKEEB_RCG(kc) (QK_RCTL | QK_RGUI | (kc))
#define MONKEEB_RSA(kc) (QK_RSFT | QK_RALT | (kc))
#define MONKEEB_RSG(kc) (QK_RSFT | QK_RGUI | (kc))
#define MONKEEB_RAG(kc) (QK_RALT | QK_RGUI | (kc))
#define MONKEEB_RCSG(kc) (QK_RCTL | QK_RSFT | QK_RGUI | (kc))
#define MONKEEB_RCAG(kc) (QK_RCTL | QK_RALT | QK_RGUI | (kc))
#define MONKEEB_RSAG(kc) (QK_RSFT | QK_RALT | QK_RGUI | (kc))

#endif // KEYCODES_H
