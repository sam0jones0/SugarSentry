#ifndef DEXCOM_UTILS_H
#define DEXCOM_UTILS_H

#include "dexcom_constants.h"

namespace DexcomUtils
{
    /**
     * @brief Converts a trend string to its corresponding TrendDirection enum value.
     * 
     * @param trendString The trend string to convert.
     * @return DexcomConst::TrendDirection The corresponding TrendDirection enum value.
     */
    DexcomConst::TrendDirection stringToTrendDirection(const char* trendString);
}

#endif // DEXCOM_UTILS_H