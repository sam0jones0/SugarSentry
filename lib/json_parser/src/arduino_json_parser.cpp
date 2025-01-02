#include "arduino_json_parser.h"
#include "debug_print.h"

std::optional<std::string> ArduinoJsonValue::getString(const std::string& key) const {
    if (!_obj.containsKey(key) || !_obj[key].is<const char*>()) {
        return std::nullopt;
    }
    return std::string(_obj[key].as<const char*>());
}

std::optional<int> ArduinoJsonValue::getInt(const std::string& key) const {
    if (!_obj.containsKey(key) || !_obj[key].is<int>()) {
        return std::nullopt;
    }
    return _obj[key].as<int>();
}

std::optional<double> ArduinoJsonValue::getDouble(const std::string& key) const {
    if (!_obj.containsKey(key) || !_obj[key].is<double>()) {
        return std::nullopt;
    }
    return _obj[key].as<double>();
}

std::optional<bool> ArduinoJsonValue::getBool(const std::string& key) const {
    if (!_obj.containsKey(key) || !_obj[key].is<bool>()) {
        return std::nullopt;
    }
    return _obj[key].as<bool>();
}

std::shared_ptr<IJsonValue> ArduinoJsonParser::parseObject(const std::string& jsonString) {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        DEBUG_PRINT("Failed to parse JSON object: ");
        DEBUG_PRINT(error.c_str());
        return nullptr;
    }

    JsonObjectConst obj = doc.as<JsonObjectConst>();
    return std::make_shared<ArduinoJsonValue>(obj);
}

std::vector<std::shared_ptr<IJsonValue>> ArduinoJsonParser::parseArray(const std::string& jsonString) {
    std::vector<std::shared_ptr<IJsonValue>> result;
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        DEBUG_PRINT("Failed to parse JSON array: ");
        DEBUG_PRINT(error.c_str());
        return result;
    }

    JsonArrayConst array = doc.as<JsonArrayConst>();
    for (JsonObjectConst obj : array) {
        result.push_back(std::make_shared<ArduinoJsonValue>(obj));
    }

    return result;
}
