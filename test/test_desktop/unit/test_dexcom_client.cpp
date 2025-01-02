#include <unity.h>
#include "dexcom_client.h"
#include "../mocks/mock_http_client.h"
#include <memory>
#include <stdexcept>

// Helper macro for testing exceptions
#define TEST_ASSERT_THROWS(expression, exception_type) \
    do { \
        bool caught_expected = false; \
        try { \
            expression; \
        } catch (const exception_type&) { \
            caught_expected = true; \
        } catch (...) { \
            TEST_FAIL_MESSAGE("Wrong exception type caught"); \
        } \
        if (!caught_expected) { \
            TEST_FAIL_MESSAGE("Expected exception was not thrown"); \
        } \
    } while (0)

namespace test_dexcom_client
{
    // Test fixture
    static std::shared_ptr<MockHttpClient> mockClient;
    static std::unique_ptr<DexcomClient> dexcomClient;

    void setUp(void)
    {
        mockClient = std::make_shared<MockHttpClient>();
        mockClient->setShouldConnect(true);
        mockClient->setNextResponse(200, "\"test-session-id\"", {
            {"Content-Type", "application/json"}
        });
        dexcomClient = std::make_unique<DexcomClient>(
            mockClient,
            "username",
            "account_id",
            "password",
            false);
    }

    void tearDown(void)
    {
        dexcomClient.reset();
        mockClient.reset();
    }

    void test_dexcom_client_constructor()
    {
        TEST_ASSERT_NOT_NULL(dexcomClient.get());
        TEST_ASSERT_EQUAL_STRING("share2.dexcom.com", mockClient->last_host.c_str());
        TEST_ASSERT_EQUAL_UINT16(443, mockClient->last_port);
    }

    void test_failed_connection()
    {
        mockClient->setShouldConnect(false);
        TEST_ASSERT_THROWS(
            std::make_unique<DexcomClient>(mockClient, "username", "account_id", "password", false),
            SessionError);
    }

    void test_invalid_credentials()
    {
        mockClient->setNextResponse(401, "{\"Code\":\"SSO_InternalError\",\"Message\":\"Cannot Authenticate\"}");
        TEST_ASSERT_THROWS(
            std::make_unique<DexcomClient>(mockClient, "bad_user", "bad_account", "bad_pass", false),
            AccountError);
    }

    void test_get_glucose_readings()
    {
        // Setup mock response for glucose readings
        const char* mockReadings = "["
            "{\"Value\":120,\"Trend\":\"Flat\",\"WT\":\"Date(1609459200000)\"},"
            "{\"Value\":115,\"Trend\":\"FortyFiveDown\",\"WT\":\"Date(1609455600000)\"}"
            "]";
        mockClient->setNextResponse(200, mockReadings, {{"Content-Type", "application/json"}});

        // Get readings
        auto readings = dexcomClient->getGlucoseReadings(10, 2);

        // Verify request
        TEST_ASSERT_EQUAL_STRING("POST", mockClient->last_request.method.c_str());
        TEST_ASSERT_TRUE(mockClient->last_request.url.find("/ShareWebServices/Services/Publisher/ReadPublisherLatestGlucoseValues") != std::string::npos);
        
        // Verify readings
        TEST_ASSERT_EQUAL(2, readings.size());
        TEST_ASSERT_EQUAL(120, readings[0].getValue());
        TEST_ASSERT_EQUAL(115, readings[1].getValue());
    }
} // namespace test_dexcom_client

void run_dexcom_client_tests(void)
{
    RUN_TEST(test_dexcom_client::test_dexcom_client_constructor);
    RUN_TEST(test_dexcom_client::test_failed_connection);
    RUN_TEST(test_dexcom_client::test_invalid_credentials);
    RUN_TEST(test_dexcom_client::test_get_glucose_readings);
}
