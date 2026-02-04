# Troubleshooting Guide

## Python Issues on Windows

### Problem: Windows Store Python Blocks Installation

**Symptoms:**
```
Error: Failed to install Python dependencies (exit code: 2)
Caused by: failed to create directory `C:\Program Files\WindowsApps\PythonSoftwareFoundation.Python.3.13_...`
Odmowa dostępu. (os error 5)
```

**Solution:**
PlatformIO has a built-in Python interpreter. Use it instead of Windows Store Python:

1. **Option 1: Use build script** (Recommended)
   ```powershell
   .\build.ps1
   ```

2. **Option 2: Set PATH manually**
   ```powershell
   $env:PATH = "$env:USERPROFILE\.platformio\penv\Scripts;" + ($env:PATH -replace [regex]::Escape("$env:LOCALAPPDATA\Microsoft\WindowsApps"), "")
   pio run
   ```

3. **Option 3: Configure VS Code/Cursor**
   Edit `.vscode/settings.json`:
   ```json
   {
       "platformio-ide.useBuiltinPython": true,
       "platformio-ide.customPATH": "${env:USERPROFILE}\\.platformio\\penv\\Scripts;${env:PATH}"
   }
   ```

### Verify Python
```powershell
python --version
where.exe python
```
Should show: `C:\Users\<user>\.platformio\penv\Scripts\python.exe`

## SDK Config Issues

### Problem: sdkconfig.h Not Found

**Symptoms:**
```
fatal error: sdkconfig.h: No such file or directory
```

**Solution:**
The project includes a pre-build script (`scripts/copy_sdkconfig.py`) that automatically converts `sdkconfig.t2can` to `sdkconfig.h`. This should run automatically during build.

If it doesn't work:
1. Check that `sdkconfig.t2can` exists in project root
2. Verify `extra_scripts = pre:scripts/copy_sdkconfig.py` in `platformio.ini`
3. Run `pio run --target clean` and rebuild

## Compilation Errors

### Problem: FILE type errors

**Symptoms:**
```
error: '__FILE' does not name a type
error: 'FILE' does not name a type
```

**Solution:**
This is usually related to `sdkconfig.h` being included incorrectly. The pre-build script should handle this automatically. If errors persist:

1. Clean build: `pio run --target clean`
2. Check generated `sdkconfig.h` in `.pio/build/lilygo-t2can/`
3. Verify it starts with `#ifndef SDKCONFIG_H`

## Library Issues

### Problem: Libraries Not Found

**Symptoms:**
```
fatal error: mcp2515.h: No such file or directory
```

**Solution:**
1. Install libraries: `pio lib install`
2. Check `lib_deps` in `platformio.ini`
3. Verify library names are correct (use `pio lib search`)

## Hardware Issues

### Problem: CAN Controllers Not Initializing

**Symptoms:**
```
Initialization CAN0 failed
Initialization CAN1 failed
```

**Solution:**
1. Check pin definitions in `include/BoardConfig_t2can.h`
2. Verify SPI pins are correct for your board
3. Ensure MCP2515 modules are properly connected
4. Check CAN bus termination (120Ω resistors)

## Debugging

### Enable Debug Output

Edit `src/main.cpp`:
```cpp
bool debugGeneral = true;
bool debugCAN0 = true;
bool debugCAN1 = true;
```

### Serial Monitor
```bash
pio device monitor
```

## Getting Help

1. Check `docs/TECHNICAL.md` for detailed documentation
2. Check `docs/QUICKSTART.md` for setup instructions
3. Review error messages carefully
4. Check PlatformIO documentation: https://docs.platformio.org/
