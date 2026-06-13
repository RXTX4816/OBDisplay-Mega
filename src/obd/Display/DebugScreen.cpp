// SPDX-License-Identifier: GPL-3.0-or-later
#include "DebugScreen.h"
#include "../../display/Display.h"
#include <string.h>

namespace obd
{
namespace DisplayLayer
{

static uint8_t s_codes[DEBUG_MAX_ENTRIES];
static uint8_t s_count = 0;

static constexpr uint8_t ROWS_PER_PAGE = 16; // rows 3–18
static constexpr uint8_t ROW_START = 3;

void debugClear()
{
    s_count = 0;
    memset(s_codes, 0, sizeof(s_codes));
}

void debugAdd(uint8_t code)
{
    if (s_count < DEBUG_MAX_ENTRIES)
        s_codes[s_count++] = code;
}

uint8_t debugPageCount()
{
    if (s_count == 0)
        return 1;
    return (s_count + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
}

static uint16_t codeColor(uint8_t code)
{
    uint8_t nibble = code >> 4;
    switch (nibble)
    {
        case 0x1:
            return Color::Green;
        case 0x2:
            return Color::Yellow;
        case 0x3:
        case 0x4:
        case 0x5:
            return Color::Red;
        default:
            return Color::Cyan;
    }
}

void debugInit(uint8_t page, bool destroy)
{
    uint16_t color = destroy ? display.getBackColor() : display.getColor();
    display.setColor(color);
    display.setFont(FONT_SMALL);

    // Page indicator
    char pageBuf[16];
    snprintf(pageBuf, sizeof(pageBuf), "Page %u/%u", (unsigned)(page + 1),
             (unsigned)debugPageCount());
    display.print(pageBuf, DISP_CENTER, rows[19]);

    display.setFont(FONT_BIG);
    display.setColor(display.getColor());
}

bool debugRender(uint8_t page, bool forceAll)
{
    display.setFont(FONT_SMALL);
    uint8_t base = page * ROWS_PER_PAGE;

    for (uint8_t i = 0; i < ROWS_PER_PAGE; ++i)
    {
        uint8_t idx = base + i;
        if (idx >= s_count)
            break;
        display.setColor(codeColor(s_codes[idx]));
        char buf[6];
        snprintf(buf, sizeof(buf), "%02X", s_codes[idx]);
        // Index on left, code on right of same row
        display.printNumI(idx, DISP_LEFT, rows[ROW_START + i], 2, '0');
        display.print(buf, cols[3], rows[ROW_START + i]);
    }

    display.setColor(display.getColor());
    display.setFont(FONT_BIG);
    (void)forceAll;
    return true;
}

void debugRemove(uint8_t page)
{
    uint16_t savedFg = display.getColor();
    display.setColor(display.getBackColor());
    debugInit(page, true);
    display.setFont(FONT_SMALL);
    for (uint8_t r = ROW_START; r < ROW_START + ROWS_PER_PAGE + 1; ++r)
        display.clearRow(r);
    display.setFont(FONT_BIG);
    display.setColor(savedFg);
}

} // namespace DisplayLayer
} // namespace obd
