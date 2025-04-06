#ifndef ARDUINO_JSON_PARSER_H
#define ARDUINO_JSON_PARSER_H

#include "i_json_parser.h"
#include "../../dexcom_client/include/dexcom_constants.h"
#include <ArduinoJson.h>

/**
 * @brief ArduinoJson implementation of IJsonParser
 */
class ArduinoJsonParser : public IJsonParser {
public:
    bool parseJsonArray(const std::string& jsonString, 
                       std::function<bool(ArduinoJson::JsonObjectConst)> elementProcessor) override;

private:
    // Calculate buffer size based on max readings data
    // (MAX_MAX_COUNT * MAX_READING_JSON_SIZE) + overhead
    static constexpr size_t JSON_BUFFER_SIZE = (DexcomConst::MAX_MAX_COUNT * DexcomConst::MAX_READING_JSON_SIZE) + 4096; // ~76KB
    using JsonDocument = StaticJsonDocument<JSON_BUFFER_SIZE>;
};

#endif // ARDUINO_JSON_PARSER_H
