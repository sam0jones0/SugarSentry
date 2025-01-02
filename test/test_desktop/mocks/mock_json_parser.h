#pragma once

#include "../../lib/json_parser/include/i_json_value.h"
#include "mock_json_value.h"
#include <memory>
#include <vector>
#include <string>

class MockJsonParser : public IJsonParser {
public:
    std::shared_ptr<IJsonValue> parseObject(const std::string& jsonString) override {
        if (jsonString == "invalid") {
            return nullptr;
        }
        return std::make_shared<MockJsonValue>();
    }

    std::vector<std::shared_ptr<IJsonValue>> parseArray(const std::string& jsonString) override {
        if (jsonString == "[]" || jsonString == "invalid") {
            return {};
        }

        std::vector<std::shared_ptr<IJsonValue>> result;
        result.push_back(std::make_shared<MockJsonValue>());
        result.push_back(std::make_shared<MockJsonValue>());
        return result;
    }
};
