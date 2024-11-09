#include "unity.h"
#include "secure_http_client.h"
#include "mock_secure_client.h"
#include <memory>

namespace test_secure_http_client
{
    // Test fixture
    static std::shared_ptr<MockSecureClient> mock_client;
    static std::unique_ptr<SecureHttpClient> client;

    void setUp(void)
    {
        mock_client = std::make_shared<MockSecureClient>();
        client = std::make_unique<SecureHttpClient>(mock_client);
    }

    void tearDown(void)
    {
        client.reset();
        mock_client.reset();
    }

    // Test cases
    void test_connect_success(void)
    {
        mock_client->setShouldConnect(true);

        bool result = client->connect("test.com", 443);

        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL_STRING("test.com", mock_client->last_host.c_str());
        TEST_ASSERT_EQUAL_UINT16(443, mock_client->last_port);
    }

    void test_connect_failure(void)
    {
        mock_client->setShouldConnect(false);

        bool result = client->connect("test.com", 443);

        TEST_ASSERT_FALSE(result);
    }

    void test_simple_get_request(void)
    {
        // Setup mock response
        std::string mockResponse =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"test\": \"data\"}\r\n";

        mock_client->setNextReadData(mockResponse);
        mock_client->setConnected(true);

        HttpResponse response = client->get("/test");

        // Verify request
        std::string written = mock_client->getLastWrittenData();
        TEST_ASSERT_TRUE(written.find("GET /test HTTP/1.1") != std::string::npos);

        // Verify response parsing
        TEST_ASSERT_EQUAL_INT(200, response.statusCode);
        TEST_ASSERT_EQUAL_STRING("application/json",
                                 response.headers["Content-Type"].c_str());
        TEST_ASSERT_TRUE(response.body.find("{\"test\": \"data\"}") != std::string::npos);
    }

    void test_post_request_with_body(void)
    {
        // Setup mock response
        std::string mockResponse =
            "HTTP/1.1 201 Created\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"id\": 123}\r\n";

        mock_client->setNextReadData(mockResponse);
        mock_client->setConnected(true);

        std::map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}};

        HttpResponse response = client->post("/create",
                                             "{\"name\": \"test\"}",
                                             headers);

        // Verify request
        std::string written = mock_client->getLastWrittenData();
        TEST_ASSERT_TRUE(written.find("POST /create HTTP/1.1") != std::string::npos);

        // Verify headers were sent
        written = mock_client->getLastWrittenData();
        TEST_ASSERT_TRUE(written.find("Content-Type: application/json") != std::string::npos);

        // Verify response
        TEST_ASSERT_EQUAL_INT(201, response.statusCode);
        TEST_ASSERT_TRUE(response.body.find("{\"id\": 123}") != std::string::npos);
    }

    void test_disconnection(void)
    {
        mock_client->setConnected(true);
        TEST_ASSERT_TRUE(client->isConnected());

        client->disconnect();
        TEST_ASSERT_FALSE(client->isConnected());
    }

} // namespace test_secure_http_client

void run_secure_http_client_tests(void)
{
    RUN_TEST(test_secure_http_client::test_connect_success);
    RUN_TEST(test_secure_http_client::test_connect_failure);
    RUN_TEST(test_secure_http_client::test_simple_get_request);
    RUN_TEST(test_secure_http_client::test_post_request_with_body);
    RUN_TEST(test_secure_http_client::test_disconnection);
}