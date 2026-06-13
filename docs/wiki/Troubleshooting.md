# Troubleshooting

## Blank or corrupted display

**Check shield seating** — the TFT shield must be fully pressed onto all Mega pin headers. A partially-seated shield causes random pixel noise or a white screen.

**Wrong TFT driver** — MCUFRIEND_kbv auto-detects the driver IC at runtime and prints the detected ID over Serial (baud 9600) during startup. If the detected ID is `0x0000` or `0xFFFF`, the shield is not making contact. If the ID is detected but the display is garbled, update the `MCUFRIEND_kbv` library to the latest version.

**Power** — a 480×320 TFT at full brightness can draw 250 mA. Some USB ports or power banks with low current limits cause the display to flicker or reset. Use a dedicated 5V/1A supply if possible.

---

## ECU not connecting

**Ignition ON** — the ECU must be powered. Turn the ignition key to position II (dashboard on, engine not required).

**Baud rate** — try 10400 first (most common for 1998–2006 VAG). If the ECU does not respond within the timeout, the startup setup menu reappears; try 9600 next.

**K-Line wiring** — verify polarity:
- FT232 **TXD** → Mega **pin 15** (RX3)
- FT232 **RXD** → Mega **pin 14** (TX3)
- FT232 powered from Mega **5V** and **GND**

Swapping TXD/RXD is the most common wiring mistake. Use a multimeter to verify continuity from the FT232 pad to the Mega pin.

**ECU address** — confirm the correct address for your module. Start with `0x17` (Instruments cluster); it is present in virtually all VAG vehicles of this era.

**HardwareSerial timing** — this is a known issue specific to the Mega variant. The Arduino Mega's HardwareSerial implementation can miss the tight timing windows required by KWP-1281's half-duplex echo requirement. Symptoms: connection initiates but fails mid-handshake, or connects then immediately drops. Workarounds:
- Rebuild with `mega_debug` and monitor the Serial output for the point of failure.
- Adjust `ECU_TIMEOUT_MS` in `src/Config.h` (default 1300 ms) upward slightly.
- For a permanently reliable fix, use [OBDisplay-Uno](https://github.com/RXTX4816/OBDisplay-Uno), which uses Software Serial and avoids this issue.

---

## Connection drops during use

**Status bar check** — watch `AVA:` and `BC:` in the status bar. If `BC:` stops incrementing the connection has stalled; if `AVA:` grows unbounded, bytes are accumulating in the buffer faster than they are being consumed (timing problem).

**ECU keepalive timeout** — if the firmware spends too long rendering a screen frame, the KWP keepalive may miss its window. The display task runs every 40 ms; if rendering a full frame takes longer, the ECU will time out. Reduce rendering work or increase `ECU_TIMEOUT_MS` slightly.

---

## Stale or wrong sensor values

**Group mapping** — each ECU firmware version maps different signals to different group/slot positions. The values shown in the Cockpit and Experimental screens assume specific groups (documented in [KWP-1281 Protocol](KWP-1281-Protocol)). If your ECU variant uses different group assignments, the displayed values will be wrong or zero.

To verify: use the **Graphics** screen to browse groups manually and compare the raw decoded values against what VCDS shows for the same ECU.

**ECU address mismatch** — for example, if you select `0x01` but your car's instruments cluster is at a non-standard address, you will see connection errors or unexpected data.

---

## Buttons not responding

**Wiring** — all buttons connect between their pin and GND using `INPUT_PULLUP`. Connecting to 5V instead of GND will never register a press.

**Debounce** — there is a 200 ms debounce between events. Hold the button briefly; tapping faster than 200 ms is filtered out.

**RST button** — RST (pin 13) is the built-in LED pin on most Mega boards. If you are using the onboard LED as RST, it will work as a button but will not light up reliably.

---

## Build / flash issues

**Submodule missing** — if the build fails with `KLineKWP1281Lib.h: No such file`, run:

```sh
git submodule update --init
```

**Flash too large** — the `mega_debug` build is larger than `mega` due to debug output strings. If flash is full, use `pio run -e mega` (production build).

**Port not found** — PlatformIO may not detect the COM port automatically. Add `upload_port = /dev/ttyUSBx` (Linux) or `COMx` (Windows) to the `[env:mega]` section of `platformio.ini`.

---

## Using the ECU emulator for testing

Flash [OBDisplay-Emu](https://github.com/RXTX4816/OBDisplay-Emu) onto a second Arduino Mega and wire the two Megas K-Line pins together. The emulator responds on address `0x17` at 10400 baud with simulated sensor data, allowing you to test the display and menus without a car. See `OBDisplay-Emu/` in this repository and its `README` for wiring details and the `FULL_DUPLEX` / `HALF_DUPLEX` compatibility setting.
