#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "secure_http_client.h"
#include "../mocks/mock_secure_client.h"
#include <memory>
#include <string>
#include <sstream>

class SecureHttpClientTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mock_secure_client_ = std::make_shared<testing::NiceMock<MockSecureClient>>();
        
        // Set default behaviors to avoid unexpected failures
        ON_CALL(*mock_secure_client_, setTimeout).WillByDefault(testing::Return());
        ON_CALL(*mock_secure_client_, connected).WillByDefault(testing::Return(true));
        
        http_client_ = std::make_unique<SecureHttpClient>(mock_secure_client_);
    }

    void TearDown() override
    {
        http_client_.reset();
        mock_secure_client_.reset();
    }

    std::shared_ptr<testing::NiceMock<MockSecureClient>> mock_secure_client_;
    std::unique_ptr<SecureHttpClient> http_client_;
};

TEST_F(SecureHttpClientTest, Constructor_SetsTimeout)
{
    // We need to create a new mock since the one in SetUp already had setTimeout called
    auto new_mock = std::make_shared<testing::NiceMock<MockSecureClient>>();
    
    // Test that the constructor sets the timeout on the secure client
    EXPECT_CALL(*new_mock, setTimeout(5000)).Times(1);
    
    // Create a new instance to trigger the constructor
    auto client = std::make_unique<SecureHttpClient>(new_mock);
}

TEST_F(SecureHttpClientTest, Connect_Success)
{
    const std::string host = "example.com";
    const uint16_t port = 443;

    // Set expectations
    EXPECT_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillOnce(testing::Return(true));

    // Test the connect method
    bool result = http_client_->connect(host, port);
    
    // Verify the result
    EXPECT_TRUE(result);
}

TEST_F(SecureHttpClientTest, Disconnect_CallsStop)
{
    // Note: The disconnect method is called in both our explicit disconnect call
    // and in the SecureHttpClient destructor, so we need to account for that.
    
    // Create a fresh mock client for this test
    auto test_mock = std::make_shared<testing::NiceMock<MockSecureClient>>();
    
    // Setup: First connection check returns true, second returns false
    {
        testing::InSequence seq;
        EXPECT_CALL(*test_mock, connected())
            .WillOnce(testing::Return(true));  // For our explicit disconnect call
        EXPECT_CALL(*test_mock, connected())
            .WillRepeatedly(testing::Return(false));  // For destructor's disconnect call
    }
    
    // Expect stop to be called exactly once in our explicit disconnect call
    EXPECT_CALL(*test_mock, stop()).Times(1);
    
    // Create client and call disconnect
    auto test_client = std::make_unique<SecureHttpClient>(test_mock);
    test_client->disconnect();
    
    // Verify the expectations before test_client is destroyed
    testing::Mock::VerifyAndClearExpectations(test_mock.get());
}

TEST_F(SecureHttpClientTest, IsConnected_ReturnsTrue)
{
    // Create a fresh mock client for this test
    auto test_mock = std::make_shared<testing::NiceMock<MockSecureClient>>();
    auto test_client = std::make_unique<SecureHttpClient>(test_mock);
    
    // Use ON_CALL instead of EXPECT_CALL to avoid "called more than expected" errors
    ON_CALL(*test_mock, connected())
        .WillByDefault(testing::Return(true));
    
    EXPECT_TRUE(test_client->isConnected());
}

TEST_F(SecureHttpClientTest, IsConnected_ReturnsFalse)
{
    // Create a fresh mock client for this test
    auto test_mock = std::make_shared<testing::NiceMock<MockSecureClient>>();
    auto test_client = std::make_unique<SecureHttpClient>(test_mock);
    
    // Use ON_CALL instead of EXPECT_CALL to avoid "called more than expected" errors
    ON_CALL(*test_mock, connected())
        .WillByDefault(testing::Return(false));
    
    EXPECT_FALSE(test_client->isConnected());
}

TEST_F(SecureHttpClientTest, Get_Success)
{
    const std::string url = "/api/resource";
    const std::string host = "example.com";
    const uint16_t port = 443;
    std::map<std::string, std::string> headers = {
        {"Accept", "application/json"}
    };

    // Setup connection
    ON_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillByDefault(testing::Return(true));
    http_client_->connect(host, port);

    // Setup connected state
    ON_CALL(*mock_secure_client_, connected())
        .WillByDefault(testing::Return(true));

    // Expect HTTP request headers
    EXPECT_CALL(*mock_secure_client_, println("GET " + url + " HTTP/1.1")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Host: " + host)).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Accept: application/json")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println()).Times(1);

    // Setup HTTP response headers in strict sequence
    {
        testing::InSequence seq;
        
        // Return status line and headers only
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("HTTP/1.1 200 OK\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: application/json\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Length: 15\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
        
        // No body reading expectations as readResponse no longer reads the body
    }

    // Call the method under test
    HttpResponse response = http_client_->get(url, headers);

    // Verify response
    EXPECT_EQ(200, response.statusCode);
    EXPECT_EQ("", response.body); // Body should be empty as readResponse no longer reads it
    EXPECT_EQ("application/json", response.headers["Content-Type"]);
    EXPECT_EQ(15, response.contentLength); // Verify Content-Length is parsed correctly
}

