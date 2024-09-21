#include <unity.h>
#include "dexcom_constants.h"
#include "dexcom_utils.h"

void test_stringToTrendDirection_valid_inputs()
{
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, DexcomUtils::stringToTrendDirection("None"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::DoubleUp, DexcomUtils::stringToTrendDirection("DoubleUp"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::SingleUp, DexcomUtils::stringToTrendDirection("SingleUp"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::FortyFiveUp, DexcomUtils::stringToTrendDirection("FortyFiveUp"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, DexcomUtils::stringToTrendDirection("Flat"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::FortyFiveDown, DexcomUtils::stringToTrendDirection("FortyFiveDown"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::SingleDown, DexcomUtils::stringToTrendDirection("SingleDown"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::DoubleDown, DexcomUtils::stringToTrendDirection("DoubleDown"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::NotComputable, DexcomUtils::stringToTrendDirection("NotComputable"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::RateOutOfRange, DexcomUtils::stringToTrendDirection("RateOutOfRange"));
}

void test_stringToTrendDirection_invalid_inputs()
{
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, DexcomUtils::stringToTrendDirection("InvalidString"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, DexcomUtils::stringToTrendDirection(""));
}