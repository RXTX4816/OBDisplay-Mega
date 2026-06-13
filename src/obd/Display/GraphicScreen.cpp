// SPDX-License-Identifier: GPL-3.0-or-later
#include "GraphicScreen.h"
#include "../../display/Display.h"

namespace obd
{
namespace DisplayLayer
{

using namespace Model;

void graphicInit(bool destroy)
{
    uint16_t color = destroy ? display.getBackColor() : display.getColor();
    display.setColor(color);
    display.setFont(FONT_BIG);
    display.print("-- Group Reading --", DISP_CENTER, rows[2]);
    display.print("Group:", DISP_LEFT, rows[4]);
    display.print("[1]", DISP_LEFT, rows[5]);
    display.print("[2]", DISP_LEFT, rows[8]);
    display.print("[3]", DISP_LEFT, rows[11]);
    display.print("[4]", DISP_LEFT, rows[14]);
    display.setColor(display.getColor());
}

void graphicRender(const OBDSignals& sig, bool force)
{
    const ExperimentalGroup& eg = sig.experimental;
    display.setFont(FONT_BIG);

    if (eg.groupSideUpdated || force)
    {
        display.printNumI(eg.groupCurrent, cols[7], rows[4], 3, ' ');
    }

    const uint8_t baseRows[4] = {5, 8, 11, 14};
    for (uint8_t i = 0; i < 4; ++i)
    {
        if (eg.vUpdated || force)
        {
            display.printNumF(eg.v[i], 1, cols[4], baseRows[i], '.', 8, ' ');
        }
        if (eg.unitUpdated || force)
        {
            display.setFont(FONT_SMALL);
            display.print(eg.unit[i], cols[18], rows[baseRows[i]]);
            display.setFont(FONT_BIG);
        }
    }
}

void graphicRemove()
{
    uint16_t savedFg = display.getColor();
    display.setColor(display.getBackColor());
    graphicInit(true);
    for (uint8_t r = 2; r <= 17; ++r)
        display.clearRow(r);
    display.setColor(savedFg);
}

} // namespace DisplayLayer
} // namespace obd
