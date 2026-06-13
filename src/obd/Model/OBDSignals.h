// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>

namespace obd
{
namespace Model
{

struct ExperimentalGroup
{
    uint8_t k[4] = {0, 0, 0, 0};
    float v[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static constexpr uint8_t UnitWidth = 8;
    char unit[4][UnitWidth + 1] = {
        {'N', '/', 'A', '\0'}, {'N', '/', 'A', '\0'}, {'N', '/', 'A', '\0'}, {'N', '/', 'A', '\0'}};

    bool kUpdated = false;
    bool vUpdated = false;
    bool unitUpdated = false;

    uint8_t groupCurrent = 1;
    bool groupSide = false;
    bool groupSideUpdated = false;

    void reset();
    void invertGroupSide();
};

struct InstrumentSignals
{
    uint16_t vehicleSpeed = 0;
    bool vehicleSpeedUpdated = false;

    uint16_t engineRpm = 0;
    bool engineRpmUpdated = false;

    uint16_t oilPressureMin = 0;
    bool oilPressureMinUpdated = false;
    uint8_t oilPressureLowCount = 0;

    uint32_t timeEcu = 0;
    bool timeEcuUpdated = false;

    uint32_t odometer = 0;
    bool odometerUpdated = false;
    uint32_t odometerStart = 0;

    uint8_t fuelLevel = 0;
    bool fuelLevelUpdated = false;
    uint8_t fuelLevelStart = 0;

    uint16_t fuelSensorResistance = 0;
    bool fuelSensorResistanceUpdated = false;

    int8_t ambientTemp = 0;
    bool ambientTempUpdated = false;

    uint8_t coolantTemp = 0;
    bool coolantTempUpdated = false;

    uint8_t oilLevelOk = 0;
    bool oilLevelOkUpdated = false;

    uint8_t oilTemp = 0;
    bool oilTempUpdated = false;
};

struct EngineSignals
{
    uint8_t tempUnknown1 = 0;
    bool tempUnknown1Updated = false;

    int8_t lambda = 0;
    bool lambdaUpdated = false;

    bool exhaustGasRecirculationError = false;
    bool oxygenSensorHeatingError = false;
    bool oxygenSensorError = false;
    bool airConditioningError = false;
    bool secondaryAirInjectionError = false;
    bool evaporativeEmissionsError = false;
    bool catalystHeatingError = false;
    bool catalyticConverter = false;
    bool errorBitsUpdated = false;

    uint16_t pressure = 0;
    bool pressureUpdated = false;

    float tbAngle = 0.0f;
    bool tbAngleUpdated = false;

    float steeringAngle = 0.0f;
    bool steeringAngleUpdated = false;

    float voltage = 0.0f;
    bool voltageUpdated = false;

    uint8_t tempUnknown2 = 0;
    bool tempUnknown2Updated = false;

    uint8_t tempUnknown3 = 0;
    bool tempUnknown3Updated = false;

    uint16_t engineLoad = 0;
    bool engineLoadUpdated = false;

    int8_t lambda2 = 0;
    bool lambda2Updated = false;
};

struct ComputedStats
{
    uint32_t elapsedSecondsSinceStart = 0;
    bool elapsedSecondsSinceStartUpdated = false;

    uint16_t elapsedKmSinceStart = 0;
    bool elapsedKmSinceStartUpdated = false;

    uint8_t fuelBurnedSinceStart = 0;
    bool fuelBurnedSinceStartUpdated = false;

    float fuelPer100km = 0.0f;
    bool fuelPer100kmUpdated = false;

    float fuelPerHour = 0.0f;
    bool fuelPerHourUpdated = false;

    uint16_t kmRemaining = 0;
    bool kmRemainingUpdated = false;
};

enum WarnBit : uint8_t
{
    WARN_OIL_PRES = 0,
    WARN_OIL_HOT = 1,
    WARN_COOL_HOT = 2,
    WARN_OIL_LVL = 3,
    WARN_LOW_VOLT = 4,
    WARN_FUEL_CRIT = 5,
    WARN_VERY_COLD = 6,
    WARN_HIGH_LOAD = 7,
    WARN_FUEL_LOW = 8,
    WARN_COLD_ENG = 9,
    WARN_OIL_LVL_LOW = 10,
    WARN_COUNT = 11,
};

struct WarningState
{
    uint16_t bits = 0;
    uint8_t maxLevel = 0;
    bool hasNew = false;
};

struct OBDSignals
{
    InstrumentSignals instruments;
    EngineSignals engine;
    ExperimentalGroup experimental;
    ComputedStats computed;
    WarningState warnings;

    void reset();

    // Populate a single measurement from KLineKWP1281Lib readGroup output
    void populateMeasurement(uint8_t block, uint8_t idx, double value, uint8_t ecuAddr);

    void compute(uint32_t nowMs, uint32_t connectTimeStart);
    void computeWarnings(uint8_t ecuAddr);
};

} // namespace Model
} // namespace obd
