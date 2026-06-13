// SPDX-License-Identifier: GPL-3.0-or-later
#include "CockpitScreen.h"
#include "../../display/Display.h"
#include "../../Config.h"

namespace obd
{
namespace DisplayLayer
{

using namespace Model;

// ── Screen 0: Big 7-seg style numbers ─────────────────────────────────────
// Layout (480×320, landscape):
//   Row 0:  status bar (drawn by DisplayManager)
//   Row 2:  [km driven — LEFT]              [fuel L — RIGHT]
//   Row 3:  (large SevenSeg number)         (large SevenSeg number)
//   Row 6:  dividing line
//   Row 8:  [km remaining — CENTER]
//   Row 9:  (large SevenSeg number)
//   Row 13: [coolant °C — LEFT, coloured]   [L/100km — RIGHT]
//   Row 14: (BigFont numbers)

static uint8_t s_schedulerIdx = 0; // sub-frame rendering index

void cockpitInit(uint8_t screen, bool destroy)
{
    if (screen == 0)
    {
        // Static labels
        uint16_t color = destroy ? display.getBackColor() : display.getColor();
        display.setColor(color);
        display.setFont(FONT_BIG);
        display.print("km", DISP_LEFT, rows[2]);
        display.print("L", DISP_RIGHT, rows[2]);
        display.print("km left", DISP_CENTER, rows[7]);
        display.print("C", DISP_LEFT, rows[13]);
        display.print("L/100", DISP_RIGHT, rows[13]);
        display.drawHLine(cols[0], rows[6] + 4, DISP_WIDTH);
    }
    else // screen 1: text layout (speed/rpm/temps)
    {
        uint16_t color = destroy ? display.getBackColor() : display.getColor();
        display.setColor(color);
        display.setFont(FONT_BIG);
        display.print("KMH", DISP_LEFT, rows[3]);
        display.print("RPM", DISP_LEFT, rows[5]);
        display.print("COOL", DISP_CENTER, rows[3]);
        display.print("OIL", DISP_CENTER, rows[5]);
        display.print("OILLVL", DISP_CENTER, rows[7]);
        display.print("FUEL", DISP_CENTER, rows[9]);
        display.print("L/100", DISP_CENTER, rows[11]);
    }
    s_schedulerIdx = 0;
    display.setColor(display.getColor()); // restore
}

// Returns true when a full render cycle has completed
bool cockpitRender(uint8_t screen, const OBDSignals& sig, bool force)
{
    const ComputedStats& c = sig.computed;
    const InstrumentSignals& ins = sig.instruments;
    uint16_t savedColor = display.getColor();

    if (screen == 0)
    {
        // Sub-frame scheduler: render one item per call to avoid blocking KWP
        switch (s_schedulerIdx)
        {
            case 0: // km driven (top-left, alt colour)
                if (c.elapsedKmSinceStartUpdated || force)
                {
                    display.setColor(Color::DarkGrey);
                    display.setFont(FONT_SEVENSEG);
                    uint16_t km = c.elapsedKmSinceStart > 999 ? 999 : c.elapsedKmSinceStart;
                    display.printNumI(km, DISP_LEFT, rows[3], 3, '0');
                    display.setColor(savedColor);
                }
                s_schedulerIdx++;
                break;

            case 1: // fuel level (top-right, alt colour)
                if (ins.fuelLevelUpdated || force)
                {
                    display.setColor(Color::DarkGrey);
                    display.setFont(FONT_SEVENSEG);
                    display.printNumI(ins.fuelLevel, DISP_RIGHT, rows[3], 2, '0');
                    display.setColor(savedColor);
                }
                s_schedulerIdx++;
                break;

            case 2: // km remaining (center)
                if (c.kmRemainingUpdated || force)
                {
                    display.setColor(Color::DarkGrey);
                    display.setFont(FONT_SEVENSEG);
                    uint16_t kmR = c.kmRemaining > 999 ? 999 : c.kmRemaining;
                    display.printNumI(kmR, DISP_CENTER, rows[8], 3, '0');
                    display.setColor(savedColor);
                }
                s_schedulerIdx++;
                break;

            case 3: // coolant (bottom-left, colour-coded) + L/100km (bottom-right)
                display.setFont(FONT_BIG);
                if (ins.coolantTempUpdated || force)
                {
                    uint8_t ct = ins.coolantTemp;
                    if (ct < 40)
                        display.setColor(Color::Blue);
                    else if (ct < WARN_COOLANT_HIGH_C)
                        display.setColor(savedColor);
                    else
                        display.setColor(Color::Red);

                    if (ct > 99)
                        display.printNumI(ct, DISP_LEFT, rows[14], 3, '0');
                    else
                        display.printNumI(ct, DISP_LEFT, rows[14], 2, '0');
                    display.setColor(savedColor);
                }
                if (c.fuelPer100kmUpdated || force)
                {
                    display.setColor(Color::DarkGrey);
                    uint16_t f100 = (uint16_t)c.fuelPer100km;
                    if (f100 > 99)
                        f100 = 99;
                    display.printNumI(f100, DISP_RIGHT, rows[14], 2, '0');
                    display.setColor(savedColor);
                }
                s_schedulerIdx = 0;
                return true;

            default:
                s_schedulerIdx = 0;
                return true;
        }
        return false;
    }
    else // screen 1: text layout
    {
        display.setFont(FONT_BIG);
        if (ins.vehicleSpeedUpdated || force)
            display.printNumI(ins.vehicleSpeed, cols[7], rows[3], 3, '0');
        if (ins.engineRpmUpdated || force)
            display.printNumI(ins.engineRpm, cols[7], rows[5], 4, '0');
        if (ins.coolantTempUpdated || force)
            display.printNumI(ins.coolantTemp, cols[17] + cols[4], rows[3], 3, '0');
        if (ins.oilTempUpdated || force)
            display.printNumI(ins.oilTemp, cols[17] + cols[4], rows[5], 3, '0');
        if (ins.fuelLevelUpdated || force)
            display.printNumI(ins.fuelLevel, cols[17] + cols[4], rows[9], 2, '0');
        if (ins.oilLevelOkUpdated || force)
            display.printNumI(ins.oilLevelOk, cols[17] + cols[4], rows[7], 1, '-');
        if (c.fuelPer100kmUpdated || force)
        {
            uint16_t f100 = (uint16_t)c.fuelPer100km;
            display.printNumI(f100 > 99 ? 99 : f100, cols[17] + cols[4], rows[11], 2, '0');
        }
        return true;
    }
}

void cockpitRemove(uint8_t screen)
{
    uint16_t savedFg = display.getColor();
    uint16_t savedBg = display.getBackColor();
    display.setColor(savedBg);
    cockpitInit(screen, true);
    // Clear value areas
    display.setFont(FONT_SEVENSEG);
    if (screen == 0)
    {
        display.print("   ", DISP_LEFT, rows[3]);
        display.print("  ", DISP_RIGHT, rows[3]);
        display.print("   ", DISP_CENTER, rows[8]);
        display.setFont(FONT_BIG);
        display.print("   ", DISP_LEFT, rows[14]);
        display.print("  ", DISP_RIGHT, rows[14]);
    }
    else
    {
        display.setFont(FONT_BIG);
        for (uint8_t r = 3; r <= 12; r += 2)
            display.clearRow(r);
    }
    display.setColor(savedFg);
    s_schedulerIdx = 0;
}

} // namespace DisplayLayer
} // namespace obd
