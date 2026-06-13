// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "DisplayTypes.h"
#include "../Model/OBDSignals.h"
#include "../Model/DTCStore.h"
#include "../Input/MenuState.h"

namespace obd
{
namespace DisplayLayer
{

class DisplayManager
{
  public:
    void begin();

    // Called when the active menu/screen changes (redraws static labels)
    void initMenu(MenuId menu, uint8_t screen);

    // Called every display tick to refresh dynamic values
    void render(MenuId menu, uint8_t screen, const Model::OBDSignals& sig,
                const Model::DTCStore& dtc, bool forceRedraw);

    // Status bar (connection state, block counter) — drawn atop all menus
    void renderStatusBar(bool connected, uint8_t blockCounter, bool nightMode);
    void initStatusBar();

    // Waiting / setup overlays
    void showWaiting(uint16_t reconnectCountdown);
    void showConnecting(uint8_t ecuAddr, uint32_t baud);
    void showConnected();
    void showStartupAnimation();
    void showSetupMenu(uint8_t ecuAddr, uint32_t baud, bool debug, bool sim, bool night,
                       uint8_t selectedRow);
    void updateSetupMenu(uint8_t ecuAddr, uint32_t baud, bool debug, bool sim, bool night,
                         uint8_t selectedRow, uint8_t prevRow);
    void clearSetupMenu(uint8_t arrowRow);

    // Warning flash overlay
    void showWarningOverlay(const Model::WarningState& w);
    void clearWarningOverlay();

    // Night/day colour scheme
    void setNightMode(bool night);
    bool nightMode() const { return nightMode_; }

  private:
    bool nightMode_ = true;
};

} // namespace DisplayLayer
} // namespace obd
