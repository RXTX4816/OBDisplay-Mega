// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

namespace obd
{

class Controller
{
  public:
    static void begin();

    static void taskKwp();
    static void taskInput();
    static void taskCompute();
    static void taskDisplay();
};

} // namespace obd
