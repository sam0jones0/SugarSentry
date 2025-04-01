#include "dexcom_utils.h"
#include <cstring>

namespace DexcomUtils
{
DexcomConst::TrendDirection stringToTrendDirection(const char *trendString)
{
    // Check for null pointer
    if (trendString == nullptr)
    {
        return DexcomConst::TrendDirection::None;
    }
    
    for (uint8_t i = 0; i < 10; ++i)
    {
        if (std::strcmp(trendString, DexcomConst::TREND_DIRECTION_STRINGS[i]) == 0)
        {
            return static_cast<DexcomConst::TrendDirection>(i);
        }
    }
    return DexcomConst::TrendDirection::None;
}
}  // namespace DexcomUtils
