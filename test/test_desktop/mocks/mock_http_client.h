#pragma once

#include "i_http_client.h"
#include <string>
#include <map>
#include <optional>

class MockHttpClient : public IHttpClient
{
public:
    bool connect(const std::string &host, std::uint16_t port) override { return true; }
    void disconnect() override {}
    bool isConnected() const override { return true; }

    HttpResponse send(const HttpRequest &request) override {
        HttpResponse response;
        response.statusCode = 200;
        response.body = "{}";
        return response;
    }

    HttpResponse get(const std::string &url,
                     const std::map<std::string, std::string> &headers = {}) override {
        HttpResponse response;
        response.statusCode = 200;
        response.body = "{}";
        return response;
    }

    HttpResponse post(const std::string &url,
                      const std::string &body,
                      const std::map<std::string, std::string> &headers = {}) override {
        HttpResponse response;
        response.statusCode = 200;
        response.body = "{}";
        return response;
    }
};
