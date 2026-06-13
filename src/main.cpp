// SPDX-License-Identifier: GPL-3.0-or-later
#include <Arduino.h>
#include "Controller.h"
#include "Config.h"

// ── Cooperative scheduler ──────────────────────────────────────────────────
// Each task runs at its own interval; the KWP task (interval=0) runs every
// loop iteration since K-Line timing is critical.

struct Task
{
    void (*fn)();
    uint32_t interval;
    uint32_t lastRun;
};

static Task tasks[] = {
    {obd::Controller::taskKwp, INTERVAL_KWP_MS, 0},
    {obd::Controller::taskInput, INTERVAL_INPUT_MS, 0},
    {obd::Controller::taskCompute, INTERVAL_COMPUTE_MS, 0},
    {obd::Controller::taskDisplay, INTERVAL_DISPLAY_MS, 0},
};

void setup()
{
    Serial.begin(115200); // USB debug monitor

    obd::Controller::begin();

    // Stagger initial task times to avoid all tasks firing at once
    tasks[1].lastRun = millis();
    tasks[2].lastRun = millis();
    tasks[3].lastRun = millis();
}

void loop()
{
    uint32_t now = millis();
    for (auto& t : tasks)
    {
        if (now - t.lastRun >= t.interval)
        {
            t.fn();
            t.lastRun = now;
        }
    }
}
