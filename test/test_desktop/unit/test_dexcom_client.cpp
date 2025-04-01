#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>
#include "dexcom_client.h"
#include "../mocks/mock_http_client.h"
#include "../mocks/mock_glucose_reading_parser.h"
#include "glucose_reading.h"
#include "dexcom_errors.h"
#include "dexcom_constants.h"

class DexcomClientTest : public ::testing::Test {
protected:
    // Mock objects
    std::shared_ptr<testing::NiceMock<MockHttpClient>> mock_http_client_;
    std::shared_ptr<testing::NiceMock<MockGlucoseReadingParser>> mock_glucose_parser_;
    std::unique_ptr<DexcomClient> dexcom_client_;
    
    // Dummy credentials
    const std::string USERNAME = "test_user";
    const std::string PASSWORD = "test_password";
    const std::string ACCOUNT_ID = "test_account_id";
    const std::string SESSION_ID = "test_session_id";
    
    // Dexcom URLs
    const std::string DEXCOM_URL = DexcomConst::DEXCOM_BASE_URL;
    const std::string DEXCOM_URL_OUS = DexcomConst::DEXCOM_BASE_URL_OUS;
    
    void SetUp() override {
        // Initialize mocks
        mock_http_client_ = std::make_shared<testing::NiceMock<MockHttpClient>>();
        mock_glucose_parser_ = std::make_shared<testing::NiceMock<MockGlucoseReadingParser>>();
        
        // Set default behavior for frequently called methods
        ON_CALL(*mock_http_client_, isConnected()).WillByDefault(testing::Return(true));
    }
    
    void TearDown() override {
        dexcom_client_.reset();
        mock_http_client_.reset();
        mock_glucose_parser_.reset();
    }
};

TEST_F(DexcomClientTest, FixtureSetup) {
    // This test doesn't do anything, it just ensures the fixture compiles
}

TEST_F(DexcomClientTest, SuccessfulConstructionOUSFalse) {
    // Define expected responses
    const std::string dummy_account_id = ACCOUNT_ID;
    const std::string dummy_session_id = SESSION_ID;
    
    // Use InSequence to enforce order of calls
    testing::InSequence seq;
    
    // Set expectations on mock_http_client_
    EXPECT_CALL(*mock_http_client_, connect(DexcomConst::DEXCOM_BASE_URL, 443))
        .Times(1)
        .WillOnce(testing::Return(true));
    
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_AUTHENTICATE_ENDPOINT), testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, "\"" + std::string(ACCOUNT_ID) + "\"", {}}));
    
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_LOGIN_ID_ENDPOINT), testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, "\"" + std::string(SESSION_ID) + "\"", {}}));
    
    // Instantiate client with username but no account_id (will trigger getAccountId)
    EXPECT_NO_THROW({
        dexcom_client_ = std::make_unique<DexcomClient>(mock_http_client_, mock_glucose_parser_, 
                                                       USERNAME, "", PASSWORD, false);
    });
}

TEST_F(DexcomClientTest, SuccessfulConstructionOUSFalseWithAccountID) {
    // Define expected responses
    const std::string dummy_session_id = SESSION_ID;
    
    // Use InSequence to enforce order of calls
    testing::InSequence seq;
    
    // Set expectations on mock_http_client_
    EXPECT_CALL(*mock_http_client_, connect(DexcomConst::DEXCOM_BASE_URL, 443))
        .Times(1)
        .WillOnce(testing::Return(true));
    
    // Only getSessionId should be called when account_id is provided
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_LOGIN_ID_ENDPOINT), testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, "\"" + std::string(SESSION_ID) + "\"", {}}));
    
    // Instantiate client with account_id but no username
    EXPECT_NO_THROW({
        dexcom_client_ = std::make_unique<DexcomClient>(mock_http_client_, mock_glucose_parser_, 
                                                       "", ACCOUNT_ID, PASSWORD, false);
    });
}

TEST_F(DexcomClientTest, SuccessfulConstructionOUSTrue) {
    // Define expected responses
    const std::string dummy_account_id = ACCOUNT_ID;
    const std::string dummy_session_id = SESSION_ID;
    
    // Use InSequence to enforce order of calls
    testing::InSequence seq;
    
    // Set expectations on mock_http_client_ - verify OUS URL is used
    EXPECT_CALL(*mock_http_client_, connect(DexcomConst::DEXCOM_BASE_URL_OUS, 443))
        .Times(1)
        .WillOnce(testing::Return(true));
    
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_AUTHENTICATE_ENDPOINT), testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, "\"" + std::string(ACCOUNT_ID) + "\"", {}}));
    
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_LOGIN_ID_ENDPOINT), testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, "\"" + std::string(SESSION_ID) + "\"", {}}));
    
    // Instantiate client with ous = true
    EXPECT_NO_THROW({
        dexcom_client_ = std::make_unique<DexcomClient>(mock_http_client_, mock_glucose_parser_, 
                                                       USERNAME, "", PASSWORD, true);
    });
}

