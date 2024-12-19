#include "unity.h"
#include "secure_http_client.h"
#include "mock_secure_client.h"
#include <memory>
#include <sstream>

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
        // Setup mock response - note the lack of trailing newline
        std::string mockResponse =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 16\r\n"
            "\r\n"
            "{\"test\":\"data\"}"; // No trailing newline

        mock_client->setNextReadData(mockResponse);
        mock_client->setConnected(true);
        mock_client->clearRequestData();

        HttpResponse response = client->get("/test");

        // Get the full request and print it for debugging
        auto request_lines = mock_client->getRequestLines();

        // Create debug message
        std::stringstream debug_msg;
        debug_msg << "Request lines received:\n";
        for (const auto &line : request_lines)
        {
            debug_msg << line << "\n";
        }

        // Verify request line
        TEST_ASSERT_TRUE_MESSAGE(
            request_lines[0] == "GET /test HTTP/1.1",
            debug_msg.str().c_str());

        // Verify response parsing
        TEST_ASSERT_EQUAL_INT(200, response.statusCode);
        TEST_ASSERT_EQUAL_STRING("application/json",
                                 response.headers["Content-Type"].c_str());

        // Add debug information for body comparison
        std::stringstream body_debug;
        body_debug << "Expected: '{\"test\":\"data\"}'\n"
                   << "Actual: '" << response.body << "'\n"
                   << "Body length: " << response.body.length() << "\n"
                   << "Last character code: " << (int)response.body.back();

        TEST_ASSERT_EQUAL_STRING_MESSAGE(
            "{\"test\":\"data\"}",
            response.body.c_str(),
            body_debug.str().c_str());
    }

    void test_post_request_with_body(void)
    {
        // Setup mock response
        std::string mockResponse =
            "HTTP/1.1 201 Created\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 11\r\n"
            "\r\n"
            "{\"id\":123}";

        mock_client->setNextReadData(mockResponse);
        mock_client->setConnected(true);
        mock_client->clearRequestData();

        std::map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}};

        HttpResponse response = client->post("/create", "{\"name\":\"test\"}", headers);

        // Get the full request and print it for debugging
        auto request_lines = mock_client->getRequestLines();

        // Create debug message
        std::stringstream debug_msg;
        debug_msg << "Request lines received:\n";
        for (const auto &line : request_lines)
        {
            debug_msg << line << "\n";
        }

        // Verify request line
        TEST_ASSERT_TRUE_MESSAGE(
            request_lines[0] == "POST /create HTTP/1.1",
            debug_msg.str().c_str());

        // Verify Content-Type header was sent
        bool found_content_type = false;
        for (const auto &line : request_lines)
        {
            if (line == "Content-Type: application/json")
            {
                found_content_type = true;
                break;
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(found_content_type,
                                 "Content-Type header not found in request");

        // Verify response
        TEST_ASSERT_EQUAL_INT(201, response.statusCode);
        TEST_ASSERT_TRUE(response.body.find("{\"id\":123}") != std::string::npos);
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