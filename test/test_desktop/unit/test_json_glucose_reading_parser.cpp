#include <gtest/gtest.h>
#include "json_glucose_reading_parser.h"
#include "dexcom_constants.h"
#include "../mocks/mock_json_parser.h"
#include "../mocks/mock_json_value.h"
#include <memory>
#include <ArduinoJson.h>

class JsonGlucoseReadingParserTest : public ::testing::Test {
protected:
    std::shared_ptr<testing::NiceMock<MockJsonParser>> mock_json_parser;
    std::unique_ptr<JsonGlucoseReadingParser> parser;

    void SetUp() override {
        mock_json_parser = std::make_shared<testing::NiceMock<MockJsonParser>>();
        parser = std::make_unique<JsonGlucoseReadingParser>(mock_json_parser);
    }

    void TearDown() override {
        parser.reset();
        mock_json_parser.reset();
    }
};

TEST_F(JsonGlucoseReadingParserTest, ParseEmptyArray) {
    // Setup parseJsonArray to return true but not call the callback (empty array)
    EXPECT_CALL(*mock_json_parser, parseJsonArray("[]", testing::_))
        .WillOnce(testing::Return(true));
    
    auto readings = parser->parse("[]");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseInvalidJson) {
    // Setup parseJsonArray to return false (failed parsing)
    EXPECT_CALL(*mock_json_parser, parseJsonArray("invalid", testing::_))
        .WillOnce(testing::Return(false));
    
    auto readings = parser->parse("invalid");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseValidArray) {
    // Use DoAnswer to call the callback with valid JSON objects
    EXPECT_CALL(*mock_json_parser, parseJsonArray("[{},{}]", testing::_))
        .WillOnce(testing::DoAll(
            // First callback invocation - simulate first JSON object
            testing::Invoke([](const std::string&, std::function<bool(ArduinoJson::JsonObjectConst)> callback) {
                // Create a JSON document for our test
                ArduinoJson::StaticJsonDocument<512> doc;
                doc["Value"] = 120;
                doc["Trend"] = "Flat";
                doc["WT"] = "Date(1609459200000)";
                
                // Call the callback with our object
                callback(doc.as<ArduinoJson::JsonObjectConst>());
                
                // Create a second JSON object
                ArduinoJson::StaticJsonDocument<512> doc2;
                doc2["Value"] = 120;
                doc2["Trend"] = "Flat";
                doc2["WT"] = "Date(1609459200000)";
                
                // Call the callback again
                callback(doc2.as<ArduinoJson::JsonObjectConst>());
                
                return true;
            })
        ));
    
    auto readings = parser->parse("[{},{}]");
    EXPECT_EQ(2, readings.size());
    
    // Verify first reading
    EXPECT_EQ(120, readings[0].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
    
    // Verify second reading
    EXPECT_EQ(120, readings[1].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[1].getTrend());
}

TEST_F(JsonGlucoseReadingParserTest, ParseArrayWithInvalidObject) {
    // Create test JSON array with mixed valid and invalid objects
    const std::string mixedArray = "[{valid},{invalid},{valid}]";
    
    // Use WithArgs to capture and invoke the callback with our test data
    EXPECT_CALL(*mock_json_parser, parseJsonArray(mixedArray, testing::_))
        .WillOnce(testing::WithArgs<1>(testing::Invoke(
            [](std::function<bool(ArduinoJson::JsonObjectConst)> callback) {
                // First valid object
                ArduinoJson::StaticJsonDocument<512> valid1;
                valid1["Value"] = 120;
                valid1["Trend"] = "Flat";
                valid1["WT"] = "Date(1609459200000)";
                
                // Invalid object (missing required WT field)
                ArduinoJson::StaticJsonDocument<512> invalid;
                invalid["Value"] = 130;
                invalid["Trend"] = "FortyFiveUp";
                // No WT field, should cause constructor to throw
                
                // Second valid object
                ArduinoJson::StaticJsonDocument<512> valid2;
                valid2["Value"] = 140;
                valid2["Trend"] = "SingleDown";
                valid2["WT"] = "Date(1609459800000)";
                
                // Process all three objects in sequence
                callback(valid1.as<ArduinoJson::JsonObjectConst>());
                callback(invalid.as<ArduinoJson::JsonObjectConst>());
                callback(valid2.as<ArduinoJson::JsonObjectConst>());
                
                return true;
            }
        )));
    
    // Parse the array
    auto readings = parser->parse(mixedArray);
    
    // Should have only 2 readings (the valid ones)
    EXPECT_EQ(2, readings.size());
    
    // Verify first reading
    EXPECT_EQ(120, readings[0].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
    
    // Verify second reading (which was the third object)
    EXPECT_EQ(140, readings[1].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::SingleDown, readings[1].getTrend());
}

TEST_F(JsonGlucoseReadingParserTest, ParseMaxSizeArray) {
    // Create a large array string that exceeds MAX_MAX_COUNT
    std::string largeArray = "[";
    for (int i = 0; i < DexcomConst::MAX_MAX_COUNT + 5; i++) {
        if (i > 0) largeArray += ",";
        largeArray += "{}";
    }
    largeArray += "]";
    
    // Test that the processing stops after MAX_MAX_COUNT elements
    EXPECT_CALL(*mock_json_parser, parseJsonArray(largeArray, testing::_))
        .WillOnce(testing::DoAll(
            testing::Invoke([](const std::string&, std::function<bool(ArduinoJson::JsonObjectConst)> callback) {
                // Create a JSON object template
                ArduinoJson::StaticJsonDocument<512> doc;
                doc["Value"] = 120;
                doc["Trend"] = "Flat";
                doc["WT"] = "Date(1609459200000)";
                auto jsonObj = doc.as<ArduinoJson::JsonObjectConst>();
                
                // Call the callback MAX_MAX_COUNT + 5 times
                // The callback should return false after MAX_MAX_COUNT calls
                bool continueProcessing = true;
                int callCount = 0;
                
                while (continueProcessing && callCount < DexcomConst::MAX_MAX_COUNT + 5) {
                    continueProcessing = callback(jsonObj);
                    callCount++;
                }
                
                // The callback should have stopped processing after MAX_MAX_COUNT elements
                EXPECT_EQ(DexcomConst::MAX_MAX_COUNT + 1, callCount);
                EXPECT_FALSE(continueProcessing);
                
                return true;
            })
        ));

    auto readings = parser->parse(largeArray);
    
    // Verify that parser correctly limits to MAX_MAX_COUNT
    EXPECT_EQ(DexcomConst::MAX_MAX_COUNT, readings.size());
}
