#include <gtest/gtest.h>
#include "glucose_reading.h"
#include "i_json_value.h"
#include <memory>
#include <stdexcept>

/**
 * Configurable mock JSON value for testing missing fields
 */
class ConfigurableMockJsonValue : public IJsonValue {
public:
    bool hasValueField = true;
    bool hasTrendField = true;
    bool hasWTField = true;
    
    std::optional<std::string> getString(const std::string& key) const override {
        if (key == "Trend" && hasTrendField) return "Flat";
        if (key == "WT" && hasWTField) return "Date(1609459200000)";
        return std::nullopt;
    }
    
    std::optional<int> getInt(const std::string& key) const override {
        if (key == "Value" && hasValueField) return 120;
        return std::nullopt;
    }
    
    std::optional<double> getDouble(const std::string& key) const override {
        return std::nullopt;
    }
    
    std::optional<bool> getBool(const std::string& key) const override {
        return std::nullopt;
    }
};

class GlucoseReadingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * Test Case 1: Valid input (all fields present)
 */
TEST_F(GlucoseReadingTest, ConstructorWithValidJson) {
    ConfigurableMockJsonValue jsonValue;
    // All fields are present by default
    
    EXPECT_NO_THROW({
        GlucoseReading reading(jsonValue);
        // Verify the reading has expected values
        EXPECT_EQ(120, reading.getValue());
        EXPECT_EQ(DexcomConst::TrendDirection::Flat, reading.getTrend());
        EXPECT_EQ(1609459200, reading.getTimestamp()); // 2021-01-01 00:00:00 UTC
    });
}

/**
 * Test Case 2: Value field is missing
 */
TEST_F(GlucoseReadingTest, ConstructorThrowsWhenValueFieldMissing) {
    ConfigurableMockJsonValue jsonValue;
    jsonValue.hasValueField = false;
    
    EXPECT_THROW({
        GlucoseReading reading(jsonValue);
    }, std::runtime_error);
}

/**
 * Test Case 3: Trend field is missing
 */
TEST_F(GlucoseReadingTest, ConstructorThrowsWhenTrendFieldMissing) {
    ConfigurableMockJsonValue jsonValue;
    jsonValue.hasTrendField = false;
    
    EXPECT_THROW({
        GlucoseReading reading(jsonValue);
    }, std::runtime_error);
}

/**
 * Test Case 4: WT (timestamp) field is missing
 */
TEST_F(GlucoseReadingTest, ConstructorThrowsWhenWTFieldMissing) {
    ConfigurableMockJsonValue jsonValue;
    jsonValue.hasWTField = false;
    
    EXPECT_THROW({
        GlucoseReading reading(jsonValue);
    }, std::runtime_error);
}