TEST_F(SecureHttpClientTest, Post_Success)
{
    const std::string url = "/api/resource";
    const std::string host = "example.com";
    const uint16_t port = 443;
    const std::string requestBody = "{\"name\":\"test\"}";
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"}
    };
    
    // Setup connection
    ON_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillByDefault(testing::Return(true));
    http_client_->connect(host, port);

    // Setup connected state
    ON_CALL(*mock_secure_client_, connected())
        .WillByDefault(testing::Return(true));

    // Expect HTTP request headers and body
    EXPECT_CALL(*mock_secure_client_, println("POST " + url + " HTTP/1.1")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Host: " + host)).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Content-Type: application/json")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Content-Length: " + std::to_string(requestBody.length()))).Times(1);
    EXPECT_CALL(*mock_secure_client_, println()).Times(1);
    EXPECT_CALL(*mock_secure_client_, println(requestBody)).Times(1);

    // Setup HTTP response headers in strict sequence
    {
        testing::InSequence seq;
        
        // Return status line and headers only
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("HTTP/1.1 201 Created\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: application/json\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Length: 31\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
        
        // No body reading expectations as readResponse no longer reads the body
    }

    // Call the method under test
    HttpResponse response = http_client_->post(url, requestBody, headers);

    // Verify response
    EXPECT_EQ(201, response.statusCode);
    EXPECT_EQ("", response.body); // Body should be empty as readResponse no longer reads it
    EXPECT_EQ("application/json", response.headers["Content-Type"]);
    EXPECT_EQ(31, response.contentLength); // Verify Content-Length is parsed correctly
}

// Test Case 1: Connection Failure
TEST_F(SecureHttpClientTest, ConnectFailure)
{
    const std::string host = "nonexistent.example.com";
    const uint16_t port = 443;

    // Setup: Expect connect to fail
    EXPECT_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillOnce(testing::Return(false));

    // Test: Should return false when connection fails
    bool result = http_client_->connect(host, port);
    EXPECT_FALSE(result);
}

// Additional test for send() after connect failure
TEST_F(SecureHttpClientTest, SendAfterConnectFailure)
{
    const std::string url = "/api/resource";
    const std::string host = "nonexistent.example.com";
    const uint16_t port = 443;
    std::map<std::string, std::string> headers;

    // Setup: Set host and port in the client
    http_client_->connect(host, port);
    
    // Setup: Ensure client reports not connected
    ON_CALL(*mock_secure_client_, connected())
        .WillByDefault(testing::Return(false));
    
    // Setup: Any connect attempt should fail
    ON_CALL(*mock_secure_client_, connect(testing::_, testing::_))
        .WillByDefault(testing::Return(false));

    // Test: GET request should return a 500 error when connection fails
    HttpResponse response = http_client_->get(url, headers);
    
    // Verify: Status code should be 500 (server error)
    EXPECT_EQ(500, response.statusCode);
    // Body should contain error message
    EXPECT_EQ("Failed to connect", response.body);
}

