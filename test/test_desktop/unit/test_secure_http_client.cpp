#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "secure_http_client.h"
#include "mock_secure_client.h"
#include <memory>
#include <string>
#include <sstream>

// Extend MockSecureClient with GMock methods for testing
// Use testing::NiceMock instead of StrictMock to allow uninteresting calls
class MockSecureClientImpl : public MockSecureClient
{
public:
    MOCK_METHOD(bool, connect, (const char* host, uint16_t port), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, connected, (), (override));
    MOCK_METHOD(void, setTimeout, (uint32_t timeout), (override));
    MOCK_METHOD(void, println, (const std::string& data), (override));
    MOCK_METHOD(void, println, (), (override));
    MOCK_METHOD(std::string, readStringUntil, (char terminator), (override));
    MOCK_METHOD(int, available, (), (override));
    MOCK_METHOD(int, read, (), (override));
    MOCK_METHOD(int, read, (uint8_t* buf, size_t size), (override));
};

class SecureHttpClientTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mock_secure_client_ = std::make_shared<testing::NiceMock<MockSecureClientImpl>>();
        
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

    std::shared_ptr<testing::NiceMock<MockSecureClientImpl>> mock_secure_client_;
    std::unique_ptr<SecureHttpClient> http_client_;
};

TEST_F(SecureHttpClientTest, Constructor_SetsTimeout)
{
    // We need to create a new mock since the one in SetUp already had setTimeout called
    auto new_mock = std::make_shared<testing::NiceMock<MockSecureClientImpl>>();
    
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
    auto test_mock = std::make_shared<testing::NiceMock<MockSecureClientImpl>>();
    
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
    auto test_mock = std::make_shared<testing::NiceMock<MockSecureClientImpl>>();
    auto test_client = std::make_unique<SecureHttpClient>(test_mock);
    
    // Use ON_CALL instead of EXPECT_CALL to avoid "called more than expected" errors
    ON_CALL(*test_mock, connected())
        .WillByDefault(testing::Return(true));
    
    EXPECT_TRUE(test_client->isConnected());
}

TEST_F(SecureHttpClientTest, IsConnected_ReturnsFalse)
{
    // Create a fresh mock client for this test
    auto test_mock = std::make_shared<testing::NiceMock<MockSecureClientImpl>>();
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

    // Store the complete body content we want to receive
    const std::string body = "{\"key\":\"value\"}";

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

    // Setup HTTP response
    // First return status line and headers
    {
        testing::InSequence seq;
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("HTTP/1.1 200 OK\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: application/json\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Length: " + std::to_string(body.length()) + "\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
    }

    // Setup the body reading
    // First make available() return the correct length
    ON_CALL(*mock_secure_client_, available())
        .WillByDefault(testing::Return(body.length()));

    // Then set up the read() function to return characters from our body string
    {
        testing::InSequence seq;
        for (char c : body) {
            EXPECT_CALL(*mock_secure_client_, read())
                .WillOnce(testing::Return(static_cast<int>(c)));
        }
        // After returning all characters, return -1 to indicate no more data
        EXPECT_CALL(*mock_secure_client_, read())
            .WillRepeatedly(testing::Return(-1));
    }

    // Call the method under test
    HttpResponse response = http_client_->get(url, headers);

    // Verify response
    EXPECT_EQ(200, response.statusCode);
    EXPECT_EQ(body, response.body);
    EXPECT_EQ("application/json", response.headers["Content-Type"]);
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
    
    // Complete response body we expect
    const std::string responseBody = "{\"id\":123,\"status\":\"created\"}";

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

    // Setup HTTP response
    {
        testing::InSequence seq;
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("HTTP/1.1 201 Created\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Type: application/json\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("Content-Length: " + std::to_string(responseBody.length()) + "\r\n"));
        EXPECT_CALL(*mock_secure_client_, readStringUntil('\n'))
            .WillOnce(testing::Return("\r\n"));
    }

    // Setup the body reading
    ON_CALL(*mock_secure_client_, available())
        .WillByDefault(testing::Return(responseBody.length()));

    // Return each character of the body in sequence
    {
        testing::InSequence seq;
        for (char c : responseBody) {
            EXPECT_CALL(*mock_secure_client_, read())
                .WillOnce(testing::Return(static_cast<int>(c)));
        }
        // After returning all characters, return -1 to indicate no more data
        EXPECT_CALL(*mock_secure_client_, read())
            .WillRepeatedly(testing::Return(-1));
    }

    // Call the method under test
    HttpResponse response = http_client_->post(url, requestBody, headers);

    // Verify response
    EXPECT_EQ(201, response.statusCode);
    EXPECT_EQ(responseBody, response.body);
    EXPECT_EQ("application/json", response.headers["Content-Type"]);
}
