# Operation

> For full screen layouts, field tables, and DTC decoding see [Screen Reference](Screen-Reference).

## Buttons

| Button | Action |
|---|---|
| RST | Jump to Cockpit (menu 0) from anywhere |
| SET | Advance to the next menu (wraps around) |
| UP / DOWN | Next / previous screen within the current menu |
| LEFT / RIGHT | Also next/previous screen in Cockpit and Experimental; cursor navigation in DTC and Settings |
| MID | Context action — varies per screen |

Button events have a 200 ms debounce.

## Menu order

SET cycles through menus in this order:

```
Cockpit → Experimental → Graphics → DTC → Settings → Debug → (back to Cockpit)
```

RST always returns directly to Cockpit.

## Menus at a glance

### Cockpit
Live sensor data from the connected ECU. Two screens:
- **Screen 0**: Large 7-segment-style display — km driven, fuel remaining, km remaining, coolant °C (colour-coded), L/100km.
- **Screen 1**: Text layout — speed, RPM, coolant, oil temp, oil level status, fuel level, L/100km.

UP/DOWN (or LEFT/RIGHT) switches between the two screens.

### Experimental
Raw data split across two pages:
- **Page 0**: Fuel and timing data — odometer, fuel resistance, oil pressure flag, elapsed time, distance, fuel consumption figures, km remaining.
- **Page 1**: Engine parameters — OBD readiness bits, throttle angle, steering angle, voltage, engine load, ambient temp, manifold pressure, lambda 1 & 2.

UP/DOWN or LEFT/RIGHT switches pages.

### Graphics (Group Reading)
Browse raw KWP measurement groups. UP increments the group number (1–255), DOWN decrements it. The ECU returns four values per group; each is shown with its decoded value and unit string. Use this to explore any group your ECU supports.

### DTC (Fault Codes)
Read and clear Diagnostic Trouble Codes. UP/DOWN moves the action cursor; MID executes the selected action. After a read, navigate to **Show** to page through stored codes.

### Settings
Configure KWP mode, night mode, clear the debug log, or disconnect and return to the setup menu. UP/DOWN moves the cursor; MID acts on the selected item.

### Debug
Paginated log of 8-bit debug event codes emitted at runtime. UP shows the previous page, DOWN shows the next page. Each code is colour-coded by severity.

## Status bar

A persistent status bar is visible at the top of every screen while connected, showing:

- `AVA:` — bytes available in the serial receive buffer
- `BC: 0x` — current KWP block counter (increments each KWP exchange)

These update continuously and are useful to confirm the connection is alive.

## Warning overlay

When a new warning condition is detected (oil pressure lost, overheating, low fuel, low voltage, high engine load) a colour-coded banner appears at the bottom of the display (row 17) for approximately 3 seconds before returning to normal. Warnings are shown even while navigating other screens.