// Test Case 2: GET with 404 response
TEST_F(SecureHttpClientTest, GetNon200Response)
{
    const std::string url = "/api/nonexistent";
    const std::string host = "example.com";
    const uint16_t port = 443;
    std::map<std::string, std::string> headers;

    // Setup connection
    ON_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillByDefault(testing::Return(true));
    http_client_->connect(host, port);

    // Setup connected state
    ON_CALL(*mock_secure_client_, connected())
        .WillByDefault(testing::Return(true));

    // Expect HTTP request headers
    EXPECT_CALL(*mock_secure_client_, println("GET " + url + " HTTP/1.1")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Host: " + host)).Times(1);
    EXPECT_CALL(*mock_secure_client_, println()).Times(1);

    // Setup HTTP response headers in strict sequence
    {
        testing::InSequence seq;
        
        // Return status line and headers only
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("HTTP/1.1 404 Not Found\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: text/plain\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Length: 9\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
        
        // No body reading expectations as readResponse no longer reads the body
    }

    // Call the method under test
    HttpResponse response = http_client_->get(url, headers);

    // Verify response has correct status code and content type
    EXPECT_EQ(404, response.statusCode);
    EXPECT_EQ("", response.body); // Body should be empty as readResponse no longer reads it
    EXPECT_EQ("text/plain", response.headers["Content-Type"]);
    EXPECT_EQ(9, response.contentLength); // Verify Content-Length is parsed correctly
}

// Test Case 3: POST with 500 response
TEST_F(SecureHttpClientTest, PostNon200Response)
{
    const std::string url = "/api/error";
    const std::string host = "example.com";
    const uint16_t port = 443;
    const std::string requestBody = "{\"problematic\":\"data\"}";
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"}
    };

    // Setup connection
    ON_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillByDefault(testing::Return(true));
    http_client_->connect(host, port);

    // Setup connected state
    ON_CALL(*mock_secure_client_, connected())
        .WillByDefault(testing::Return(true));

    // Expect HTTP request headers and body
    EXPECT_CALL(*mock_secure_client_, println("POST " + url + " HTTP/1.1")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Host: " + host)).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Content-Type: application/json")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Content-Length: " + std::to_string(requestBody.length()))).Times(1);
    EXPECT_CALL(*mock_secure_client_, println()).Times(1);
    EXPECT_CALL(*mock_secure_client_, println(requestBody)).Times(1);

    // Setup HTTP response headers in strict sequence
    {
        testing::InSequence seq;
        
        // Return status line and headers only
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("HTTP/1.1 500 Internal Server Error\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: application/json\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Length: 38\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
        
        // No body reading expectations as readResponse no longer reads the body
    }

    // Call the method under test
    HttpResponse response = http_client_->post(url, requestBody, headers);

    // Verify response
    EXPECT_EQ(500, response.statusCode);
    EXPECT_EQ("", response.body); // Body should be empty as readResponse no longer reads it
    EXPECT_EQ("application/json", response.headers["Content-Type"]);
    EXPECT_EQ(38, response.contentLength); // Verify Content-Length is parsed correctly
}

// Test Case 4: Response with empty body
TEST_F(SecureHttpClientTest, ResponseWithEmptyBody)
{
    const std::string url = "/api/empty";
    const std::string host = "example.com";
    const uint16_t port = 443;
    std::map<std::string, std::string> headers;

    // Setup connection
    ON_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillByDefault(testing::Return(true));
    http_client_->connect(host, port);

    // Setup connected state
    ON_CALL(*mock_secure_client_, connected())
        .WillByDefault(testing::Return(true));

    // Expect HTTP request headers
    EXPECT_CALL(*mock_secure_client_, println("GET " + url + " HTTP/1.1")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Host: " + host)).Times(1);
    EXPECT_CALL(*mock_secure_client_, println()).Times(1);

    // Setup HTTP response headers in strict sequence
    {
        testing::InSequence seq;
        
        // Return status line and headers only
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("HTTP/1.1 200 OK\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: text/plain\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Length: 0\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
        
        // No body reading expectations as readResponse no longer reads the body
    }

    // Call the method under test
    HttpResponse response = http_client_->get(url, headers);

    // Verify response has correct status code, empty body
    EXPECT_EQ(200, response.statusCode);
    EXPECT_EQ("", response.body);
    EXPECT_EQ("text/plain", response.headers["Content-Type"]);
    EXPECT_EQ(0, response.contentLength); // Verify Content-Length is parsed correctly
}

// Test Case 5: Malformed status line
TEST_F(SecureHttpClientTest, MalformedStatusLine)
{
    const std::string url = "/api/resource";
    const std::string host = "example.com";
    const uint16_t port = 443;
    std::map<std::string, std::string> headers;

    // Setup connection
    ON_CALL(*mock_secure_client_, connect(testing::_, port))
        .WillByDefault(testing::Return(true));
    http_client_->connect(host, port);

    // Setup connected state
    ON_CALL(*mock_secure_client_, connected())
        .WillByDefault(testing::Return(true));

    // Expect HTTP request headers
    EXPECT_CALL(*mock_secure_client_, println("GET " + url + " HTTP/1.1")).Times(1);
    EXPECT_CALL(*mock_secure_client_, println("Host: " + host)).Times(1);
    EXPECT_CALL(*mock_secure_client_, println()).Times(1);

    // Setup HTTP response headers in strict sequence
    {
        testing::InSequence seq;
        
        // From looking at the SecureHttpClient::parseResponse implementation,
        // it expects a status line to be at least 12 chars with status code at position 9-11
        // Use a string that's too short to trigger the default 500 status code
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Invalid\r\n"));
        // Add some headers to make it look somewhat like a response
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: text/plain\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
        
        // No body reading expectations as readResponse no longer reads the body
    }

    // Call the method under test
    HttpResponse response = http_client_->get(url, headers);

    // Verify: Status code should be 500 (internal error) due to parsing failure
    EXPECT_EQ(500, response.statusCode);
}
