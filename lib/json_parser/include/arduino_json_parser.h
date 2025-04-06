#ifndef ARDUINO_JSON_PARSER_H
#define ARDUINO_JSON_PARSER_H

#include "i_json_value.h"
#include "i_json_parser.h"
#include "../../dexcom_client/include/dexcom_constants.h"
#include <ArduinoJson.h>

/**
 * @brief ArduinoJson implementation of IJsonValue
 */
class ArduinoJsonValue : public IJsonValue {
private:
    StaticJsonDocument<2048> _doc;  // Own the document
    JsonObjectConst _obj;           // Reference to document's data

public:
    explicit ArduinoJsonValue(const StaticJsonDocument<2048>& doc) 
        : _doc(doc)  // Copy the document
        , _obj(_doc.as<JsonObjectConst>())  // Get reference from our copy
    {}

    std::optional<std::string> getString(const std::string& key) const override;
    std::optional<int> getInt(const std::string& key) const override;
    std::optional<double> getDouble(const std::string& key) const override;
    std::optional<bool> getBool(const std::string& key) const override;
};

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
