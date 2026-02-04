# PSA Comfort CAN Adapter

Arduino/ESP32-S3 project to operate new PSA comfort devices (CAN2010) on old BSI CAN-BUS (CAN2004).

This adapter allows you to connect modern PSA CAN2010 devices (like NAC, SMEG, Color Matrix) to older vehicles with CAN2004 BSI.

## Features

- Bidirectional CAN bus translation between CAN2004 and CAN2010
- Support for dual MCP2515 CAN controllers (LilyGO T2CAN board)
- Real-time clock (RTC) support via DS1307/DS3231
- Language and unit conversion
- Climate control translation
- Steering wheel commands mapping
- Alert/notification system
- Personalization settings sync

## Hardware

### Supported Boards

- **LilyGO T2CAN** (ESP32-S3 with dual MCP2515 CAN controllers) - Primary target
- ESP32-S3-DevKitC-1 (with external CAN shields)

### Required Components

- ESP32-S3 board (LilyGO T2CAN recommended)
- Dual MCP2515 CAN controllers (built-in on T2CAN)
- DS1307 or DS3231 RTC module (optional, for time sync)
- CAN transceivers (MCP2551 or similar)

## Project Structure

```
psa-comfort-can-adapter/
├── include/              # Header files
│   ├── BoardConfig_t2can.h  # LilyGO T2CAN pin definitions
│   ├── config.h            # Project configuration
│   └── can_utils.h         # CAN utility functions declarations
├── src/                  # Source files
│   ├── main.cpp           # Main application (setup/loop)
│   └── can_utils.cpp      # CAN utility functions implementation
├── lib/                  # Private libraries (if any)
├── test/                 # Unit tests
├── platformio.ini        # PlatformIO configuration
└── README.md            # This file
```

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- USB cable for programming
- CAN bus connection to vehicle

### Installation

1. Clone the repository:
```bash
git clone https://github.com/wprzybyla/arduino-psa-comfort-can-adapter.git
cd arduino-psa-comfort-can-adapter
```

2. Open project in PlatformIO:
```bash
pio project init
```

3. Build the project:
```bash
pio run
```

4. Upload to board:
```bash
pio run --target upload
```

5. Monitor serial output:
```bash
pio device monitor
```

### Configuration

Edit `include/config.h` to adjust:
- CAN bus speed (default: 125 kbps)
- Serial speed (default: 115200)
- Pin definitions (if using different board)

For LilyGO T2CAN, pin definitions are in `include/BoardConfig_t2can.h`.

## Usage

### CAN Bus Connections

- **CAN0** (destination): Connected to vehicle's CAN2004 bus
- **CAN1** (source): Connected to CAN2010 device (NAC/SMEG/etc.)

### Features Configuration

Edit variables in `src/main.cpp`:

- `debugGeneral`, `debugCAN0`, `debugCAN1`: Enable debug output
- `EconomyModeEnabled`: Enable/disable economy mode
- `TemperatureInF`: Temperature unit (Celsius/Fahrenheit)
- `languageID`: Default language (0=FR, 1=EN, 2=DE, etc.)
- `generatePOPups`: Enable popup notifications from alerts
- `noFMUX`: Enable steering wheel button remapping

## Real Life Examples

> ![NAC on RCZ](https://i.imgur.com/Nb3CrUN.jpg)
> NAC (Connected NAV) on Peugeot RCZ (2012)

> ![COLOR MATRIX on RCZ](https://i.imgur.com/7zP7qf6.jpg)
> Color 2010 Matrix from Peugeot 2008 (2019) on Peugeot RCZ (2012)

> ![NAC on C4](https://i.imgur.com/Fjc8PhI.jpg)
> NAC (Connected NAV) on Citroen C4 first model (2004)

> ![NAC on 207](https://i.imgur.com/rdJuei6.jpg)
> NAC (Connected NAV) on Peugeot 207 (2010)

## Documentation

- [Original Tutorial (Forum-Peugeot.com)](http://www.forum-peugeot.com/Forum/threads/tuto-adaptateur-pour-smeg-nac-en-seconde-monte-sur-bsi-evolu%C3%A9.18068/)

## Development

### Code Structure

- **main.cpp**: Main application loop, CAN message processing, state management
- **can_utils.cpp**: Helper functions for CAN operations (checksums, popups, date calculations)
- **config.h**: Centralized configuration and pin definitions
- **BoardConfig_t2can.h**: Hardware-specific pin mappings for LilyGO T2CAN

### Adding New Features

1. **New CAN message handler**: Add to `loop()` function in `main.cpp`
2. **New utility function**: Add to `can_utils.cpp` and declare in `can_utils.h`
3. **New board support**: Create new `BoardConfig_*.h` and update `config.h`

### Testing

Run unit tests (when implemented):
```bash
pio test
```

## Libraries

This project uses the following libraries (managed by PlatformIO):

- `autowp/arduino-mcp2515@^1.2.0` - MCP2515 CAN controller library
- `paulstoffregen/DS1307RTC@^1.4.1` - RTC support
- `paulstoffregen/Time@^1.6.1` - Time library

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Credits

- Original project: [Ludwig V.](https://github.com/ludwig-v)
- Contributions: [Nick V. (V3nn3tj3)](https://github.com/v3nn3tj3)
- Adapted for LilyGO T2CAN and PlatformIO by [wprzybyla](https://github.com/wprzybyla)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

For issues and questions, please open an issue on GitHub.
