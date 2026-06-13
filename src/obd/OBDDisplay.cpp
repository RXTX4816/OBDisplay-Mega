// SPDX-License-Identifier: GPL-3.0-or-later
#include "OBDDisplay.h"
#include "KWP/KWPComm.h" // defines diag + callbacks — included exactly once here
#include "Display/DebugScreen.h"
#include "../display/Display.h"
#include "../debug.h"
#include <EEPROM.h>

namespace obd
{

OBDDisplay::OBDDisplay() {}

// ── begin ──────────────────────────────────────────────────────────────────

void OBDDisplay::begin()
{
    pinMode(TX_PIN, OUTPUT);
    digitalWrite(TX_PIN, HIGH); // K-Line idle high

    buttons_.begin();
    display_.begin();
    display_.setNightMode(nightMode_);
    display_.showStartupAnimation();
    display_.initStatusBar();

    phase_ = Phase::Setup;
}

// ── updateKwp ──────────────────────────────────────────────────────────────

void OBDDisplay::updateKwp()
{
    switch (phase_)
    {
        case Phase::Setup:
            runSetupFlow_();
            break;

        case Phase::WaitingForConnect:
            if (millis() >= reconnectAfterMs_)
            {
                phase_ = Phase::Running;
                connected_ = false;
            }
            break;

        case Phase::Running:
            if (!ensureConnected_())
                return;
            pollEcu_();
            break;
    }
}

// ── updateCompute ──────────────────────────────────────────────────────────

void OBDDisplay::updateCompute()
{
    if (phase_ != Phase::Running || !connected_)
        return;

    if (simulationMode_)
        simulateValues_();

    signals_.compute(millis(), connectTimeStart_);
    signals_.computeWarnings(addrSelected_);

    if (signals_.warnings.hasNew)
    {
        warningFlashUntilMs_ = millis() + 3000;
        warningFlashSnapshot_ = signals_.warnings;
        signals_.warnings.hasNew = false;
    }

    displayDirty_ = true;
}

// ── updateDisplay ──────────────────────────────────────────────────────────

void OBDDisplay::updateDisplay()
{
    if (phase_ == Phase::WaitingForConnect)
    {
        uint16_t secs = (uint16_t)((reconnectAfterMs_ - millis()) / 1000);
        display_.showWaiting(secs);
        return;
    }

    if (phase_ != Phase::Running)
        return;

    handleInput_();

    // Status bar
    display_.renderStatusBar(connected_, (uint8_t)blockCounter_, nightMode_);

    // Warning flash
    bool flashActive = (millis() < warningFlashUntilMs_);
    if (flashActive)
    {
        display_.showWarningOverlay(warningFlashSnapshot_);
    }
    else
    {
        display_.clearWarningOverlay();
    }

    // Menu switch
    if (menuState_.menuChanged() || menuState_.screenChanged())
    {
        DisplayLayer::MenuId mid = (DisplayLayer::MenuId)menuState_.menu();
        display_.initMenu(mid, menuState_.screen());
        menuState_.consumeMenuChange();
        menuState_.consumeScreenChange();
        displayDirty_ = true;
    }

    if (displayDirty_)
    {
        DisplayLayer::MenuId mid = (DisplayLayer::MenuId)menuState_.menu();
        display_.render(mid, menuState_.screen(), signals_, dtcStore_, false);
        displayDirty_ = false;
    }
}

// ── pollButtons ────────────────────────────────────────────────────────────

void OBDDisplay::pollButtons()
{
    buttons_.update();

    uint8_t cur = 0;
    if (buttons_.upPressed())
        cur |= 0x01;
    if (buttons_.downPressed())
        cur |= 0x02;
    if (buttons_.leftPressed())
        cur |= 0x04;
    if (buttons_.rightPressed())
        cur |= 0x08;
    if (buttons_.midPressed())
        cur |= 0x10;
    if (buttons_.setPressed())
        cur |= 0x20;
    if (buttons_.rstPressed())
        cur |= 0x40;

    // Rising-edge detection
    uint8_t rising = cur & ~lastBtns_;
    pendingBtns_ |= rising;
    lastBtns_ = cur;

    // Auto-repeat for directional buttons
    static constexpr uint8_t DIR_MASK = 0x0F;
    uint32_t now = millis();
    if ((cur & DIR_MASK) && repeatFireAt_ == 0)
    {
        repeatBtns_ = cur & DIR_MASK;
        repeatFireAt_ = now + 400;
    }
    else if (!(cur & DIR_MASK))
    {
        repeatBtns_ = 0;
        repeatFireAt_ = 0;
    }
    else if (now >= repeatFireAt_)
    {
        pendingBtns_ |= repeatBtns_;
        repeatFireAt_ = now + 120;
    }
}

// ── ensureConnected_ ───────────────────────────────────────────────────────

bool OBDDisplay::ensureConnected_()
{
    if (connected_)
        return true;

    reconnectAttempts_++;
    display_.showConnecting(addrSelected_, baudRate_);

    KLineKWP1281Lib::executionStatus status = diag.attemptConnect(addrSelected_, baudRate_, false);

    if (status == KLineKWP1281Lib::SUCCESS)
    {
        connected_ = true;
        connectTimeStart_ = millis();
        signals_.reset();

        // Seed fuel start (from EEPROM if 0x17; else 0)
        signals_.instruments.fuelLevelStart =
            (addrSelected_ == ADDR_INSTRUMENTS) ? readEepromFuel() : 0;

        display_.showConnected();
        delay(500);
        display_.clearSetupMenu(10);
        display_.initStatusBar();
        DisplayLayer::MenuId mid = (DisplayLayer::MenuId)menuState_.menu();
        display_.initMenu(mid, menuState_.screen());
        displayDirty_ = true;
        blockCounter_ = 0;
        return true;
    }

    // Connection failed
    connected_ = false;
    if (autoReconnect_)
    {
        reconnectAfterMs_ = millis() + RECONNECT_DELAY_MS;
        phase_ = Phase::WaitingForConnect;
    }
    else
    {
        phase_ = Phase::Setup;
    }
    return false;
}

// ── pollEcu_ ──────────────────────────────────────────────────────────────

void OBDDisplay::pollEcu_()
{
    if (kwpMode_ == KWP_MODE_ACK)
    {
        diag.update(); // returns void; errors detected implicitly via next readGroup
        blockCounter_++;
        return;
    }

    // Groups to read for each ECU address
    static const uint8_t INSTR_GROUPS[] = {1, 2, 3};
    static const uint8_t ENGINE_GROUPS[] = {1, 3, 4, 6};
    static uint8_t groupIdx = 0;

    uint8_t block;
    if (kwpMode_ == KWP_MODE_READGROUP)
    {
        block = signals_.experimental.groupCurrent;
    }
    else if (addrSelected_ == ADDR_INSTRUMENTS)
    {
        block = INSTR_GROUPS[groupIdx % 3];
        groupIdx++;
    }
    else
    {
        block = ENGINE_GROUPS[groupIdx % 4];
        groupIdx++;
    }

    uint8_t measurements[29];
    uint8_t amount = 0;
    KLineKWP1281Lib::executionStatus s =
        diag.readGroup(amount, block, measurements, sizeof(measurements));

    blockCounter_++;

    switch (s)
    {
        case KLineKWP1281Lib::ERROR:
            diag.disconnect();
            connected_ = false;
            DBG_ADD(0x22);
            return;

        case KLineKWP1281Lib::FAIL:
            return; // group doesn't exist; skip

        case KLineKWP1281Lib::SUCCESS:
            break;

        default:
            return;
    }

    // Populate signals from measurement buffer
    for (uint8_t i = 0; i < amount && i < 4; ++i)
    {
        auto type =
            KLineKWP1281Lib::getMeasurementType(i, amount, measurements, sizeof(measurements));
        if (type == KLineKWP1281Lib::VALUE)
        {
            double val =
                KLineKWP1281Lib::getMeasurementValue(i, amount, measurements, sizeof(measurements));

            if (kwpMode_ == KWP_MODE_READGROUP)
            {
                // Experimental group: store raw value and unit
                signals_.experimental.v[i] = (float)val;
                signals_.experimental.k[i] = block;
                char units[Model::ExperimentalGroup::UnitWidth + 1];
                KLineKWP1281Lib::getMeasurementUnits(i, amount, measurements, sizeof(measurements),
                                                     units, sizeof(units));
                strncpy(signals_.experimental.unit[i], units, Model::ExperimentalGroup::UnitWidth);
                signals_.experimental.unit[i][Model::ExperimentalGroup::UnitWidth] = '\0';
            }
            else
            {
                signals_.populateMeasurement(block, i, val, addrSelected_);
            }
        }
    }

    if (kwpMode_ == KWP_MODE_READGROUP)
    {
        signals_.experimental.vUpdated = true;
        signals_.experimental.unitUpdated = true;
    }

    displayDirty_ = true;
}

// ── computeValues_ ────────────────────────────────────────────────────────

void OBDDisplay::computeValues_()
{
    // called from updateCompute — nothing extra needed for Mega
}

// ── resetState_ ───────────────────────────────────────────────────────────

void OBDDisplay::resetState_()
{
    connected_ = false;
    blockCounter_ = 0;
    signals_.reset();
    dtcStore_.reset();
    dtcState_ = DisplayLayer::DtcState::Idle;
}

// ── updateDisplay_ ────────────────────────────────────────────────────────

void OBDDisplay::updateDisplay_()
{
    // Stub — full logic is in updateDisplay()
}

// ── simulateValues_ ───────────────────────────────────────────────────────

void OBDDisplay::simulateValues_()
{
    static bool rpmDir = true;
    static bool spdDir = true;
    static bool coolDir = false;

    auto bump = [](uint16_t& v, bool& dir, uint16_t step, uint16_t lo, uint16_t hi)
    {
        if (dir)
        {
            v = (v + step > hi) ? hi : v + step;
            if (v >= hi)
                dir = false;
        }
        else
        {
            v = (v < lo + step) ? lo : v - step;
            if (v <= lo)
                dir = true;
        }
    };

    bump(signals_.instruments.engineRpm, rpmDir, 87, 700, 7000);
    bump(signals_.instruments.vehicleSpeed, spdDir, 1, 0, 200);
    {
        uint16_t tmp = signals_.instruments.coolantTemp;
        bump(tmp, coolDir, 1, 20, 110);
        signals_.instruments.coolantTemp = (uint8_t)tmp;
    }
    signals_.instruments.engineRpmUpdated = true;
    signals_.instruments.vehicleSpeedUpdated = true;
    signals_.instruments.coolantTempUpdated = true;

    if (signals_.instruments.fuelLevel > 1)
        signals_.instruments.fuelLevel--;
    else
        signals_.instruments.fuelLevel = 50;
    signals_.instruments.fuelLevelUpdated = true;
}

// ── showWaitingScreen_ ───────────────────────────────────────────────────

void OBDDisplay::showWaitingScreen_()
{
    uint16_t secs =
        (reconnectAfterMs_ > millis()) ? (uint16_t)((reconnectAfterMs_ - millis()) / 1000) : 0;
    display_.showWaiting(secs);
}

} // namespace obd
