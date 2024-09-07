#ifndef DEXCOM_CONSTANTS_H
#define DEXCOM_CONSTANTS_H

#include <Arduino.h>
#include <map>
#include <vector>

namespace DexcomConst
{
    const String DEXCOM_APPLICATION_ID = "d89443d2-327c-4a6f-89e5-496bbb0317db";
    const String DEXCOM_BASE_URL = "https://share2.dexcom.com/ShareWebServices/Services";
    const String DEXCOM_BASE_URL_OUS = "https://shareous1.dexcom.com/ShareWebServices/Services";
    const String DEXCOM_LOGIN_ID_ENDPOINT = "General/LoginPublisherAccountById";
    const String DEXCOM_AUTHENTICATE_ENDPOINT = "General/AuthenticatePublisherAccount";
    const String DEXCOM_GLUCOSE_READINGS_ENDPOINT = "Publisher/ReadPublisherLatestGlucoseValues";
    const String DEFAULT_UUID = "00000000-0000-0000-0000-000000000000";

    const std::map<String, int> DEXCOM_TREND_DIRECTIONS = {
        {"None", 0},
        {"DoubleUp", 1},
        {"SingleUp", 2},
        {"FortyFiveUp", 3},
        {"Flat", 4},
        {"FortyFiveDown", 5},
        {"SingleDown", 6},
        {"DoubleDown", 7},
        {"NotComputable", 8},
        {"RateOutOfRange", 9}};

    const std::vector<String> TREND_DESCRIPTIONS = {
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

    const std::vector<String> TREND_ARROWS = {
        "", "↑↑", "↑", "↗", "→", "↘", "↓", "↓↓", "?", "-"};

    const int MAX_MINUTES = 1440;
    const int MAX_MAX_COUNT = 288;
    const float MMOL_L_CONVERSION_FACTOR = 0.0555;
}

#endif // DEXCOM_CONSTANTS_H