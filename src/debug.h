// SPDX-License-Identifier: GPL-3.0-or-later
// Binary debug logging — zero flash cost in production builds.
// Include from any file that needs to emit debug codes.
#pragma once

#include "obd/Display/DebugScreen.h"

// In production (OBD_DEBUG not defined), all macros expand to nothing.
#ifdef OBD_DEBUG
#define DBG_ADD(code) obd::DisplayLayer::debugAdd(code)
#define DBG_CLEAR() obd::DisplayLayer::debugClear()
#else
#define DBG_ADD(code) ((void)0)
#define DBG_CLEAR() ((void)0)
#endif

// Convenience alias used inside OBDDisplay.cpp
#define debugAdd(c) DBG_ADD(c)
