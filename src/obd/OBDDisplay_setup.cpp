// SPDX-License-Identifier: GPL-3.0-or-later
// Interactive setup flow + EEPROM helpers
#include "OBDDisplay.h"
#include "../display/Display.h"
#include <EEPROM.h>

namespace obd
{

// ── EEPROM fuel helpers ────────────────────────────────────────────────────

uint8_t readEepromFuel()
{
    uint8_t v = EEPROM.read(EEPROM_ADDR_FUEL_L);
    return (v == 0xFF) ? 0 : v;
}

void writeEepromFuel(uint8_t liters)
{
    EEPROM.update(EEPROM_ADDR_FUEL_L, liters);
}

// ── runSetupFlow_ ──────────────────────────────────────────────────────────
// Blocking interactive setup: let the user choose ECU address, baud rate,
// night mode, and simulation mode, then connect.

void OBDDisplay::runSetupFlow_()
{
    // Load EEPROM defaults (0xFF = uninitialised → use compile-time defaults)
    uint8_t baudIdx = EEPROM.read(EEPROM_ADDR_BAUD);
    if (baudIdx >= SUPPORTED_BAUD_COUNT)
        baudIdx = 4; // default 10400

    uint8_t ecuIdx = EEPROM.read(EEPROM_ADDR_ECU);
    if (ecuIdx >= ECU_ADDR_COUNT)
        ecuIdx = 3; // default 0x17

    uint8_t nightByte = EEPROM.read(EEPROM_ADDR_NIGHT);
    bool night = (nightByte == 0xFF) ? DEFAULT_NIGHT_MODE : (bool)nightByte;

    baudRate_ = SUPPORTED_BAUD_RATES[baudIdx];
    addrSelected_ = ECU_ADDRS[ecuIdx];
    nightMode_ = night;
    display_.setNightMode(nightMode_);

    // Row order for arrow navigation:
    // 10=sim, 11=baud, 12=debug, 13=ecu addr, 15=night mode, 18=connect
    static constexpr uint8_t NAV_ROWS[] = {10, 11, 13, 15, 18};
    static constexpr uint8_t NAV_ROW_COUNT = 5;
    uint8_t navIdx = 0;
    uint8_t prevNavIdx = 0;
    bool debugMode = false;

    display_.showSetupMenu(addrSelected_, baudRate_, debugMode, simulationMode_, nightMode_,
                           NAV_ROWS[navIdx]);

    uint32_t btnDebounce = 0;
    static constexpr uint32_t DEBOUNCE_MS = 200;

    for (;;)
    {
        buttons_.update();
        uint32_t now = millis();
        if (now - btnDebounce < DEBOUNCE_MS)
            continue;

        bool anyBtn = false;

        if (buttons_.upPressed())
        {
            anyBtn = true;
            prevNavIdx = navIdx;
            navIdx = (navIdx == 0) ? NAV_ROW_COUNT - 1 : navIdx - 1;
        }
        else if (buttons_.downPressed())
        {
            anyBtn = true;
            prevNavIdx = navIdx;
            navIdx = (navIdx + 1 >= NAV_ROW_COUNT) ? 0 : navIdx + 1;
        }
        else if (buttons_.rightPressed() || buttons_.midPressed())
        {
            anyBtn = true;
            switch (navIdx)
            {
                case 0: // sim toggle
                    simulationMode_ = !simulationMode_;
                    break;
                case 1: // baud
                    baudIdx = (baudIdx + 1 >= SUPPORTED_BAUD_COUNT) ? 0 : baudIdx + 1;
                    baudRate_ = SUPPORTED_BAUD_RATES[baudIdx];
                    break;
                case 2: // ECU address
                    ecuIdx = (ecuIdx + 1 >= ECU_ADDR_COUNT) ? 0 : ecuIdx + 1;
                    addrSelected_ = ECU_ADDRS[ecuIdx];
                    break;
                case 3: // night mode
                    nightMode_ = !nightMode_;
                    break;
                case 4: // Connect
                    goto done;
            }
        }
        else if (buttons_.leftPressed())
        {
            anyBtn = true;
            switch (navIdx)
            {
                case 1:
                    baudIdx = (baudIdx == 0) ? SUPPORTED_BAUD_COUNT - 1 : baudIdx - 1;
                    baudRate_ = SUPPORTED_BAUD_RATES[baudIdx];
                    break;
                case 2:
                    ecuIdx = (ecuIdx == 0) ? ECU_ADDR_COUNT - 1 : ecuIdx - 1;
                    addrSelected_ = ECU_ADDRS[ecuIdx];
                    break;
                default:
                    break;
            }
        }

        if (anyBtn)
        {
            btnDebounce = now;
            display_.updateSetupMenu(addrSelected_, baudRate_, debugMode, simulationMode_,
                                     nightMode_, NAV_ROWS[navIdx], NAV_ROWS[prevNavIdx]);
        }
    }
done:
    // Persist choices
    EEPROM.update(EEPROM_ADDR_BAUD, baudIdx);
    EEPROM.update(EEPROM_ADDR_ECU, ecuIdx);
    EEPROM.update(EEPROM_ADDR_NIGHT, (uint8_t)nightMode_);

    display_.clearSetupMenu(NAV_ROWS[navIdx]);
    display_.setNightMode(nightMode_);
    display_.initStatusBar();

    phase_ = Phase::Running;
    menuState_.reset();
    DisplayLayer::MenuId mid = (DisplayLayer::MenuId)menuState_.menu();
    display_.initMenu(mid, menuState_.screen());
    displayDirty_ = true;
}

} // namespace obd
