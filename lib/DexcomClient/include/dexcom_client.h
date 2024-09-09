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

/*

createSession = getAccountId -> getSessionId 

E.g. Response from Dexcom API for getAccountId: `ae3b74f1-243e-45bc-bb6c-84e997456e5a`
E.g. Response from Dexcom API for getSessionId: `7b644843-e1f9-42a5-8288-1cddaf6102e3`

E.g. Resposne from Dexcom API for getGlucoseReadingsRaw(max_count=2)
```json
[
    {
        "WT": "Date(1725875745000)",
        "ST": "Date(1725875745000)",
        "DT": "Date(1725875745000+0100)",
        "Value": 39,
        "Trend": "NotComputable"
    },
    {
        "WT": "Date(1725875445000)",
        "ST": "Date(1725875445000)",
        "DT": "Date(1725875445000+0100)",
        "Value": 52,
        "Trend": "NotComputable"
    }
]
```

*/