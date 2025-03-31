#include <gtest/gtest.h>
#include "json_glucose_reading_parser.h"
#include "dexcom_constants.h"
#include "../mocks/mock_json_parser.h"
#include "../mocks/mock_json_value.h"
#include <memory>

class JsonGlucoseReadingParserTest : public ::testing::Test {
protected:
    std::shared_ptr<MockJsonParser> mock_json_parser;
    std::unique_ptr<JsonGlucoseReadingParser> parser;

    void SetUp() override {
        mock_json_parser = std::make_shared<MockJsonParser>();
        mock_json_parser->SetDefaultBehaviors();
        parser = std::make_unique<JsonGlucoseReadingParser>(mock_json_parser);
    }

    void TearDown() override {
        parser.reset();
        mock_json_parser.reset();
    }
};

TEST_F(JsonGlucoseReadingParserTest, ParseEmptyArray) {
    EXPECT_CALL(*mock_json_parser, parseArray("[]"))
        .WillOnce(testing::Return(std::vector<std::shared_ptr<IJsonValue>>()));
    
    auto readings = parser->parse("[]");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseInvalidJson) {
    EXPECT_CALL(*mock_json_parser, parseArray("invalid"))
        .WillOnce(testing::Return(std::vector<std::shared_ptr<IJsonValue>>()));
    
    auto readings = parser->parse("invalid");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseValidArray) {
    // Create mock JSON values with expected behavior
    auto mockValue1 = std::make_shared<testing::NiceMock<MockJsonValue>>();
    auto mockValue2 = std::make_shared<testing::NiceMock<MockJsonValue>>();
    
    // Setup expected calls for first mock value
    EXPECT_CALL(*mockValue1, getInt("Value"))
        .WillOnce(testing::Return(120));
    EXPECT_CALL(*mockValue1, getString("Trend"))
        .WillOnce(testing::Return("Flat"));
    EXPECT_CALL(*mockValue1, getString("WT"))
        .WillOnce(testing::Return("Date(1609459200000)"));
    
    // Setup expected calls for second mock value
    EXPECT_CALL(*mockValue2, getInt("Value"))
        .WillOnce(testing::Return(120));
    EXPECT_CALL(*mockValue2, getString("Trend"))
        .WillOnce(testing::Return("Flat"));
    EXPECT_CALL(*mockValue2, getString("WT"))
        .WillOnce(testing::Return("Date(1609459200000)"));
    
    // Create vector of mock values to return
    std::vector<std::shared_ptr<IJsonValue>> mockValues = {mockValue1, mockValue2};
    
    // Setup expectation for parseArray
    EXPECT_CALL(*mock_json_parser, parseArray("[{},{}]"))
        .WillOnce(testing::Return(mockValues));
    
    auto readings = parser->parse("[{},{}]");
    EXPECT_EQ(2, readings.size());
    
    // Verify first reading
    EXPECT_EQ(120, readings[0].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
    
    // Verify second reading
    EXPECT_EQ(120, readings[1].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[1].getTrend());
}

TEST_F(JsonGlucoseReadingParserTest, ParseMaxSizeArray) {
    // Create a large array string that the mock will parse into MAX_MAX_COUNT+5 readings
    std::string largeArray = "[";
    for (int i = 0; i < DexcomConst::MAX_MAX_COUNT + 5; i++) {
        if (i > 0) largeArray += ",";
        largeArray += "{}";
    }
    largeArray += "]";
    
    // Create exactly MAX_MAX_COUNT + 5 mock values to return
    const int totalMockValues = DexcomConst::MAX_MAX_COUNT + 5;
    std::vector<std::shared_ptr<IJsonValue>> mockValues;
    mockValues.reserve(totalMockValues);  // Optimize space allocation
    
    for (int i = 0; i < totalMockValues; i++) {
        auto mockValue = std::make_shared<testing::NiceMock<MockJsonValue>>();
        ON_CALL(*mockValue, getInt("Value"))
            .WillByDefault(testing::Return(120));
        ON_CALL(*mockValue, getString("Trend"))
            .WillByDefault(testing::Return("Flat"));
        ON_CALL(*mockValue, getString("WT"))
            .WillByDefault(testing::Return("Date(1609459200000)"));
        mockValues.push_back(mockValue);
    }
    
    // Verify we actually created the expected number of mock values
    ASSERT_EQ(totalMockValues, mockValues.size());
    
    // Setup expectation for parseArray
    EXPECT_CALL(*mock_json_parser, parseArray(largeArray))
        .WillOnce(testing::Return(mockValues));

    auto readings = parser->parse(largeArray);
    
    // Verify that parser correctly limits to MAX_MAX_COUNT
    EXPECT_EQ(DexcomConst::MAX_MAX_COUNT, readings.size());
}
