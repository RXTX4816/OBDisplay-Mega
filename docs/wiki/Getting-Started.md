# Getting Started

## Prerequisites

Install PlatformIO:

```sh
pip install --upgrade platformio
```

or on Arch Linux:

```sh
sudo pacman -S platformio-core
```

Alternatively, install the **PlatformIO IDE** extension for VS Code and use the toolbar buttons.

The project depends on [KLineKWP1281Lib](https://github.com/domnulvlad/KLineKWP1281Lib), which is included as a git submodule in `lib/KLineKWP1281Lib/`. After cloning, run:

```sh
git submodule update --init
```

## Build environments

| Environment | Command | Description |
|---|---|---|
| `mega` | `pio run -e mega` | Production build |
| `mega_debug` | `pio run -e mega_debug` | Debug build — enables `OBD_DEBUG` serial logging |

## Build

```sh
pio run -e mega
```

## Flash

```sh
pio run -e mega --target upload
```

Or use the PlatformIO VS Code extension's **Upload** button.

## First use

On first power-up, the setup menu appears:

1. **Baud rate** — use LEFT/RIGHT to cycle through `1200 / 2400 / 4800 / 9600 / 10400`. Most cars from 1997 onward use **10400**.
2. **ECU address** — cycle through `0x01 / 0x03 / 0x08 / 0x17 / 0x46`. Start with **0x17** (Instruments cluster) for a general overview.
3. **Night mode** — ON (dark background, cyan text) or OFF (light background).
4. Press **MID** to confirm and begin connecting.

Settings are saved to EEPROM and restored on next power-up.

Turn ignition ON (engine does not need to be running) before the firmware attempts to connect.

## ECU emulator

To test without a real vehicle, flash [OBDisplay-Emu](https://github.com/RXTX4816/OBDisplay-Emu) onto a second Arduino Mega. It emulates a KWP-1281 ECU on address `0x17` at 10400 baud and responds with simulated sensor data. See the `OBDisplay-Emu/` directory in this repository.

> **Known limitation:** HardwareSerial timing on the Mega can cause intermittent connection failures with some ECUs. If you experience this, the [OBDisplay-Uno](https://github.com/RXTX4816/OBDisplay-Uno) variant uses Software Serial which avoids this issue entirely.
