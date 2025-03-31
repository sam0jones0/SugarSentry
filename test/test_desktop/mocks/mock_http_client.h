#pragma once

#include <gmock/gmock.h>
#include "i_http_client.h"
#include <string>
#include <map>
#include <optional>
#include <cstdint>

class MockHttpClient : public IHttpClient {
public:
    MOCK_METHOD(bool, connect, (const std::string& host, std::uint16_t port), (override));
    MOCK_METHOD(void, disconnect, (), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
    MOCK_METHOD(HttpResponse, send, (const HttpRequest& request), (override));
    MOCK_METHOD(HttpResponse, get, (const std::string& url, (const std::map<std::string, std::string>&) headers), (override));
    MOCK_METHOD(HttpResponse, post, (const std::string& url, const std::string& body, (const std::map<std::string, std::string>&) headers), (override));
};
