#include <gtest/gtest.h>
#include "glucose_reading.h"
#include <memory>
#include <stdexcept>
#include <ArduinoJson.h>

class GlucoseReadingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * Test Case 5: Valid direct ArduinoJson constructor
 */
TEST_F(GlucoseReadingTest, ArduinoJsonConstructorWithValidJson) {
    StaticJsonDocument<200> doc;
    doc["Value"] = 120;
    doc["Trend"] = "Flat";
    doc["WT"] = "Date(1609459200000)";
    
    EXPECT_NO_THROW({
        GlucoseReading reading(doc.as<JsonObjectConst>());
        // Verify the reading has expected values
        EXPECT_EQ(120, reading.getValue());
        EXPECT_EQ(DexcomConst::TrendDirection::Flat, reading.getTrend());
        EXPECT_EQ(1609459200, reading.getTimestamp()); // 2021-01-01 00:00:00 UTC
    });
}

/**
 * Test Case 6: ArduinoJson constructor with missing Value field
 */
TEST_F(GlucoseReadingTest, ArduinoJsonConstructorThrowsWhenValueFieldMissing) {
    StaticJsonDocument<200> doc;
    // Deliberately not setting Value field
    doc["Trend"] = "Flat";
    doc["WT"] = "Date(1609459200000)";
    
    EXPECT_THROW({
        GlucoseReading reading(doc.as<JsonObjectConst>());
    }, std::runtime_error);
}

/**
 * Test Case 7: ArduinoJson constructor with missing Trend field
 */
TEST_F(GlucoseReadingTest, ArduinoJsonConstructorThrowsWhenTrendFieldMissing) {
    StaticJsonDocument<200> doc;
    doc["Value"] = 120;
    // Deliberately not setting Trend field
    doc["WT"] = "Date(1609459200000)";
    
    EXPECT_THROW({
        GlucoseReading reading(doc.as<JsonObjectConst>());
    }, std::runtime_error);
}

/**
 * Test Case 8: ArduinoJson constructor with missing WT field
 */
TEST_F(GlucoseReadingTest, ArduinoJsonConstructorThrowsWhenWTFieldMissing) {
    StaticJsonDocument<200> doc;
    doc["Value"] = 120;
    doc["Trend"] = "Flat";
    // Deliberately not setting WT field
    
    EXPECT_THROW({
        GlucoseReading reading(doc.as<JsonObjectConst>());
    }, std::runtime_error);
}

/**
 * Test Case 9: ArduinoJson constructor with incorrect type for Value
 */
TEST_F(GlucoseReadingTest, ArduinoJsonConstructorThrowsWhenValueHasWrongType) {
    StaticJsonDocument<200> doc;
    doc["Value"] = "not-a-number"; // Wrong type (string instead of int)
    doc["Trend"] = "Flat";
    doc["WT"] = "Date(1609459200000)";
    
    EXPECT_THROW({
        GlucoseReading reading(doc.as<JsonObjectConst>());
    }, std::runtime_error);
}
