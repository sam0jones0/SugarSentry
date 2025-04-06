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

bool ArduinoJsonParser::parseJsonArray(const std::string& jsonString, 
                                     std::function<bool(ArduinoJson::JsonObjectConst)> elementProcessor) {
    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        DEBUG_PRINT("Failed to parse JSON array: ");
        DEBUG_PRINT(error.c_str());
        return false;
    }

    if (!doc.is<JsonArrayConst>()) {
        DEBUG_PRINT("JSON is not an array");
        return false;
    }

    JsonArrayConst array = doc.as<JsonArrayConst>();
    for (JsonVariantConst item : array) {
        if (item.is<JsonObjectConst>()) {
            bool shouldContinue = elementProcessor(item.as<JsonObjectConst>());
            if (!shouldContinue) {
                break; // Processor requested to stop early
            }
        } else {
            // If the element is not an object, skip it but log a warning
            DEBUG_PRINT("Skipping non-object element in array");
        }
    }
    
    return true;
}
