// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <stdint.h>

namespace obd
{
namespace DisplayLayer
{

// Simple paginated debug log — stores up to 80 one-byte event codes and renders them
// as colour-coded hex strings across 17 rows per page.

static constexpr uint8_t DEBUG_MAX_ENTRIES = 80;

// Severity colours mapped from the high nibble of the debug code byte:
// 0x0x = info (cyan), 0x1x = KWP protocol (green), 0x2x = warn (yellow), 0x3x+ = error (red)

void debugClear();
void debugAdd(uint8_t code);

void debugInit(uint8_t page, bool destroy = false);
bool debugRender(uint8_t page, bool forceAll); // returns true when cycle complete
void debugRemove(uint8_t page);

uint8_t debugPageCount();

} // namespace DisplayLayer
} // namespace obd
