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
1. getAccountId

Request:
```
curl --location 'https://shareous1.dexcom.com/ShareWebServices/Services/General/AuthenticatePublisherAccount' \
--header 'Accept-Encoding: application/json' \
--header 'Content-Type: application/json' \
--data-raw '{
    "accountName": "OBFUSCATED_USERNAME",
    "password": "OBFUSCATED_PASSWORD",
    "applicationId": "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
}'
```

Response:
```json
"yyyyyyyy-yyyy-yyyy-yyyy-yyyyyyyyyyyy"
```

2. getSessionId

Request:
```
curl --location 'https://shareous1.dexcom.com/ShareWebServices/Services/General/LoginPublisherAccountById' \
--header 'Accept-Encoding: application/json' \
--header 'Content-Type: application/json' \
--data-raw '{
    "accountId": "yyyyyyyy-yyyy-yyyy-yyyy-yyyyyyyyyyyy",
    "password": "OBFUSCATED_PASSWORD",
    "applicationId": "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
}'
```

Response:
```json
"zzzzzzzz-zzzz-zzzz-zzzz-zzzzzzzzzzzz"
```

3. getGlucoseReadings

Request:
```
curl --location 'https://shareous1.dexcom.com/ShareWebServices/Services/Publisher/ReadPublisherLatestGlucoseValues?sessionId=zzzzzzzz-zzzz-zzzz-zzzz-zzzzzzzzzzzz&minutes=1440&maxCount=2' \
--header 'Accept-Encoding: application/json' \
--header 'Content-Type: application/json' \
--data-raw '{
    "accountName": "yyyyyyyy-yyyy-yyyy-yyyy-yyyyyyyyyyyy",
    "password": "OBFUSCATED_PASSWORD",
    "applicationId": "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
}'
```

Response:
```json
[
  {
    "DT": "/Date(1600000000000)/",
    "ST": "/Date(1600000000000)/",
    "Trend": 4,
    "Value": 120,
    "WT": "/Date(1600000000000)/"
  },
  {
    "DT": "/Date(1599999700000)/",
    "ST": "/Date(1599999700000)/",
    "Trend": 4,
    "Value": 118,
    "WT": "/Date(1599999700000)/"
  }
]
```

Flow:

1. getAccountId:
   - Input: Username, password, and application ID
   - Output: Account ID

2. getSessionId:
   - Input: Account ID (from step 1), password, and application ID
   - Output: Session ID

3. getGlucoseReadings:
   - Input: Session ID (from step 2), time range (minutes), and max count of readings
   - Output: Array of glucose readings, each containing:
     - DT: Device Time
     - ST: System Time
     - Trend: Glucose trend indicator
     - Value: Glucose value
     - WT: Wall Time

This flow allows you to:
1. Authenticate and get an account ID
2. Use the account ID to get a session ID
3. Use the session ID to fetch glucose readings
```

*/