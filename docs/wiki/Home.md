# OBDisplay-Mega Wiki

KWP-1281 K-Line trip computer for Arduino Mega with 480×320 colour TFT display. Reads live sensor data and fault codes from VAG vehicles (Golf Mk4, Bora, Jetta, ~1998–2006) over the K-Line OBD interface.

> **Note:** HardwareSerial timing issues are a known limitation on the Mega. For a production-ready build, see [OBDisplay-Uno](https://github.com/RXTX4816/OBDisplay-Uno).

## Pages

| Page | Description |
|---|---|
| [Hardware Setup](Hardware-Setup) | Components, pinout, TFT shield, K-Line cable modification |
| [Getting Started](Getting-Started) | Build and flash with PlatformIO |
| [Operation](Operation) | Buttons, menu order, and navigation overview |
| [Screen Reference](Screen-Reference) | Every screen and menu explained in detail |
| [KWP-1281 Protocol](KWP-1281-Protocol) | Protocol details, ECU addresses, measurement groups |
| [Troubleshooting](Troubleshooting) | Common issues and fixes |

## Quick links

- [OBDisplay-Uno](https://github.com/RXTX4816/OBDisplay-Uno) — recommended Uno variant with OLED display
- [OBDisplay-Emu](https://github.com/RXTX4816/OBDisplay-Emu) — KWP-1281 ECU emulator for bench testing
- [KLineKWP1281Lib](https://github.com/domnulvlad/KLineKWP1281Lib) — KWP-1281 library dependency
