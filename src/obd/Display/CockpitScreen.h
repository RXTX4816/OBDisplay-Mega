// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../Model/OBDSignals.h"

namespace obd
{
namespace DisplayLayer
{

// Screen 0: large 7-seg style numbers — km driven / fuel remaining / km left / coolant
void cockpitInit(uint8_t screen, bool destroy = false);
bool cockpitRender(uint8_t screen, const Model::OBDSignals& sig, bool force);
void cockpitRemove(uint8_t screen);

} // namespace DisplayLayer
} // namespace obd
