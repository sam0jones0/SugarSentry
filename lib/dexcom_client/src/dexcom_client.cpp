#include "dexcom_client.h"
#include "dexcom_utils.h"
#include <ArduinoJson.h>
#include <i_secure_client.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <string>

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
      _session_id("")
{
    createSession();
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
    constexpr int MAX_RETRIES = 3;
    int retries = 0;

    while (retries < MAX_RETRIES)
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

            // session creation was successful
            return;
        }
        catch (const DexcomError &e)
        {
            retries++;
            if (retries >= MAX_RETRIES)
            {
                throw; // rethrow last error if exhausted retries
            }
            std::this_thread::sleep_for(std::chrono::seconds(retries));
        }
    }
}

std::string DexcomClient::getAccountId()
{
    std::string json = "{\"accountName\":\"" + _username + "\",\"password\":\"" + _password + "\",\"applicationId\":\"" + DexcomConst::DEXCOM_APPLICATION_ID + "\"}";
    std::string response = post(DexcomConst::DEXCOM_AUTHENTICATE_ENDPOINT, "", json);

    auto error = handleResponse(response);
    if (error)
    {
        throw *error;
    }

    return response;
}

std::string DexcomClient::getSessionId()
{
    std::string json = "{\"accountId\":\"" + _account_id + "\",\"password\":\"" + _password + "\",\"applicationId\":\"" + DexcomConst::DEXCOM_APPLICATION_ID + "\"}";
    std::string response = post(DexcomConst::DEXCOM_LOGIN_ID_ENDPOINT, "", json);

    auto error = handleResponse(response);
    if (error)
    {
        throw *error;
    }

    return response;
}

std::string DexcomClient::post(const std::string &endpoint, const std::string &params, const std::string &json)
{
    std::string url = "/ShareWebServices/Services/" + endpoint;
    if (!params.empty())
    {
        url += "?" + params;
    }

    Serial.println("Attempting to connect...");
    if (!_client.connect(_base_url.c_str(), 443))
    {
        Serial.println("Connection failed");
        return "Connection failed";
    }
    Serial.println("Connected successfully");

    Serial.println("Sending request...");
    _client.println("POST " + url + " HTTP/1.1");
    _client.println("Host: " + _base_url);
    _client.println("Accept-Encoding: application/json");
    _client.println("Content-Type: application/json");
    _client.println("Connection: close");
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

    Serial.println("Reading response...");
    std::string response;
    while (_client.connected())
    {
        std::string line = _client.readStringUntil('\n');
        if (line == "\r")
        {
            Serial.println("Headers received");
            break;
        }
    }

    while (_client.available())
    {
        char c = _client.read();
        response += c;
    }

    Serial.println("Response received");
    _client.stop();
    Serial.println("Raw response:");
    Serial.println(response.c_str());
    return response;
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

    auto error = handleResponse(response);
    if (error)
    {
        throw *error;
    }

    return response;
}

std::vector<GlucoseReading> DexcomClient::parseGlucoseReadings(const std::string &response)
{
    std::vector<GlucoseReading> readings;
    readings.reserve(DexcomConst::MAX_MAX_COUNT);

    Serial.println("Parsing glucose readings. Raw response:");
    Serial.println(response.c_str());

    StaticJsonDocument<256> doc;
    size_t pos = 0;
    while (pos < response.length())
    {
        size_t start = response.find('{', pos);
        if (start == std::string::npos)
        {
            Serial.println("No more JSON objects found in response");
            break;
        }

        size_t end = response.find('}', start);
        if (end == std::string::npos)
        {
            Serial.println("Incomplete JSON object found in response");
            break;
        }

        std::string jsonObject = response.substr(start, end - start + 1);
        Serial.print("Parsing JSON object: ");
        Serial.println(jsonObject.c_str());

        DeserializationError err = deserializeJson(doc, jsonObject);
        if (err == DeserializationError::Ok)
        {
            readings.emplace_back(doc.as<JsonObjectConst>());
            Serial.println("Successfully parsed glucose reading");
        }
        else
        {
            Serial.print("Failed to parse JSON object: ");
            Serial.println(err.c_str());
        }
        doc.clear();
        pos = end + 1;
    }

    Serial.print("Total glucose readings parsed: ");
    Serial.println(readings.size());

    return readings;
}
