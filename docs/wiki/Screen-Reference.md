# Screen Reference

## Navigation overview

The display is a 480×320 px landscape TFT panel. When connected, content is organised into **menus** (cycled with SET / RST) and **screens** (navigated within a menu with UP/DOWN). A status bar runs across the top of every screen.

```
RST                 jump to Cockpit
SET                 advance to next menu
↑ UP / DOWN ↓       switch screen within current menu
← LEFT / RIGHT →    also switches screen in Cockpit and Experimental
MID                 context action (varies per screen)
```

### Menu order

```
Cockpit → Experimental → Graphics → DTC → Settings → Debug
```

---

## Status bar

Visible at the top of every screen while connected:

```
AVA: 0    BC: 0x2A
```

| Field | Meaning |
|---|---|
| `AVA:` | Bytes currently available in the Serial3 receive buffer |
| `BC: 0x` | KWP block counter (hex) — increments with every KWP exchange |

If `AVA` is permanently non-zero or `BC` stops incrementing, the connection has stalled.

---

## Warning overlay

When a new warning condition fires, a colour-coded banner replaces the bottom row of the display for ~3 seconds:

```
!! OIL PRESSURE LOW !!        ← example, rendered in red
```

Warning conditions include:
- Oil pressure drop
- Oil temperature ≥ 93 °C
- Coolant temperature ≥ 93 °C
- Coolant below 40 °C (cold engine caution)
- Fuel critical (≤ 4 L)
- Fuel low (≤ 8 L)
- Battery voltage < 12.0 V
- Engine load ≥ 90 %

The overlay appears regardless of which screen is currently active and returns to normal after ~3 seconds.

---

## Cockpit

Live sensor data. The two screens share the same data source; choose the layout that suits you. Data updates every ~40 ms.

### Screen 0 — Big display

Large 7-segment-style (`SevenSeg` and `BigFont`) numbers arranged in six regions:

```
┌─────────────────────────────────────┐
│ km                               L  │  ← labels
│   [km driven]         [fuel L]      │  ← large numbers
│                                     │
│              km left                │  ← label
│           [km remaining]            │  ← large number
│                                     │
│ [coolant °C]              [L/100]   │  ← big font numbers
└─────────────────────────────────────┘
```

| Region | Field | Notes |
|---|---|---|
| Top-left | km driven | Odometer value from ECU |
| Top-right | Fuel level | Litres remaining |
| Centre | km remaining | Estimated range; `---` until data available |
| Bottom-left | Coolant °C | Blue if < 40 °C; red if ≥ WARN threshold; normal otherwise |
| Bottom-right | L/100 km | Instantaneous fuel consumption; capped at 99 L/100 |

### Screen 1 — Text layout

Smaller text, more fields visible at once:

```
KMH   [speed]
RPM   [rpm]
      COOL  [coolant °C]
      OIL   [oil temp °C]
      OILLVL [ok/warn]
      FUEL  [fuel L]
      L/100 [consumption]
```

| Field | Notes |
|---|---|
| KMH | Vehicle speed, km/h |
| RPM | Engine RPM |
| COOL | Coolant temperature °C |
| OIL | Oil temperature °C |
| OILLVL | Oil level OK / warning |
| FUEL | Fuel level, litres |
| L/100 | L/100 km; capped at 99 |

---

## Experimental

Raw ECU data split across two pages. Switch pages with UP/DOWN or LEFT/RIGHT.

### Page 0 — Fuel & timing

```
[odometer km, 6 digits]
[fuel resistance Ω, 4 digits]
[oil pressure flag, 2 digits]
[ECU internal time ms, 7 digits]
[elapsed seconds, 6 digits]
[distance this session km, 4 digits]
[fuel used L, 2 digits]
[fuel L/hr, 1 decimal]
[fuel L/100km, 1 decimal]
[km remaining, 4 digits]
```

| Field | Source | Notes |
|---|---|---|
| Odometer | Group data | Running km total from ECU |
| Fuel resistance | Group data | Raw fuel sender resistance in Ω |
| Oil pressure | Group data | 0 = OK, 1 = low pressure flag |
| ECU time | Group data | ECU internal millisecond counter |
| Elapsed s | Firmware | Seconds since session start |
| Distance | Firmware | km driven this session |
| Fuel used | Firmware | Litres consumed this session |
| L/hr | Computed | Instantaneous litres per hour |
| L/100 | Computed | Instantaneous litres per 100 km |
| km remaining | Computed | Fuel L × inverse of L/100 |

### Page 1 — Engine parameters

```
[OBD readiness bits × 8, alternating yellow/cyan]
[throttle angle °, 1 decimal]
[steering angle °, 1 decimal]
[battery voltage V, 1 decimal]
[engine load %, 4 digits]
[ambient temperature °C, 2 digits]
[manifold pressure mbar, 5 digits]
[lambda 1 %, 3 digits]
[lambda 2 %, 3 digits]
```

| Field | Notes |
|---|---|
| Readiness bits | 8 individual OBD monitor flags; yellow = not ready / cyan = ready |
| Throttle angle | Degrees from fully-closed |
| Steering angle | Degrees (if supported by ECU) |
| Voltage | Battery voltage in volts |
| Load | Engine load 0–100 % |
| Ambient | Intake air temperature °C |
| Pressure | Manifold absolute pressure, mbar |
| Lambda 1/2 | Lambda controller percentage ±; 0 % = stoichiometric |

