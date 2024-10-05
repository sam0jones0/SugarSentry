#ifndef DEXCOM_CLIENT_H
#define DEXCOM_CLIENT_H

#include <ISecureClient.h>
#include <vector>
#include <optional>
#include <string>
#include "dexcom_constants.h"
#include "dexcom_errors.h"
#include "glucose_reading.h"

class DexcomClient
{
public:
    DexcomClient(ISecureClient &client,
                 const std::string &password,
                 const std::string &account_id = "",
                 const std::string &username = "",
                 bool ous = false);

    std::vector<GlucoseReading> getGlucoseReadings(uint16_t minutes = DexcomConst::MAX_MINUTES,
                                                   uint16_t max_count = DexcomConst::MAX_MAX_COUNT);

    std::optional<GlucoseReading> getLatestGlucoseReading();
    std::optional<GlucoseReading> getCurrentGlucoseReading();
    std::vector<GlucoseReading> parseGlucoseReadings(const std::string &response);

private:
    ISecureClient &_client;
    std::string _base_url;
    std::string _password;
    std::string _account_id;
    std::string _username;
    std::string _session_id;

    std::string post(const std::string &endpoint,
                     const std::string &params = "",
                     const std::string &json = "");

    std::optional<DexcomError> handleResponse(const std::string &response);

    std::string getAccountId();
    std::string getSessionId();

    void createSession();

    std::string getGlucoseReadingsRaw(uint16_t minutes = DexcomConst::MAX_MINUTES, uint16_t max_count = DexcomConst::MAX_MAX_COUNT);
};

#endif // DEXCOM_CLIENT_H