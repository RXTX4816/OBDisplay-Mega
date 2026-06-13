// SPDX-License-Identifier: GPL-3.0-or-later
#include "OBDSignals.h"
#include "../../Config.h"

namespace obd
{
namespace Model
{

// ── ExperimentalGroup ──────────────────────────────────────────────────────

void ExperimentalGroup::reset()
{
    for (uint8_t i = 0; i < 4; ++i)
    {
        k[i] = 0;
        v[i] = 0.0f;
        unit[i][0] = 'N';
        unit[i][1] = '/';
        unit[i][2] = 'A';
        unit[i][3] = '\0';
        for (uint8_t j = 4; j < UnitWidth + 1; ++j)
            unit[i][j] = '\0';
    }
    kUpdated = vUpdated = unitUpdated = false;
    groupSide = false;
    groupSideUpdated = false;
}

void ExperimentalGroup::invertGroupSide()
{
    groupSide = !groupSide;
    groupSideUpdated = true;
}

// ── OBDSignals::reset ──────────────────────────────────────────────────────

void OBDSignals::reset()
{
    instruments = InstrumentSignals{};
    engine = EngineSignals{};
    experimental.reset();
    computed = ComputedStats{};
    warnings = WarningState{};
}

// ── populateMeasurement ────────────────────────────────────────────────────
// Called once per KLineKWP1281Lib measurement after a successful readGroup().
// block/idx come from the readGroup loop; value is getMeasurementValue().

void OBDSignals::populateMeasurement(uint8_t block, uint8_t idx, double value, uint8_t ecuAddr)
{
    if (ecuAddr == ADDR_INSTRUMENTS) // 0x17
    {
        switch (block)
        {
            case 1:
                switch (idx)
                {
                    case 0:
                        instruments.vehicleSpeed = (uint16_t)value;
                        instruments.vehicleSpeedUpdated = true;
                        break;
                    case 1:
                        instruments.engineRpm = (uint16_t)value;
                        instruments.engineRpmUpdated = true;
                        break;
                    case 2:
                        instruments.oilPressureMin = (uint16_t)value;
                        instruments.oilPressureMinUpdated = true;
                        break;
                    case 3:
                        instruments.timeEcu = (uint32_t)value;
                        instruments.timeEcuUpdated = true;
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                switch (idx)
                {
                    case 0:
                        instruments.odometer = (uint32_t)value;
                        instruments.odometerUpdated = true;
                        break;
                    case 1:
                        instruments.fuelLevel = (uint8_t)value;
                        instruments.fuelLevelUpdated = true;
                        break;
                    case 2:
                        instruments.fuelSensorResistance = (uint16_t)value;
                        instruments.fuelSensorResistanceUpdated = true;
                        break;
                    case 3:
                        instruments.ambientTemp = (int8_t)value;
                        instruments.ambientTempUpdated = true;
                        break;
                    default:
                        break;
                }
                break;
            case 3:
                switch (idx)
                {
                    case 0:
                        instruments.coolantTemp = (uint8_t)value;
                        instruments.coolantTempUpdated = true;
                        break;
                    case 1:
                        instruments.oilLevelOk = (uint8_t)value;
                        instruments.oilLevelOkUpdated = true;
                        break;
                    case 2:
                        instruments.oilTemp = (uint8_t)value;
                        instruments.oilTempUpdated = true;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    else if (ecuAddr == ADDR_ENGINE) // 0x01
    {
        switch (block)
        {
            case 1:
                switch (idx)
                {
                    case 0:
                        instruments.engineRpm = (uint16_t)value;
                        instruments.engineRpmUpdated = true;
                        break;
                    case 1:
                        engine.tempUnknown1 = (uint8_t)value;
                        engine.tempUnknown1Updated = true;
                        break;
                    case 2:
                        engine.lambda = (int8_t)value;
                        engine.lambdaUpdated = true;
                        break;
                    case 3:
                    {
                        // Bit-encoded error flags
                        uint8_t bits = (uint8_t)value;
                        engine.exhaustGasRecirculationError = (bits >> 0) & 1;
                        engine.oxygenSensorHeatingError = (bits >> 1) & 1;
                        engine.oxygenSensorError = (bits >> 2) & 1;
                        engine.airConditioningError = (bits >> 3) & 1;
                        engine.secondaryAirInjectionError = (bits >> 4) & 1;
                        engine.evaporativeEmissionsError = (bits >> 5) & 1;
                        engine.catalystHeatingError = (bits >> 6) & 1;
                        engine.catalyticConverter = (bits >> 7) & 1;
                        engine.errorBitsUpdated = true;
                        break;
                    }
                    default:
                        break;
                }
                break;
            case 3:
                switch (idx)
                {
                    case 1:
                        engine.pressure = (uint16_t)value;
                        engine.pressureUpdated = true;
                        break;
                    case 2:
                        engine.tbAngle = (float)value;
                        engine.tbAngleUpdated = true;
                        break;
                    case 3:
                        engine.steeringAngle = (float)value;
                        engine.steeringAngleUpdated = true;
                        break;
                    default:
                        break;
                }
                break;
            case 4:
                switch (idx)
                {
                    case 1:
                        engine.voltage = (float)value;
                        engine.voltageUpdated = true;
                        break;
                    case 2:
                        engine.tempUnknown2 = (uint8_t)value;
                        engine.tempUnknown2Updated = true;
                        break;
                    case 3:
                        engine.tempUnknown3 = (uint8_t)value;
                        engine.tempUnknown3Updated = true;
                        break;
                    default:
                        break;
                }
                break;
            case 6:
                switch (idx)
                {
                    case 1:
                        engine.engineLoad = (uint16_t)value;
                        engine.engineLoadUpdated = true;
                        break;
                    case 3:
                        engine.lambda2 = (int8_t)value;
                        engine.lambda2Updated = true;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

// ── compute ────────────────────────────────────────────────────────────────

void OBDSignals::compute(uint32_t nowMs, uint32_t connectTimeStart)
{
    computed.elapsedSecondsSinceStart = (nowMs - connectTimeStart) / 1000UL;
    computed.elapsedSecondsSinceStartUpdated = true;

    uint32_t distKm = abs((int32_t)instruments.odometer - (int32_t)instruments.odometerStart);
    computed.elapsedKmSinceStart = (uint16_t)(distKm > 9999 ? 9999 : distKm);
    computed.elapsedKmSinceStartUpdated = true;

    uint8_t burned = (instruments.fuelLevel < instruments.fuelLevelStart)
                         ? (instruments.fuelLevelStart - instruments.fuelLevel)
                         : 0;
    computed.fuelBurnedSinceStart = burned;
    computed.fuelBurnedSinceStartUpdated = true;

    uint16_t km = computed.elapsedKmSinceStart;
    if (km > 0)
        computed.fuelPer100km = (100.0f / (float)km) * (float)burned;
    else
        computed.fuelPer100km = 0.0f;
    computed.fuelPer100kmUpdated = true;

    uint32_t secs = computed.elapsedSecondsSinceStart;
    computed.fuelPerHour = (secs > 0) ? ((float)burned * 3600.0f / (float)secs) : 0.0f;
    computed.fuelPerHourUpdated = true;

    if (computed.fuelPer100km > 0.0f)
    {
        uint32_t kmR = (uint32_t)((float)instruments.fuelLevel * 100.0f / computed.fuelPer100km);
        computed.kmRemaining = (uint16_t)(kmR > 9999 ? 9999 : kmR);
    }
    else
    {
        computed.kmRemaining = 0;
    }
    computed.kmRemainingUpdated = true;
}

// ── computeWarnings ────────────────────────────────────────────────────────

static inline void setWarn(WarningState& w, WarnBit bit, uint8_t level)
{
    w.bits |= (uint16_t)(1u << bit);
    if (level > w.maxLevel)
        w.maxLevel = level;
}

void OBDSignals::computeWarnings(uint8_t ecuAddr)
{
    uint16_t prevBits = warnings.bits;
    warnings.bits = 0;
    warnings.maxLevel = 0;

    if (ecuAddr == ADDR_INSTRUMENTS)
    {
        if (instruments.oilPressureMinUpdated)
        {
            if (instruments.oilPressureMin != 31)
            {
                if (instruments.oilPressureLowCount < 3)
                    instruments.oilPressureLowCount++;
            }
            else
            {
                if (instruments.oilPressureLowCount > 0)
                    instruments.oilPressureLowCount--;
            }
        }
        if (instruments.oilPressureLowCount >= 3)
            setWarn(warnings, WARN_OIL_PRES, 3);
        if (instruments.oilTempUpdated && instruments.oilTemp > WARN_OIL_TEMP_HIGH_C)
            setWarn(warnings, WARN_OIL_HOT, 3);
        if (instruments.coolantTempUpdated)
        {
            uint8_t ct = instruments.coolantTemp;
            if (ct > WARN_COOLANT_HIGH_C)
                setWarn(warnings, WARN_COOL_HOT, 3);
            if (ct < WARN_COOLANT_COLD_C)
                setWarn(warnings, WARN_VERY_COLD, 2);
            if (ct < WARN_COOLANT_WARM_C)
                setWarn(warnings, WARN_COLD_ENG, 1);
        }
        if (instruments.oilLevelOkUpdated)
        {
            uint8_t ol = instruments.oilLevelOk;
            if (ol < WARN_OIL_LVL_CRIT_RAW)
                setWarn(warnings, WARN_OIL_LVL, 3);
            if (ol < WARN_OIL_LVL_LOW_RAW)
                setWarn(warnings, WARN_OIL_LVL_LOW, 1);
        }
        if (instruments.fuelLevelUpdated)
        {
            uint8_t fl = instruments.fuelLevel;
            if (fl < WARN_FUEL_CRIT_L)
                setWarn(warnings, WARN_FUEL_CRIT, 2);
            if (fl < WARN_FUEL_LOW_L)
                setWarn(warnings, WARN_FUEL_LOW, 1);
        }
    }
    else if (ecuAddr == ADDR_ENGINE)
    {
        if (engine.voltageUpdated && engine.voltage < WARN_VOLTAGE_LOW_V)
            setWarn(warnings, WARN_LOW_VOLT, 2);
        if (engine.engineLoadUpdated && engine.engineLoad > WARN_ENGINE_LOAD_HIGH)
            setWarn(warnings, WARN_HIGH_LOAD, 1);
        if (engine.tempUnknown2Updated)
        {
            uint8_t t2 = engine.tempUnknown2;
            if (t2 > WARN_COOLANT_HIGH_C)
                setWarn(warnings, WARN_COOL_HOT, 3);
            if (t2 < WARN_COOLANT_COLD_C)
                setWarn(warnings, WARN_VERY_COLD, 2);
            if (t2 < WARN_COOLANT_WARM_C)
                setWarn(warnings, WARN_COLD_ENG, 1);
        }
    }

    if (warnings.bits != prevBits && (warnings.bits & ~prevBits) != 0)
        warnings.hasNew = true;
}

} // namespace Model
} // namespace obd
