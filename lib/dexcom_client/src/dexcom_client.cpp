#include "dexcom_client.h"
#include "dexcom_utils.h"
#include <ArduinoJson.h>
#include <i_secure_client.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <string>
#include <algorithm>
#include "debug_print.h"

DexcomClient::DexcomClient(ISecureClient &client,
                           const std::string &username,
                           const std::string &account_id,
                           const std::string &password,
                           bool ous)
    : _client(client),
      _base_url(ous ? DexcomConst::DEXCOM_BASE_URL_OUS : DexcomConst::DEXCOM_BASE_URL),
      _password(password),
      _account_id(account_id),
      _username(username),
      _session_id(""),
      _connected(false)
{
    createSession();
}

DexcomClient::~DexcomClient()
{
    disconnect();
}

std::vector<GlucoseReading> DexcomClient::getGlucoseReadings(uint16_t minutes, uint16_t max_count)
{
    std::vector<GlucoseReading> readings;

    auto fetchAndParseReadings = [this, minutes, max_count, &readings]()
    {
        std::string response = getGlucoseReadingsRaw(minutes, max_count);
        readings = parseGlucoseReadings(response);
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

    if (response.substr(0, 11) == "HTTP Error:" || response == "Max retries reached")
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

    if (response.substr(0, 11) == "HTTP Error:" || response == "Max retries reached")
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
    for (int attempt = 0; attempt < DexcomConst::MAX_POST_RETRIES; ++attempt)
    {
        if (!ensureConnected())
        {
            DEBUG_PRINT("Connection failed");
            continue;
        }

        std::string url = "/ShareWebServices/Services/" + endpoint;
        if (!params.empty())
        {
            url += "?" + params;
        }

        DEBUG_PRINTF("Attempt %d: Sending request to %s\n", attempt + 1, url.c_str());
        DEBUG_PRINTF("Headers:\nHost: %s\nContent-Type: application/json\n", _base_url.c_str());
        if (!json.empty())
        {
            DEBUG_PRINTF("Body: %s\n", json.c_str());
        }

        _client.println("POST " + url + " HTTP/1.1");
        _client.println("Host: " + _base_url);
        _client.println("Content-Type: application/json");
        _client.println("Connection: keep-alive");
        if (!json.empty())
        {
            _client.println("Content-Length: " + std::to_string(json.length()));
            _client.println();
            _client.println(json);
        }
        else
        {
            _client.println();
        }

        std::string statusLine = _client.readStringUntil('\n');
        int statusCode = parseHttpStatusCode(statusLine);

        DEBUG_PRINTF("Received status code: %d\n", statusCode);

        if (statusCode == 200)
        {
            std::string response;
            while (_client.connected())
            {
                std::string line = _client.readStringUntil('\n');
                if (line == "\r")
                {
                    break;
                }
            }
            while (_client.available())
            {
                char c = _client.read();
                response += c;
            }
            DEBUG_PRINTF("Response: %s\n", response.c_str());
            return response;
        }
        else if (statusCode == 500)
        {
            DEBUG_PRINT("Received 500 error, retrying...");
            disconnect();
            PLATFORM_DELAY(1000 * (attempt + 1)); // Exponential backoff
        }
        else
        {
            disconnect();
            return "HTTP Error: " + std::to_string(statusCode);
        }
    }
    return "Max retries reached";
}

bool DexcomClient::ensureConnected()
{
    if (!_connected)
    {
        _connected = _client.connect(_base_url.c_str(), 443);
        if (_connected)
        {
            DEBUG_PRINT("Connected to server");
        }
        else
        {
            DEBUG_PRINT("Failed to connect to server");
        }
    }
    return _connected;
}

void DexcomClient::disconnect()
{
    if (_connected)
    {
        _client.stop();
        _connected = false;
        DEBUG_PRINT("Disconnected from server");
    }
}

int DexcomClient::parseHttpStatusCode(const std::string &statusLine)
{
    size_t pos = statusLine.find(' ');
    if (pos != std::string::npos)
    {
        std::string codeStr = statusLine.substr(pos + 1, 3);
        return std::stoi(codeStr);
    }
    return 0;
}

std::optional<DexcomError> DexcomClient::handleResponse(const std::string &response)
{
    constexpr size_t ERROR_JSON_SIZE = 512;
    StaticJsonDocument<ERROR_JSON_SIZE> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error)
    {
        return std::nullopt;
    }

    std::string code = doc["Code"].as<std::string>();
    std::string message = doc["Message"].as<std::string>();

    if (!code.empty())
    {
        if (code == "SessionIdNotFound")
        {
            return SessionError(DexcomErrors::SessionError::NOT_FOUND);
        }
        else if (code == "SessionNotValid")
        {
            return SessionError(DexcomErrors::SessionError::INVALID);
        }
        else if (code == "SSO_AuthenticateMaxAttemptsExceeed")
        {
            return AccountError(DexcomErrors::AccountError::MAX_ATTEMPTS);
        }
        else if (code == "SSO_InternalError")
        {
            if (message.find("Cannot Authenticate") != std::string::npos)
            {
                return AccountError(DexcomErrors::AccountError::FAILED_AUTHENTICATION);
            }
        }
        else if (code == "InvalidArgument")
        {
            if (message.find("accountName") != std::string::npos)
            {
                return ArgumentError(DexcomErrors::ArgumentError::USERNAME_INVALID);
            }
            else if (message.find("password") != std::string::npos)
            {
                return ArgumentError(DexcomErrors::ArgumentError::PASSWORD_INVALID);
            }
            else if (message.find("UUID") != std::string::npos)
            {
                return ArgumentError(DexcomErrors::ArgumentError::ACCOUNT_ID_INVALID);
            }
        }
    }

    return std::nullopt;
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

    if (response.substr(0, 11) == "HTTP Error:")
    {
        throw SessionError(DexcomErrors::SessionError::INVALID);
    }

    return response;
}

std::vector<GlucoseReading> DexcomClient::parseGlucoseReadings(const std::string &response)
{
    std::vector<GlucoseReading> readings;
    readings.reserve(DexcomConst::MAX_MAX_COUNT);

    DEBUG_PRINT("Parsing glucose readings. Raw response:");
    DEBUG_PRINT(response.c_str());

    StaticJsonDocument<256> doc;
    size_t pos = 0;
    while (pos < response.length())
    {
        size_t start = response.find('{', pos);
        if (start == std::string::npos)
        {
            DEBUG_PRINT("No more JSON objects found in response");
            break;
        }

        size_t end = response.find('}', start);
        if (end == std::string::npos)
        {
            DEBUG_PRINT("Incomplete JSON object found in response");
            break;
        }

        std::string jsonObject = response.substr(start, end - start + 1);
        DEBUG_PRINT("Parsing JSON object: ");
        DEBUG_PRINT(jsonObject.c_str());

        DeserializationError err = deserializeJson(doc, jsonObject);
        if (err == DeserializationError::Ok)
        {
            readings.emplace_back(doc.as<JsonObjectConst>());
            DEBUG_PRINT("Successfully parsed glucose reading");
        }
        else
        {
            DEBUG_PRINT("Failed to parse JSON object: ");
            DEBUG_PRINT(err.c_str());
        }
        doc.clear();
        pos = end + 1;
    }

    DEBUG_PRINT("Total glucose readings parsed: ");
    DEBUG_PRINTF("%d\n", readings.size());

    return readings;
}