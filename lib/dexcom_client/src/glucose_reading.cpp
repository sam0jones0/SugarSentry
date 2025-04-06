#include "glucose_reading.h"
#include <stdexcept>
#include <ArduinoJson.h>

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

GlucoseReading::GlucoseReading(ArduinoJson::JsonObjectConst obj)
{
    // Check and extract Value field
    if (obj.containsKey("Value") && obj["Value"].is<int>()) {
        _value = static_cast<uint16_t>(obj["Value"].as<int>());
    } else {
        throw std::runtime_error("Missing or invalid 'Value' field in JSON object");
    }

    // Check and extract Trend field
    if (obj.containsKey("Trend") && obj["Trend"].is<const char*>()) {
        const char* trendStr = obj["Trend"].as<const char*>();
        _trend = DexcomUtils::stringToTrendDirection(trendStr);
    } else {
        throw std::runtime_error("Missing or invalid 'Trend' field in JSON object");
    }

    // Check and extract WT (timestamp) field
    if (obj.containsKey("WT") && obj["WT"].is<const char*>()) {
        const char* timestampStr = obj["WT"].as<const char*>();
        if (timestampStr && timestampStr[0] == 'D') {
            _timestamp = strtoull(timestampStr + 5, nullptr, 10) / 1000; // milliseconds to seconds
        } else {
            _timestamp = 0;
        }
    } else {
        throw std::runtime_error("Missing or invalid 'WT' field in JSON object");
    }
}
