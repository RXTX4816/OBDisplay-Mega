// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <stdint.h>
#include "../KWP/KWPBlocks.h"

namespace obd
{
namespace DisplayLayer
{

void settingsInit(bool destroy = false);
void settingsRender(uint8_t kwpMode, bool nightMode, uint8_t selectedSetting, bool force);
void settingsRemove();

} // namespace DisplayLayer
} // namespace obd
