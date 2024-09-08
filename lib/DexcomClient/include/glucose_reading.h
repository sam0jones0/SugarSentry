#ifndef GLUCOSE_READING_H
#define GLUCOSE_READING_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ctime>
#include "dexcom_constants.h"
#include "dexcom_errors.h"

class GlucoseReading {
public:
    explicit GlucoseReading(const JsonObjectConst& jsonGlucoseReading);

    int getValue() const noexcept { return _value; }
    int getMgDl() const noexcept { return _value; }
    float getMmolL() const noexcept { return _value * DexcomConst::MMOL_L_CONVERSION_FACTOR; }
    DexcomConst::TrendDirection getTrend() const noexcept { return _trend; }
    const char* getTrendDirection() const noexcept { return DexcomConst::TREND_DIRECTION_STRINGS[static_cast<int>(_trend)]; }
    const char* getTrendDescription() const noexcept { return DexcomConst::TREND_DESCRIPTIONS[static_cast<int>(_trend)]; }
    const char* getTrendArrow() const noexcept { return DexcomConst::TREND_ARROWS[static_cast<int>(_trend)]; }
    time_t getDateTime() const noexcept { return _datetime; }

    const char* toString() const noexcept { return String(_value).c_str(); }

private:
    int _value;
    DexcomConst::TrendDirection _trend;
    time_t _datetime;
};

#endif // GLUCOSE_READING_H