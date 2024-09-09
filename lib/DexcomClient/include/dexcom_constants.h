#ifndef DEXCOM_CONSTANTS_H
#define DEXCOM_CONSTANTS_H

#include <cstring>
#include <cstdint>

namespace DexcomConst
{
    extern const char *DEXCOM_APPLICATION_ID;
    extern const char *DEXCOM_BASE_URL;
    extern const char *DEXCOM_BASE_URL_OUS;
    extern const char *DEXCOM_LOGIN_ID_ENDPOINT;
    extern const char *DEXCOM_AUTHENTICATE_ENDPOINT;
    extern const char *DEXCOM_GLUCOSE_READINGS_ENDPOINT;
    extern const char *DEFAULT_UUID;

    enum TrendDirection
    {
        None = 0,
        DoubleUp,
        SingleUp,
        FortyFiveUp,
        Flat,
        FortyFiveDown,
        SingleDown,
        DoubleDown,
        NotComputable,
        RateOutOfRange
    };

    extern const char *TREND_DIRECTION_STRINGS[];
    extern const char *TREND_DESCRIPTIONS[];
    extern const char *TREND_ARROWS[];

    constexpr uint16_t MAX_MINUTES = 1440;
    constexpr uint16_t MAX_MAX_COUNT = 288;
    constexpr float MMOL_L_CONVERSION_FACTOR = 0.0555f;

    TrendDirection stringToTrendDirection(const char *trendString);
}

#endif // DEXCOM_CONSTANTS_H