#include <gtest/gtest.h>
#include "json_glucose_reading_parser.h"
#include "dexcom_constants.h"
#include "../mocks/mock_json_parser.h"
#include "../mocks/mock_json_value.h"
#include <memory>

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
    EXPECT_CALL(*mock_json_parser, parseArray("[]"))
        .WillOnce(testing::Return(std::vector<std::shared_ptr<IJsonValue>>{}));
    
    auto readings = parser->parse("[]");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseInvalidJson) {
    EXPECT_CALL(*mock_json_parser, parseArray("invalid"))
        .WillOnce(testing::Return(std::vector<std::shared_ptr<IJsonValue>>{}));
    
    auto readings = parser->parse("invalid");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseValidArray) {
    // Create mock JSON values with expected behavior
    auto make_valid_mock_value = []() {
        auto val = std::make_shared<testing::NiceMock<MockJsonValue>>();
        ON_CALL(*val, getInt("Value")).WillByDefault(testing::Return(120));
        ON_CALL(*val, getString("Trend")).WillByDefault(testing::Return("Flat"));
        ON_CALL(*val, getString("WT")).WillByDefault(testing::Return("Date(1609459200000)"));
        return val;
    };

    std::vector<std::shared_ptr<IJsonValue>> mock_values;
    mock_values.push_back(make_valid_mock_value());
    mock_values.push_back(make_valid_mock_value());
    
    // Setup expectation for parseArray
    EXPECT_CALL(*mock_json_parser, parseArray("[{},{}]"))
        .WillOnce(testing::Return(mock_values));
    
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
    
    // Create a function to make mock values
    auto make_valid_mock_value = []() {
        auto val = std::make_shared<testing::NiceMock<MockJsonValue>>();
        ON_CALL(*val, getInt("Value")).WillByDefault(testing::Return(120));
        ON_CALL(*val, getString("Trend")).WillByDefault(testing::Return("Flat"));
        ON_CALL(*val, getString("WT")).WillByDefault(testing::Return("Date(1609459200000)"));
        return val;
    };
    
    // Create exactly MAX_MAX_COUNT + 5 mock values to return
    const int totalMockValues = DexcomConst::MAX_MAX_COUNT + 5;
    std::vector<std::shared_ptr<IJsonValue>> mockValues;
    mockValues.reserve(totalMockValues);  // Optimize space allocation
    
    for (int i = 0; i < totalMockValues; i++) {
        mockValues.push_back(make_valid_mock_value());
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
