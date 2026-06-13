// SPDX-License-Identifier: GPL-3.0-or-later
// KWP communication callbacks and global KLineKWP1281Lib instance.
// Include this exactly once (from OBDDisplay.cpp).
#pragma once

#include <KLineKWP1281Lib.h>
#include "../../Config.h"

// ── Serial callbacks ────────────────────────────────────────────────────────

inline void kwpBegin(uint32_t baud)
{
    KWP_SERIAL.begin(baud);
}

inline void kwpEnd()
{
    KWP_SERIAL.end();
}

inline void kwpSend(uint8_t b)
{
    KWP_SERIAL.write(b);
}

inline bool kwpReceive(uint8_t* b)
{
    if (KWP_SERIAL.available())
    {
        *b = (uint8_t)KWP_SERIAL.read();
        return true;
    }
    return false;
}

// ── Global diag instance ────────────────────────────────────────────────────
// Constructed once; TX_PIN and IS_FULL_DUPLEX come from Config.h.
KLineKWP1281Lib diag(kwpBegin, kwpEnd, kwpSend, kwpReceive, TX_PIN, IS_FULL_DUPLEX);
