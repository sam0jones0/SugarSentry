#ifndef DEXCOM_CONSTANTS_H
#define DEXCOM_CONSTANTS_H

#include <cstdint>

#ifdef ARDUINO
#include <pgmspace.h>
#define PROGMEM_CERT PROGMEM
#else
#define PROGMEM_CERT
#endif

/**
 * @file dexcom_constants.h
 * @brief Defines constants and enums used in the Dexcom API client.
 */

namespace DexcomConst
{
    // API endpoints, application IDs, etc...

    inline constexpr const char *DEXCOM_APPLICATION_ID = "d89443d2-327c-4a6f-89e5-496bbb0317db";
    inline constexpr const char *DEXCOM_BASE_URL = "share2.dexcom.com";
    inline constexpr const char *DEXCOM_BASE_URL_OUS = "shareous1.dexcom.com";

    // inline constexpr const char *DEXCOM_BASE_URL = "https://share2.dexcom.com/ShareWebServices/Services";
    // inline constexpr const char *DEXCOM_BASE_URL_OUS = "https://shareous1.dexcom.com/ShareWebServices/Services";

    inline constexpr const char *DEXCOM_LOGIN_ID_ENDPOINT = "General/LoginPublisherAccountById";
    inline constexpr const char *DEXCOM_AUTHENTICATE_ENDPOINT = "General/AuthenticatePublisherAccount";
    inline constexpr const char *DEXCOM_GLUCOSE_READINGS_ENDPOINT = "Publisher/ReadPublisherLatestGlucoseValues";
    inline constexpr const char *DEFAULT_UUID = "00000000-0000-0000-0000-000000000000";

    /**
     * @brief Represents the trend direction of glucose readings.
     */
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

    inline constexpr const char *TREND_DIRECTION_STRINGS[] = {
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

    inline constexpr const char *TREND_DESCRIPTIONS[] = {
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

    inline constexpr const char *TREND_ARROWS[] = {
        "", "↑↑", "↑", "↗", "→", "↘", "↓", "↓↓", "?", "-"};

    constexpr uint16_t MAX_MINUTES = 1440;
    constexpr uint16_t MAX_MAX_COUNT = 288;
    constexpr uint16_t MAX_READING_JSON_SIZE = 256;
    constexpr float MMOL_L_CONVERSION_FACTOR = 0.0555f;

    const char rootCA[] PROGMEM_CERT = R"(
-----BEGIN CERTIFICATE-----
MIIDljCCAn6gAwIBAgIQC5McOtY5Z+pnI7/Dr5r0SzANBgkqhkiG9w0BAQsFADBl
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSQwIgYDVQQDExtEaWdpQ2VydCBBc3N1cmVkIElEIFJv
b3QgRzIwHhcNMTMwODAxMTIwMDAwWhcNMzgwMTE1MTIwMDAwWjBlMQswCQYDVQQG
EwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3d3cuZGlnaWNl
cnQuY29tMSQwIgYDVQQDExtEaWdpQ2VydCBBc3N1cmVkIElEIFJvb3QgRzIwggEi
MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDZ5ygvUj82ckmIkzTz+GoeMVSA
n61UQbVH35ao1K+ALbkKz3X9iaV9JPrjIgwrvJUXCzO/GU1BBpAAvQxNEP4Htecc
biJVMWWXvdMX0h5i89vqbFCMP4QMls+3ywPgym2hFEwbid3tALBSfK+RbLE4E9Hp
EgjAALAcKxHad3A2m67OeYfcgnDmCXRwVWmvo2ifv922ebPynXApVfSr/5Vh88lA
bx3RvpO704gqu52/clpWcTs/1PPRCv4o76Pu2ZmvA9OPYLfykqGxvYmJHzDNw6Yu
YjOuFgJ3RFrngQo8p0Quebg/BLxcoIfhG69Rjs3sLPr4/m3wOnyqi+RnlTGNAgMB
AAGjQjBAMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgGGMB0GA1UdDgQW
BBTOw0q5mVXyuNtgv6l+vVa1lzan1jANBgkqhkiG9w0BAQsFAAOCAQEAyqVVjOPI
QW5pJ6d1Ee88hjZv0p3GeDgdaZaikmkuOGybfQTUiaWxMTeKySHMq2zNixya1r9I
0jJmwYrA8y8678Dj1JGG0VDjA9tzd29KOVPt3ibHtX2vK0LRdWLjSisCx1BL4Gni
lmwORGYQRI+tBev4eaymG+g3NJ1TyWGqolKvSnAWhsI6yLETcDbYz+70CjTVW0z9
B5yiutkBclzzTcHdDrEcDcRjvq30FPuJ7KJBDkzMyFdA0G4Dqs0MjomZmWzwPDCv
ON9vvKO+KSAnq3T/EyJ43pdSVR6DtVQgA+6uwE9W3jfMw3+qBCe703e4YtsXfJwo
IhNzbM8m9Yop5w==
-----END CERTIFICATE-----
)";
}

#endif // DEXCOM_CONSTANTS_H