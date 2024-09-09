#include "dexcom_constants.h"

namespace DexcomConst
{
    const char *DEXCOM_APPLICATION_ID = "d89443d2-327c-4a6f-89e5-496bbb0317db";
    const char *DEXCOM_BASE_URL = "https://share2.dexcom.com/ShareWebServices/Services";
    const char *DEXCOM_BASE_URL_OUS = "https://shareous1.dexcom.com/ShareWebServices/Services";
    const char *DEXCOM_LOGIN_ID_ENDPOINT = "General/LoginPublisherAccountById";
    const char *DEXCOM_AUTHENTICATE_ENDPOINT = "General/AuthenticatePublisherAccount";
    const char *DEXCOM_GLUCOSE_READINGS_ENDPOINT = "Publisher/ReadPublisherLatestGlucoseValues";
    const char *DEFAULT_UUID = "00000000-0000-0000-0000-000000000000";

    const char *TREND_DIRECTION_STRINGS[] = {
        "None",
        "DoubleUp",
        "SingleUp",
        "FortyFiveUp",
        "Flat",
        "FortyFiveDown",
        "SingleDown",
        "DoubleDown",
        "NotComputable",
        "RateOutOfRange"};

    const char *TREND_DESCRIPTIONS[] = {
        "",
        "rising quickly",
        "rising",
        "rising slightly",
        "steady",
        "falling slightly",
        "falling",
        "falling quickly",
        "unable to determine trend",
        "trend unavailable"};

    const char *TREND_ARROWS[] = {
        "", "↑↑", "↑", "↗", "→", "↘", "↓", "↓↓", "?", "-"};

    TrendDirection stringToTrendDirection(const char *trendString)
    {
        for (uint8_t i = 0; i < 10; ++i)
        {
            if (strcmp(trendString, TREND_DIRECTION_STRINGS[i]) == 0)
            {
                return static_cast<TrendDirection>(i);
            }
        }
        return TrendDirection::None;
    }
}