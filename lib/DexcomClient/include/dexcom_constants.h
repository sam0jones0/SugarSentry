#ifndef DEXCOM_CONSTANTS_H
#define DEXCOM_CONSTANTS_H

#include <cstdint>

namespace DexcomConst
{
    inline constexpr const char* DEXCOM_APPLICATION_ID = "d89443d2-327c-4a6f-89e5-496bbb0317db";
    inline constexpr const char* DEXCOM_BASE_URL = "https://share2.dexcom.com/ShareWebServices/Services";
    inline constexpr const char* DEXCOM_BASE_URL_OUS = "https://shareous1.dexcom.com/ShareWebServices/Services";
    inline constexpr const char* DEXCOM_LOGIN_ID_ENDPOINT = "General/LoginPublisherAccountById";
    inline constexpr const char* DEXCOM_AUTHENTICATE_ENDPOINT = "General/AuthenticatePublisherAccount";
    inline constexpr const char* DEXCOM_GLUCOSE_READINGS_ENDPOINT = "Publisher/ReadPublisherLatestGlucoseValues";
    inline constexpr const char* DEFAULT_UUID = "00000000-0000-0000-0000-000000000000";

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

    inline constexpr const char* TREND_DIRECTION_STRINGS[] = {
        "None",
        "DoubleUp",
        "SingleUp",
        "FortyFiveUp",
        "Flat",
        "FortyFiveDown",
        "SingleDown",
        "DoubleDown",
        "NotComputable",
        "RateOutOfRange"
    };

    inline constexpr const char* TREND_DESCRIPTIONS[] = {
        "",
        "rising quickly",
        "rising",
        "rising slightly",
        "steady",
        "falling slightly",
        "falling",
        "falling quickly",
        "unable to determine trend",
        "trend unavailable"
    };

    inline constexpr const char* TREND_ARROWS[] = {
        "", "↑↑", "↑", "↗", "→", "↘", "↓", "↓↓", "?", "-"
    };

    constexpr uint16_t MAX_MINUTES = 1440;
    constexpr uint16_t MAX_MAX_COUNT = 288;
    constexpr float MMOL_L_CONVERSION_FACTOR = 0.0555f;
}

#endif // DEXCOM_CONSTANTS_H