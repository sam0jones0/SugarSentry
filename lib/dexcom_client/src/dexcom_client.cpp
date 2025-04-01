#include <sstream>
#include <chrono>
#include <thread>
#include <string>
#include <algorithm>
#include <memory>
#include <map>

#include "dexcom_client.h"
#include "dexcom_utils.h"
#include <debug_print.h>

// Platform-specific delay macro
#ifndef PLATFORM_DELAY
#ifdef ARDUINO
#define PLATFORM_DELAY(ms) delay(ms)
#else
#include <chrono>
#include <thread>
#define PLATFORM_DELAY(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#endif
#endif

DexcomClient::DexcomClient(std::shared_ptr<IHttpClient> httpClient,
                           std::shared_ptr<IGlucoseReadingParser> glucoseParser,
                           const std::string &username,
                           const std::string &account_id,
                           const std::string &password,
                           bool ous)
    : _httpClient(std::move(httpClient)),
      _glucoseParser(std::move(glucoseParser)),
      _base_url(ous ? DexcomConst::DEXCOM_BASE_URL_OUS : DexcomConst::DEXCOM_BASE_URL),
      _password(password),
      _account_id(account_id),
      _username(username),
      _session_id("")
{
    // Try to connect first
    if (!_httpClient->connect(_base_url, 443)) {
        DEBUG_PRINT("Initial connection failed");
        // Don't throw here, let createSession handle the error
    }
    createSession(); // This will handle both connection and authentication errors
}

DexcomClient::~DexcomClient() = default;

std::vector<GlucoseReading> DexcomClient::getGlucoseReadings(uint16_t minutes, uint16_t max_count)
{
    std::vector<GlucoseReading> readings;

    auto fetchAndParseReadings = [this, minutes, max_count, &readings]()
    {
        std::string response = getGlucoseReadingsRaw(minutes, max_count);
        readings = _glucoseParser->parse(response);
    };

    try
    {
        fetchAndParseReadings();
    }
    catch (SessionError &)
    {
        createSession();
        fetchAndParseReadings();
    }

    return readings;
}

std::optional<GlucoseReading> DexcomClient::getLatestGlucoseReading()
{
    auto readings = getGlucoseReadings(DexcomConst::MAX_MINUTES, 1);
    return readings.empty() ? std::nullopt : std::make_optional(readings[0]);
}

std::optional<GlucoseReading> DexcomClient::getCurrentGlucoseReading()
{
    auto readings = getGlucoseReadings(10, 1);
    return readings.empty() ? std::nullopt : std::make_optional(readings[0]);
}

void DexcomClient::createSession()
{
    int retries = 0;

    while (retries < DexcomConst::MAX_CONNECT_RETRIES)
    {
        try
        {
            if (_account_id.empty())
            {
                if (_username.empty())
                {
                    throw ArgumentError(DexcomErrors::ArgumentError::USERNAME_INVALID);
                }
                _account_id = getAccountId();
            }

            if (_account_id.empty() || _account_id == DexcomConst::DEFAULT_UUID)
            {
                throw ArgumentError(DexcomErrors::ArgumentError::ACCOUNT_ID_INVALID);
            }

            _session_id = getSessionId();

            if (_session_id.empty() || _session_id == DexcomConst::DEFAULT_UUID)
            {
                throw ArgumentError(DexcomErrors::ArgumentError::SESSION_ID_INVALID);
            }

            DEBUG_PRINT("Session created successfully");
            DEBUG_PRINTF("Session ID: %s\n", _session_id.c_str());
            return;
        }
        catch (const DexcomError &e)
        {
            retries++;
            if (retries >= DexcomConst::MAX_CONNECT_RETRIES)
            {
                throw;
            }
            PLATFORM_DELAY(1000 * retries);
        }
    }
}

