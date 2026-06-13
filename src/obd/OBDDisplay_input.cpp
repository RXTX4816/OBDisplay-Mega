// SPDX-License-Identifier: GPL-3.0-or-later
// Button-to-action mapping for OBDDisplay
#include "OBDDisplay.h"
#include "../display/Display.h"

namespace obd
{

// Bit positions matching pollButtons_ encoding
static constexpr uint8_t BTN_UP = 0x01;
static constexpr uint8_t BTN_DOWN = 0x02;
static constexpr uint8_t BTN_LEFT = 0x04;
static constexpr uint8_t BTN_RIGHT = 0x08;
static constexpr uint8_t BTN_MID = 0x10;
static constexpr uint8_t BTN_SET = 0x20;
static constexpr uint8_t BTN_RST = 0x40;

static bool consume(uint8_t& pending, uint8_t bit)
{
    if (pending & bit)
    {
        pending &= ~bit;
        return true;
    }
    return false;
}

void OBDDisplay::handleInput_()
{
    uint8_t btns = pendingBtns_;
    pendingBtns_ = 0;

    if (btns == 0)
        return;

    DisplayLayer::MenuId mid = (DisplayLayer::MenuId)menuState_.menu();

    // Global: RST → go to menu 0
    if (consume(btns, BTN_RST))
    {
        menuState_.reset();
        menuState_.nextMenu(); // trigger redraw via menuChanged
        menuState_.prevMenu();
        displayDirty_ = true;
        return;
    }

    // SET cycles menus
    if (consume(btns, BTN_SET))
    {
        menuState_.nextMenu();
        displayDirty_ = true;
        return;
    }

    switch (mid)
    {
        case DisplayLayer::MenuId::Cockpit:
        case DisplayLayer::MenuId::Experimental:
            if (consume(btns, BTN_UP) || consume(btns, BTN_RIGHT))
                menuState_.nextScreen();
            if (consume(btns, BTN_DOWN) || consume(btns, BTN_LEFT))
                menuState_.prevScreen();
            break;

        case DisplayLayer::MenuId::Graphics:
            // UP/DOWN changes the group number
            if (consume(btns, BTN_UP))
            {
                if (signals_.experimental.groupCurrent < 255)
                    signals_.experimental.groupCurrent++;
                signals_.experimental.groupSideUpdated = true;
                displayDirty_ = true;
            }
            if (consume(btns, BTN_DOWN))
            {
                if (signals_.experimental.groupCurrent > 1)
                    signals_.experimental.groupCurrent--;
                signals_.experimental.groupSideUpdated = true;
                displayDirty_ = true;
            }
            break;

        case DisplayLayer::MenuId::Dtc:
            if (consume(btns, BTN_UP) || consume(btns, BTN_LEFT))
                dtcMenuCursor_ = (dtcMenuCursor_ == 0) ? 2 : dtcMenuCursor_ - 1;
            if (consume(btns, BTN_DOWN) || consume(btns, BTN_RIGHT))
                dtcMenuCursor_ = (dtcMenuCursor_ >= 2) ? 0 : dtcMenuCursor_ + 1;
            if (consume(btns, BTN_MID))
            {
                switch (dtcMenuCursor_)
                {
                    case 0: // Reset local buffer
                        dtcStore_.reset();
                        dtcState_ = DisplayLayer::DtcState::Idle;
                        break;
                    case 1: // Read from ECU
                    {
                        dtcState_ = DisplayLayer::DtcState::Reading;
                        displayDirty_ = true;
                        uint8_t buf[Model::DTCStore::MaxCount * 3];
                        uint8_t amount = 0;
                        dtcStore_.reset();
                        auto s = diag.readFaults(amount, buf, sizeof(buf));
                        if (s == KLineKWP1281Lib::SUCCESS && amount > 0)
                        {
                            uint8_t avail = (sizeof(buf) < (size_t)amount * 3)
                                                ? (uint8_t)(sizeof(buf) / 3)
                                                : amount;
                            for (uint8_t i = 0; i < avail && i < Model::DTCStore::MaxCount; ++i)
                            {
                                uint16_t code =
                                    KLineKWP1281Lib::getFaultCode(i, avail, buf, sizeof(buf));
                                uint8_t elab = KLineKWP1281Lib::getFaultElaborationCode(
                                    i, avail, buf, sizeof(buf));
                                dtcStore_.set(i, code, elab);
                            }
                            dtcState_ = DisplayLayer::DtcState::Done;
                        }
                        else
                        {
                            dtcState_ = DisplayLayer::DtcState::NoDtc;
                        }
                        break;
                    }
                    case 2: // Clear DTCs
                        diag.clearFaults();
                        dtcStore_.reset();
                        dtcState_ = DisplayLayer::DtcState::NoDtc;
                        break;
                }
                displayDirty_ = true;
            }
            break;

        case DisplayLayer::MenuId::Settings:
            if (consume(btns, BTN_UP) || consume(btns, BTN_LEFT))
                settingsMenuCursor_ = (settingsMenuCursor_ == 0) ? 2 : settingsMenuCursor_ - 1;
            if (consume(btns, BTN_DOWN) || consume(btns, BTN_RIGHT))
                settingsMenuCursor_ = (settingsMenuCursor_ >= 2) ? 0 : settingsMenuCursor_ + 1;
            if (consume(btns, BTN_MID))
            {
                switch (settingsMenuCursor_)
                {
                    case 0: // Cycle KWP mode
                        kwpMode_++;
                        if (kwpMode_ > KWP_MODE_READGROUP)
                            kwpMode_ = KWP_MODE_ACK;
                        break;
                    case 1: // Toggle night mode
                        nightMode_ = !nightMode_;
                        display_.setNightMode(nightMode_);
                        break;
                    case 2: // Disconnect
                        diag.disconnect();
                        connected_ = false;
                        resetState_();
                        phase_ = Phase::Setup;
                        return;
                }
                displayDirty_ = true;
            }
            break;

        case DisplayLayer::MenuId::Debug:
            if (consume(btns, BTN_UP))
            {
                uint8_t pg = menuState_.screen();
                if (pg > 0)
                    menuState_.prevScreen();
            }
            if (consume(btns, BTN_DOWN))
            {
                menuState_.nextScreen();
            }
            break;
    }
}

} // namespace obd
