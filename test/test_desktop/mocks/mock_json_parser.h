#pragma once

#include <gmock/gmock.h>
#include "../../lib/json_parser/include/i_json_value.h"
#include "mock_json_value.h"
#include <memory>
#include <vector>
#include <string>

class MockJsonParser : public IJsonParser {
public:
    MOCK_METHOD(std::shared_ptr<IJsonValue>, parseObject, (const std::string& jsonString), (override));
    MOCK_METHOD(std::vector<std::shared_ptr<IJsonValue>>, parseArray, (const std::string& jsonString), (override));

    // Helper method to set default behaviors
    void SetDefaultBehaviors() {
        ON_CALL(*this, parseObject)
            .WillByDefault([this](const std::string& jsonString) {
                if (jsonString == "invalid") {
                    return std::shared_ptr<IJsonValue>(nullptr);
                }
                return CreateMockJsonValue();
            });

        ON_CALL(*this, parseArray)
            .WillByDefault([this](const std::string& jsonString) {
                if (jsonString == "[]" || jsonString == "invalid") {
                    return std::vector<std::shared_ptr<IJsonValue>>();
                }

                std::vector<std::shared_ptr<IJsonValue>> result;
                result.push_back(CreateMockJsonValue());
                result.push_back(CreateMockJsonValue());
                return result;
            });
    }

private:
    // Helper to create a mock IJsonValue with default values set up
    std::shared_ptr<IJsonValue> CreateMockJsonValue() {
        auto mockValue = std::make_shared<testing::NiceMock<MockJsonValue>>();
        
        ON_CALL(*mockValue, getInt("Value"))
            .WillByDefault(testing::Return(120));
        ON_CALL(*mockValue, getString("Trend"))
            .WillByDefault(testing::Return("Flat"));
        ON_CALL(*mockValue, getString("WT"))
            .WillByDefault(testing::Return("Date(1609459200000)"));
        
        return mockValue;
    }
};
