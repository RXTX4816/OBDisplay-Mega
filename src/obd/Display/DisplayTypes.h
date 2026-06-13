// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <stdint.h>

namespace obd
{
namespace DisplayLayer
{

enum class MenuId : uint8_t
{
    Cockpit = 0,
    Experimental = 1,
    Graphics = 2,
    Dtc = 3,
    Settings = 4,
    Debug = 5,
};

} // namespace DisplayLayer
} // namespace obd
