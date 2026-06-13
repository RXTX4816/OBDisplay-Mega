# KWP-1281 Protocol

## Overview

KWP-1281 (Keyword Protocol 1281) is a single-wire half-duplex serial diagnostic protocol used by VAG (Volkswagen Audi Group) vehicles from the mid-1990s through the mid-2000s. It operates over the K-Line of the OBD-II connector (pin 7) and uses a proprietary block-based message format.

This firmware uses [KLineKWP1281Lib](https://github.com/domnulvlad/KLineKWP1281Lib) (included as a submodule in `lib/`) to handle the low-level protocol.

---

## Connection sequence

### 1 — 5-baud initialisation

The controller transmits the ECU address at 5 baud (200 ms per bit) over the K-Line TX pin (pin 14). This wakes the ECU and selects the protocol.

### 2 — Sync byte and keyword

The ECU responds with:
- `0x55` — sync byte (used to measure the actual baud rate)
- `0x01 0x8A` — keyword bytes identifying KWP-1281

The controller complements and echoes the second keyword byte.

### 3 — Connect blocks

The ECU sends one or more identification blocks (part number, ECU description, coding). The controller ACKs each one.

### 4 — Session open

After the last connect block, normal data exchange begins. The session must be kept alive by sending ACK blocks at regular intervals; a timeout of ~1.3 s causes the ECU to drop the session.

---

## Block format

Every KWP-1281 message (in both directions) follows this structure:

```
[length]  [counter]  [title]  [data ...]  [end: 0x03]
```

| Byte | Field | Description |
|---|---|---|
| 0 | Length | Number of bytes in this block, including title but not length or end byte |
| 1 | Counter | Block counter, increments each exchange (wraps at 0xFF) |
| 2 | Title | Block type identifier |
| 3…N-1 | Data | Payload bytes (0 or more) |
| N | End | Always `0x03` |

In half-duplex mode the controller echoes each received byte (complemented) before the ECU sends the next.

---

## Supported ECU addresses

| Address | Module | Notes |
|---|---|---|
| `0x01` | Engine ECU | Marelli 1.6 16V, Bosch Motronic; most common for engine data |
| `0x03` | ABS / ESP | Teves Mk60; brake system fault codes |
| `0x08` | Auto HVAC | Climate control module |
| `0x17` | Instruments | Dashboard cluster; default address; fuel level, speed, odometer |
| `0x46` | Central Convenience | Central locking, window control |

> **Warning:** Address `0x15` (Airbag) is **not listed** intentionally. On some ECUs with pre-existing electrical faults, sending a Delete Faults command to the airbag module can trigger deployment. Do not access this address with self-made diagnostic tools.

Older vehicles (pre-1997) may respond on different addresses or require lower baud rates (1200 or 2400 baud).

---

## Supported baud rates

`1200 / 2400 / 4800 / 9600 / 10400`

10400 baud is standard for most VAG vehicles from 1997 onward. Select the rate at startup; it is saved to EEPROM.

---

## Block title reference

Common titles used by this firmware:

| Title (hex) | Direction | Description |
|---|---|---|
| `0x04` | Controller → ECU | ACK / keepalive |
| `0x07` | Controller → ECU | Read fault codes (DTC read) |
| `0x05` | Controller → ECU | Delete fault codes (DTC clear) |
| `0x29` | Controller → ECU | Read measurement group (Group Reading mode) |
| `0x2C` | Controller → ECU | Read measured values (Sensor mode) |
| `0x06` | ECU → Controller | ACK response |
| `0x0F` | ECU → Controller | Fault code data block |
| `0xE7` | ECU → Controller | Measured value block (4 values per group) |
| `0x09` | Either | End of communication |

---

## Measurement groups

Each ECU address exposes a set of numbered measurement groups (1–255). Each group contains 4 value slots. The meaning of each slot depends on the ECU firmware version.

### Instruments cluster (0x17) — common groups

| Group | Slot 0 | Slot 1 | Slot 2 | Slot 3 |
|---|---|---|---|---|
| 1 | Vehicle speed (km/h) | Engine RPM | Oil temp (°C) | Coolant temp (°C) |
| 2 | Odometer (km) | Fuel level (sender Ω) | Oil pressure flag | ECU time (ms) |
| 3 | Ambient temp (°C) | Oil level (raw) | — | — |

### Engine ECU (0x01) — common groups

| Group | Slot 0 | Slot 1 | Slot 2 | Slot 3 |
|---|---|---|---|---|
| 1 | RPM | Coolant (°C) | Lambda ctrl (%) | Basic setting bits |
| 3 | Throttle angle (°) | Steering angle (°) | Manifold pressure (mbar) | — |
| 4 | Voltage (V) | Coolant (°C) | Intake air (°C) | — |
| 5 | Vehicle speed (km/h) | Engine load (%) | — | — |
| 6 | Heights correction lambda (%) | — | — | — |

Group contents vary between ECU part numbers and software versions. Use VCDS or the **Graphics** screen to explore groups manually for your specific ECU. Label files (e.g. `036-906-034-AM.LBL`) document the exact mapping for a given ECU variant.

---

## Label files

Label files map raw group/channel numbers to human-readable descriptions for a specific ECU part number. Sources:

- [Ross-Tech VCDS label file archive](https://www.ross-tech.com/vcds/label-files/)
- [mkirbst's label file collection](https://github.com/mkirbst/lupo-gti-tripcomputer-kw1281)

The label file for the instruments cluster on most Golf Mk4 / Bora / Jetta is `1J0-920-xx0.LBL`.

---

## KWP modes

The firmware supports three operating modes, switchable via the Settings menu:

| Mode | Behaviour |
|---|---|
| ACK | Sends keepalive ACK blocks only. No measurement data is requested. Useful to keep the session open passively. |
| GRP (sequential) | Cycles through measurement groups 1, 2, 3, … in order, requesting one group per KWP cycle. Normal operating mode. |
| READGROUP (fixed) | Repeatedly reads the single group number selected in the **Graphics** screen. Use with Graphics to inspect one group at high update rate. |

---

## Half-duplex wiring note

`IS_FULL_DUPLEX` is set to `false` in `src/Config.h`. In half-duplex mode the TX line echoes each received byte, which is the standard K-Line configuration. If you are connecting to the emulator (`OBDisplay-Emu`) running with `FULL_DUPLEX true`, change this constant accordingly.
