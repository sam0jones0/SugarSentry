#include <unity.h>
#include "arduino_json_parser.h"
#include "dexcom_errors.h"
#include "dexcom_constants.h"
#include <memory>

std::unique_ptr<ArduinoJsonParser> parser;

// These need to be global (outside any namespace) for Unity to find them
void setUp(void) {
    parser = std::make_unique<ArduinoJsonParser>();
}

void tearDown(void) {
    parser.reset();
}

namespace test_arduino_json_parser {

    void test_parse_single_glucose_reading() {
        const char* json = "{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\"}";
        
        auto result = parser->parseGlucoseReading(json);
        TEST_ASSERT_TRUE(result.has_value());
        
        auto reading = result.value();
        TEST_ASSERT_EQUAL(120, reading.getValue());
        TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, reading.getTrend());
        TEST_ASSERT_EQUAL(1609459200, reading.getTimestamp());
    }

    void test_parse_invalid_glucose_reading() {
        const char* json = "{\"Value\":\"invalid\",\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\"}";
        auto result = parser->parseGlucoseReading(json);
        TEST_ASSERT_FALSE(result.has_value());
    }

    void test_parse_multiple_glucose_readings() {
        const char* json = "["
            "{\"Value\": 120, \"Trend\": \"Flat\", \"WT\": \"Date(1609459200000)\"},"
            "{\"Value\": 115, \"Trend\": \"FortyFiveDown\", \"WT\": \"Date(1609455600000)\"}"
            "]";

        auto readings = parser->parseGlucoseReadings(json);
        TEST_ASSERT_EQUAL(2, readings.size());
        
        TEST_ASSERT_EQUAL(120, readings[0].getValue());
        TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
        TEST_ASSERT_EQUAL(1609459200, readings[0].getTimestamp());
        
        TEST_ASSERT_EQUAL(115, readings[1].getValue());
        TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::FortyFiveDown, readings[1].getTrend());
        TEST_ASSERT_EQUAL(1609455600, readings[1].getTimestamp());
    }

    void test_parse_error_response_session_not_found() {
        const char* json = "{\"Code\":\"SessionIdNotFound\",\"Message\":\"Session ID is invalid\"}";
        auto error = parser->parseErrorResponse(json);
        
        TEST_ASSERT_NOT_NULL(error.get());
        auto* sessionError = dynamic_cast<const SessionError*>(error.get());
        TEST_ASSERT_NOT_NULL(sessionError);
        TEST_ASSERT_EQUAL_STRING("Session not found", sessionError->what());
    }

    void test_parse_error_response_invalid_credentials() {
        const char* json = "{\"Code\":\"SSO_InternalError\",\"Message\":\"Cannot Authenticate\"}";
        auto error = parser->parseErrorResponse(json);
        
        TEST_ASSERT_NOT_NULL(error.get());
        auto* accountError = dynamic_cast<const AccountError*>(error.get());
        TEST_ASSERT_NOT_NULL(accountError);
        TEST_ASSERT_EQUAL_STRING("Failed authentication", accountError->what());
    }

    void test_parse_error_response_invalid_json() {
        const char* json = "invalid json";
        auto error = parser->parseErrorResponse(json);
        TEST_ASSERT_NULL(error.get());
    }

    void test_parse_empty_array() {
        const char* json = "[]";
        auto readings = parser->parseGlucoseReadings(json);
        TEST_ASSERT_EQUAL(0, readings.size());
    }

    void test_parse_malformed_array() {
        const char* json = "[{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\",}]"; // Extra comma
        auto readings = parser->parseGlucoseReadings(json);
        TEST_ASSERT_EQUAL(0, readings.size());
    }

    void test_parse_invalid_trend() {
        const char* json = "{\"Value\":120,\"Trend\":\"InvalidTrend\",\"WT\":\"Date(1609459200000)\"}";
        auto result = parser->parseGlucoseReading(json);
        TEST_ASSERT_TRUE(result.has_value());
        TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::None, result.value().getTrend());
    }

    void test_parse_invalid_timestamp() {
        const char* json = "{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"InvalidDate\"}";
        auto result = parser->parseGlucoseReading(json);
        TEST_ASSERT_TRUE(result.has_value());
        TEST_ASSERT_EQUAL(0, result.value().getTimestamp());
    }

    void test_parse_missing_fields() {
        const char* json = "{\"Value\":120}"; // Missing Trend and WT
        auto result = parser->parseGlucoseReading(json);
        TEST_ASSERT_FALSE(result.has_value());
    }

    void test_parse_max_size_array() {
        // Create a large array of readings
        std::string json = "[";
        for (int i = 0; i < 10; i++) { // Reduced size for testing
            if (i > 0) json += ",";
            json += "{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\"}";
        }
        json += "]";

        auto readings = parser->parseGlucoseReadings(json);
        TEST_ASSERT_EQUAL(10, readings.size());
    }

} // namespace test_arduino_json_parser

void run_arduino_json_parser_tests() {
    RUN_TEST(test_arduino_json_parser::test_parse_single_glucose_reading);
    RUN_TEST(test_arduino_json_parser::test_parse_invalid_glucose_reading);
    RUN_TEST(test_arduino_json_parser::test_parse_multiple_glucose_readings);
    RUN_TEST(test_arduino_json_parser::test_parse_error_response_session_not_found);
    RUN_TEST(test_arduino_json_parser::test_parse_error_response_invalid_credentials);
    RUN_TEST(test_arduino_json_parser::test_parse_error_response_invalid_json);
    RUN_TEST(test_arduino_json_parser::test_parse_empty_array);
    RUN_TEST(test_arduino_json_parser::test_parse_malformed_array);
    RUN_TEST(test_arduino_json_parser::test_parse_invalid_trend);
    RUN_TEST(test_arduino_json_parser::test_parse_invalid_timestamp);
    RUN_TEST(test_arduino_json_parser::test_parse_missing_fields);
    RUN_TEST(test_arduino_json_parser::test_parse_max_size_array);
}
