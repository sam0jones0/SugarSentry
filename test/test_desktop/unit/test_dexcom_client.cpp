#include <unity.h>
#include "dexcom_client.h"
#include "mock_secure_client.h"
#include <memory>
#include <stdexcept>

// Global variables for testing
std::unique_ptr<MockSecureClient> mockClient;
std::unique_ptr<DexcomClient> dexcomClient;

void setUp(void)
{
    mockClient = std::make_unique<MockSecureClient>();
    mockClient->setConnected(true);
    // Set up a successful authentication response
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "\"test-session-id\"");
    dexcomClient = std::make_unique<DexcomClient>(*mockClient, "username", "account_id", "password", false);
}

void tearDown(void)
{
    dexcomClient.reset();
    mockClient.reset();
}

void test_dexcom_client_constructor()
{
    TEST_ASSERT_NOT_NULL(dexcomClient.get());
}

void test_dexcom_client_constructor_success()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "\"test-session-id\"");

    bool exceptionThrown = false;
    try
    {
        DexcomClient client(*mockClient, "username", "account_id", "password", false);
    }
    catch (...)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_FALSE(exceptionThrown);
}

void test_dexcom_client_constructor_auth_failure()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 401 Unauthorized\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{\"Code\":\"AccountError\",\"Message\":\"Authentication failed\"}");

    bool exceptionThrown = false;
    try
    {
        DexcomClient client(*mockClient, "username", "account_id", "wrong_password", false);
    }
    catch (const AccountError &)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void test_dexcom_client_get_glucose_readings_success()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "[{\"WT\":\"Date(1625874245000)\",\"ST\":\"Date(1625874245000)\",\"DT\":\"Date(1625874245000+0100)\",\"Value\":120,\"Trend\":\"Flat\"},"
        "{\"WT\":\"Date(1625873945000)\",\"ST\":\"Date(1625873945000)\",\"DT\":\"Date(1625873945000+0100)\",\"Value\":118,\"Trend\":\"FortyFiveUp\"}]");

    auto readings = dexcomClient->getGlucoseReadings();
    TEST_ASSERT_EQUAL(2, readings.size());
    TEST_ASSERT_EQUAL(120, readings[0].getValue());
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
    TEST_ASSERT_EQUAL(118, readings[1].getValue());
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::FortyFiveUp, readings[1].getTrend());
}

void test_dexcom_client_get_glucose_readings_empty()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "[]");

    auto readings = dexcomClient->getGlucoseReadings();
    TEST_ASSERT_EQUAL(0, readings.size());
}

void test_dexcom_client_get_glucose_readings_invalid_arguments()
{
    bool exceptionThrown;

    exceptionThrown = false;
    try
    {
        dexcomClient->getGlucoseReadings(0, 1);
    }
    catch (const ArgumentError &)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);

    exceptionThrown = false;
    try
    {
        dexcomClient->getGlucoseReadings(1, 0);
    }
    catch (const ArgumentError &)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);

    exceptionThrown = false;
    try
    {
        dexcomClient->getGlucoseReadings(DexcomConst::MAX_MINUTES + 1, 1);
    }
    catch (const ArgumentError &)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);

    exceptionThrown = false;
    try
    {
        dexcomClient->getGlucoseReadings(1, DexcomConst::MAX_MAX_COUNT + 1);
    }
    catch (const ArgumentError &)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void test_dexcom_client_get_latest_glucose_reading_success()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "[{\"WT\":\"Date(1625874245000)\",\"ST\":\"Date(1625874245000)\",\"DT\":\"Date(1625874245000+0100)\",\"Value\":120,\"Trend\":\"Flat\"}]");

    auto reading = dexcomClient->getLatestGlucoseReading();
    TEST_ASSERT_TRUE(reading.has_value());
    TEST_ASSERT_EQUAL(120, reading->getValue());
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, reading->getTrend());
}

void test_dexcom_client_get_latest_glucose_reading_empty()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "[]");

    auto reading = dexcomClient->getLatestGlucoseReading();
    TEST_ASSERT_FALSE(reading.has_value());
}

