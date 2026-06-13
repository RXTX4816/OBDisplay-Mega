// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../Model/OBDSignals.h"

namespace obd
{
namespace DisplayLayer
{

// Raw group reading view — shows live ExperimentalGroup data for any group number
void graphicInit(bool destroy = false);
void graphicRender(const Model::OBDSignals& sig, bool force);
void graphicRemove();

} // namespace DisplayLayer
} // namespace obd
