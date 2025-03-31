#include <gtest/gtest.h>
#include "glucose_reading.h"
#include "i_json_value.h"
#include "mock_json_value.h"
#include <memory>
#include <stdexcept>

class GlucoseReadingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * Test Case 1: Valid input (all fields present)
 */
TEST_F(GlucoseReadingTest, ConstructorWithValidJson) {
    auto mockJsonValue = std::make_shared<testing::StrictMock<MockJsonValue>>();
    
    EXPECT_CALL(*mockJsonValue, getInt("Value"))
        .WillOnce(testing::Return(120));
    EXPECT_CALL(*mockJsonValue, getString("Trend"))
        .WillOnce(testing::Return("Flat"));
    EXPECT_CALL(*mockJsonValue, getString("WT"))
        .WillOnce(testing::Return("Date(1609459200000)"));
    
    EXPECT_NO_THROW({
        GlucoseReading reading(*mockJsonValue);
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
    auto mockJsonValue = std::make_shared<testing::StrictMock<MockJsonValue>>();
    
    EXPECT_CALL(*mockJsonValue, getInt("Value"))
        .WillOnce(testing::Return(std::nullopt));
    
    // The constructor exits early when Value is missing, so Trend and WT are not checked
    
    EXPECT_THROW({
        GlucoseReading reading(*mockJsonValue);
    }, std::runtime_error);
}

/**
 * Test Case 3: Trend field is missing
 */
TEST_F(GlucoseReadingTest, ConstructorThrowsWhenTrendFieldMissing) {
    auto mockJsonValue = std::make_shared<testing::StrictMock<MockJsonValue>>();
    
    EXPECT_CALL(*mockJsonValue, getInt("Value"))
        .WillOnce(testing::Return(120));
    EXPECT_CALL(*mockJsonValue, getString("Trend"))
        .WillOnce(testing::Return(std::nullopt));
    
    // The constructor exits early when Trend is missing, so WT is not checked
    
    EXPECT_THROW({
        GlucoseReading reading(*mockJsonValue);
    }, std::runtime_error);
}

/**
 * Test Case 4: WT (timestamp) field is missing
 */
TEST_F(GlucoseReadingTest, ConstructorThrowsWhenWTFieldMissing) {
    auto mockJsonValue = std::make_shared<testing::StrictMock<MockJsonValue>>();
    
    EXPECT_CALL(*mockJsonValue, getInt("Value"))
        .WillOnce(testing::Return(120));
    EXPECT_CALL(*mockJsonValue, getString("Trend"))
        .WillOnce(testing::Return("Flat"));
    EXPECT_CALL(*mockJsonValue, getString("WT"))
        .WillOnce(testing::Return(std::nullopt));
    
    EXPECT_THROW({
        GlucoseReading reading(*mockJsonValue);
    }, std::runtime_error);
}
