#include "arduino_json_parser.h"
#include <debug_print.h>
#include <memory>

std::optional<GlucoseReading> ArduinoJsonParser::parseGlucoseReading(const std::string& jsonString) {
    StaticJsonDocument<GLUCOSE_JSON_SIZE> doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        DEBUG_PRINT("Failed to parse JSON object: ");
        DEBUG_PRINT(error.c_str());
        return std::nullopt;
    }

    auto obj = doc.as<JsonObjectConst>();
    
    // Validate required fields exist and have correct types
    // Check all required fields exist
    if (!obj.containsKey("Value") || !obj["Value"].is<int>() ||
        !obj.containsKey("Trend") || !obj["Trend"].is<const char*>() ||
        !obj.containsKey("WT") || !obj["WT"].is<const char*>()) {
        DEBUG_PRINT("Missing required fields");
        return std::nullopt;
    }

    // Get value with bounds checking
    int value = obj["Value"].as<int>();
    if (value < 0 || value > 1000) { // Reasonable glucose range
        DEBUG_PRINT("Glucose value out of reasonable range");
        return std::nullopt;
    }

    const char* trend = obj["Trend"].as<const char*>();
    const char* timestamp = obj["WT"].as<const char*>();

    return GlucoseReading(
        static_cast<uint16_t>(value),
        trend,
        timestamp
    );
}

std::vector<GlucoseReading> ArduinoJsonParser::parseGlucoseReadings(const std::string& response) {
    std::vector<GlucoseReading> readings;
    readings.reserve(DexcomConst::MAX_MAX_COUNT);

    DEBUG_PRINT("Parsing glucose readings. Raw response:");
    DEBUG_PRINT(response.c_str());

    StaticJsonDocument<GLUCOSE_ARRAY_SIZE> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        DEBUG_PRINT("Failed to parse JSON array: ");
        DEBUG_PRINT(error.c_str());
        return readings;
    }

    // Handle single object case
    if (doc.is<JsonObject>()) {
        if (auto reading = parseGlucoseReading(response)) {
            readings.push_back(*reading);
        }
        return readings;
    }

    // Handle array case
    if (!doc.is<JsonArray>()) {
        DEBUG_PRINT("JSON is neither an object nor an array");
        return readings;
    }

    JsonArray array = doc.as<JsonArray>();
    for (JsonVariant value : array) {
        if (readings.size() >= DexcomConst::MAX_MAX_COUNT) {
            DEBUG_PRINT("Maximum number of readings reached");
            break;
        }

        if (!value.is<JsonObject>()) {
            DEBUG_PRINT("Invalid array element - not an object");
            continue;
        }

        JsonObject obj = value.as<JsonObject>();
        
        // Validate required fields
        if (!obj.containsKey("Value") || !obj["Value"].is<int>()) {
            DEBUG_PRINT("Missing or invalid Value field in array element");
            continue;
        }

        int glucoseValue = obj["Value"].as<int>();
        if (glucoseValue < 0 || glucoseValue > 1000) {
            DEBUG_PRINT("Glucose value out of reasonable range");
            continue;
        }

        const char* trend = obj.containsKey("Trend") ? obj["Trend"].as<const char*>() : "None";
        const char* timestamp = obj.containsKey("WT") ? obj["WT"].as<const char*>() : "Date(0)";

        readings.push_back(GlucoseReading(
            static_cast<uint16_t>(glucoseValue),
            trend,
            timestamp
        ));
    }

    DEBUG_PRINT("Total glucose readings parsed: ");
    DEBUG_PRINTF("%d\n", readings.size());

    return readings;
}

std::unique_ptr<DexcomError> ArduinoJsonParser::parseErrorResponse(const std::string& response) {
    StaticJsonDocument<ERROR_JSON_SIZE> doc;
    DeserializationError jsonError = deserializeJson(doc, response);

    if (jsonError) {
        DEBUG_PRINT("Failed to parse error response JSON: ");
        DEBUG_PRINT(jsonError.c_str());
        return nullptr;
    }

    if (!doc.is<JsonObject>()) {
        DEBUG_PRINT("Error response is not a JSON object");
        return nullptr;
    }

    JsonObject obj = doc.as<JsonObject>();
    DEBUG_PRINT("Parsed JSON object. Checking fields...");

    if (!obj.containsKey("Code")) {
        DEBUG_PRINT("Missing Code field");
        return nullptr;
    }
    if (!obj.containsKey("Message")) {
        DEBUG_PRINT("Missing Message field");
        return nullptr;
    }

    DEBUG_PRINT("Code field value: ");
    DEBUG_PRINT(obj["Code"].as<const char*>());
    DEBUG_PRINT("Message field value: ");
    DEBUG_PRINT(obj["Message"].as<const char*>());

    std::string code = obj["Code"].as<const char*>();
    std::string message = obj["Message"].as<const char*>();

    // Create and return the appropriate error type based on the code
    if (code == "SessionIdNotFound") {
        return std::make_unique<SessionError>(DexcomErrors::SessionError::NOT_FOUND);
    }
    else if (code == "SessionNotValid") {
        return std::make_unique<SessionError>(DexcomErrors::SessionError::INVALID);
    }
    // Handle authentication errors
    else if (code == "SSO_AuthenticateMaxAttemptsExceeed") {
        return std::make_unique<AccountError>(DexcomErrors::AccountError::MAX_ATTEMPTS);
    }
    else if (code == "SSO_InternalError" && message.find("Cannot Authenticate") != std::string::npos) {
        return std::make_unique<AccountError>(DexcomErrors::AccountError::FAILED_AUTHENTICATION);
    }
    // Handle argument errors
    else if (code == "InvalidArgument") {
        if (message.find("accountName") != std::string::npos) {
            return std::make_unique<ArgumentError>(DexcomErrors::ArgumentError::USERNAME_INVALID);
        }
        else if (message.find("password") != std::string::npos) {
            return std::make_unique<ArgumentError>(DexcomErrors::ArgumentError::PASSWORD_INVALID);
        }
        else if (message.find("UUID") != std::string::npos) {
            return std::make_unique<ArgumentError>(DexcomErrors::ArgumentError::ACCOUNT_ID_INVALID);
        }
    }

    return nullptr;
}
