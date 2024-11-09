#include <unity.h>
#include "dexcom_client.h"
#include "mock_secure_client.h"
#include <memory>
#include <stdexcept>

namespace test_dexcom_client
{
    // Test fixture
    static std::shared_ptr<MockSecureClient> mockClient;
    static std::unique_ptr<DexcomClient> dexcomClient;

    void setUp(void)
    {
        mockClient = std::make_shared<MockSecureClient>();
        mockClient->setConnected(true);
        mockClient->setNextReadData(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "\"test-session-id\"");
        dexcomClient = std::make_unique<DexcomClient>(
            *mockClient,
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
    }
} // namespace test_dexcom_client

void run_dexcom_client_tests(void)
{
    RUN_TEST(test_dexcom_client::test_dexcom_client_constructor);
}