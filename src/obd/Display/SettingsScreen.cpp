// SPDX-License-Identifier: GPL-3.0-or-later
#include "SettingsScreen.h"
#include "../../display/Display.h"

namespace obd
{
namespace DisplayLayer
{

static const uint8_t SETTINGS_ROW_START = 5;
static constexpr uint8_t SETTINGS_COUNT = 4;

static const char* kwpModeStr(uint8_t mode)
{
    switch (mode)
    {
        case KWP_MODE_ACK:
            return "ACK";
        case KWP_MODE_READGROUP:
            return "GRP";
        case KWP_MODE_READSENSORS:
            return "SEN";
        default:
            return "N/A";
    }
}

void settingsInit(bool destroy)
{
    uint16_t color = destroy ? display.getBackColor() : display.getColor();
    display.setColor(color);
    display.setFont(FONT_SMALL);
    display.print("Settings", DISP_LEFT, rows[SETTINGS_ROW_START - 1]);
    display.print("KWP Mode:", DISP_LEFT, rows[SETTINGS_ROW_START]);
    display.print("Night mode:", DISP_LEFT, rows[SETTINGS_ROW_START + 1]);
    display.print("-> Clear DEBUG <-", DISP_CENTER, rows[SETTINGS_ROW_START + 2]);
    display.print("-> Disconnect <-", DISP_CENTER, rows[SETTINGS_ROW_START + 4]);
    display.setFont(FONT_BIG);
    display.setColor(display.getColor());
}

void settingsRender(uint8_t kwpMode, bool nightMode, uint8_t selectedSetting, bool force)
{
    display.setFont(FONT_SMALL);
    static uint8_t lastSelected = 0xFF;
    static uint8_t lastKwpMode = 0xFF;
    static bool lastNightMode = false;

    if (lastKwpMode != kwpMode || force)
    {
        display.print(kwpModeStr(kwpMode), DISP_RIGHT, rows[SETTINGS_ROW_START]);
        lastKwpMode = kwpMode;
    }
    if (lastNightMode != nightMode || force)
    {
        display.print(nightMode ? "ON " : "OFF", DISP_RIGHT, rows[SETTINGS_ROW_START + 1]);
        lastNightMode = nightMode;
    }

    if (lastSelected != selectedSetting || force)
    {
        // Clear old arrow
        if (lastSelected < 2)
            display.print("   ", cols[1], rows[SETTINGS_ROW_START + lastSelected]);
        else if (lastSelected == 2)
            display.print(" ", DISP_LEFT, rows[SETTINGS_ROW_START + 2]);
        else if (lastSelected == 3)
            display.print(" ", DISP_LEFT, rows[SETTINGS_ROW_START + 4]);

        // Draw new arrow
        if (selectedSetting < 2)
            display.print("-->", cols[1], rows[SETTINGS_ROW_START + selectedSetting]);
        else if (selectedSetting == 2)
            display.print(">", DISP_LEFT, rows[SETTINGS_ROW_START + 2]);
        else if (selectedSetting == 3)
            display.print(">", DISP_LEFT, rows[SETTINGS_ROW_START + 4]);

        lastSelected = selectedSetting;
    }
    display.setFont(FONT_BIG);
}

void settingsRemove()
{
    uint16_t savedFg = display.getColor();
    display.setColor(display.getBackColor());
    settingsInit(true);
    display.setFont(FONT_SMALL);
    for (uint8_t r = SETTINGS_ROW_START - 1; r <= SETTINGS_ROW_START + 5; ++r)
        display.clearRow(r);
    display.setFont(FONT_BIG);
    display.setColor(savedFg);
}

} // namespace DisplayLayer
} // namespace obd
