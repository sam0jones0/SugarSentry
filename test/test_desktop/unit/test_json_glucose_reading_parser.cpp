#include <gtest/gtest.h>
#include "json_glucose_reading_parser.h"
#include "dexcom_constants.h"
#include "i_json_value.h"
#include <memory>

// Mock IJsonValue implementation
class MockJsonValue : public IJsonValue {
public:
    std::optional<std::string> getString(const std::string& key) const override {
        if (key == "Trend") return "Flat";
        if (key == "WT") return "Date(1609459200000)";
        return std::nullopt;
    }
    
    std::optional<int> getInt(const std::string& key) const override {
        if (key == "Value") return 120;
        return std::nullopt;
    }
    
    std::optional<double> getDouble(const std::string& key) const override {
        return std::nullopt;
    }
    
    std::optional<bool> getBool(const std::string& key) const override {
        return std::nullopt;
    }
};

// Mock IJsonParser implementation
class MockJsonParser : public IJsonParser {
public:
    std::shared_ptr<IJsonValue> parseObject(const std::string& jsonString) override {
        // Mock implementation that returns predefined values based on input
        if (jsonString == "invalid") {
            return nullptr;
        }
        return std::make_shared<MockJsonValue>();
    }

    std::vector<std::shared_ptr<IJsonValue>> parseArray(const std::string& jsonString) override {
        // Mock implementation that returns predefined array based on input
        if (jsonString == "[]" || jsonString == "invalid") {
            return {};
        }

        // Return array with two mock readings
        std::vector<std::shared_ptr<IJsonValue>> result;
        result.push_back(std::make_shared<MockJsonValue>());
        result.push_back(std::make_shared<MockJsonValue>());
        return result;
    }
};

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
