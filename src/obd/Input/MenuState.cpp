// SPDX-License-Identifier: GPL-3.0-or-later
#include "MenuState.h"

namespace obd
{
namespace Input
{

void MenuState::reset()
{
    menu_ = 0;
    for (uint8_t i = 0; i < MENU_COUNT; ++i)
        screens_[i] = 0;
    menuChanged_ = false;
    screenChanged_ = false;
}

void MenuState::nextMenu()
{
    menu_ = (menu_ + 1 >= MENU_COUNT) ? 0 : menu_ + 1;
    menuChanged_ = true;
}

void MenuState::prevMenu()
{
    menu_ = (menu_ == 0) ? MENU_COUNT - 1 : menu_ - 1;
    menuChanged_ = true;
}

void MenuState::nextScreen()
{
    uint8_t maxScreens = MENU_SCREEN_COUNTS[menu_];
    screens_[menu_] = (screens_[menu_] + 1 >= maxScreens) ? 0 : screens_[menu_] + 1;
    screenChanged_ = true;
}

void MenuState::prevScreen()
{
    uint8_t maxScreens = MENU_SCREEN_COUNTS[menu_];
    screens_[menu_] = (screens_[menu_] == 0) ? maxScreens - 1 : screens_[menu_] - 1;
    screenChanged_ = true;
}

} // namespace Input
} // namespace obd
