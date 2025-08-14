# Development Setup & IntelliSense Configuration

This document explains the development environment setup for this custom QMK keymap with C++ testing.

## VS Code Extensions & IntelliSense

This project uses **clangd** for IntelliSense (as recommended by QMK's official dev container).

### ✅ Required Extensions (automatically installed by dev container):
- `llvm-vs-code-extensions.vscode-clangd` - Modern C/C++ language server
- `xaver.clang-format` - Code formatting
- `EditorConfig.EditorConfig` - Consistent coding style

### ⚠️ Important - Extension Conflicts:
**Do NOT install `ms-vscode.cpptools` (Microsoft C/C++ extension)** as it will conflict with clangd.

If you accidentally install it, you have two options:
1. **Recommended**: Disable/uninstall `ms-vscode.cpptools`
2. **Alternative**: Add `"C_Cpp.intelliSenseEngine": "disabled"` to `.vscode/settings.json`

The `clangd.detectExtensionConflicts: true` setting will warn you about conflicts automatically.

## IntelliSense Data Flow

```
QMK Build System → compile_commands.json → clangd → VS Code Extension → IntelliSense
```

1. **QMK generates compilation database**: `qmk compile --compiledb` creates `compile_commands.json`
2. **clangd reads build info**: Uses exact compiler flags, includes, and defines
3. **VS Code displays results**: Code completion, error checking, go-to-definition

## Build Commands

### For IntelliSense Updates (when rules.mk changes):
```bash
# Regenerates compile_commands.json for updated IntelliSense
qmk compile -kb crkbd -km soycabanillas --compiledb
```

### For Regular Development:
```bash
# Faster incremental builds
make crkbd:soycabanillas
```

### For C++ Tests:
```bash
# Clean test build
make -f Makefile.fixed clean

# Build and run all tests
make -f Makefile.fixed comprehensive-fixed

# Build single test
make -f Makefile.fixed build/tests/test_basic_tap_fixed
```

## Configuration Files

- **`.clangd`** - Main language server configuration (C vs C++ handling)
- **`.vscode/settings.json`** - VS Code workspace settings (clangd arguments)
- **`.vscode/tasks.json`** - Build tasks for convenience
- **`compile_commands.json`** - Generated compilation database (do not edit manually)

## Mixed C/C++ Project Structure

This project combines:
- **C code**: QMK firmware implementation (`*.c` files)
- **C++ code**: Google Test unit tests (`tests/*.cpp` files)

The `.clangd` configuration handles both languages appropriately:
- C files use QMK's AVR compilation flags
- C++ files use test-specific flags with Google Test includes

## Troubleshooting IntelliSense

If IntelliSense stops working:

1. **Restart clangd**: VS Code Command Palette → "clangd: Restart language server"
2. **Regenerate compilation database**: Run `qmk compile --compiledb`
3. **Check for extension conflicts**: Look for warnings about multiple C++ extensions
4. **Verify files exist**: Ensure `.clangd` and `compile_commands.json` exist

## File Synchronization Notes

This file (`DEVELOPMENT.md`) is specific to your keymap and won't be overwritten when syncing with upstream QMK userspace.

The main `README.md` in the root directory will be updated when you sync with the original QMK userspace repository.
