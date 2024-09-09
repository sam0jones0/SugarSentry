#ifndef GLUCOSE_READING_H
#define GLUCOSE_READING_H

#include <cstdint>
#include <ctime>
#include <ArduinoJson.h>
#include "dexcom_constants.h"

class GlucoseReading
{
public:
    explicit GlucoseReading(const JsonObjectConst &jsonGlucoseReading)
    {
        _value = static_cast<uint16_t>(jsonGlucoseReading["Value"].as<int>());
        _trend = DexcomConst::stringToTrendDirection(jsonGlucoseReading["Trend"].as<const char *>());

        const char *wtStr = jsonGlucoseReading["WT"].as<const char *>();
        if (wtStr && *wtStr == 'D')
        {                                                         
            _timestamp = strtoull(wtStr + 5, nullptr, 10) / 1000; // milliseconds to seconds
        }
        else
        {
            _timestamp = 0;
        }
    }

    uint16_t getValue() const noexcept { return _value; }
    uint16_t getMgDl() const noexcept { return _value; }
    float getMmolL() const noexcept { return _value * DexcomConst::MMOL_L_CONVERSION_FACTOR; }
    DexcomConst::TrendDirection getTrend() const noexcept { return _trend; }
    const char *getTrendDirection() const noexcept { return DexcomConst::TREND_DIRECTION_STRINGS[static_cast<int>(_trend)]; }
    const char *getTrendDescription() const noexcept { return DexcomConst::TREND_DESCRIPTIONS[static_cast<int>(_trend)]; }
    const char *getTrendArrow() const noexcept { return DexcomConst::TREND_ARROWS[static_cast<int>(_trend)]; }
    time_t getTimestamp() const noexcept { return _timestamp; }

private:
    uint16_t _value;
    DexcomConst::TrendDirection _trend;
    time_t _timestamp;
};

#endif // GLUCOSE_READING_H