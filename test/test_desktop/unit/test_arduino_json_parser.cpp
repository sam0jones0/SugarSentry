#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "arduino_json_parser.h"
#include <memory>
#include <functional>
#include <ArduinoJson.h>

class ArduinoJsonParserTest : public ::testing::Test {
protected:
    std::unique_ptr<ArduinoJsonParser> parser;

    void SetUp() override {
        parser = std::make_unique<ArduinoJsonParser>();
    }

    void TearDown() override {
        parser.reset();
    }
};

TEST_F(ArduinoJsonParserTest, ParseArray) {
    const char* json = "[{\"name\":\"John\"},{\"name\":\"Jane\"}]";
    auto results = parser->parseArray(json);
    EXPECT_EQ(2, results.size());
    
    auto name1 = results[0]->getString("name");
    EXPECT_TRUE(name1.has_value());
    EXPECT_STREQ("John", name1.value().c_str());
    
    auto name2 = results[1]->getString("name");
    EXPECT_TRUE(name2.has_value());
    EXPECT_STREQ("Jane", name2.value().c_str());
}

TEST_F(ArduinoJsonParserTest, ParseEmptyArray) {
    const char* json = "[]";
    auto results = parser->parseArray(json);
    EXPECT_EQ(0, results.size());
}

TEST_F(ArduinoJsonParserTest, ParseInvalidArray) {
    const char* json = "[{\"name\":\"John\",}]"; // Extra comma
    auto results = parser->parseArray(json);
    EXPECT_EQ(0, results.size());
}

// New tests for parseJsonArray

TEST_F(ArduinoJsonParserTest, ParseJsonArrayValid) {
    const char* json = "[{\"key\":1},{\"key\":2}]";
    testing::MockFunction<bool(ArduinoJson::JsonObjectConst)> mock_processor;
    
    // Expect two calls: first with {"key":1}, then with {"key":2}
    EXPECT_CALL(mock_processor, Call(testing::_))
        .Times(2)
        .WillOnce(testing::DoAll(
            testing::Invoke([](ArduinoJson::JsonObjectConst obj) {
                EXPECT_TRUE(obj.containsKey("key"));
                EXPECT_EQ(1, obj["key"].as<int>());
                return true;
            }),
            testing::Return(true)
        ))
        .WillOnce(testing::DoAll(
            testing::Invoke([](ArduinoJson::JsonObjectConst obj) {
                EXPECT_TRUE(obj.containsKey("key"));
                EXPECT_EQ(2, obj["key"].as<int>());
                return true;
            }),
            testing::Return(true)
        ));
    
    bool result = parser->parseJsonArray(json, mock_processor.AsStdFunction());
    EXPECT_TRUE(result);
}

TEST_F(ArduinoJsonParserTest, ParseJsonArrayEmpty) {
    const char* json = "[]";
    testing::MockFunction<bool(ArduinoJson::JsonObjectConst)> mock_processor;
    
    // Mock processor should not be called for empty array
    EXPECT_CALL(mock_processor, Call(testing::_)).Times(0);
    
    bool result = parser->parseJsonArray(json, mock_processor.AsStdFunction());
    EXPECT_TRUE(result);
}

TEST_F(ArduinoJsonParserTest, ParseJsonArrayInvalid) {
    const char* json = "invalid";
    testing::MockFunction<bool(ArduinoJson::JsonObjectConst)> mock_processor;
    
    // Mock processor should not be called for invalid JSON
    EXPECT_CALL(mock_processor, Call(testing::_)).Times(0);
    
    bool result = parser->parseJsonArray(json, mock_processor.AsStdFunction());
    EXPECT_FALSE(result);
}

TEST_F(ArduinoJsonParserTest, ParseJsonArrayNotArray) {
    const char* json = "{\"key\":1}";
    testing::MockFunction<bool(ArduinoJson::JsonObjectConst)> mock_processor;
    
    // Mock processor should not be called when input is not an array
    EXPECT_CALL(mock_processor, Call(testing::_)).Times(0);
    
    bool result = parser->parseJsonArray(json, mock_processor.AsStdFunction());
    EXPECT_FALSE(result);
}

TEST_F(ArduinoJsonParserTest, ParseJsonArrayMixedTypes) {
    const char* json = "[1, {\"key\":2}]";
    testing::MockFunction<bool(ArduinoJson::JsonObjectConst)> mock_processor;
    
    // Mock processor should be called once for the object element
    EXPECT_CALL(mock_processor, Call(testing::_))
        .WillOnce(testing::DoAll(
            testing::Invoke([](ArduinoJson::JsonObjectConst obj) {
                EXPECT_TRUE(obj.containsKey("key"));
                EXPECT_EQ(2, obj["key"].as<int>());
                return true;
            }),
            testing::Return(true)
        ));
    
    bool result = parser->parseJsonArray(json, mock_processor.AsStdFunction());
    EXPECT_TRUE(result);
}

TEST_F(ArduinoJsonParserTest, ParseJsonArrayCallbackReturnsFalse) {
    const char* json = "[{\"key\":1},{\"key\":2}]";
    testing::MockFunction<bool(ArduinoJson::JsonObjectConst)> mock_processor;
    
    // Mock processor returns false on first call to stop processing
    EXPECT_CALL(mock_processor, Call(testing::_))
        .WillOnce(testing::Return(false));
    
    bool result = parser->parseJsonArray(json, mock_processor.AsStdFunction());
    EXPECT_TRUE(result);
}
