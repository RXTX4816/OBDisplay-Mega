// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>
#include <KLineKWP1281Lib.h>
#include "Display/DisplayManager.h"
#include "Display/DTCScreen.h"
#include "KWP/KWPBlocks.h"
#include "Model/OBDSignals.h"
#include "Model/DTCStore.h"
#include "Input/MenuState.h"
#include "Input/ButtonInput.h"
#include "../Config.h"

// Forward-declared from KWPComm.h — included once from OBDDisplay.cpp
extern KLineKWP1281Lib diag;

namespace obd
{

// EEPROM helpers implemented in OBDDisplay_setup.cpp
uint8_t readEepromFuel();
void writeEepromFuel(uint8_t liters);

class OBDDisplay
{
  public:
    OBDDisplay();

    void begin();

    // Scheduler task entry points (called from Controller)
    void updateKwp();
    void updateCompute();
    void updateDisplay();
    void pollButtons();

    bool isConnected() const { return connected_; }

  private:
    DisplayLayer::DisplayManager display_;
    Model::OBDSignals signals_;
    Model::DTCStore dtcStore_;
    Input::MenuState menuState_;
    Input::ButtonInput buttons_;

    uint32_t baudRate_ = DEFAULT_BAUD_RATE;
    uint8_t addrSelected_ = DEFAULT_ECU_ADDR;
    uint8_t kwpMode_ = KWP_MODE_READSENSORS;
    uint8_t kwpGroup_ = 1;

    bool nightMode_ = DEFAULT_NIGHT_MODE;
    bool simulationMode_ = false;

    // DTC state
    DisplayLayer::DtcState dtcState_ = DisplayLayer::DtcState::Idle;
    uint8_t dtcMenuCursor_ = 0; // 0=Reset, 1=Read, 2=Clear

    // Settings state
    uint8_t settingsMenuCursor_ = 0; // 0=KWP mode, 1=night mode, 2=disconnect

    bool autoReconnect_ = true;
    uint8_t reconnectAttempts_ = 0;
    uint32_t reconnectAfterMs_ = 0;

    bool connected_ = false;
    uint32_t connectTimeStart_ = 0;

    bool displayDirty_ = false;

    // Warning flash overlay (3 s on new warning)
    uint32_t warningFlashUntilMs_ = 0;
    Model::WarningState warningFlashSnapshot_;

    // Button latch state (populated by pollButtons, consumed by handleInput_)
    uint8_t pendingBtns_ = 0;
    uint8_t lastBtns_ = 0;
    uint8_t repeatBtns_ = 0;
    uint32_t repeatFireAt_ = 0;

    // Benchmark stats for debug overlay
    uint16_t blockCounter_ = 0;

    enum class Phase : uint8_t
    {
        Setup,
        WaitingForConnect,
        Running
    } phase_ = Phase::Setup;

    // ── private methods ────────────────────────────────────────────────────
    void runSetupFlow_();
    void showWaitingScreen_();
    void resetState_();
    bool ensureConnected_();
    void pollEcu_();
    void computeValues_();
    void handleInput_(); // implemented in OBDDisplay_input.cpp
    void updateDisplay_();
    void simulateValues_();
};

} // namespace obd
