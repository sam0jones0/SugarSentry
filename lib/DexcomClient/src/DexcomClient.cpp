#include "dexcom_client.h"
#include "dexcom_utils.h"
#include <ArduinoJson.h>
#include <ISecureClient.h>
#include <sstream>
#include <chrono>
#include <thread>

DexcomClient::DexcomClient(ISecureClient &client,
                           const std::string &password,
                           const std::string &account_id,
                           const std::string &username,
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

std::string DexcomClient::post(const std::string &endpoint, const std::string &params, const std::string &json)
{
    std::string url = _base_url + "/" + endpoint;
    if (!params.empty())
    {
        url += "?" + params;
    }

    constexpr uint32_t TIMEOUT_MS = 10000;
    _client.setTimeout(TIMEOUT_MS);

    if (!_client.connect(_base_url.c_str(), 443))
    {
        return "Connection failed";
    }

    std::ostringstream request;
    request << "POST " << url << " HTTP/1.1\r\n";
    request << "Host: " << _base_url << "\r\n";
    request << "Accept-Encoding: application/json\r\n";
    request << "Content-Type: application/json\r\n";
    request << "Connection: close\r\n";
    if (!json.empty())
    {
        request << "Content-Length: " << json.length() << "\r\n\r\n";
        request << json << "\r\n";
    }
    else
    {
        request << "\r\n";
    }

    std::string requestStr = request.str();
    _client.write(reinterpret_cast<const uint8_t *>(requestStr.c_str()), requestStr.length());

    std::string response;
    while (_client.connected() && _client.available())
    {
        response += static_cast<char>(_client.read());
    }

    _client.stop();
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

    const char *code = doc["Code"];
    const char *message = doc["Message"];

    if (code)
    {
        if (strcmp(code, "SessionIdNotFound") == 0)
        {
            return SessionError(DexcomErrors::SessionError::NOT_FOUND);
        }
        else if (strcmp(code, "SessionNotValid") == 0)
        {
            return SessionError(DexcomErrors::SessionError::INVALID);
        }
        else if (strcmp(code, "SSO_AuthenticateMaxAttemptsExceeed") == 0)
        {
            return AccountError(DexcomErrors::AccountError::MAX_ATTEMPTS);
        }
        else if (strcmp(code, "SSO_InternalError") == 0)
        {
            if (message && strstr(message, "Cannot Authenticate"))
            {
                return AccountError(DexcomErrors::AccountError::FAILED_AUTHENTICATION);
            }
        }
        else if (strcmp(code, "InvalidArgument") == 0)
        {
            if (message && strstr(message, "accountName"))
            {
                return ArgumentError(DexcomErrors::ArgumentError::USERNAME_INVALID);
            }
            else if (message && strstr(message, "password"))
            {
                return ArgumentError(DexcomErrors::ArgumentError::PASSWORD_INVALID);
            }
            else if (message && strstr(message, "UUID"))
            {
                return ArgumentError(DexcomErrors::ArgumentError::ACCOUNT_ID_INVALID);
            }
        }
    }

    return std::nullopt;
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

std::vector<std::string> DexcomClient::get_glucose_readingsRaw(uint16_t minutes, uint16_t max_count)
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

    constexpr size_t MAX_RESPONSE_SIZE = 100000; // Adjust this value as needed
    if (response.length() > MAX_RESPONSE_SIZE)
    {
        throw ArgumentError(DexcomErrors::ArgumentError::RESPONSE_TOO_LARGE);
    }

    std::vector<std::string> readings;
    constexpr size_t JSON_BUFFER_SIZE = 40960; // TODO: DynamicJsonDocument? Currently 40kb is max theoretical size. This is wasteful for smaller requests.
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError err = deserializeJson(doc, response);

    if (err)
    {
        return readings;
    }

    JsonArray array = doc.as<JsonArray>();
    readings.reserve(array.size());
    for (JsonVariant v : array)
    {
        readings.push_back(v.as<std::string>());
    }

    return readings;
}

std::vector<GlucoseReading> DexcomClient::get_glucose_readings(uint16_t minutes, uint16_t max_count)
{
    std::vector<GlucoseReading> readings;

    auto processRawReadings = [this, &readings](const std::vector<std::string> &raw_readings)
    {
        for (const auto &raw : raw_readings)
        {
            StaticJsonDocument<DexcomConst::MAX_READING_JSON_SIZE> doc;
            DeserializationError err = deserializeJson(doc, raw);
            if (!err)
            {
                readings.emplace_back(doc.as<JsonObjectConst>());
            }
        }
    };

    try
    {
        auto raw_readings = get_glucose_readingsRaw(minutes, max_count);
        processRawReadings(raw_readings);
    }
    catch (SessionError &)
    {
        createSession();
        auto raw_readings = get_glucose_readingsRaw(minutes, max_count);
        processRawReadings(raw_readings);
    }

    return readings;
}

std::optional<GlucoseReading> DexcomClient::getLatestGlucoseReading()
{
    auto readings = get_glucose_readings(DexcomConst::MAX_MINUTES, 1);
    return readings.empty() ? std::nullopt : std::make_optional(readings[0]);
}

std::optional<GlucoseReading> DexcomClient::getCurrentGlucoseReading()
{
    auto readings = get_glucose_readings(10, 1);
    return readings.empty() ? std::nullopt : std::make_optional(readings[0]);
}