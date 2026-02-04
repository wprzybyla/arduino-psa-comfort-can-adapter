# Technical Documentation - PSA Comfort CAN Adapter

## Table of Contents
1. [System Architecture](#system-architecture)
2. [Hardware Configuration](#hardware-configuration)
3. [CAN Bus Communication](#can-bus-communication)
4. [Code Structure](#code-structure)
5. [Configuration Variables](#configuration-variables)
6. [EEPROM Layout](#eeprom-layout)
7. [CAN Message Mapping](#can-message-mapping)
8. [Key Functions](#key-functions)
9. [Development Guide](#development-guide)

---

## System Architecture

### Overview
The PSA Comfort CAN Adapter is a bidirectional CAN bus translator that bridges communication between:
- **CAN2004** (old BSI protocol) - connected to vehicle's original CAN bus
- **CAN2010** (new protocol) - connected to modern PSA devices (NAC, SMEG, Color Matrix)

### Hardware Setup
- **Board**: LilyGO T2CAN (ESP32-S3 with dual MCP2515 CAN controllers)
- **CAN0** (destination): Connected to vehicle's CAN2004 bus
- **CAN1** (source): Connected to CAN2010 device (NAC/SMEG/etc.)
- **RTC**: DS1307/DS3231 via I2C (QWIIC interface on T2CAN)

### Data Flow
```
Vehicle CAN2004 Bus <---> CAN0 (MCP2515) <---> ESP32-S3 <---> CAN1 (MCP2515) <---> CAN2010 Device
```

---

## Hardware Configuration

### Pin Definitions (LilyGO T2CAN)
Located in `include/BoardConfig_t2can.h`:

```cpp
// SPI for MCP2515
BOARD_SCK_PIN  = 12  // SPI Clock
BOARD_MISO_PIN = 13  // SPI Master In Slave Out
BOARD_MOSI_PIN = 11  // SPI Master Out Slave In

// CAN Controllers
BOARD_CAN1_CS_PIN = 10  // CAN0 (destination) - vehicle CAN2004
BOARD_CAN2_CS_PIN = 14  // CAN1 (source) - CAN2010 device

// I2C for RTC
BOARD_SDA_PIN = 8   // I2C Data
BOARD_SCL_PIN = 9   // I2C Clock
```

### CAN Bus Settings
Located in `include/config.h`:
- **CAN Speed**: 125 kbps (Entertainment CAN bus - Low speed)
- **MCP2515 Frequency**: 16 MHz
- **Serial Speed**: 115200 baud

---

## CAN Bus Communication

### CAN Message Processing Flow

#### From Vehicle (CAN0 → CAN1)
1. Read message from CAN0 (vehicle CAN2004 bus)
2. Process/transform message based on ID
3. Send transformed message to CAN1 (CAN2010 device)

#### From Device (CAN1 → CAN0)
1. Read message from CAN1 (CAN2010 device)
2. Process/transform message based on ID
3. Send transformed message to CAN0 (vehicle CAN2004 bus)

### Message Transformation Types
- **Direct Forward**: Message passed through unchanged
- **Format Conversion**: CAN2004 format → CAN2010 format
- **Data Translation**: Values converted between protocols
- **Message Generation**: New messages created from received data
- **Filtering**: Some messages blocked to prevent loops

---

## Code Structure

### File Organization
```
src/
├── main.cpp          # Main application (setup/loop, CAN message processing)
├── can_utils.cpp     # Utility functions (checksums, popups, date calculations)
└── cluster_test.cpp  # Instrument cluster test mode implementation

include/
├── BoardConfig_t2can.h  # Hardware pin definitions
├── config.h             # Project configuration
├── can_utils.h          # Function declarations
└── cluster_test.h       # Instrument cluster test mode declarations
```

### Main Components

#### `main.cpp`
- **Global Objects**: `CAN0`, `CAN1` (MCP2515 instances)
- **Global Variables**: State variables, configuration flags, caches
- **setup()**: Initialization, EEPROM reading, CAN bus setup, RTC sync
- **loop()**: Main message processing loop

#### `can_utils.cpp`
- **checksumm_0E6()**: Calculates checksum for CAN frame 0xE6
- **sendPOPup()**: Manages popup notifications on CAN2010 devices
- **daysSinceYearStartFct()**: Calculates day of year

#### `cluster_test.cpp`
- **clusterTestInit()**: Initializes cluster test mode
- **clusterTestLoop()**: Main test loop (sends simulated CAN2004 messages)
- **encodeRPM()**: Encodes RPM to CAN format
- **encodeSpeed()**: Encodes speed to CAN format
- **encodeOdometerBCD()**: Encodes odometer to BCD format
- **setTestScenario()**: Sets test values based on scenario (0-15)

#### `main.cpp` Helper Functions
- **eepromUpdate()**: Updates EEPROM only if value changed (protects flash wear)
  - ESP32 EEPROM is emulated using flash with limited write cycles
  - Reads before writing and skips if unchanged
  - Used in CAN message handlers to prevent unnecessary flash writes

---

## Configuration Variables

### Debug Flags
```cpp
bool debugGeneral = false;  // General debug output
bool debugCAN0 = false;     // Log all CAN0 messages
bool debugCAN1 = false;     // Log all CAN1 messages
```

### Feature Flags
```cpp
bool EconomyModeEnabled = true;           // Enable economy mode simulation
bool Send_CAN2010_ForgedMessages = false; // Send test messages to CAN0
bool TemperatureInF = false;              // Temperature unit (Celsius/Fahrenheit)
bool mpgMi = false;                       // MPG/Miles units
bool kmL = false;                         // km/L instead of L/100km
bool fixedBrightness = false;             // Force brightness value
bool noFMUX = false;                      // Enable steering wheel button remapping
bool generatePOPups = false;              // Generate popup notifications
bool CVM_Emul = true;                     // Emulate CVM (camera) messages
bool emulateVIN = false;                  // Replace VIN number
bool hasAnalogicButtons = false;          // Use analog buttons instead of FMUX
bool listenCAN2004Language = false;       // Sync language from CAN2004
bool testClusterMode = false;             // Enable instrument cluster test mode
```

### Cluster Test Mode Configuration
```cpp
bool testClusterMode = false;        // Enable/disable cluster test mode
int testSpeed = 0;                   // Vehicle speed (km/h)
int testRPM = 0;                     // Engine RPM
int testFuel = 0;                    // Fuel level (0-100%)
unsigned long testOdometer = 0;     // Odometer (km)
bool testIgnition = true;            // Ignition state
byte testScenario = 0;               // Test scenario (0-15)
bool testAutoIncrement = false;      // Auto-cycle through scenarios
int testOilTemp = 0xAC;             // Oil temperature (default 0xAC)
```

**Note**: Cluster test mode simulates CAN2004 messages and sends them to CAN1 (CAN2010 cluster) for testing without a connected car/BSI. Messages are sent in CAN2004 format but routed to CAN2010 device, mimicking normal adapter behavior.

### Steering Wheel Commands Type
```cpp
byte steeringWheelCommands_Type = 0;
// 0 = Generic
// 1 = C4 I / C5 X7 NAV+MUSIC+APPS+PHONE mapping
// 2 = C4 I / C5 X7 MENU mapping
// 3 = C4 I / C5 X7 MENU + SRC on wiper button
// 4 = C4 I / C5 X7 MENU + TRIP on wiper button
// 5 = C4 I / C5 X7 MENU + SRC on wiper + TRIP on ESC
```

### Language Settings
```cpp
byte languageID = 0;  // 0=FR, 1=EN, 2=DE, 3=ES, 4=IT, 5=PT, 6=NL, 9=BR, 12=TR, 14=RU
```

---

## EEPROM Layout

| Address | Size | Description |
|---------|------|-------------|
| 0 | 1 byte | Language and unit number (CAN2010) |
| 1 | 1 byte | Language ID (CAN2004) |
| 2 | 1 byte | Language ID (CAN2010) |
| 3 | 1 byte | Temperature unit (0=Celsius, 1=Fahrenheit) |
| 4 | 1 byte | MPG/Miles unit (0=no, 1=yes) |
| 5 | 1 byte | Default day |
| 6 | 1 byte | Default month |
| 7 | 2 bytes | Default year (int) |
| 10-16 | 7 bytes | Personalization settings |

### Flash Wear Protection

**Important**: ESP32 EEPROM is emulated using flash storage, which has limited write cycles (typically 10,000-100,000 writes per sector). To protect flash from unnecessary wear:

- **`eepromUpdate()` function**: Used in CAN message handlers to update EEPROM only when values actually change
  - Reads current value before writing
  - Skips write if value is unchanged
  - Prevents unnecessary flash writes during normal operation

- **Usage**:
  - CAN message handlers use `eepromUpdate()` for frequently-triggered settings (language, temperature, personalization)
  - `EEPROM.write()` is only used in `resetEEPROM` block (intentional reset)
  - `EEPROM.put()` is used for multi-byte values (year)

**Example**:
```cpp
// Good: Only writes if value changed
eepromUpdate(0, languageAndUnitNum);

// Avoid: Always writes (causes flash wear)
EEPROM.write(0, languageAndUnitNum);
```

---

## CAN Message Mapping

### Messages from Vehicle (CAN0 → CAN1)

#### 0x36 - Economy Mode & Brightness
- **Length**: 8 bytes
- **Function**: Detects economy mode, forwards brightness value
- **Processing**: Adjusts brightness if `fixedBrightness` enabled

#### 0xB6 - Engine RPM & Vehicle Speed
- **Length**: 8 bytes
- **Function**: Extracts engine RPM and vehicle speed
- **Processing**: Calculates `engineRPM` and `vehicleSpeed`, detects engine running state

#### 0x336, 0x3B6, 0x2B6 - VIN Number
- **Length**: 3, 6, 8 bytes
- **Function**: Vehicle Identification Number
- **Processing**: Replaces VIN if `emulateVIN` enabled

#### 0xE6 - ABS Status
- **Length**: < 8 bytes
- **Function**: ABS status frame
- **Processing**: Extends to 8 bytes, calculates checksum using `checksumm_0E6()`

#### 0x21F - Steering Wheel Commands (Generic)
- **Length**: 3 bytes
- **Function**: Generic steering wheel button commands
- **Processing**: Can remap SRC to MENU if `noFMUX` enabled

#### 0xA2 - Steering Wheel Commands (C4 I / C5 X7)
- **Length**: Variable
- **Function**: Advanced steering wheel commands
- **Processing**: Complex mapping based on `steeringWheelCommands_Type`

#### 0x217 - Cluster Status (CMB)
- **Length**: 8 bytes
- **Function**: Instrument cluster status
- **Processing**: Cached in `statusCMB[]` array

#### 0x1D0 - Climate Control
- **Length**: 7 bytes
- **Function**: HVAC system status
- **Processing**: Converts to CAN2010 format (0x350), extracts:
  - Left/Right temperature
  - Fan speed
  - Fan position
  - Air conditioning state
  - Air recycle mode
  - Defrost mode

#### 0xF6 - Ignition & External Temperature
- **Length**: 8 bytes
- **Function**: Ignition status and external temperature
- **Processing**: Detects ignition state, extracts temperature

#### 0x168 - Instrument Panel
- **Length**: 8 bytes
- **Function**: Instrument panel status
- **Processing**: Modifies bits for CAN2010 compatibility

#### 0x120 - Alerts Journal
- **Length**: 8 bytes
- **Function**: Diagnostic alerts
- **Processing**: Generates popup notifications if `generatePOPups` enabled

#### 0x221 - Trip Information
- **Length**: 8 bytes
- **Function**: Trip computer data
- **Processing**: Cached in `statusTRIP[]`, generates fake EMF time frame (0x3F6)

#### 0x128 - Instrument Panel (Alternative)
- **Length**: 8 bytes
- **Function**: Alternative instrument panel format
- **Processing**: Converts gearbox reports, alerts, seatbelt status

#### 0x3A7 - Maintenance
- **Length**: 8 bytes
- **Function**: Maintenance intervals
- **Processing**: Converts to new format (0x3E7)

#### 0x1A8 - Cruise Control
- **Length**: 8 bytes
- **Function**: Cruise control status
- **Processing**: Converts to new format (0x228)

#### 0x2D7 - CAN2004 Matrix Language
- **Length**: 5 bytes
- **Function**: Language change from CAN2004 matrix
- **Processing**: Updates language if `listenCAN2004Language` enabled

#### 0x361 - Personalization Menus Availability
- **Length**: Variable
- **Function**: Available personalization options
- **Processing**: Converts to CAN2010 format

#### 0x260 - Personalization Settings Status
- **Length**: 8 bytes
- **Function**: Current personalization settings
- **Processing**: Complex conversion, generates multiple response frames

#### 0x321 - DrumVlado Frame
- **Length**: < 5 bytes
- **Function**: Special frame reconstruction
- **Processing**: Extends to 5 bytes

### Messages from Device (CAN1 → CAN0)

#### 0x39B - Time/Date Setting
- **Length**: 5 bytes
- **Function**: Set RTC time from CAN2010 device
- **Processing**: Updates RTC, EEPROM, sends clock frame (0x228)

#### 0x1A9 - Telematic Commands
- **Length**: 8 bytes
- **Function**: Commands from telematic device
- **Processing**: Extracts commands (trip reset, dark mode, etc.), generates fake EMF status (0x167)

#### 0x329 - ESP/ASR Status
- **Length**: 8 bytes
- **Function**: ESP system status
- **Processing**: Extracts ASR button state

#### 0x31C - MATT Status
- **Length**: 5 bytes
- **Function**: MATT (trip computer) status
- **Processing**: Adds trip reset commands if needed

#### 0x217 - Cluster Status
- **Length**: 8 bytes
- **Function**: Cluster status from CAN2010
- **Processing**: Adds telematic commands, forwards to vehicle

#### 0x15B - Personalization Settings Change
- **Length**: 8 bytes
- **Function**: Personalization settings from CAN2010
- **Processing**: Converts to CAN2004 format, stores in EEPROM

#### 0x1E9 - Telematic Suggested Speed (CVM Emulation)
- **Length**: >= 2 bytes
- **Function**: Speed limit from navigation
- **Processing**: Generates fake CVM frame (0x268) if `CVM_Emul` enabled

#### 0x1E5 - Ambience Settings
- **Length**: 7 bytes
- **Function**: Audio ambience and balance settings
- **Processing**: Converts CAN2010 format to CAN2004 format

---

## Key Functions

### `checksumm_0E6(const byte* frame)`
Calculates checksum for CAN frame 0xE6 (ABS status).

**Algorithm**:
1. Sum all nibbles (4-bit values) from bytes 0-6
2. Add iteration counter
3. XOR with 0xFF, subtract 3, mask to 4 bits
4. XOR with iteration counter shifted left 4 bits
5. Increment iteration counter (0-15, wraps)

**Usage**: Called when processing 0xE6 frame to extend it to 8 bytes.

### `sendPOPup(bool present, int id, byte priority, byte parameters)`
Manages popup notifications on CAN2010 devices.

**Parameters**:
- `present`: true to show, false to clear
- `id`: Alert/notification ID
- `priority`: Priority level (0-14)
- `parameters`: Additional parameters

**Functionality**:
- Maintains cache of active popups (max 8)
- Prevents duplicate notifications
- Clears previous popup before showing new one
- Sends CAN message 0x1A1 to CAN2010 device

### `daysSinceYearStartFct()`
Calculates number of days since January 1st of current year.

**Usage**: Used in trip information processing to generate time stamps.

---

## Development Guide

### Adding New CAN Message Handler

1. **Identify Message ID**: Determine CAN ID to handle
2. **Add Handler in loop()**: 
   ```cpp
   else if (id == 0xXXX && len == Y) {
       // Your processing code
   }
   ```
3. **Determine Direction**: 
   - CAN0 → CAN1: Add in first `CAN0.readMessage()` block
   - CAN1 → CAN0: Add in second `CAN1.readMessage()` block
4. **Process Message**: Transform data as needed
5. **Send Message**: Use `CAN0.sendMessage()` or `CAN1.sendMessage()`

### Adding New Utility Function

1. **Add Declaration**: In `include/can_utils.h`
2. **Add Implementation**: In `src/can_utils.cpp`
3. **Document Function**: Add Doxygen-style comments

### Modifying Pin Configuration

1. **Update BoardConfig**: Edit `include/BoardConfig_t2can.h`
2. **Verify SPI Init**: Ensure `initSPIForT2CAN()` uses correct pins
3. **Test Hardware**: Verify all connections work

### Debugging

1. **Enable Debug Flags**:
   ```cpp
   bool debugGeneral = true;
   bool debugCAN0 = true;  // Log all CAN0 messages
   bool debugCAN1 = true;  // Log all CAN1 messages
   ```

2. **Monitor Serial**: Use `pio device monitor` at 115200 baud

3. **CAN Message Format**: 
   ```
   FRAME:ID=0xXXX:LEN=Y:BYTE0:BYTE1:...:BYTE7
   ```

### Testing

1. **Unit Testing**: Add tests in `test/` directory
2. **Hardware Testing**: Use CAN bus analyzer/monitor
3. **Integration Testing**: Test with actual vehicle and device

---

## Troubleshooting

### Common Issues

#### CAN Messages Not Received
- Check CAN bus speed (125 kbps)
- Verify MCP2515 initialization
- Check SPI connections
- Verify CS pin configuration

#### SPI Initialization Issues
- Ensure `initSPIForT2CAN()` is called before MCP2515 constructors
- Verify pin definitions match hardware
- Check for pin conflicts

#### RTC Not Working
- Verify I2C connections (SDA/SCL)
- Check RTC module address
- Ensure RTC is properly initialized in setup()

#### EEPROM Issues
- Check if `resetEEPROM` is set to true (will erase all data)
- Verify EEPROM address ranges
- Check for address conflicts

---

## References

- [Original Tutorial](http://www.forum-peugeot.com/Forum/threads/tuto-adaptateur-pour-smeg-nac-en-seconde-monte-sur-bsi-evolu%C3%A9.18068/)
- [MCP2515 Library](https://github.com/autowp/arduino-mcp2515)
- [DS1307RTC Library](https://github.com/PaulStoffregen/DS1307RTC)
- [Time Library](https://github.com/PaulStoffregen/Time)

---

## Version History

- **v1.0** - Initial PlatformIO conversion, LilyGO T2CAN support
- **v0.x** - Original Arduino IDE version

---

*Last Updated: 2025*
