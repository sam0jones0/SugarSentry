#ifndef ARDUINO_JSON_PARSER_H
#define ARDUINO_JSON_PARSER_H

#include "i_json_value.h"
#include <ArduinoJson.h>

/**
 * @brief ArduinoJson implementation of IJsonValue
 */
class ArduinoJsonValue : public IJsonValue {
private:
    JsonObjectConst _obj;

public:
    explicit ArduinoJsonValue(JsonObjectConst obj) : _obj(obj) {}

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
    std::shared_ptr<IJsonValue> parseObject(const std::string& jsonString) override;
    std::vector<std::shared_ptr<IJsonValue>> parseArray(const std::string& jsonString) override;

private:
    static constexpr size_t JSON_BUFFER_SIZE = 2048;  // Adjust size as needed
};

#endif // ARDUINO_JSON_PARSER_H
