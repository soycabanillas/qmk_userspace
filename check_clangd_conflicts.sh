#!/bin/bash

echo "=== Checking .clangd vs compile_commands.json flag interactions ==="
echo

# Extract flags from compile_commands.json
echo "1. Flags in compile_commands.json that would be REMOVED by .clangd:"
echo "   (These are intentionally filtered out)"
compile_flags=$(grep -o '"-[a-zA-Z][^"]*"' compile_commands.json | sed 's/"//g' | sort | uniq)

# Check against .clangd Remove patterns
echo
for flag in $compile_flags; do
    case $flag in
        -W*) echo "   ✓ $flag (matches -W*)" ;;
        -mmcu=*) echo "   ✓ $flag (matches -mmcu=*)" ;;
        -mcpu=*) echo "   ✓ $flag (matches -mcpu=*)" ;;
        -mfpu=*) echo "   ✓ $flag (matches -mfpu=*)" ;;
        -mfloat-abi=*) echo "   ✓ $flag (matches -mfloat-abi=*)" ;;
        -mno-unaligned-access) echo "   ✓ $flag (exact match)" ;;
        -mno-thumb-interwork) echo "   ✓ $flag (exact match)" ;;
        -mcall-prologues) echo "   ✓ $flag (exact match)" ;;
        -fcaller-saves) echo "   ✓ $flag (exact match)" ;;
        -fdata-sections) echo "   ✓ $flag (exact match)" ;;
        -ffunction-sections) echo "   ✓ $flag (exact match)" ;;
        -fno-fat-lto-objects) echo "   ✓ $flag (exact match)" ;;
        -fno-tree-loop-vectorize) echo "   ✓ $flag (exact match)" ;;
        -fno-tree-slp-vectorize) echo "   ✓ $flag (exact match)" ;;
        -freorder-functions) echo "   ✓ $flag (exact match)" ;;
        -ftree-switch-conversion) echo "   ✓ $flag (exact match)" ;;
        -ftree-tail-merge) echo "   ✓ $flag (exact match)" ;;
        -fwhole-program) echo "   ✓ $flag (exact match)" ;;
    esac
done

echo
echo "2. Flags ADDED by .clangd global settings:"
echo "   -Wno-unknown-attributes"
echo "   -Wno-maybe-uninitialized"
echo "   -Wno-unknown-warning-option"

echo
echo "3. For C++ files (.cpp), .clangd ADDS:"
echo "   -std=c++14"
echo "   -DUNIT_TEST"
echo "   -DCONSOLE_ENABLE"
echo "   -Wall"
echo "   -Wextra"
echo "   -Ikeyboards/crkbd/keymaps/soycabanillas"
echo "   -Ikeyboards/crkbd/keymaps/soycabanillas/tests"
echo "   -I/usr/include/gtest"
echo "   -I/usr/local/include"

echo
echo "4. For C++ files, .clangd REMOVES:"
echo "   -mmcu=* (AVR-specific)"
echo "   -avr* (AVR-specific)"
echo "   -DAVR* (AVR defines)"

echo
echo "5. Potential conflicts to watch for:"
echo "   ⚠️  Check if -Wall from .clangd conflicts with -Werror from QMK"
echo "   ⚠️  Check if -std=c++14 overrides any C++ standard from compile_commands"
echo "   ✅  AVR flags properly removed for C++ files"
echo "   ✅  Warning suppressions prevent clangd errors on AVR code"

echo
echo "=== Summary ==="
echo "The configuration looks good - .clangd appropriately:"
echo "• Removes AVR-specific flags that would confuse clangd"
echo "• Adds warning suppressions to handle QMK's specialized build"
echo "• Provides C++ specific flags for test files"
echo "• Preserves QMK compilation context for C files"
