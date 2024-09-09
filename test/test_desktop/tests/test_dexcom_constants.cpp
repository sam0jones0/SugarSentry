#include <unity.h>
#include "dexcom_constants.h"

void test_stringToTrendDirection_valid_inputs()
{
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, DexcomConst::stringToTrendDirection("None"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::DoubleUp, DexcomConst::stringToTrendDirection("DoubleUp"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::SingleUp, DexcomConst::stringToTrendDirection("SingleUp"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::FortyFiveUp, DexcomConst::stringToTrendDirection("FortyFiveUp"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, DexcomConst::stringToTrendDirection("Flat"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::FortyFiveDown, DexcomConst::stringToTrendDirection("FortyFiveDown"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::SingleDown, DexcomConst::stringToTrendDirection("SingleDown"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::DoubleDown, DexcomConst::stringToTrendDirection("DoubleDown"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::NotComputable, DexcomConst::stringToTrendDirection("NotComputable"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::RateOutOfRange, DexcomConst::stringToTrendDirection("RateOutOfRange"));
}

void test_stringToTrendDirection_invalid_inputs()
{
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, DexcomConst::stringToTrendDirection("InvalidString"));
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, DexcomConst::stringToTrendDirection(""));
}
