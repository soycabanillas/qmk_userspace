# Test build configuration for tap dance functionality

# Basic tap functionality test
tap_dance_basic_tap_functionality_SRC := \
    $(TESTPATH)/test_tap_dance_basic_tap.cpp \
    $(TESTPATH)/mock_qmk.cpp \
    $(KEYMAP_PATH)/pipeline_tap_dance.c \
    $(KEYMAP_PATH)/abstractionsqmk.c

tap_dance_basic_tap_functionality_DEFS := $(QMK_DEFS)
tap_dance_basic_tap_functionality_INC := \
    $(QUANTUM_DIR) \
    $(TMK_DIR) \
    $(TESTPATH) \
    $(KEYMAP_PATH)

# Basic hold functionality test
tap_dance_basic_hold_functionality_SRC := \
    $(TESTPATH)/test_tap_dance_basic_hold.cpp \
    $(TESTPATH)/mock_qmk.cpp \
    $(KEYMAP_PATH)/pipeline_tap_dance.c \
    $(KEYMAP_PATH)/abstractionsqmk.c

tap_dance_basic_hold_functionality_DEFS := $(QMK_DEFS)
tap_dance_basic_hold_functionality_INC := \
    $(QUANTUM_DIR) \
    $(TMK_DIR) \
    $(TESTPATH) \
    $(KEYMAP_PATH)

# Multiple tap functionality test
tap_dance_multiple_tap_functionality_SRC := \
    $(TESTPATH)/test_tap_dance_multiple_tap.cpp \
    $(TESTPATH)/mock_qmk.cpp \
    $(KEYMAP_PATH)/pipeline_tap_dance.c \
    $(KEYMAP_PATH)/abstractionsqmk.c

tap_dance_multiple_tap_functionality_DEFS := $(QMK_DEFS)
tap_dance_multiple_tap_functionality_INC := \
    $(QUANTUM_DIR) \
    $(TMK_DIR) \
    $(TESTPATH) \
    $(KEYMAP_PATH)

# Tap-hold interaction test
tap_dance_tap_hold_interaction_SRC := \
    $(TESTPATH)/test_tap_dance_tap_hold_interaction.cpp \
    $(TESTPATH)/mock_qmk.cpp \
    $(KEYMAP_PATH)/pipeline_tap_dance.c \
    $(KEYMAP_PATH)/abstractionsqmk.c

tap_dance_tap_hold_interaction_DEFS := $(QMK_DEFS)
tap_dance_tap_hold_interaction_INC := \
    $(QUANTUM_DIR) \
    $(TMK_DIR) \
    $(TESTPATH) \
    $(KEYMAP_PATH)

# Layer switching test
tap_dance_layer_switching_SRC := \
    $(TESTPATH)/test_tap_dance_layer_switching.cpp \
    $(TESTPATH)/mock_qmk.cpp \
    $(KEYMAP_PATH)/pipeline_tap_dance.c \
    $(KEYMAP_PATH)/abstractionsqmk.c

tap_dance_layer_switching_DEFS := $(QMK_DEFS)
tap_dance_layer_switching_INC := \
    $(QUANTUM_DIR) \
    $(TMK_DIR) \
    $(TESTPATH) \
    $(KEYMAP_PATH)

# Timeout behavior test
tap_dance_timeout_behavior_SRC := \
    $(TESTPATH)/test_tap_dance_timeout.cpp \
    $(TESTPATH)/mock_qmk.cpp \
    $(KEYMAP_PATH)/pipeline_tap_dance.c \
    $(KEYMAP_PATH)/abstractionsqmk.c

tap_dance_timeout_behavior_DEFS := $(QMK_DEFS)
tap_dance_timeout_behavior_INC := \
    $(QUANTUM_DIR) \
    $(TMK_DIR) \
    $(TESTPATH) \
    $(KEYMAP_PATH)

# Interruption handling test
tap_dance_interruption_handling_SRC := \
    $(TESTPATH)/test_tap_dance_interruption.cpp \
    $(TESTPATH)/mock_qmk.cpp \
    $(KEYMAP_PATH)/pipeline_tap_dance.c \
    $(KEYMAP_PATH)/abstractionsqmk.c

tap_dance_interruption_handling_DEFS := $(QMK_DEFS)
tap_dance_interruption_handling_INC := \
    $(QUANTUM_DIR) \
    $(TMK_DIR) \
    $(TESTPATH) \
    $(KEYMAP_PATH)
