# Hardware Setup

## Components

| Component | Details |
|---|---|
| Microcontroller | Arduino Mega (ATmega2560-16AU, 16 MHz) |
| Display | 3.5" 480×320 TFT LCD shield, ILI9486 or ILI9488, 16-bit parallel (MCUFRIEND_kbv) |
| Buttons | 7-way joystick: UP / DOWN / LEFT / RIGHT / MID / SET / RST |
| K-Line interface | Modified KKL OBD-to-USB cable with FT232R or FT232RQ MCU |

## Pinout

```
Arduino Mega        Function
──────────────────────────────────────────────
GND                 Ground (K-Line and common)
5V                  Power (FT232 board)
Pin 14 (TX3)        K-Line TX → FT232 RXD  (half-duplex TX control)
Pin 15 (RX3)        K-Line RX ← FT232 TXD
Pin 2               SET button   (active LOW, INPUT_PULLUP)
Pin 3               MID button   (active LOW, INPUT_PULLUP)
Pin 4               RIGHT button (active LOW, INPUT_PULLUP)
Pin 5               LEFT button  (active LOW, INPUT_PULLUP)
Pin 6               DOWN button  (active LOW, INPUT_PULLUP)
Pin 7               UP button    (active LOW, INPUT_PULLUP)
Pin 13              RST button   (active LOW, INPUT_PULLUP)
TFT shield          Plugs directly onto Mega pin headers (no individual wiring)
```

All button pins use `INPUT_PULLUP`. Connect each button between the listed pin and GND. The joystick module's common pin goes to GND.

## TFT Shield

The 480×320 display is a plug-in shield that sits directly on the Mega's pin headers — no jumper wires needed for the display itself.

- Driver IC: ILI9486 or ILI9488 (detected automatically by MCUFRIEND_kbv)
- Interface: 16-bit parallel
- Operating voltage: **5V** — use a shield that includes a voltage level shifter or 5V-tolerant interface. The ILI9486/9488 panels themselves run at 3.3V; a proper Mega shield handles this internally.
- Current draw: ~200–300 mA at full brightness. Ensure your USB host or power supply can deliver this.

## K-Line cable modification

The K-Line interface uses a modified **KKL OBD-to-USB cable** (Autodia K409 or compatible). The FT232R/FT232RQ USB-to-serial chip is repurposed as a level shifter between the ECU's K-Line and the Arduino's 5V UART.

**Steps:**

1. Open the KKL cable connector and locate the FT232R(Q) chip.
2. Find the **TXD** and **RXD** pads using the FT232 datasheet (the first exposed trace point after the chip).
3. Solder connections to the Mega Serial3 pins:
   - FT232 **TXD** → Arduino **pin 15** (RX3)
   - FT232 **RXD** → Arduino **pin 14** (TX3)
4. **Cut both PCB traces** immediately after the solder points to isolate the FT232 from the USB data lines.
5. Power the FT232 board from Arduino **5V** and **GND**.
6. The original K-Line wire and GND from the OBD-9 connector remain connected to the FT232 board.

You should have 4 wires between the FT232 board and the Mega (TXD, RXD, 5V, GND) plus the OBD connector wired to the FT232 board's K-Line input.

See `assets/` in the repository for photos of the cable modification.

```
Not all pins on the Mega support change interrupts. Valid RX pins for SoftwareSerial
(if needed) are: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8–A15.
This firmware uses Hardware Serial3 (pins 14/15) directly.
```

## Button wiring

All seven buttons wire between their Arduino pin and GND. Internal pull-ups are enabled, so unpressed = HIGH, pressed = LOW.

- Debounce: 200 ms between button events
- RST returns immediately to the Cockpit menu from anywhere

## Power

| Rail | Draw (approx.) |
|---|---|
| TFT shield | ~200–300 mA |
| Arduino Mega | ~50 mA |
| FT232 board | ~50 mA |
| **Total** | **~300–400 mA** |

When testing in-car, use a separate USB power source or 12V buck converter for the Arduino to avoid ground loop interference through the OBD port.