std::string DexcomClient::getAccountId()
{
    std::string json = "{\"accountName\":\"" + _username + "\",\"password\":\"" + _password + "\",\"applicationId\":\"" + DexcomConst::DEXCOM_APPLICATION_ID + "\"}";
    std::string response = post(DexcomConst::DEXCOM_AUTHENTICATE_ENDPOINT, "", json);

    if (response.substr(0, 11) == "HTTP Error:" || response == "Connection Error")
    {
        throw AccountError(DexcomErrors::AccountError::FAILED_AUTHENTICATION);
    }

    response.erase(std::remove_if(response.begin(), response.end(), [](char c)
                                  { return std::isspace(c) || c == '\"'; }),
                   response.end());

    return response;
}

std::string DexcomClient::getSessionId()
{
    std::string json = "{\"accountId\":\"" + _account_id + "\",\"password\":\"" + _password + "\",\"applicationId\":\"" + DexcomConst::DEXCOM_APPLICATION_ID + "\"}";
    std::string response = post(DexcomConst::DEXCOM_LOGIN_ID_ENDPOINT, "", json);

    if (response.substr(0, 11) == "HTTP Error:" || response == "Connection Error")
    {
        throw SessionError(DexcomErrors::SessionError::INVALID);
    }

    response.erase(std::remove_if(response.begin(), response.end(), [](char c)
                                  { return std::isspace(c) || c == '\"'; }),
                   response.end());

    return response;
}

std::string DexcomClient::post(const std::string &endpoint, const std::string &params, const std::string &json)
{
    // Check if connected first
    if (!_httpClient->isConnected()) {
        // Try to connect
        if (!_httpClient->connect(_base_url, 443)) {
            DEBUG_PRINT("Connection failed");
            throw SessionError(DexcomErrors::SessionError::INVALID);
        }
    }

    // Construct the URL
    std::string url = "/ShareWebServices/Services/" + endpoint;
    if (!params.empty()) {
        url += "?" + params;
    }

    DEBUG_PRINTF("Sending request to %s\n", url.c_str());

    // Prepare headers
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Connection", "keep-alive"}
    };

    try {
        // Make a single call to _httpClient->post
        HttpResponse response = _httpClient->post(url, json, headers);

        DEBUG_PRINTF("Received status code: %d\n", response.statusCode);

        // Return the body for successful responses
        if (response.statusCode == 200) {
            DEBUG_PRINTF("Response: %s\n", response.body.c_str());
            return response.body;
        } 
        // Throw appropriate exception for error status codes
        else if (response.statusCode == 401) {
            throw AccountError(DexcomErrors::AccountError::FAILED_AUTHENTICATION);
        } 
        else if (response.statusCode == 500) {
            throw SessionError(DexcomErrors::SessionError::INVALID);
        }
        // For any other non-200 status code, throw a generic SessionError
        else {
            throw SessionError(DexcomErrors::SessionError::INVALID);
        }
    } 
    // Re-throw DexcomError exceptions
    catch (const DexcomError& e) {
        throw;
    } 
    // For other exceptions, throw a SessionError
    catch (const std::exception& e) {
        DEBUG_PRINTF("Request failed: %s\n", e.what());
        throw SessionError(DexcomErrors::SessionError::INVALID);
    }
}

std::string DexcomClient::getGlucoseReadingsRaw(uint16_t minutes, uint16_t max_count)
{
    if (minutes == 0 || minutes > DexcomConst::MAX_MINUTES)
    {
        throw ArgumentError(DexcomErrors::ArgumentError::MINUTES_INVALID);
    }
    if (max_count == 0 || max_count > DexcomConst::MAX_MAX_COUNT)
    {
        throw ArgumentError(DexcomErrors::ArgumentError::MAX_COUNT_INVALID);
    }

    std::string params = "sessionId=" + _session_id + "&minutes=" + std::to_string(minutes) + "&maxCount=" + std::to_string(max_count);
    std::string response = post(DexcomConst::DEXCOM_GLUCOSE_READINGS_ENDPOINT, params);

    if (response.substr(0, 11) == "HTTP Error:" || response == "Connection Error")
    {
        throw SessionError(DexcomErrors::SessionError::INVALID);
    }

    return response;
}
