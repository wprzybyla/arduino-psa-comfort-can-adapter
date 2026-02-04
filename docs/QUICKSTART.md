# Quick Start Guide

## First Time Setup

### 1. Clone and Open
```bash
git clone https://github.com/wprzybyla/arduino-psa-comfort-can-adapter.git
cd arduino-psa-comfort-can-adapter
```

### 2. Install PlatformIO
- Install [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) (VS Code extension recommended)
- Or install [PlatformIO Core](https://platformio.org/install/cli)

### 3. Build Project

**Windows (Recommended):**
```powershell
.\build.ps1
```

**Linux/Mac or Direct:**
```bash
pio run
```

### 4. Upload to Board

**Windows:**
```powershell
.\build.ps1 --target upload
```

**Linux/Mac or Direct:**
```bash
pio run --target upload
```

### 5. Monitor Serial Output
```bash
pio device monitor
```

**Note**: The `build.ps1` script automatically uses PlatformIO's built-in Python, avoiding Windows Store Python issues. See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for details.

## Basic Configuration

### Enable Debug Mode
Edit `src/main.cpp`:
```cpp
bool debugGeneral = true;  // Enable general debug
bool debugCAN0 = true;     // Log all CAN0 messages
bool debugCAN1 = true;     // Log all CAN1 messages
```

### Configure Language
Edit `src/main.cpp`:
```cpp
byte languageID = 1;  // 0=FR, 1=EN, 2=DE, 3=ES, 4=IT, 5=PT, 6=NL, 9=BR, 12=TR, 14=RU
```

### Enable Features
Edit `src/main.cpp`:
```cpp
bool generatePOPups = true;      // Enable popup notifications
bool noFMUX = true;              // Enable steering wheel button remapping
bool CVM_Emul = true;            // Enable CVM (camera) emulation
```

## Hardware Connections

### LilyGO T2CAN
- **CAN0** (pin 10): Connect to vehicle CAN2004 bus
- **CAN1** (pin 14): Connect to CAN2010 device (NAC/SMEG)
- **I2C** (pins 8/9): Connect RTC module (optional)

### CAN Bus Wiring
- Use proper CAN transceivers (MCP2551 or similar)
- Connect CAN_H and CAN_L with 120Ω termination resistors
- Ensure proper ground connection

## Testing

### 1. Check CAN Communication
Enable debug mode and monitor serial output. You should see:
```
Initialization CAN0
Initialization CAN1
RTC has set the system time
```

### 2. Verify Message Flow
With `debugCAN0` and `debugCAN1` enabled, you'll see all CAN messages:
```
FRAME:ID=0x36:LEN=8:00:01:02:03:04:05:06:07
```

### 3. Instrument Cluster Test Mode

The adapter includes a test mode for testing CAN2010 instrument clusters without a connected car/BSI. This mode simulates CAN2004 messages and sends them to the CAN2010 cluster.

**Enable Test Mode:**
Edit `src/main.cpp`:
```cpp
bool testClusterMode = true;  // Enable cluster test mode
```

**Configure Test Values:**
```cpp
int testSpeed = 50;           // Vehicle speed (km/h)
int testRPM = 2000;           // Engine RPM
int testFuel = 50;            // Fuel level (0-100%)
unsigned long testOdometer = 12345;  // Odometer (km)
bool testIgnition = true;     // Ignition state
byte testScenario = 3;        // Test scenario (0-15)
bool testAutoIncrement = true; // Auto-cycle through scenarios
```

**Test Scenarios:**
- 0: All zeros
- 1-15: Various speed/RPM/fuel combinations (see `cluster_test.cpp` for details)

**Note**: Test mode sends CAN2004 format messages to CAN1 (CAN2010 cluster), simulating normal adapter behavior where CAN2004 messages from the car are forwarded to CAN2010 devices.

### 4. Test Features
- Change language in CAN2010 device → should sync
- Adjust climate control → should translate
- Press steering wheel buttons → should remap (if enabled)

## Common Issues

### No CAN Messages
- Check CAN bus speed (125 kbps)
- Verify MCP2515 connections
- Check termination resistors

### RTC Not Working
- Verify I2C connections
- Check RTC module address
- Ensure RTC is initialized

### Compilation Errors
- **Windows**: Use `.\build.ps1` script to avoid Python issues (recommended)
- Ensure all libraries are installed: `pio lib install`
- Check Python installation (PlatformIO has built-in Python)
- Verify `platformio.ini` configuration
- See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for detailed solutions

## Next Steps

- Read [TECHNICAL.md](TECHNICAL.md) for complete documentation
- Check [README.md](../README.md) for project overview
- Review code comments in `src/main.cpp` for detailed explanations

---

*For detailed technical information, see [TECHNICAL.md](TECHNICAL.md)*
