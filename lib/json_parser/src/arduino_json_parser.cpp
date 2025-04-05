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
    auto doc = std::make_unique<JsonDocument>();
    DeserializationError error = deserializeJson(*doc, jsonString);

    if (error) {
        DEBUG_PRINT("Failed to parse JSON object: ");
        DEBUG_PRINT(error.c_str());
        return nullptr;
    }

    if (!doc->is<JsonObject>()) {
        DEBUG_PRINT("JSON is not an object");
        return nullptr;
    }

    return std::make_shared<ArduinoJsonValue>(*doc);
}

std::vector<std::shared_ptr<IJsonValue>> ArduinoJsonParser::parseArray(const std::string& jsonString) {
    std::vector<std::shared_ptr<IJsonValue>> result;
    auto arrayDoc = std::make_unique<JsonDocument>();
    DeserializationError error = deserializeJson(*arrayDoc, jsonString);

    if (error) {
        DEBUG_PRINT("Failed to parse JSON array: ");
        DEBUG_PRINT(error.c_str());
        return result;
    }

    if (!arrayDoc->is<JsonArray>()) {
        DEBUG_PRINT("JSON is not an array");
        return result;
    }

    JsonArrayConst array = arrayDoc->as<JsonArrayConst>();
    for (JsonVariantConst item : array) {
        auto itemDoc = std::make_unique<JsonDocument>();
        itemDoc->to<JsonObject>().set(item);
        result.push_back(std::make_shared<ArduinoJsonValue>(*itemDoc));
    }

    return result;
}

bool ArduinoJsonParser::parseJsonArray(const std::string& jsonString, 
                                     std::function<bool(ArduinoJson::JsonObjectConst)> elementProcessor) {
    JsonDocument doc;
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
