#include "monkeeb_QMK_keycodes.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Check if a QMK keycode is a basic keycode with modifiers
 */
bool is_qmk_basic_with_mods(uint16_t qmk_keycode) {
    // Check if it's in the QMK modifier range
    if (qmk_keycode >= QK_MODS && qmk_keycode <= QK_MODS_MAX) {
        // Extract the basic keycode (lower 8 bits)
        uint8_t basic_key = qmk_keycode & 0xFF;
        // Verify it's a valid basic keycode
        return basic_key <= QK_BASIC_MAX;
    }

    // Also handle pure basic keycodes (no modifiers)
    return qmk_keycode >= QK_BASIC && qmk_keycode <= QK_BASIC_MAX;
}

/**
 * Convert QMK basic+modifier keycode to our 32-bit format
 */
uint32_t qmk_to_32bit_keycode(uint16_t qmk_keycode) {
    // Handle pure basic keycodes (no modifiers)
    if (qmk_keycode >= QK_BASIC && qmk_keycode <= QK_BASIC_MAX) {
        return qmk_keycode; // Direct mapping for 0x00-0xFF range
    }

    // Handle modified keycodes
    if (qmk_keycode >= QK_MODS && qmk_keycode <= QK_MODS_MAX) {
        // Extract basic keycode (lower 8 bits)
        uint8_t basic_key = qmk_keycode & 0xFF;

        // Verify it's a valid basic keycode
        if (basic_key > QK_BASIC_MAX) {
            return 0; // Invalid keycode
        }

        // Extract QMK modifier bits (upper 8 bits, shifted down)
        uint16_t qmk_mod_bits = (qmk_keycode >> 8) & 0x1F;

        // Convert QMK modifier format to our 32-bit format
        uint32_t our_mods = 0;

        // Check if this is a right modifier (bit 4 set in modifier field)
        bool is_right_modifier = (qmk_mod_bits & 0x10) != 0;

        if (is_right_modifier) {
            // Right modifiers: only set the right modifier bits
            if (qmk_mod_bits & 0x01) our_mods |= MOD32_RCTL;
            if (qmk_mod_bits & 0x02) our_mods |= MOD32_RSFT;
            if (qmk_mod_bits & 0x04) our_mods |= MOD32_RALT;
            if (qmk_mod_bits & 0x08) our_mods |= MOD32_RGUI;
        } else {
            // Left modifiers: only set the left modifier bits
            if (qmk_mod_bits & 0x01) our_mods |= MOD32_LCTL;
            if (qmk_mod_bits & 0x02) our_mods |= MOD32_LSFT;
            if (qmk_mod_bits & 0x04) our_mods |= MOD32_LALT;
            if (qmk_mod_bits & 0x08) our_mods |= MOD32_LGUI;
        }

        // Combine modifier bits with basic key
        return our_mods | basic_key;
    }

    return 0; // Not a basic keycode with modifiers
}

/**
 * Helper function to extract QMK modifiers more accurately
 * This handles the complex QMK modifier encoding
 */
uint32_t qmk_mods_to_32bit_mods(uint16_t qmk_keycode) {
    uint32_t our_mods = 0;

    // Extract QMK's modifier bits (bits 8-12)
    uint16_t qmk_mod_bits = (qmk_keycode >> 8) & 0x1F;

    // Check if this is a right modifier (bit 4 set in modifier field)
    bool is_right_modifier = (qmk_mod_bits & 0x10) != 0;

    if (is_right_modifier) {
        // Right modifiers: only set the right modifier bits
        if (qmk_mod_bits & 0x01) our_mods |= MOD32_RCTL;
        if (qmk_mod_bits & 0x02) our_mods |= MOD32_RSFT;
        if (qmk_mod_bits & 0x04) our_mods |= MOD32_RALT;
        if (qmk_mod_bits & 0x08) our_mods |= MOD32_RGUI;
    } else {
        // Left modifiers: only set the left modifier bits
        if (qmk_mod_bits & 0x01) our_mods |= MOD32_LCTL;
        if (qmk_mod_bits & 0x02) our_mods |= MOD32_LSFT;
        if (qmk_mod_bits & 0x04) our_mods |= MOD32_LALT;
        if (qmk_mod_bits & 0x08) our_mods |= MOD32_LGUI;
    }

    return our_mods;
}

/**
 * More robust converter that handles QMK's actual encoding
 */
