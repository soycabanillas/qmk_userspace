# QMK Userspace test build integration
# Include test build rules from keymaps

# Check if we're in the QMK_USERSPACE context
ifdef QMK_USERSPACE
    # Set paths for userspace tests
    TESTPATH := $(QMK_USERSPACE)/keyboards/crkbd/keymaps/soycabanillas/tests
    KEYMAP_PATH := $(QMK_USERSPACE)/keyboards/crkbd/keymaps/soycabanillas

    # Include userspace-specific test rules
    include $(QMK_USERSPACE)/keyboards/crkbd/keymaps/soycabanillas/tests/rules.mk
endif
