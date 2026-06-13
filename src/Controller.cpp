// SPDX-License-Identifier: GPL-3.0-or-later
#include "Controller.h"
#include "obd/OBDDisplay.h"

namespace obd
{

static OBDDisplay obdDisplay;

void Controller::begin()
{
    obdDisplay.begin();
}

void Controller::taskKwp()
{
    obdDisplay.updateKwp();
}

void Controller::taskInput()
{
    obdDisplay.pollButtons();
}

void Controller::taskCompute()
{
    obdDisplay.updateCompute();
}

void Controller::taskDisplay()
{
    obdDisplay.updateDisplay();
}

} // namespace obd
