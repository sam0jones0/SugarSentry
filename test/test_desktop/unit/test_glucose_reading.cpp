#include <unity.h>
#include <ArduinoJson.h>
#include "glucose_reading.h"


void test_glucose_reading_construction()
{
    StaticJsonDocument<200> doc;
    doc["WT"] = "Date(1625873945000)";
    doc["Value"] = 120;
    doc["Trend"] = "Flat";

    GlucoseReading reading(doc.as<JsonObjectConst>());

    TEST_ASSERT_EQUAL(120, reading.getValue());
    TEST_ASSERT_EQUAL(120, reading.getMgDl());
    TEST_ASSERT_FLOAT_WITHIN(6.65, 6.67, reading.getMmolL());
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, reading.getTrend());
    TEST_ASSERT_EQUAL_STRING("Flat", reading.getTrendDirection());
    TEST_ASSERT_EQUAL_STRING("steady", reading.getTrendDescription());
    TEST_ASSERT_EQUAL_STRING("→", reading.getTrendArrow());
    TEST_ASSERT_EQUAL(1625873945, reading.getTimestamp());
}

void test_glucose_reading_invalid_input()
{
    StaticJsonDocument<200> doc;
    doc["WT"] = "InvalidDate";
    doc["Value"] = 90000000; // Out of valid range
    doc["Trend"] = "InvalidTrend";

    GlucoseReading reading(doc.as<JsonObjectConst>());

    TEST_ASSERT_EQUAL(19072, reading.getValue()); // Should be capped at 19072
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, reading.getTrend());
    TEST_ASSERT_EQUAL(0, reading.getTimestamp());
}