#ifndef DEXCOM_CLIENT_H
#define DEXCOM_CLIENT_H

#include "i_http_client.h"

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <memory>

#include "dexcom_constants.h"
#include "dexcom_errors.h"
#include "glucose_reading.h"

/**
 * @file dexcom_client.h
 * @brief Defines the DexcomClient class for interacting with the Dexcom API.
 *
 * This class handles authentication, session management, and retrieval of glucose readings
 * from the Dexcom API. It uses an IHttpClient for network communication.
 */

class DexcomClient {
private:
    std::shared_ptr<IHttpClient> _httpClient;
    std::string _base_url;
    std::string _password;
    std::string _account_id;
    std::string _username;
    std::string _session_id;

    void createSession();
    std::string getAccountId();
    std::string getSessionId();
    std::string post(const std::string &endpoint,
                    const std::string &params = "",
                    const std::string &json = "");
    std::optional<DexcomError> handleResponse(const std::string &response);
    std::string getGlucoseReadingsRaw(uint16_t minutes = DexcomConst::MAX_MINUTES,
                                     uint16_t max_count = DexcomConst::MAX_MAX_COUNT);
    std::vector<GlucoseReading> parseGlucoseReadings(const std::string &response);

public:
    /**
     * @brief Constructs a DexcomClient and initializes a session with the Dexcom API.
     *
     * @param client An ISecureClient instance for network communication
     * @param password Dexcom account password
     * @param account_id Dexcom account ID
     * @param username Dexcom username
     * @param ous Whether to use the out-of-US server (default: false)
     *
     * @throws AccountError if authentication fails
     * @throws SessionError if session creation fails
     */
    DexcomClient(
        std::shared_ptr<IHttpClient> httpClient,
        const std::string& username = "",
        const std::string& account_id = "",
        const std::string& password = "",
        bool ous = false
    );

    ~DexcomClient();

    /**
     * @brief Retrieves glucose readings from the Dexcom API.
     *
     * @param minutes Number of historical minutes to retrieve (max 1440)
     * @param max_count Maximum number of readings to retrieve (max 288)
     * @return std::vector<GlucoseReading>
     *
     * @throws ArgumentError if parameters are invalid
     * @throws SessionError if the session is invalid
     */
    std::vector<GlucoseReading> getGlucoseReadings(uint16_t minutes = DexcomConst::MAX_MINUTES,
                                                   uint16_t max_count = DexcomConst::MAX_MAX_COUNT);

    /**
     * @brief Retrieves the latest glucose reading, within the last 24 hours.
     *
     * @return std::optional<GlucoseReading> The latest reading, or nullopt if none available
     *
     * @throws SessionError if the session is invalid
     */
    std::optional<GlucoseReading> getLatestGlucoseReading();

    /**
     * @brief Retrieves the current glucose reading (within last 10 minutes).
     *
     * @return std::optional<GlucoseReading> The current reading, or nullopt if none available
     *
     * @throws SessionError if the session is invalid
     */
    std::optional<GlucoseReading> getCurrentGlucoseReading();

};

#endif // DEXCOM_CLIENT_H
