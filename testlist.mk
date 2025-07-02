# QMK Userspace test integration
# Include test lists from keymaps

# Check if we're in the QMK_USERSPACE context
ifdef QMK_USERSPACE
    # Include userspace-specific tests
    include $(QMK_USERSPACE)/keyboards/crkbd/keymaps/soycabanillas/tests/testlist.mk
endif
