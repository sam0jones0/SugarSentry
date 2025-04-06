#pragma once

#include <gmock/gmock.h>
#include <vector>
#include <memory>
#include <string>
#include "i_json_parser.h"
#include "i_json_value.h"

class MockJsonParser : public IJsonParser {
public:
    MOCK_METHOD(std::shared_ptr<IJsonValue>, parseObject, (const std::string& jsonString), (override));
    MOCK_METHOD(std::vector<std::shared_ptr<IJsonValue>>, parseArray, (const std::string& jsonString), (override));
    MOCK_METHOD(bool, parseJsonArray, (const std::string& jsonString, std::function<bool(ArduinoJson::JsonObjectConst)> elementProcessor), (override));
};
