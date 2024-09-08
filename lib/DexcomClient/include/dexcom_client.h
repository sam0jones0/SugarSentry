#ifndef DEXCOM_CLIENT_H
#define DEXCOM_CLIENT_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <vector>
#include <optional>
#include "dexcom_constants.h"
#include "dexcom_errors.h"
#include "glucose_reading.h"

class DexcomClient
{
public:
    DexcomClient(WiFiClientSecure &client,
                 const String &password,
                 const String &account_id = "",
                 const String &username = "",
                 bool ous = false);

    std::vector<GlucoseReading> getGlucoseReadings(uint16_t minutes = DexcomConst::MAX_MINUTES,
                                                   uint16_t max_count = DexcomConst::MAX_MAX_COUNT);

    std::optional<GlucoseReading> getLatestGlucoseReading();
    std::optional<GlucoseReading> getCurrentGlucoseReading();

private:
    WiFiClientSecure &_client;
    String _base_url;
    String _password;
    String _account_id;
    String _username;
    String _session_id;

    String post(const String &endpoint,
                const String &params = "",
                const String &json = "");

    std::optional<DexcomError> handleResponse(const String &response);

    String getAccountId();
    String getSessionId();

    void createSession();

    std::vector<String> getGlucoseReadingsRaw(uint16_t minutes = DexcomConst::MAX_MINUTES, uint16_t max_count = DexcomConst::MAX_MAX_COUNT);
};

#endif // DEXCOM_CLIENT_H