# VS Code / Cursor Extensions Guide

## Required Extensions

### ✅ PlatformIO IDE
- **ID**: `platformio.platformio-ide`
- **Required**: Yes
- **Purpose**: Provides complete PlatformIO integration for building, uploading, debugging, and library management
- **Why**: This is the core extension needed for ESP32-S3 development with PlatformIO

## Optional Extensions (Not Required)

### Python Extension
- **ID**: `ms-python.python`
- **Required**: No
- **Purpose**: Python syntax highlighting and IntelliSense
- **Note**: 
  - PlatformIO has its own Python environment (`~/.platformio/penv`)
  - This extension won't interfere, but it's not needed
  - If you have it installed, make sure it doesn't override PlatformIO's Python path
  - **Recommendation**: Keep it disabled or configure it to not interfere with PlatformIO

### Python Debugger
- **ID**: `ms-python.debugpy`
- **Required**: No
- **Purpose**: Python debugging support
- **Note**: 
  - Not needed for PlatformIO ESP32 projects (C/C++ code)
  - Only useful if you're writing standalone Python scripts
  - Won't help with PlatformIO build scripts
  - **Recommendation**: Safe to keep, but not needed for this project

### Anysphere Python (Cursor-specific)
- **ID**: May be built-in to Cursor or `anysphere.python`
- **Required**: No
- **Purpose**: Python support specific to Cursor IDE (by Anysphere)
- **Note**: 
  - Cursor is built by Anysphere and may have built-in Python support
  - This is similar to standard Python extension but Cursor-specific
  - Should not interfere with PlatformIO if configured correctly
  - **Recommendation**: 
    - Keep it if it's built into Cursor (can't disable)
    - Ensure it doesn't override PlatformIO's Python path
    - Check `.vscode/settings.json` - `python.defaultInterpreterPath` should be empty

## Unwanted Extensions (Can Cause Conflicts)

### ❌ C/C++ Extension Pack
- **ID**: `ms-vscode.cpptools-extension-pack`
- **Why Unwanted**: Conflicts with PlatformIO's built-in IntelliSense engine
- **Impact**: Can cause incorrect error highlighting, wrong include paths, and build issues

### ❌ C/C++ Extension
- **ID**: `ms-vscode.cpptools`
- **Why Unwanted**: PlatformIO has its own IntelliSense that understands ESP32-S3 framework
- **Impact**: Duplicate IntelliSense engines can cause confusion and performance issues

### ❌ clangd Extension
- **ID**: `llvm-vs-code-extensions.vscode-clangd`
- **Why Unwanted**: Can conflict with PlatformIO's IntelliSense
- **Note**: If you have this installed, you may see `clangd.path` in settings - this is fine, but the extension itself may cause conflicts

## Checking Your Extensions

### In Cursor/VS Code:
1. Press `Ctrl+Shift+X` to open Extensions view
2. Search for installed extensions
3. Check if you have any unwanted extensions installed

### Recommended Setup:
- ✅ **PlatformIO IDE** - Keep enabled
- ❌ **C/C++ Extension Pack** - Disable or uninstall
- ❌ **C/C++ Extension** - Disable or uninstall  
- ❌ **clangd Extension** - Disable or uninstall (if causing issues)

## Troubleshooting Extension Conflicts

### Problem: IntelliSense shows incorrect errors
**Solution**: Disable C/C++ extension and use only PlatformIO's IntelliSense

### Problem: Build fails with extension-related errors
**Solution**: 
1. Disable all C/C++ related extensions
2. Reload Cursor/VS Code
3. Try building again

### Problem: Python path issues
**Symptoms**: 
- Build fails with Python errors
- PlatformIO can't find Python
- Windows Store Python errors

**Solution**: 
1. PlatformIO uses its own Python (`~/.platformio/penv`)
2. Don't configure Python extension to override PlatformIO's Python
3. Check `.vscode/settings.json` - `platformio-ide.useBuiltinPython` should be `true`
4. Use `build.ps1` script which automatically sets correct Python path
5. If Python extension is installed, ensure it doesn't set `python.defaultInterpreterPath` to override PlatformIO

### Problem: Python Debugger conflicts
**Solution**: 
- Python Debugger extension is harmless but not needed for ESP32 projects
- It only works with Python scripts, not C/C++ code
- Safe to keep installed, but won't help with PlatformIO builds

## PlatformIO IntelliSense

PlatformIO has its own IntelliSense engine that:
- Automatically configures include paths for ESP32-S3
- Understands Arduino framework
- Works with `platformio.ini` configuration
- Updates automatically when libraries are installed

You don't need additional C/C++ extensions for IntelliSense to work properly.
