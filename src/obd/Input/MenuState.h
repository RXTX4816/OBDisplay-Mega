// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>

namespace obd
{
namespace Input
{

// Number of top-level menus
static constexpr uint8_t MENU_COUNT = 6;

// Per-menu screen counts
static constexpr uint8_t MENU_SCREEN_COUNTS[MENU_COUNT] = {
    2, // Cockpit: page 0 (7-seg) + page 1 (text)
    2, // Experimental: page 0 (instrument stats) + page 1 (engine stats)
    1, // Graphics (group reading)
    1, // DTC
    1, // Settings
    1, // Debug
};

class MenuState
{
  public:
    void reset();

    uint8_t menu() const { return menu_; }
    uint8_t screen() const { return screens_[menu_]; }
    bool menuChanged() const { return menuChanged_; }
    bool screenChanged() const { return screenChanged_; }
    void consumeMenuChange() { menuChanged_ = false; }
    void consumeScreenChange() { screenChanged_ = false; }

    void nextMenu();
    void prevMenu();
    void nextScreen();
    void prevScreen();

  private:
    uint8_t menu_ = 0;
    uint8_t screens_[MENU_COUNT] = {0, 0, 0, 0, 0, 0};
    bool menuChanged_ = false;
    bool screenChanged_ = false;
};

} // namespace Input
} // namespace obd
