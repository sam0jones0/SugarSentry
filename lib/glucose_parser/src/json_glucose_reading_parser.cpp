#include "json_glucose_reading_parser.h"

JsonGlucoseReadingParser::JsonGlucoseReadingParser(std::shared_ptr<IJsonParser> jsonParser)
    : _jsonParser(std::move(jsonParser))
{
}

std::vector<GlucoseReading> JsonGlucoseReadingParser::parse(const std::string &response)
{
    std::vector<GlucoseReading> readings;
    readings.reserve(DexcomConst::MAX_MAX_COUNT);

    DEBUG_PRINT("Parsing glucose readings. Raw response:");
    DEBUG_PRINT(response.c_str());

    auto jsonArray = _jsonParser->parseArray(response);
    if (jsonArray.empty()) {
        DEBUG_PRINT("Failed to parse JSON array");
        return readings;
    }

    for (const auto& jsonValue : jsonArray) {
        // Stop processing if we've reached the maximum number of readings
        if (readings.size() >= DexcomConst::MAX_MAX_COUNT) {
            break;
        }
        
        if (!jsonValue) continue;
        
        try {
            GlucoseReading reading(*jsonValue);
            readings.push_back(reading);
        } catch (const std::exception& e) {
            DEBUG_PRINT("Failed to create GlucoseReading: ");
            DEBUG_PRINT(e.what());
            continue;
        }
    }

    DEBUG_PRINT("Total glucose readings parsed: ");
    DEBUG_PRINTF("%d\n", readings.size());

    return readings;
}

std::optional<GlucoseReading> JsonGlucoseReadingParser::parseJsonObject(const std::string &jsonObject)
{
    auto obj = _jsonParser->parseObject(jsonObject);
    if (!obj) {
        DEBUG_PRINT("Failed to parse JSON object");
        return std::nullopt;
    }

    try {
        return GlucoseReading(*obj);
    } catch (const std::exception& e) {
        DEBUG_PRINT("Failed to create GlucoseReading: ");
        DEBUG_PRINT(e.what());
        return std::nullopt;
    }
}
