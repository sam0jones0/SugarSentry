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

    bool parseSuccess = _jsonParser->parseJsonArray(response, 
        [&](ArduinoJson::JsonObjectConst obj) -> bool {
            // Check if we have already reached the maximum number of readings
            if (readings.size() >= DexcomConst::MAX_MAX_COUNT) {
                DEBUG_PRINT("Reached MAX_MAX_COUNT limit, stopping parse.");
                return false; // Stop processing further elements
            }

            // Try to construct a GlucoseReading from the JSON object
            try {
                // Use emplace_back for potentially slightly better efficiency
                readings.emplace_back(obj); // Calls the new constructor GlucoseReading(JsonObjectConst)
            } catch (const std::exception& e) {
                // Log the error but continue processing other elements
                DEBUG_PRINT("Skipping invalid glucose reading object: ");
                DEBUG_PRINT(e.what());
            }

            return true; // Continue processing the next element
        }
    );

    if (!parseSuccess) {
        DEBUG_PRINT("Failed to parse JSON array");
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
