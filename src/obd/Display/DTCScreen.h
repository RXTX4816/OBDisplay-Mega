// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../Model/DTCStore.h"

namespace obd
{
namespace DisplayLayer
{

enum class DtcState : uint8_t
{
    Idle,    // waiting for user action
    Reading, // ECU read in progress
    Done,    // results shown
    NoDtc,   // no faults found
};

void dtcInit(bool destroy = false);
void dtcRender(const Model::DTCStore& dtc, DtcState state, uint8_t selectedRow, bool force);
void dtcRemove();

} // namespace DisplayLayer
} // namespace obd
