#include "dexcom_client.h"
#include "dexcom_utils.h"
#include <ArduinoJson.h>

DexcomClient::DexcomClient(WiFiClientSecure &client,
                           const String &password,
                           const String &account_id,
                           const String &username,
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

String DexcomClient::post(const String &endpoint, const String &params, const String &json)
{
    String url = _base_url + "/" + endpoint;
    if (params.length() > 0)
    {
        url += "?" + params;
    }

    constexpr uint32_t TIMEOUT_MS = 10000;
    _client.setTimeout(TIMEOUT_MS);

    if (!_client.connect(_base_url.c_str(), 443))
    {
        return "Connection failed";
    }

    _client.println("POST " + url + " HTTP/1.1");
    _client.println("Host: " + String(_base_url));
    _client.println("Accept-Encoding: application/json");
    _client.println("Content-Type: application/json");
    _client.println("Connection: close");
    if (json.length() > 0)
    {
        _client.println("Content-Length: " + String(json.length()));
        _client.println();
        _client.println(json);
    }
    else
    {
        _client.println();
    }

    // Wait for the response
    unsigned long start = millis();
    while (_client.connected() && !_client.available())
    {
        if (millis() - start > TIMEOUT_MS)
        {
            _client.stop();
            return "Response timeout";
        }
    }

    String response;
    while (_client.available())
    {
        response += (char)_client.read();
    }

    _client.stop();
    return response;
}

std::optional<DexcomError> DexcomClient::handleResponse(const String &response)
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

String DexcomClient::getAccountId()
{
    String json = "{\"accountName\":\"" + _username + "\",\"password\":\"" + _password + "\",\"applicationId\":\"" + DexcomConst::DEXCOM_APPLICATION_ID + "\"}";
    String response = post(DexcomConst::DEXCOM_AUTHENTICATE_ENDPOINT, "", json);

    auto error = handleResponse(response);
    if (error)
    {
        throw *error;
    }

    return response;
}

String DexcomClient::getSessionId()
{
    String json = "{\"accountId\":\"" + _account_id + "\",\"password\":\"" + _password + "\",\"applicationId\":\"" + DexcomConst::DEXCOM_APPLICATION_ID + "\"}";
    String response = post(DexcomConst::DEXCOM_LOGIN_ID_ENDPOINT, "", json);

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
            if (_account_id.isEmpty())
            {
                if (_username.isEmpty())
                {
                    throw ArgumentError(DexcomErrors::ArgumentError::USERNAME_INVALID);
                }
                _account_id = getAccountId();
            }

            if (_account_id.isEmpty() || _account_id == DexcomConst::DEFAULT_UUID)
            {
                throw ArgumentError(DexcomErrors::ArgumentError::ACCOUNT_ID_INVALID);
            }

            _session_id = getSessionId();

            if (_session_id.isEmpty() || _session_id == DexcomConst::DEFAULT_UUID)
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
            delay(1000 * retries);
        }
    }
}

std::vector<String> DexcomClient::getGlucoseReadingsRaw(uint16_t minutes, uint16_t max_count)
{
    if (minutes == 0 || minutes > DexcomConst::MAX_MINUTES)
    {
        throw ArgumentError(DexcomErrors::ArgumentError::MINUTES_INVALID);
    }
    if (max_count == 0 || max_count > DexcomConst::MAX_MAX_COUNT)
    {
        throw ArgumentError(DexcomErrors::ArgumentError::MAX_COUNT_INVALID);
    }

    String params = "sessionId=" + _session_id + "&minutes=" + String(minutes) + "&maxCount=" + String(max_count);
    String response = post(DexcomConst::DEXCOM_GLUCOSE_READINGS_ENDPOINT, params);

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

    std::vector<String> readings;
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
        readings.push_back(v.as<String>());
    }

    return readings;
}

std::vector<GlucoseReading> DexcomClient::getGlucoseReadings(uint16_t minutes, uint16_t max_count)
{
    std::vector<GlucoseReading> readings;

    auto processRawReadings = [this, &readings](const std::vector<String>& raw_readings) {
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
        auto raw_readings = getGlucoseReadingsRaw(minutes, max_count);
        processRawReadings(raw_readings);
    }
    catch (SessionError &)
    {
        createSession();
        auto raw_readings = getGlucoseReadingsRaw(minutes, max_count);
        processRawReadings(raw_readings);
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