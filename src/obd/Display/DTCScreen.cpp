// SPDX-License-Identifier: GPL-3.0-or-later
#include "DTCScreen.h"
#include "../../display/Display.h"

namespace obd
{
namespace DisplayLayer
{

using namespace Model;

static constexpr uint8_t DTC_ACTION_ROWS = 3;
static const uint8_t DTC_ACTION_ROW_START = 15;

void dtcInit(bool destroy)
{
    uint16_t color = destroy ? display.getBackColor() : display.getColor();
    display.setColor(color);
    display.setFont(FONT_SMALL);
    display.print("Reset DTC buffer", DISP_CENTER, rows[DTC_ACTION_ROW_START]);
    display.print("!Read DTC ECU!", DISP_CENTER, rows[DTC_ACTION_ROW_START + 1]);
    display.print("!Clear DTC ECU!", DISP_CENTER, rows[DTC_ACTION_ROW_START + 2]);
    display.setFont(FONT_BIG);
    display.setColor(display.getColor());
}

void dtcRender(const DTCStore& dtc, DtcState state, uint8_t selectedRow, bool force)
{
    display.setFont(FONT_SMALL);

    // Cursor arrow for action menu
    static uint8_t lastRow = 0xFF;
    if (lastRow != selectedRow || force)
    {
        if (lastRow != 0xFF)
            display.print("  ", cols[2], rows[DTC_ACTION_ROW_START + lastRow]);
        display.print("->", cols[2], rows[DTC_ACTION_ROW_START + selectedRow]);
        lastRow = selectedRow;
    }

    // DTC result area (rows 3–14)
    switch (state)
    {
        case DtcState::Reading:
            display.print("Reading DTC from ECU...", DISP_CENTER, rows[10]);
            break;
        case DtcState::NoDtc:
            display.print("No DTC errors found", DISP_CENTER, rows[10]);
            break;
        case DtcState::Done:
            if (force)
            {
                for (uint8_t i = 0; i < DTCStore::MaxCount && i < 12; ++i)
                {
                    if (dtc.errorAt(i) == 0xFFFF)
                        continue;
                    char buf[24];
                    snprintf(buf, sizeof(buf), "%02u  %04X  %02X", i, dtc.errorAt(i),
                             dtc.statusAt(i));
                    display.print(buf, DISP_LEFT, rows[3 + i]);
                }
            }
            break;
        case DtcState::Idle:
        default:
            break;
    }

    display.setFont(FONT_BIG);
}

void dtcRemove()
{
    uint16_t savedFg = display.getColor();
    display.setColor(display.getBackColor());
    dtcInit(true);
    display.setFont(FONT_SMALL);
    for (uint8_t r = 3; r <= DTC_ACTION_ROW_START + DTC_ACTION_ROWS; ++r)
        display.clearRow(r);
    display.setFont(FONT_BIG);
    display.setColor(savedFg);
}

} // namespace DisplayLayer
} // namespace obd
