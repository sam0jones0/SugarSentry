#ifndef MOCK_HTTP_CLIENT_H
#define MOCK_HTTP_CLIENT_H

#include "i_http_client.h"
#include <string>
#include <vector>
#include <map>

class MockHttpClient : public IHttpClient {
public:
    MockHttpClient() : _connected(false), _shouldConnect(true) {}

    bool connect(const std::string &host, std::uint16_t port) override {
        last_host = host;
        last_port = port;
        _connected = _shouldConnect;
        return _shouldConnect;
    }

    void disconnect() override {
        _connected = false;
    }

    bool isConnected() const override {
        return _connected;
    }

    HttpResponse send(const HttpRequest &request) override {
        last_request = request;
        return next_response;
    }

    HttpResponse get(const std::string &url,
                    const std::map<std::string, std::string> &headers = {}) override {
        HttpRequest req{url, "GET", headers, std::nullopt};
        return send(req);
    }

    HttpResponse post(const std::string &url,
                     const std::string &body,
                     const std::map<std::string, std::string> &headers = {}) override {
        HttpRequest req{url, "POST", headers, body};
        return send(req);
    }

    // Test control methods
    void setShouldConnect(bool should_connect) {
        _shouldConnect = should_connect;
    }

    void setNextResponse(const HttpResponse &response) {
        next_response = response;
    }

    void setNextResponse(int statusCode, const std::string &body, 
                        const std::map<std::string, std::string> &headers = {}) {
        next_response = HttpResponse{statusCode, body, headers};
    }

    // Test inspection methods
    std::string last_host;
    uint16_t last_port;
    HttpRequest last_request;
    
private:
    bool _connected;
    bool _shouldConnect;
    HttpResponse next_response;
};

#endif // MOCK_HTTP_CLIENT_H
