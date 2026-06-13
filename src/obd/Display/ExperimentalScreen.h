// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../Model/OBDSignals.h"

namespace obd
{
namespace DisplayLayer
{

// Page 0: instrument stats (odo, fuel sensor, oil pressure, time, connection stats)
// Page 1: engine stats (error bits, TB angle, steering, voltage, load, lambda)
void experimentalInit(uint8_t page, bool destroy = false);
void experimentalRender(uint8_t page, const Model::OBDSignals& sig, bool force);
void experimentalRemove(uint8_t page);

} // namespace DisplayLayer
} // namespace obd
