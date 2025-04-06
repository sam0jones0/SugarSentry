#pragma once

#include <gmock/gmock.h>
#include <string>
#include "i_json_parser.h"

class MockJsonParser : public IJsonParser {
public:
    MOCK_METHOD(bool, parseJsonArray, (const std::string& jsonString, std::function<bool(ArduinoJson::JsonObjectConst)> elementProcessor), (override));
};