Readiness bit order (bit 7 → 0):

| Bit | Monitor |
|---|---|
| 7 | EGR system |
| 6 | O2 sensor heater |
| 5 | O2 sensor |
| 4 | A/C refrigerant |
| 3 | Secondary air injection |
| 2 | Evaporative emissions |
| 1 | Catalyst heater |
| 0 | Catalytic converter |

---

## Graphics (Group Reading)

Browse raw KWP measurement groups interactively.

```
-- Group Reading --

Group:  [NNN]

[value 0, 1 decimal]    [unit 0]
[value 1, 1 decimal]    [unit 1]
[value 2, 1 decimal]    [unit 2]
[value 3, 1 decimal]    [unit 3]
```

| Button | Action |
|---|---|
| UP | Increment group number |
| DOWN | Decrement group number |

Group numbers range from 1 to 255. The ECU returns four value/unit pairs per group. Not all groups are populated — unpopulated slots return 0. Use VCDS or label files to identify which groups contain the data you want for your specific ECU.

---

## DTC (Fault Codes)

Read and clear Diagnostic Trouble Codes stored in the ECU.

### Main view

```
[DTC results area — rows 3–14]

  Reset DTC buffer      ← row 15
  !Read DTC ECU!        ← row 16
  !Clear DTC ECU!       ← row 17
```

The `->` cursor (rendered in the left margin) shows the currently selected action.

| Button | Action |
|---|---|
| UP | Move cursor up |
| DOWN | Move cursor down |
| LEFT | Move cursor up |
| RIGHT | Move cursor down |
| MID | Execute selected action |

### Actions

| Action | What it does |
|---|---|
| Reset DTC buffer | Clears the local DTC storage without talking to the ECU. Use this to clear a previous read before starting a new one. |
| !Read DTC ECU! | Sends a KWP Read Faults request (block title 0x07). Stores up to 12 DTCs locally. |
| !Clear DTC ECU! | Sends a KWP Delete Faults request (block title 0x05) to erase stored faults from the ECU. |

### DTC results area

While reading: `Reading DTC from ECU...`

If no faults: `No DTC errors found`

After a successful read, up to 12 entries are listed:

```
[idx]  [code hex]  [status]
```

- **idx** — entry index (0-based)
- **code hex** — raw fault code in hexadecimal (VAG-specific 5-digit code when converted to decimal)
- **status** — status byte in hexadecimal

### Decoding DTC codes

VAG KWP-1281 fault codes are not standard OBD-II P/U/B/C codes. To look up a code:

- **VCDS label files** — [Ross-Tech label file archive](https://www.ross-tech.com/vcds/label-files/) maps codes to plain-text descriptions per ECU part number.
- **Ross-Tech Wiki** — [wiki.ross-tech.com](https://wiki.ross-tech.com) has fault code lists by code number.
- Searching `VAG [decimal code]` (e.g. `VAG 16825`) usually returns a description.

> **Caution:** Avoid accessing address `0x15` (Airbag). On some affected ECUs, clearing airbag DTCs when an electrical fault is present can trigger airbag deployment.

---

## Settings

Configuration menu. UP/DOWN moves the cursor; MID acts on the selected item.

```
Settings

KWP Mode:   [ACK / GRP / READGROUP]
Night mode: [ON  / OFF]
-> Clear DEBUG <-
-> Disconnect <-
```

| Item | MID action |
|---|---|
| KWP Mode | Cycles: `ACK` → `GRP` → `READGROUP` → `ACK` … |
| Night mode | Toggles between ON (dark background, cyan text) and OFF (light background, dark text). Saved to EEPROM. |
| Clear DEBUG | Wipes the debug event log in RAM. Takes effect immediately. |
| Disconnect | Sends the KWP exit sequence cleanly and returns to the startup setup menu. |

### KWP modes

| Display | Mode | Behaviour |
|---|---|---|
| `ACK` | Acknowledge only | Sends keepalive blocks only — no sensor data is read. Useful to hold a session open without loading the ECU. |
| `GRP` | Group read (sequential) | Reads measurement groups in order, incrementing the group number each cycle. |
| `READGROUP` | Group read (fixed) | Reads a specific measurement group — the one currently set in the Graphics screen. |

---

## Debug

Paginated log of 8-bit debug event codes recorded at runtime. Only meaningful when the firmware is built with the `mega_debug` environment (`-D OBD_DEBUG`).

```
00  [code hex]      ← entry 0
01  [code hex]      ← entry 1
...
15  [code hex]      ← entry 15 (last on page)

Page 1/8
```

| Button | Action |
|---|---|
| UP | Previous page |
| DOWN | Next page |

Up to 128 entries are stored (16 per page, 8 pages maximum). Entries are colour-coded by the high nibble of the code:

| High nibble | Colour | Meaning |
|---|---|---|
| `0x1_` | Green | Informational / success |
| `0x2_` | Yellow | Warning / non-critical |
| `0x3_` – `0x5_` | Red | Error / critical |
| Other | Cyan | Miscellaneous / protocol |

Wrap-around: once 128 entries are stored, new entries overwrite the oldest. Use **Settings → Clear DEBUG** to reset the log.
