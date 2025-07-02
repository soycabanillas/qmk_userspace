# QMK Userspace Tap Dance Tests

This directory contains comprehensive unit tests for the custom tap dance functionality implemented in the `soycabanillas` keymap for the Corne (CRKBD) keyboard.

## Overview

The tap dance functionality provides advanced key behavior that responds differently to taps, holds, and multiple taps. These tests ensure the reliability and correctness of this complex input handling system.

## Test Structure

### Test Files

- `test_tap_dance_basic_tap.cpp` - Tests basic single tap functionality
- `test_tap_dance_basic_hold.cpp` - Tests basic hold functionality 
- `test_tap_dance_multiple_tap.cpp` - Tests multiple tap sequences (double-tap, etc.)
- `test_tap_dance_tap_hold_interaction.cpp` - Tests interaction between tap and hold behaviors
- `test_tap_dance_layer_switching.cpp` - Tests layer activation/deactivation
- `test_tap_dance_timeout.cpp` - Tests timeout behavior and timing
- `test_tap_dance_interruption.cpp` - Tests interruption handling

### Mock Framework

- `mock_qmk.h` / `mock_qmk.cpp` - Mock QMK framework for testing
- Provides mocked implementations of QMK functions like `layer_on()`, `tap_code16()`, etc.
- Includes time simulation for testing timeout behaviors

## Features Tested

### Basic Tap Functionality
- Single tap on `CKC_LAY_MOUSE_Q` outputs `KC_Q`
- Single tap on `CKC_LAY_NUMBERS_R` outputs `KC_R`
- Single tap on `CKC_EXCL` outputs `KC_EXLM`
- Single tap on `CKC_QUES` outputs `KC_QUES`

### Basic Hold Functionality
- Hold on `CKC_LAY_MOUSE_Q` activates mouse layer (`_LMOUSE`)
- Hold on `CKC_LAY_NUMBERS_R` activates numbers layer (`_LNUMBERS`)
- Hold on `CKC_LAY_MOVEMENT_F` activates movement layer (`_LMOVEMENT`)
- Hold on `CKC_LAY_RIGHT_THUMB` activates right thumb layer (`_LRIGHT_THUMB`)

### Multiple Tap Functionality
- Double tap on `CKC_LAY_NUMBERS_R` outputs `S(KC_R)` (shifted R)
- Double tap on `CKC_EXCL` outputs `RALT(KC_1)`
- Double tap on `CKC_QUES` outputs `RALT(KC_SLSH)`

### Complex Behaviors
- Tap vs hold decision based on timing (200ms threshold)
- Interruption handling when other keys are pressed during decision period
- Layer activation/deactivation with proper cleanup
- Multiple simultaneous layer handling

## Configuration

The tests are configured with:
- **Tap timeout**: 200ms (defined as `g_tap_timeout`)
- **Test framework**: Google Test/Google Mock
- **Language**: C++ (tests) with C (implementation)

## Running Tests

### Prerequisites

1. **QMK Setup**: Ensure QMK is properly configured
   ```bash
   qmk setup
   qmk config user.overlay_dir="$(realpath .)"
   ```

2. **Google Test**: Ensure Google Test is available in your QMK environment

### Running All Tests

Use the provided test runner script:
```bash
./run_tap_dance_tests.sh
```

### Running Individual Tests

You can run specific test groups:
```bash
# Basic functionality
make test:tap_dance:basic_tap_functionality
make test:tap_dance:basic_hold_functionality

# Advanced functionality  
make test:tap_dance:multiple_tap_functionality
make test:tap_dance:tap_hold_interaction

# System behaviors
make test:tap_dance:layer_switching
make test:tap_dance:timeout_behavior
make test:tap_dance:interruption_handling
```

### Running Tests with Debug Output

To see detailed debug output:
```bash
make test:tap_dance:basic_tap_functionality DEBUG=1
```

## Test Implementation Details

### Mock QMK Environment

The tests use a comprehensive mock of the QMK environment that simulates:

- **Time Management**: `timer_read()`, `timer_elapsed()`, `advance_time()`
- **Layer Management**: `layer_on()`, `layer_off()`, layer state tracking
- **Key Registration**: `register_code()`, `unregister_code()`, `tap_code16_delay()`
- **Deferred Execution**: `defer_exec()`, `cancel_deferred_exec()` with time simulation
- **Debug Output**: `uprintf()` for logging

### Test Patterns

Each test follows a consistent pattern:
1. **Setup**: Initialize mock state and tap dance system
2. **Simulate**: Create key events with precise timing
3. **Verify**: Check expected behaviors occurred
4. **Cleanup**: Reset state for next test

### Key Event Simulation

```cpp
void simulate_key_event(uint16_t keycode, bool pressed, uint16_t time_offset = 0);
```

This helper function simulates keypresses with precise timing control, essential for testing timeout-dependent behaviors.

## Integration with QMK Userspace

The tests are integrated into the QMK userspace build system:

- `testlist.mk` - Defines test targets
- `rules.mk` - Defines build rules for each test
- Integration with root `testlist.mk` and `build_test.mk`

## Test Coverage

The test suite provides comprehensive coverage of:

✅ **Basic Operations** (tap, hold, layer switching)  
✅ **Timing Behaviors** (timeouts, decision periods)  
✅ **Edge Cases** (interruptions, rapid inputs, state transitions)  
✅ **Error Handling** (invalid inputs, boundary conditions)  
✅ **State Management** (cleanup, multiple layers, reset scenarios)

## Extending Tests

To add new tests:

1. **Create Test File**: Follow naming pattern `test_tap_dance_*.cpp`
2. **Update testlist.mk**: Add new test target
3. **Update rules.mk**: Add build configuration
4. **Follow Patterns**: Use existing test structure for consistency

## Troubleshooting

### Common Issues

1. **Build Errors**: Ensure QMK userspace is properly configured
2. **Test Failures**: Check mock state between tests (reset_mock_state())
3. **Timing Issues**: Verify timeout values match implementation (200ms)

### Debug Tips

- Use `DEBUG=1` flag for verbose output
- Add `uprintf()` statements in implementation for debugging
- Check mock state counters to verify expected function calls
- Use debugger on test executables in `./build/test/` folder

## Contributing

When modifying the tap dance implementation:
1. Run all tests to ensure no regressions
2. Add tests for new functionality
3. Update documentation for new behaviors
4. Ensure tests pass in CI environment

## License

These tests are part of the QMK userspace and follow the same licensing as the main codebase.
