#include "glucose_reading.h"
#include <stdexcept>

GlucoseReading::GlucoseReading(uint16_t value, const std::string& trend, const std::string& timestamp)
    : _value(value),
      _trend(DexcomUtils::stringToTrendDirection(trend.c_str()))
{
    if (!timestamp.empty() && timestamp[0] == 'D') {
        _timestamp = strtoull(timestamp.c_str() + 5, nullptr, 10) / 1000; // milliseconds to seconds
    } else {
        _timestamp = 0;
    }
}

GlucoseReading::GlucoseReading(const IJsonValue& json)
{
    auto value = json.getInt("Value");
    if (!value) {
        throw std::runtime_error("Missing or invalid 'Value' field in glucose reading");
    }
    _value = static_cast<uint16_t>(*value);

    auto trend = json.getString("Trend");
    if (!trend) {
        throw std::runtime_error("Missing or invalid 'Trend' field in glucose reading");
    }
    _trend = DexcomUtils::stringToTrendDirection(trend->c_str());

    auto timestamp = json.getString("WT");
    if (!timestamp) {
        throw std::runtime_error("Missing or invalid 'WT' field in glucose reading");
    }
    if (!timestamp->empty() && (*timestamp)[0] == 'D') {
        _timestamp = strtoull(timestamp->c_str() + 5, nullptr, 10) / 1000; // milliseconds to seconds
    } else {
        _timestamp = 0;
    }
}
