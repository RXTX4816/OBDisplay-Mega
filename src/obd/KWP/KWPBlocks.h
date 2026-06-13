// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <stdint.h>

// KWP operation modes used by OBDDisplay state machine
static constexpr uint8_t KWP_MODE_ACK = 0;         // keepalive only
static constexpr uint8_t KWP_MODE_READSENSORS = 1; // sequential group scan (0x17/0x01 groups)
static constexpr uint8_t KWP_MODE_READGROUP = 2;   // single raw group (Experimental screen)
