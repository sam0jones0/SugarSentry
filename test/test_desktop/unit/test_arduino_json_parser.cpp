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

TEST_F(ArduinoJsonParserTest, ParseObjectWithString) {
    const char* json = "{\"name\":\"John\",\"city\":\"New York\"}";
    auto result = parser->parseObject(json);
    EXPECT_NE(nullptr, result.get());
    
    auto name = result->getString("name");
    EXPECT_TRUE(name.has_value());
    EXPECT_STREQ("John", name.value().c_str());
    
    auto city = result->getString("city");
    EXPECT_TRUE(city.has_value());
    EXPECT_STREQ("New York", city.value().c_str());
}

TEST_F(ArduinoJsonParserTest, ParseObjectWithNumbers) {
    const char* json = "{\"age\":30,\"height\":1.75}";
    auto result = parser->parseObject(json);
    EXPECT_NE(nullptr, result.get());
    
    auto age = result->getInt("age");
    EXPECT_TRUE(age.has_value());
    EXPECT_EQ(30, age.value());
    
    auto height = result->getDouble("height");
    EXPECT_TRUE(height.has_value());
    EXPECT_FLOAT_EQ(1.75, height.value());
}

TEST_F(ArduinoJsonParserTest, ParseObjectWithBoolean) {
    const char* json = "{\"active\":true,\"verified\":false}";
    auto result = parser->parseObject(json);
    EXPECT_NE(nullptr, result.get());
    
    auto active = result->getBool("active");
    EXPECT_TRUE(active.has_value());
    EXPECT_TRUE(active.value());
    
    auto verified = result->getBool("verified");
    EXPECT_TRUE(verified.has_value());
    EXPECT_FALSE(verified.value());
}

TEST_F(ArduinoJsonParserTest, ParseObjectMissingField) {
    const char* json = "{\"name\":\"John\"}";
    auto result = parser->parseObject(json);
    EXPECT_NE(nullptr, result.get());
    
    auto age = result->getInt("age");
    EXPECT_FALSE(age.has_value());
}

TEST_F(ArduinoJsonParserTest, ParseInvalidObject) {
    const char* json = "invalid json";
    auto result = parser->parseObject(json);
    EXPECT_EQ(nullptr, result.get());
}

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

TEST_F(ArduinoJsonParserTest, ParseWrongType) {
    const char* json = "{\"age\":\"thirty\"}";
    auto result = parser->parseObject(json);
    EXPECT_NE(nullptr, result.get());
    
    auto age = result->getInt("age");
    EXPECT_FALSE(age.has_value());
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