uint32_t qmk_to_32bit_robust(uint16_t qmk_keycode) {
    // Handle pure basic keycodes
    if (qmk_keycode <= QK_BASIC_MAX) {
        return qmk_keycode;
    }

    // Handle modified keycodes
    if (qmk_keycode >= QK_MODS && qmk_keycode <= QK_MODS_MAX) {
        // Extract basic key
        uint8_t basic_key = qmk_keycode & 0xFF;
        if (basic_key > QK_BASIC_MAX) {
            return 0; // Invalid
        }

        // Convert modifiers using the robust helper
        uint32_t our_mods = qmk_mods_to_32bit_mods(qmk_keycode);

        return our_mods | basic_key;
    }

    return 0; // Not convertible
}

// // Example usage and test cases
// void test_converter() {
//     // Test basic keys
//     uint32_t result1 = qmk_to_32bit_robust(0x41); // 'A'
//     // Should return: 0x00000041

//     // Test Ctrl+A (QK_LCTL | 'A')
//     uint32_t result2 = qmk_to_32bit_robust(0x0141);
//     // Should return: 0x00008041 (MOD32_LCTL | 'A')

//     // Test Shift+A (QK_LSFT | 'A')
//     uint32_t result3 = qmk_to_32bit_robust(0x0241);
//     // Should return: 0x00004041 (MOD32_LSFT | 'A')

//     // Test Ctrl+Shift+A
//     uint32_t result4 = qmk_to_32bit_robust(0x0341);
//     // Should return: 0x0000C041 (MOD32_LCTL | MOD32_LSFT | 'A')
// }

/**
 * Reverse function: Extract components from 32-bit keycode
 */
keycode_32bit_info_t decode_32bit_keycode(uint32_t keycode) {
    keycode_32bit_info_t info = {0};

    if (keycode <= KC32_BASIC_MAX) {
        // Pure basic keycode
        info.basic_key = keycode & 0xFF;
        info.modifiers = 0;
        info.is_modified = false;
    } else if (keycode <= KC32_MODIFIED_MAX) {
        // Modified keycode
        info.basic_key = keycode & 0xFF;
        info.modifiers = (keycode >> 8) & 0xFF;
        info.is_modified = true;
    }

    return info;
}

// =============================================================================
// KEYMAP CONVERSION HELPERS
// =============================================================================

/**
 * Convert an array of QMK keycodes to 32-bit format
 */
void convert_qmk_keymap(const uint16_t* qmk_keymap, uint32_t* output_keymap, size_t count) {
    for (size_t i = 0; i < count; i++) {
        output_keymap[i] = QMK_TO_32BIT(qmk_keymap[i]);
    }
}

/**
 * Convert QMK keymap with validation and error reporting
 */
conversion_stats_t convert_qmk_keymap_with_stats(const uint16_t* qmk_keymap,
                                                uint32_t* output_keymap,
                                                size_t count) {
    conversion_stats_t stats = {0};
    stats.total_count = count;

    for (size_t i = 0; i < count; i++) {
        uint32_t converted = QMK_TO_32BIT(qmk_keymap[i]);
        output_keymap[i] = converted;

        if (converted != 0 || qmk_keymap[i] == 0) {
            stats.converted_count++;
        } else {
            stats.failed_count++;
        }
    }

    return stats;
}

// =============================================================================
// USAGE EXAMPLES
// =============================================================================

/*
Example usage in keymap definitions:

// Direct conversion of individual keys
uint32_t my_key1 = QMK_TO_32BIT(KC_A);                    // Basic 'A'
uint32_t my_key2 = QMK_TO_32BIT(LCTL(KC_A));              // Ctrl+A
uint32_t my_key3 = QMK_TO_32BIT(RSFT(KC_B));              // Right Shift+B

// Using convenience macros
uint32_t ctrl_c = QMK_LCTL_TO_32BIT(KC_C);                // Ctrl+C
uint32_t ralt_f = QMK_RALT_TO_32BIT(KC_F);                // Right Alt+F

// Converting entire keymap
const uint16_t qmk_layer[12] = {
    KC_Q,    KC_W,    KC_E,    KC_R,
    KC_A,    KC_S,    KC_D,    KC_F,
    LCTL(KC_Z), LSFT(KC_X), KC_C, KC_V
};

uint32_t new_layer[12];
convert_qmk_keymap(qmk_layer, new_layer, 12);

// With error checking
conversion_stats_t stats = convert_qmk_keymap_with_stats(qmk_layer, new_layer, 12);
printf("Converted: %zu/%zu, Failed: %zu\n",
       stats.converted_count, stats.total_count, stats.failed_count);
*/