void test_dexcom_client_get_current_glucose_reading_success()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "[{\"WT\":\"Date(1625874245000)\",\"ST\":\"Date(1625874245000)\",\"DT\":\"Date(1625874245000+0100)\",\"Value\":120,\"Trend\":\"Flat\"}]");

    auto reading = dexcomClient->getCurrentGlucoseReading();
    TEST_ASSERT_TRUE(reading.has_value());
    TEST_ASSERT_EQUAL(120, reading->getValue());
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, reading->getTrend());
}

void test_dexcom_client_get_current_glucose_reading_empty()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "[]");

    auto reading = dexcomClient->getCurrentGlucoseReading();
    TEST_ASSERT_FALSE(reading.has_value());
}

void test_dexcom_client_get_glucose_readings_max_size()
{
    mockClient->setConnected(true);

    std::string largeResponse = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: application/json\r\n"
                                "\r\n"
                                "[";
    for (int i = 0; i < DexcomConst::MAX_MAX_COUNT; ++i)
    {
        if (i > 0)
            largeResponse += ",";
        largeResponse += "{\"WT\":\"Date(" + std::to_string(1625874245000 + i * 300000) + ")\","
                                                                                          "\"ST\":\"Date(" +
                         std::to_string(1625874245000 + i * 300000) + ")\","
                                                                      "\"DT\":\"Date(" +
                         std::to_string(1625874245000 + i * 300000) + "+0100)\","
                                                                      "\"Value\":" +
                         std::to_string(100 + i % 100) + ","
                                                         "\"Trend\":\"Flat\"}";
    }
    largeResponse += "]";

    mockClient->setNextReadData(largeResponse);

    auto readings = dexcomClient->getGlucoseReadings(DexcomConst::MAX_MINUTES, DexcomConst::MAX_MAX_COUNT);

    TEST_ASSERT_EQUAL(DexcomConst::MAX_MAX_COUNT, readings.size());
    TEST_ASSERT_EQUAL(100, readings[0].getValue());
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, readings[0].getTrend());
    TEST_ASSERT_EQUAL(187, readings[DexcomConst::MAX_MAX_COUNT - 1].getValue());
    TEST_ASSERT_EQUAL(DexcomConst::TrendDirection::Flat, readings[DexcomConst::MAX_MAX_COUNT - 1].getTrend());
}

void test_dexcom_client_get_glucose_readings_error_response()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{\"Code\":\"SessionNotValid\",\"Message\":\"Session ID is invalid\"}");

    bool exceptionThrown = false;
    try
    {
        dexcomClient->getGlucoseReadings();
    }
    catch (const SessionError &)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void test_dexcom_client_connection_failure()
{
    mockClient->setConnected(false);

    bool exceptionThrown = false;
    try
    {
        dexcomClient->getGlucoseReadings();
    }
    catch (const SessionError &)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void test_dexcom_client_get_glucose_readings_invalid_json()
{
    mockClient->setConnected(true);
    mockClient->setNextReadData(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "This is not valid JSON");

    auto readings = dexcomClient->getGlucoseReadings();
    TEST_ASSERT_EQUAL(0, readings.size());
}

void run_dexcom_client_tests()
{
    UNITY_BEGIN();
    RUN_TEST(test_dexcom_client_constructor);
    RUN_TEST(test_dexcom_client_constructor_success);
    RUN_TEST(test_dexcom_client_constructor_auth_failure);
    RUN_TEST(test_dexcom_client_get_glucose_readings_success);
    RUN_TEST(test_dexcom_client_get_glucose_readings_empty);
    RUN_TEST(test_dexcom_client_get_glucose_readings_invalid_arguments);
    RUN_TEST(test_dexcom_client_get_latest_glucose_reading_success);
    RUN_TEST(test_dexcom_client_get_latest_glucose_reading_empty);
    RUN_TEST(test_dexcom_client_get_current_glucose_reading_success);
    RUN_TEST(test_dexcom_client_get_current_glucose_reading_empty);
    RUN_TEST(test_dexcom_client_get_glucose_readings_max_size);
    RUN_TEST(test_dexcom_client_get_glucose_readings_error_response);
    RUN_TEST(test_dexcom_client_connection_failure);
    RUN_TEST(test_dexcom_client_get_glucose_readings_invalid_json);
    UNITY_END();
}