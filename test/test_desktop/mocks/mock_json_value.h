#pragma once

#include <gmock/gmock.h>
#include <optional>
#include <string>
#include "i_json_value.h"

class MockJsonValue : public IJsonValue {
public:
    MOCK_METHOD(std::optional<std::string>, getString, (const std::string& key), (const, override));
    MOCK_METHOD(std::optional<int>, getInt, (const std::string& key), (const, override));
    MOCK_METHOD(std::optional<double>, getDouble, (const std::string& key), (const, override));
    MOCK_METHOD(std::optional<bool>, getBool, (const std::string& key), (const, override));
};
