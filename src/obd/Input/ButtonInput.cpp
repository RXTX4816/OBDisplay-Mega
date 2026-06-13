// SPDX-License-Identifier: GPL-3.0-or-later
#include "ButtonInput.h"

namespace obd
{
namespace Input
{

void ButtonInput::begin()
{
    pinMode(PIN_UP, INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_LEFT, INPUT_PULLUP);
    pinMode(PIN_RIGHT, INPUT_PULLUP);
    pinMode(PIN_MID, INPUT_PULLUP);
    pinMode(PIN_SET, INPUT_PULLUP);
    pinMode(PIN_RST, INPUT_PULLUP);
}

void ButtonInput::update()
{
    up_ = readPin(PIN_UP);
    down_ = readPin(PIN_DOWN);
    left_ = readPin(PIN_LEFT);
    right_ = readPin(PIN_RIGHT);
    mid_ = readPin(PIN_MID);
    set_ = readPin(PIN_SET);
    rst_ = readPin(PIN_RST);
}

bool ButtonInput::consumeUp()
{
    bool v = up_;
    up_ = false;
    return v;
}
bool ButtonInput::consumeDown()
{
    bool v = down_;
    down_ = false;
    return v;
}
bool ButtonInput::consumeLeft()
{
    bool v = left_;
    left_ = false;
    return v;
}
bool ButtonInput::consumeRight()
{
    bool v = right_;
    right_ = false;
    return v;
}
bool ButtonInput::consumeMid()
{
    bool v = mid_;
    mid_ = false;
    return v;
}
bool ButtonInput::consumeSet()
{
    bool v = set_;
    set_ = false;
    return v;
}
bool ButtonInput::consumeRst()
{
    bool v = rst_;
    rst_ = false;
    return v;
}

} // namespace Input
} // namespace obd
