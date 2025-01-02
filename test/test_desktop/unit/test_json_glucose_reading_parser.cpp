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
        parser = std::make_unique<JsonGlucoseReadingParser>(mock_json_parser);
    }

    void TearDown() override {
        parser.reset();
        mock_json_parser.reset();
    }
};

TEST_F(JsonGlucoseReadingParserTest, ParseEmptyArray) {
    auto readings = parser->parse("[]");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseInvalidJson) {
    auto readings = parser->parse("invalid");
    EXPECT_EQ(0, readings.size());
}

TEST_F(JsonGlucoseReadingParserTest, ParseValidArray) {
    auto readings = parser->parse("[{},{}]"); // Mock will return 2 valid readings
    EXPECT_EQ(2, readings.size());
    
    // Verify first reading
    EXPECT_EQ(120, readings[0].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
    
    // Verify second reading
    EXPECT_EQ(120, readings[1].getValue());
    EXPECT_EQ(DexcomConst::TrendDirection::Flat, readings[1].getTrend());
}

TEST_F(JsonGlucoseReadingParserTest, ParseMaxSizeArray) {
    // Create a large array string that the mock will parse into MAX_MAX_COUNT readings
    std::string largeArray = "[";
    for (int i = 0; i < DexcomConst::MAX_MAX_COUNT + 5; i++) {
        if (i > 0) largeArray += ",";
        largeArray += "{}";
    }
    largeArray += "]";

    auto readings = parser->parse(largeArray);
    EXPECT_LE(readings.size(), DexcomConst::MAX_MAX_COUNT);
}
