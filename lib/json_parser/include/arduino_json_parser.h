#ifndef ARDUINO_JSON_PARSER_H
#define ARDUINO_JSON_PARSER_H

#include <ArduinoJson.h>
#include <memory>
#include "i_json_parser.h"

/**
 * @brief ArduinoJson implementation of IJsonParser
 */
class ArduinoJsonParser : public IJsonParser {
public:
    std::optional<GlucoseReading> parseGlucoseReading(const std::string& jsonString) override;
    std::vector<GlucoseReading> parseGlucoseReadings(const std::string& jsonString) override;
    std::unique_ptr<DexcomError> parseErrorResponse(const std::string& jsonString) override;

private:
    static constexpr size_t ERROR_JSON_SIZE = 512;
    static constexpr size_t GLUCOSE_JSON_SIZE = 256;
    static constexpr size_t GLUCOSE_ARRAY_SIZE = 32768; // Increased size for array parsing
};

#endif // ARDUINO_JSON_PARSER_H
