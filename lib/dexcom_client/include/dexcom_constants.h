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
MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ
RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD
VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX
DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y
ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy
VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr
mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr
IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK
mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu
XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy
dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye
jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1
BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3
DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92
9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx
jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0
Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz
ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS
R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp
-----END CERTIFICATE-----
)";
}

#endif // DEXCOM_CONSTANTS_H