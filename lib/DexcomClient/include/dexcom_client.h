#ifndef DEXCOM_CLIENT_H
#define DEXCOM_CLIENT_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <vector>
#include <optional>
#include "dexcom_constants.h"
#include "dexcom_errors.h"
#include "glucose_reading.h"


class DexcomClient {
public:
    DexcomClient(WiFiClientSecure& client, 
                 const String& password,
                 const String& account_id = "",
                 const String& username = "",
                 bool ous = false);

    std::vector<GlucoseReading> getGlucoseReadings(int minutes = DexcomConst::MAX_MINUTES, 
                                                   int max_count = DexcomConst::MAX_MAX_COUNT);

private:
    WiFiClientSecure& _client;
    String _password;
    String _account_id;
    String _username;
    bool _ous;
    String _session_id;

    String post(const String& endpoint, 
                const std::optional<String>& params = std::nullopt,
                const std::optional<String>& json = std::nullopt);

    std::optional<DexcomError> handleResponse(const String& response);

    String getAccountId();
    String getSessionId();

    void createSession();

    std::vector<String> getGlucoseReadingsRaw(int minutes = DexcomConst::MAX_MINUTES, 
                                              int max_count = DexcomConst::MAX_MAX_COUNT);
};

#endif // DEXCOM_CLIENT_H