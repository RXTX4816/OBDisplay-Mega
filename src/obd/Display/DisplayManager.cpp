// SPDX-License-Identifier: GPL-3.0-or-later
#include "DisplayManager.h"
#include "../../Config.h"
#include "../../display/Display.h"
#include "CockpitScreen.h"
#include "ExperimentalScreen.h"
#include "GraphicScreen.h"
#include "DTCScreen.h"
#include "SettingsScreen.h"
#include "DebugScreen.h"

namespace obd
{
namespace DisplayLayer
{

void DisplayManager::begin()
{
    display.begin();
    setNightMode(nightMode_);
}

void DisplayManager::setNightMode(bool night)
{
    nightMode_ = night;
    if (night)
    {
        display.setBackColor(Color::Black);
        display.setColor(Color::Cyan);
    }
    else
    {
        display.setBackColor(Color::White);
        display.setColor(Color::Black);
    }
    display.fillScreen(display.getBackColor());
}

void DisplayManager::initStatusBar()
{
    display.setFont(FONT_BIG);
    display.print("AVA:   | BC: 0x  ", DISP_LEFT, rows[0]);
    display.drawHLine(cols[0], rows[1] + 1, DISP_WIDTH);
}

void DisplayManager::renderStatusBar(bool connected, uint8_t blockCounter, bool nightMode)
{
    (void)connected;
    (void)nightMode;
    display.setFont(FONT_BIG);
    char bc[3];
    snprintf(bc, sizeof(bc), "%02X", blockCounter);
    display.print(bc, cols[16], rows[0]);
    display.printNumI(KWP_SERIAL.available(), cols[4], rows[0], 1, '0');
}

void DisplayManager::initMenu(MenuId menu, uint8_t screen)
{
    switch (menu)
    {
        case MenuId::Cockpit:
            cockpitInit(screen);
            break;
        case MenuId::Experimental:
            experimentalInit(screen);
            break;
        case MenuId::Graphics:
            graphicInit();
            break;
        case MenuId::Dtc:
            dtcInit();
            break;
        case MenuId::Settings:
            settingsInit();
            break;
        case MenuId::Debug:
            debugInit(screen);
            break;
    }
}

void DisplayManager::render(MenuId menu, uint8_t screen, const Model::OBDSignals& sig,
                            const Model::DTCStore& dtc, bool forceRedraw)
{
    switch (menu)
    {
        case MenuId::Cockpit:
            cockpitRender(screen, sig, forceRedraw);
            break;
        case MenuId::Experimental:
            experimentalRender(screen, sig, forceRedraw);
            break;
        case MenuId::Graphics:
            graphicRender(sig, forceRedraw);
            break;
        case MenuId::Dtc:
            // DTC state is managed by OBDDisplay; render called with cached state
            dtcRender(dtc, DtcState::Done, 0, forceRedraw);
            break;
        case MenuId::Settings:
            settingsRender(0, nightMode_, 0, forceRedraw);
            break;
        case MenuId::Debug:
            debugRender(screen, forceRedraw);
            break;
    }
}

void DisplayManager::showWaiting(uint16_t reconnectCountdown)
{
    display.setFont(FONT_BIG);
    display.print("Waiting to reconnect...", DISP_CENTER, rows[9]);
    char buf[12];
    snprintf(buf, sizeof(buf), "%u s", reconnectCountdown);
    display.print(buf, DISP_CENTER, rows[11]);
}

void DisplayManager::showConnecting(uint8_t ecuAddr, uint32_t baud)
{
    display.setFont(FONT_BIG);
    char buf[32];
    snprintf(buf, sizeof(buf), "Connecting 0x%02X @ %lu", ecuAddr, (unsigned long)baud);
    display.print(buf, DISP_CENTER, rows[8]);
    display.print("5-baud init...", DISP_LEFT, rows[10]);
}

void DisplayManager::showConnected()
{
    display.setFont(FONT_BIG);
    display.print("Connected!", DISP_CENTER, rows[15]);
}

void DisplayManager::showStartupAnimation()
{
    display.setFont(FONT_SEVENSEG);
    display.print("OBD", DISP_CENTER, rows[4]);
    display.setFont(FONT_BIG);
    display.print("OBDisplay v" APP_VERSION, DISP_CENTER, rows[9]);
    display.print("Mega Edition", DISP_CENTER, rows[11]);

    // Progress bar
    display.drawRect(cols[1], rows[17], cols[28], rows[17] + 14);
    for (uint8_t i = 0; i < 27; ++i)
    {
        display.fillRect(cols[1] + 1, rows[17] + 1, cols[1] + 1 + i * 16, rows[17] + 13);
        delay(10);
    }
    delay(300);
    display.fillScreen(display.getBackColor());
}

void DisplayManager::showSetupMenu(uint8_t ecuAddr, uint32_t baud, bool debug, bool sim, bool night,
                                   uint8_t selectedRow)
{
    display.setFont(FONT_BIG);
    display.print("-->", DISP_LEFT, rows[selectedRow]);
    display.print("Mode:", DISP_CENTER, rows[10]);
    display.print(sim ? "SIM" : "ECU", DISP_RIGHT, rows[10]);
    display.print("Baud:", DISP_CENTER, rows[11]);
    display.printNumI(baud, DISP_RIGHT, rows[11], 5, '0');
    display.print("Debug:", DISP_CENTER, rows[12]);
    display.print(debug ? "ON " : "OFF", DISP_RIGHT, rows[12]);
    display.print("ECU Addr: 0x", DISP_CENTER, rows[13]);
    char addr[3];
    snprintf(addr, sizeof(addr), "%02X", ecuAddr);
    display.print(addr, DISP_RIGHT, rows[13]);
    display.setColor(Color::DarkGrey);
    display.print("Night mode:", DISP_CENTER, rows[15]);
    display.print(night ? "ON " : "OFF", DISP_RIGHT, rows[15]);
    display.setColor(display.getColor());
    display.print("Connect", DISP_CENTER, rows[18]);
}

void DisplayManager::updateSetupMenu(uint8_t ecuAddr, uint32_t baud, bool debug, bool sim,
                                     bool night, uint8_t selectedRow, uint8_t prevRow)
{
    display.setFont(FONT_BIG);
    if (selectedRow != prevRow)
    {
        display.print("   ", DISP_LEFT, rows[prevRow]);
        display.print("-->", DISP_LEFT, rows[selectedRow]);
    }
    display.print(sim ? "SIM" : "ECU", DISP_RIGHT, rows[10]);
    display.printNumI(baud, DISP_RIGHT, rows[11], 5, '0');
    display.print(debug ? "ON " : "OFF", DISP_RIGHT, rows[12]);
    char addr[3];
    snprintf(addr, sizeof(addr), "%02X", ecuAddr);
    display.print(addr, DISP_RIGHT, rows[13]);
    display.print(night ? "ON " : "OFF", DISP_RIGHT, rows[15]);
}

void DisplayManager::clearSetupMenu(uint8_t arrowRow)
{
    display.setFont(FONT_BIG);
    display.print("   ", DISP_LEFT, rows[arrowRow]);
    for (uint8_t r = 10; r <= 18; ++r)
        display.clearRow(r);
}

void DisplayManager::showWarningOverlay(const Model::WarningState& w)
{
    if (w.maxLevel == 0)
        return;
    uint16_t color =
        (w.maxLevel >= 3) ? Color::Red : (w.maxLevel == 2 ? Color::Yellow : Color::DarkGrey);
    display.setColor(color);
    display.setFont(FONT_BIG);
    display.print("! WARNING !", DISP_CENTER, rows[17]);
    display.setColor(display.getColor());
}

void DisplayManager::clearWarningOverlay()
{
    uint16_t savedFg = display.getColor();
    display.setColor(display.getBackColor());
    display.setFont(FONT_BIG);
    display.clearRow(17);
    display.setColor(savedFg);
}

} // namespace DisplayLayer
} // namespace obd
