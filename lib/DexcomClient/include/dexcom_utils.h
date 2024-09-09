#ifndef DEXCOM_UTILS_H
#define DEXCOM_UTILS_H

#include "dexcom_constants.h"
#include <cstdint>

namespace DexcomUtils
{
    DexcomConst::TrendDirection stringToTrendDirection(const char *trendString);
}

#endif // DEXCOM_UTILS_H