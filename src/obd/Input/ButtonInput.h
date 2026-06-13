// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>
#include "../../Config.h"

namespace obd
{
namespace Input
{

// 7-button joystick interface for Arduino Mega
// All pins are INPUT_PULLUP (active LOW)
class ButtonInput
{
  public:
    void begin();
    void update();

    bool upPressed() const { return up_; }
    bool downPressed() const { return down_; }
    bool leftPressed() const { return left_; }
    bool rightPressed() const { return right_; }
    bool midPressed() const { return mid_; }
    bool setPressed() const { return set_; }
    bool rstPressed() const { return rst_; }

    // Consume a button press (returns true once, then clears)
    bool consumeUp();
    bool consumeDown();
    bool consumeLeft();
    bool consumeRight();
    bool consumeMid();
    bool consumeSet();
    bool consumeRst();

  private:
    bool up_ = false;
    bool down_ = false;
    bool left_ = false;
    bool right_ = false;
    bool mid_ = false;
    bool set_ = false;
    bool rst_ = false;

    static bool readPin(uint8_t pin) { return digitalRead(pin) == LOW; }
};

} // namespace Input
} // namespace obd
