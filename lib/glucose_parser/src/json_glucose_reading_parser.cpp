#include "json_glucose_reading_parser.h"

std::vector<GlucoseReading> JsonGlucoseReadingParser::parse(const std::string &response)
{
    std::vector<GlucoseReading> readings;
    readings.reserve(DexcomConst::MAX_MAX_COUNT);

    DEBUG_PRINT("Parsing glucose readings. Raw response:");
    DEBUG_PRINT(response.c_str());

    size_t pos = 0;
    while (pos < response.length())
    {
        size_t start = response.find('{', pos);
        if (start == std::string::npos)
        {
            DEBUG_PRINT("No more JSON objects found in response");
            break;
        }

        size_t end = response.find('}', start);
        if (end == std::string::npos)
        {
            DEBUG_PRINT("Incomplete JSON object found in response");
            break;
        }

        std::string jsonObject = response.substr(start, end - start + 1);
        DEBUG_PRINT("Parsing JSON object: ");
        DEBUG_PRINT(jsonObject.c_str());

        auto reading = parseJsonObject(jsonObject);
        if (reading)
        {
            readings.push_back(*reading);
            DEBUG_PRINT("Successfully parsed glucose reading");
        }

        pos = end + 1;
    }

    DEBUG_PRINT("Total glucose readings parsed: ");
    DEBUG_PRINTF("%d\n", readings.size());

    return readings;
}

std::optional<GlucoseReading> JsonGlucoseReadingParser::parseJsonObject(const std::string &jsonObject)
{
    StaticJsonDocument<DexcomConst::MAX_READING_JSON_SIZE> doc;
    DeserializationError err = deserializeJson(doc, jsonObject);

    if (err == DeserializationError::Ok)
    {
        return GlucoseReading(doc.as<JsonObjectConst>());
    }
    else
    {
        DEBUG_PRINT("Failed to parse JSON object: ");
        DEBUG_PRINT(err.c_str());
        return std::nullopt;
    }
}