TEST_F(DexcomClientTest, ConstructorConnectFailure) {
    // Define a mock action that returns an error response with limited retries
    int call_count = 0;
    auto limitedErrorResponse = [&call_count]() {
        // Return error for a limited number of calls
        if (++call_count <= 3) {
            return HttpResponse{500, "Connection Failed", {}};
        }
        // Then return an error that will cause the session creation to fail and throw immediately
        return HttpResponse{401, "Unauthorized", {}};
    };
    
    // Use InSequence to enforce order of calls
    testing::InSequence seq;
    
    // Set expectations on mock_http_client_
    EXPECT_CALL(*mock_http_client_, connect(testing::_, 443))
        .Times(1)
        .WillOnce(testing::Return(false));
    
    // Since connect fails, the auth 'post' calls inside createSession might still be attempted.
    // DexcomClient will retry multiple times, so we need to handle that in our mock
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_AUTHENTICATE_ENDPOINT), testing::_, testing::_))
        .Times(testing::AtLeast(1))
        .WillRepeatedly(testing::Invoke(limitedErrorResponse));
    
    // Verify that instantiating the client throws an appropriate exception
    // Using a try-catch with proper cleanup to prevent memory leaks
    bool exception_thrown = false;
    try {
        dexcom_client_ = std::make_unique<DexcomClient>(mock_http_client_, mock_glucose_parser_, USERNAME, "", PASSWORD, false);
    } catch (const DexcomError&) {
        exception_thrown = true;
    }
    
    // Clean up in case the test unexpectedly succeeds
    if (dexcom_client_) {
        dexcom_client_.reset();
    }
    
    EXPECT_TRUE(exception_thrown);
}

TEST_F(DexcomClientTest, ConstructorGetAccountIdFailure) {
    // Define a mock action that returns an error response
    auto errorResponse = []() {
        // Always return 401 (which triggers AccountError immediately)
        return HttpResponse{401, "Unauthorized", {}};
    };
    
    // Use InSequence to enforce order of calls
    testing::InSequence seq;
    
    // Set expectations on mock_http_client_
    EXPECT_CALL(*mock_http_client_, connect(testing::_, 443))
        .Times(1)
        .WillOnce(testing::Return(true));
    
    // DexcomClient will retry multiple times, so we need to handle that in our mock
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_AUTHENTICATE_ENDPOINT), testing::_, testing::_))
        .Times(testing::AtLeast(1))
        .WillRepeatedly(testing::Invoke(errorResponse));
    
    // Verify instantiation throws AccountError
    // Using a try-catch with proper cleanup to prevent memory leaks
    bool exception_thrown = false;
    try {
        dexcom_client_ = std::make_unique<DexcomClient>(mock_http_client_, mock_glucose_parser_, USERNAME, "", PASSWORD, false);
    } catch (const AccountError&) {
        exception_thrown = true;
    }
    
    // Clean up in case the test unexpectedly succeeds
    if (dexcom_client_) {
        dexcom_client_.reset();
    }
    
    EXPECT_TRUE(exception_thrown);
}

TEST_F(DexcomClientTest, ConstructorGetSessionIdFailure) {
    // Define a mock action that returns an error response that causes failure
    auto errorResponse = []() {
        // Return a 500 error to trigger a SessionError during session creation
        return HttpResponse{500, "Server Error", {}};
    };
    
    // Use InSequence to enforce order of calls
    testing::InSequence seq;
    
    // Set expectations on mock_http_client_
    EXPECT_CALL(*mock_http_client_, connect(testing::_, 443))
        .Times(1)
        .WillOnce(testing::Return(true));
    
    // DexcomClient will retry multiple times, so we need to handle that in our mock
    EXPECT_CALL(*mock_http_client_, post(testing::HasSubstr(DexcomConst::DEXCOM_LOGIN_ID_ENDPOINT), testing::_, testing::_))
        .Times(testing::AtLeast(1))
        .WillRepeatedly(testing::Invoke(errorResponse));
    
    // Verify instantiation throws any DexcomError
    // Using a try-catch with proper cleanup to prevent memory leaks
    bool exception_thrown = false;
    try {
        dexcom_client_ = std::make_unique<DexcomClient>(mock_http_client_, mock_glucose_parser_, "", ACCOUNT_ID, PASSWORD, false);
    } catch (const DexcomError&) {
        // Here we catch any DexcomError, which includes both SessionError and AccountError
        exception_thrown = true;
    }
    
    // Clean up in case the test unexpectedly succeeds
    if (dexcom_client_) {
        dexcom_client_.reset();
    }
    
    EXPECT_TRUE(exception_thrown);
}
