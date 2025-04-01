#include <gtest/gtest.h>
#include "dexcom_utils.h"
#include "dexcom_constants.h"

// Test fixture for DexcomUtils tests
class DexcomUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // No setup needed for these tests
    }
    
    void TearDown() override {
        // No teardown needed for these tests
    }
};

// Test case for valid trend direction strings
TEST_F(DexcomUtilsTest, StringToTrendDirection_ValidInputs) {
    // Test all valid trend strings (index 1 to 9, as index 0 is "None")
    for (uint8_t i = 1; i < 10; ++i) {
        const char* trendString = DexcomConst::TREND_DIRECTION_STRINGS[i];
        DexcomConst::TrendDirection expected = static_cast<DexcomConst::TrendDirection>(i);
        
        // Call the function and verify the result
        DexcomConst::TrendDirection actual = DexcomUtils::stringToTrendDirection(trendString);
        
        EXPECT_EQ(actual, expected) 
            << "Failed for trend string: " << trendString 
            << ", expected: " << static_cast<int>(expected) 
            << ", got: " << static_cast<int>(actual);
    }
}

// Test case for an invalid trend direction string
TEST_F(DexcomUtilsTest, StringToTrendDirection_InvalidInput) {
    // Test with an unknown trend string
    const char* invalidTrend = "UnknownTrend";
    DexcomConst::TrendDirection result = DexcomUtils::stringToTrendDirection(invalidTrend);
    
    // Should return TrendDirection::None (0)
    EXPECT_EQ(result, DexcomConst::TrendDirection::None) 
        << "Failed for invalid trend string, expected: None (0), got: " 
        << static_cast<int>(result);
    
    // Test with another invalid string
    const char* anotherInvalidTrend = "invalid";
    result = DexcomUtils::stringToTrendDirection(anotherInvalidTrend);
    
    // Should also return TrendDirection::None (0)
    EXPECT_EQ(result, DexcomConst::TrendDirection::None) 
        << "Failed for invalid trend string, expected: None (0), got: " 
        << static_cast<int>(result);
}

// Test case for an empty string
TEST_F(DexcomUtilsTest, StringToTrendDirection_EmptyInput) {
    // Test with an empty string
    const char* emptyTrend = "";
    DexcomConst::TrendDirection result = DexcomUtils::stringToTrendDirection(emptyTrend);
    
    // Should return TrendDirection::None (0)
    EXPECT_EQ(result, DexcomConst::TrendDirection::None) 
        << "Failed for empty string, expected: None (0), got: " 
        << static_cast<int>(result);
}

// Test case for nullptr
TEST_F(DexcomUtilsTest, StringToTrendDirection_NullInput) {
    // Test with nullptr
    const char* nullTrend = nullptr;
    DexcomConst::TrendDirection result = DexcomUtils::stringToTrendDirection(nullTrend);
    
    // Should return TrendDirection::None (0)
    EXPECT_EQ(result, DexcomConst::TrendDirection::None) 
        << "Failed for nullptr, expected: None (0), got: " 
        << static_cast<int>(result);
}
