// SPDX-License-Identifier: GPL-3.0-or-later
#include "ExperimentalScreen.h"
#include "../../display/Display.h"

namespace obd
{
namespace DisplayLayer
{

using namespace Model;

static void printLabel(const char* left, const char* right, uint8_t row, bool destroy)
{
    uint16_t color = destroy ? display.getBackColor() : display.getColor();
    display.setColor(color);
    display.print(left, DISP_LEFT, rows[row]);
    display.print(right, DISP_RIGHT, rows[row]);
    display.setColor(display.getColor());
}

void experimentalInit(uint8_t page, bool destroy)
{
    display.setFont(FONT_SMALL);
    if (page == 0)
    {
        printLabel("ODO", "km", 3, destroy);
        printLabel("FUEL R", "ohm", 4, destroy);
        printLabel("OILPRESS", "bool", 5, destroy);
        printLabel("TIME ECU", "ms", 6, destroy);
        printLabel("TIME CON", "secs", 7, destroy);
        printLabel("Traveled", "km", 8, destroy);
        printLabel("Fuel used", "L", 9, destroy);
        printLabel("Fuel/h", "L/h", 10, destroy);
        printLabel("Fuel/100", "L/100", 11, destroy);
        printLabel("KM left", "km", 12, destroy);
    }
    else
    {
        printLabel("ENG bits", "8-bit", 3, destroy);
        printLabel("TB angle", "deg", 4, destroy);
        printLabel("STR ang", "deg", 5, destroy);
        printLabel("Voltage", "V", 6, destroy);
        printLabel("ENG load", "%", 7, destroy);
        printLabel("AMB TEMP", "C", 8, destroy);
        printLabel("Pressure", "mbar", 9, destroy);
        printLabel("Lambda 1", "", 10, destroy);
        printLabel("Lambda 2", "", 11, destroy);
    }
    display.setFont(FONT_BIG);
}

void experimentalRender(uint8_t page, const OBDSignals& sig, bool force)
{
    const InstrumentSignals& ins = sig.instruments;
    const EngineSignals& eng = sig.engine;
    const ComputedStats& c = sig.computed;
    display.setFont(FONT_SMALL);

    if (page == 0)
    {
        if (ins.odometerUpdated || force)
            display.printNumI(ins.odometer, DISP_CENTER, rows[3], 6, '0');
        if (ins.fuelSensorResistanceUpdated || force)
            display.printNumI(ins.fuelSensorResistance, DISP_CENTER, rows[4], 4, '0');
        if (ins.oilPressureMinUpdated || force)
            display.printNumI(ins.oilPressureMin, DISP_CENTER, rows[5], 2, '0');
        if (ins.timeEcuUpdated || force)
            display.printNumI(ins.timeEcu, DISP_CENTER, rows[6], 7, '0');
        if (c.elapsedSecondsSinceStartUpdated || force)
            display.printNumI(c.elapsedSecondsSinceStart, DISP_CENTER, rows[7], 6, '0');
        if (c.elapsedKmSinceStartUpdated || force)
            display.printNumI(c.elapsedKmSinceStart, DISP_CENTER, rows[8], 4, '0');
        if (c.fuelBurnedSinceStartUpdated || force)
            display.printNumI(c.fuelBurnedSinceStart, DISP_CENTER, rows[9], 2, '0');
        if (c.fuelPerHourUpdated || force)
            display.printNumF(c.fuelPerHour, 1, DISP_CENTER, rows[10], '.', 6, '0');
        if (c.fuelPer100kmUpdated || force)
            display.printNumF(c.fuelPer100km, 1, DISP_CENTER, rows[11], '.', 5, '0');
        if (c.kmRemainingUpdated || force)
            display.printNumI(c.kmRemaining, DISP_CENTER, rows[12], 4, '0');
    }
    else
    {
        if (eng.errorBitsUpdated || force)
        {
            // Bit-per-character display with alternating warn colours
            const uint16_t savedColor = display.getColor();
            const bool bits[8] = {eng.exhaustGasRecirculationError,
                                  eng.oxygenSensorHeatingError,
                                  eng.oxygenSensorError,
                                  eng.airConditioningError,
                                  eng.secondaryAirInjectionError,
                                  eng.evaporativeEmissionsError,
                                  eng.catalystHeatingError,
                                  eng.catalyticConverter};
            for (uint8_t i = 0; i < 8; ++i)
            {
                display.setColor(i % 2 == 0 ? Color::Yellow : Color::Cyan);
                const char ch[2] = {(char)('0' + bits[i]), '\0'};
                display.print(ch, cols[11 + i], rows[3]);
            }
            display.setColor(savedColor);
        }
        if (eng.tbAngleUpdated || force)
            display.printNumF(eng.tbAngle, 1, DISP_CENTER, rows[4], '.', 4, '0');
        if (eng.steeringAngleUpdated || force)
            display.printNumF(eng.steeringAngle, 1, DISP_CENTER, rows[5], '.', 4, '0');
        if (eng.voltageUpdated || force)
            display.printNumF(eng.voltage, 1, DISP_CENTER, rows[6], '.', 4, '0');
        if (eng.engineLoadUpdated || force)
            display.printNumI(eng.engineLoad, DISP_CENTER, rows[7], 4, '0');
        if (ins.ambientTempUpdated || force)
            display.printNumI(ins.ambientTemp, DISP_CENTER, rows[8], 2, '0');
        if (eng.pressureUpdated || force)
            display.printNumI(eng.pressure, DISP_CENTER, rows[9], 5, '0');
        if (eng.lambdaUpdated || force)
            display.printNumI(eng.lambda, DISP_CENTER, rows[10], 3, '0');
        if (eng.lambda2Updated || force)
            display.printNumI(eng.lambda2, DISP_CENTER, rows[11], 3, '0');
    }
    display.setFont(FONT_BIG);
}

void experimentalRemove(uint8_t page)
{
    uint16_t savedFg = display.getColor();
    display.setColor(display.getBackColor());
    experimentalInit(page, true);
    display.setFont(FONT_SMALL);
    for (uint8_t r = 3; r <= 12; ++r)
        display.clearRow(r);
    display.setFont(FONT_BIG);
    display.setColor(savedFg);
}

} // namespace DisplayLayer
} // namespace obd
