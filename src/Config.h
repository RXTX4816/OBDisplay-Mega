// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>

// ── Firmware version (injected by tools/inject_version.py at build time) ───
#ifndef APP_VERSION
#define APP_VERSION "dev"
#endif

// ── KWP serial interface ────────────────────────────────────────────────────
#define KWP_SERIAL Serial3
#define TX_PIN 14
#define IS_FULL_DUPLEX false

// ── Button pins (7-button joystick, active LOW / INPUT_PULLUP) ──────────────
#define PIN_RST 13
#define PIN_SET 2
#define PIN_MID 3
#define PIN_RIGHT 4
#define PIN_LEFT 5
#define PIN_DOWN 6
#define PIN_UP 7

// ── Task intervals (milliseconds) ──────────────────────────────────────────
#define INTERVAL_KWP_MS 0
#define INTERVAL_INPUT_MS 20
#define INTERVAL_COMPUTE_MS 50
#define INTERVAL_DISPLAY_MS 40

// ── KWP timing ──────────────────────────────────────────────────────────────
#define ECU_TIMEOUT_MS 1300
#define RECONNECT_DELAY_MS 5000

// ── ECU addresses ───────────────────────────────────────────────────────────
#define ADDR_ENGINE 0x01
#define ADDR_ABS 0x03
#define ADDR_HVAC 0x08
#define ADDR_INSTRUMENTS 0x17
#define ADDR_CENTRAL_CONV 0x46

// ── Default startup values ──────────────────────────────────────────────────
#define DEFAULT_ECU_ADDR ADDR_INSTRUMENTS
#define DEFAULT_BAUD_RATE 10400UL
#define DEFAULT_NIGHT_MODE true

// ── Warning thresholds ──────────────────────────────────────────────────────
#define WARN_OIL_TEMP_HIGH_C 93
#define WARN_COOLANT_HIGH_C 93
#define WARN_COOLANT_COLD_C 40
#define WARN_COOLANT_WARM_C 75
#define WARN_FUEL_CRIT_L 4
#define WARN_FUEL_LOW_L 8
#define WARN_OIL_LVL_CRIT_RAW 51
#define WARN_OIL_LVL_LOW_RAW 115
#define WARN_VOLTAGE_LOW_V 12.0f
#define WARN_ENGINE_LOAD_HIGH 90

// ── Supported baud rates ────────────────────────────────────────────────────
static constexpr uint8_t SUPPORTED_BAUD_COUNT = 5;
static constexpr uint32_t SUPPORTED_BAUD_RATES[SUPPORTED_BAUD_COUNT] = {1200, 2400, 4800, 9600,
                                                                        10400};

// ── ECU address table ───────────────────────────────────────────────────────
static constexpr uint8_t ECU_ADDR_COUNT = 5;
static constexpr uint8_t ECU_ADDRS[ECU_ADDR_COUNT] = {ADDR_ENGINE, ADDR_ABS, ADDR_HVAC,
                                                      ADDR_INSTRUMENTS, ADDR_CENTRAL_CONV};

// ── EEPROM layout ───────────────────────────────────────────────────────────
#define EEPROM_ADDR_BAUD 0   // uint8_t index into SUPPORTED_BAUD_RATES
#define EEPROM_ADDR_ECU 1    // uint8_t index into ECU_ADDRS
#define EEPROM_ADDR_NIGHT 2  // uint8_t (0/1)
#define EEPROM_ADDR_FUEL_L 3 // uint8_t fuel start level in litres
