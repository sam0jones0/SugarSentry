#ifndef GLUCOSE_READING_H
#define GLUCOSE_READING_H

#include <cstdint>
#include <ctime>
#include "dexcom_constants.h"
#include "dexcom_utils.h"

/**
 * @file glucose_reading.h
 * @brief Defines the GlucoseReading class representing a single glucose measurement.
 */

class GlucoseReading
{
public:
    /**
     * @brief Constructs a GlucoseReading from parsed values.
     *
     * @param value The glucose value
     * @param trend The trend direction as a string
     * @param timestamp The timestamp string in Dexcom format (e.g., "Date(1234567890)")
     */
    GlucoseReading(uint16_t value, const std::string& trend, const std::string& timestamp)
        : _value(value),
          _trend(DexcomUtils::stringToTrendDirection(trend.c_str()))
    {
        if (!timestamp.empty() && timestamp[0] == 'D') {
            _timestamp = strtoull(timestamp.c_str() + 5, nullptr, 10) / 1000; // milliseconds to seconds
        } else {
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
