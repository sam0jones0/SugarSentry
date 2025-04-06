#include <gtest/gtest.h>
#include "json_glucose_reading_parser.h"
#include "dexcom_constants.h"
#include "../mocks/mock_json_parser.h"
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
        .WillOnce(testing::WithArgs<1>(testing::Invoke([](std::function<bool(ArduinoJson::JsonObjectConst)> processor){
            // Empty array - processor is never called
            return true;
        })));
    
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
    // Use WithArgs to capture and invoke the callback with test objects
    EXPECT_CALL(*mock_json_parser, parseJsonArray("[{},{}]", testing::_))
        .WillOnce(testing::WithArgs<1>(testing::Invoke(
            [](std::function<bool(ArduinoJson::JsonObjectConst)> processor) {
                // Create first test JSON object
                StaticJsonDocument<256> doc1;
                deserializeJson(doc1, "{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\"}");
                JsonObjectConst obj1 = doc1.as<JsonObjectConst>();
                
                // Process first object
                processor(obj1);
                
                // Create second test JSON object
                StaticJsonDocument<256> doc2;
                deserializeJson(doc2, "{\"Value\":118,\"Trend\":\"FortyFiveDown\",\"WT\":\"Date(1609455600000)\"}");
                JsonObjectConst obj2 = doc2.as<JsonObjectConst>();
                
                // Process second object
                processor(obj2);
                
                return true; // Simulate successful parsing
            }
        )));
    
    auto readings = parser->parse("[{},{}]");
    EXPECT_EQ(2, readings.size());
    
    // Verify first reading
    EXPECT_EQ(120, readings[0].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
    
    // Verify second reading (with different values)
    EXPECT_EQ(118, readings[1].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::FortyFiveDown, readings[1].getTrend());
}

TEST_F(JsonGlucoseReadingParserTest, ParseArrayWithInvalidObject) {
    // Create test JSON array with mixed valid and invalid objects
    const std::string mixedArray = "[{valid},{invalid},{valid}]";
    
    // Use WithArgs to capture and invoke the callback with our test data
    EXPECT_CALL(*mock_json_parser, parseJsonArray(mixedArray, testing::_))
        .WillOnce(testing::WithArgs<1>(testing::Invoke(
            [](std::function<bool(ArduinoJson::JsonObjectConst)> processor) {
                // First valid object
                StaticJsonDocument<256> valid1;
                deserializeJson(valid1, "{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\"}");
                JsonObjectConst obj1 = valid1.as<JsonObjectConst>();
                
                // Invalid object (missing required WT field)
                // This should cause the GlucoseReading constructor to throw an exception
                // which should be caught by the JsonGlucoseReadingParser::parse method
                StaticJsonDocument<256> invalid;
                deserializeJson(invalid, "{\"Value\":130,\"Trend\":\"FortyFiveUp\"}");
                JsonObjectConst obj2 = invalid.as<JsonObjectConst>();
                
                // Second valid object
                StaticJsonDocument<256> valid2;
                deserializeJson(valid2, "{\"Value\":140,\"Trend\":\"SingleDown\",\"WT\":\"Date(1609459800000)\"}");
                JsonObjectConst obj3 = valid2.as<JsonObjectConst>();
                
                // Process all three objects in sequence
                // The second call should throw but be caught inside the parser
                processor(obj1); // Should succeed
                processor(obj2); // Should throw, but be caught by the processor's try/catch
                processor(obj3); // Should still be processed after the exception
                
                return true;
            }
        )));
    
    // Parse the array
    auto readings = parser->parse(mixedArray);
    
    // Should have only 2 readings (the valid ones) as the invalid one caused an exception
    // that was caught inside the JsonGlucoseReadingParser::parse method
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
    
    // Test that the processor callback returns false after MAX_MAX_COUNT elements
    EXPECT_CALL(*mock_json_parser, parseJsonArray(largeArray, testing::_))
        .WillOnce(testing::WithArgs<1>(testing::Invoke(
            [](std::function<bool(ArduinoJson::JsonObjectConst)> processor) {
                // Create a valid JSON object template to use for all calls
                StaticJsonDocument<256> doc;
                deserializeJson(doc, "{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\"}");
                JsonObjectConst obj = doc.as<JsonObjectConst>();
                
                // Array to track return values of each processor call
                std::vector<bool> processorResults;
                
                // Call the processor MAX_MAX_COUNT + 5 times and track the results
                for (int i = 0; i < DexcomConst::MAX_MAX_COUNT + 5; i++) {
                    bool result = processor(obj);
                    processorResults.push_back(result);
                    
                    // If processor returns false, stop calling it
                    if (!result) break;
                }
                
                // Verify the processor returns true for exactly MAX_MAX_COUNT calls
                EXPECT_EQ(DexcomConst::MAX_MAX_COUNT + 1, processorResults.size());
                
                // Verify first MAX_MAX_COUNT calls return true
                for (int i = 0; i < DexcomConst::MAX_MAX_COUNT; i++) {
                    EXPECT_TRUE(processorResults[i]) 
                        << "Processor should return true for index " << i;
                }
                
                // Verify the call after MAX_MAX_COUNT returns false
                EXPECT_FALSE(processorResults[DexcomConst::MAX_MAX_COUNT]) 
                    << "Processor should return false after MAX_MAX_COUNT limit";
                
                return true; // Successful parsing
            }
        )));

    // Parse the array
    auto readings = parser->parse(largeArray);
    
    // Verify the final readings vector size is exactly MAX_MAX_COUNT
    EXPECT_EQ(DexcomConst::MAX_MAX_COUNT, readings.size());
}
