SRC += keymap_initializer.c
SRC += monkeyboard_interface.c
SRC += ../../../../modules/soycabanillas/src/monkeyboard_deferred_callbacks.c
SRC += ../../../../modules/soycabanillas/src/key_event_buffer.c
SRC += ../../../../modules/soycabanillas/src/key_press_buffer.c
SRC += ../../../../modules/soycabanillas/src/key_virtual_buffer.c
SRC += ../../../../modules/soycabanillas/src/monkeyboard_layer_manager.c
SRC += ../../../../modules/soycabanillas/src/pipeline_executor.c
SRC += ../../../../modules/soycabanillas/src/pipeline_key_replacer_initializer.c
SRC += ../../../../modules/soycabanillas/src/pipeline_key_replacer.c
SRC += ../../../../modules/soycabanillas/src/pipeline_oneshot_modifier_initializer.c
SRC += ../../../../modules/soycabanillas/src/pipeline_oneshot_modifier.c
SRC += ../../../../modules/soycabanillas/src/pipeline_tap_dance_initializer.c
SRC += ../../../../modules/soycabanillas/src/pipeline_tap_dance.c
SRC += ../../../../modules/soycabanillas/src/platform_layout.c
SRC += ../../../../modules/soycabanillas/src/platform_types.c

CONSOLE_ENABLE =  no          # Disable debugging console
DEFERRED_EXEC_ENABLE = no    # Enable deferred execution (needed for your code)
EXTRAKEY_ENABLE = no          # Disable extra keys (media, system, etc)
COMMAND_ENABLE = no           # Disable runtime command interface
NKRO_ENABLE = no              # Disable N-Key Rollover
AUDIO_ENABLE = no             # Disable audio output

# Disable all lighting and display
OLED_ENABLE = no              # Disable OLED screens
RGB_MATRIX_ENABLE = no        # Disable RGB matrix (covers most RGB functionality)
BACKLIGHT_ENABLE = no         # Disable simple backlighting
RGBLIGHT_ENABLE = no          # Disable RGB underglow (redundant with RGB_MATRIX=no, but explicit)
LED_MATRIX_ENABLE = no        # Disable single-color LED matrix

# Additional size optimizations
WPM_ENABLE = no               # Disable words per minute tracking
LEADER_ENABLE = no            # Disable leader key
COMBO_ENABLE = no             # Disable key combos
TAP_DANCE_ENABLE = no        # Disable tap dance (unless you need it)
MOUSEKEY_ENABLE = no          # Disable mouse keys
MUSIC_ENABLE = no             # Disable music mode
GRAVE_ESC_ENABLE = no         # Disable grave escape (if not using)
MAGIC_ENABLE = no             # Disable magic commands
SPACE_CADET_ENABLE = no       # Disable space cadet (if not using)
BOOTMAGIC_ENABLE = no         # Disable bootmagic completely

# Compiler optimizations for size
OPT = s                      # Optimize for size instead of speed
LTO_ENABLE = yes             # Link Time Optimization (saves significant space)

# Remove unused features
MIDI_ENABLE = no
BLUETOOTH_ENABLE = no
SPLIT_KEYBOARD = yes         # Keep this for Corne
ENCODER_ENABLE = no          # Disable rotary encoder support
POINTING_DEVICE_ENABLE = no  # Disable pointing device
AUTO_SHIFT_ENABLE = no       # Disable auto shift
KEY_OVERRIDE_ENABLE = no     # Disable key overrides

