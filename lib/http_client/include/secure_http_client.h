#ifndef SECURE_HTTP_CLIENT_H
#define SECURE_HTTP_CLIENT_H

#include "i_http_client.h"
#include "i_secure_client.h"
#include <memory>

class SecureHttpClient : public IHttpClient
{
public:
    explicit SecureHttpClient(std::shared_ptr<ISecureClient> client);
    ~SecureHttpClient() override;

    bool connect(const std::string &host, uint16_t port) override;
    void disconnect() override;
    bool isConnected() const override;

    HttpResponse send(const HttpRequest &request) override;
    HttpResponse get(const std::string &url,
                     const std::map<std::string, std::string> &headers = {}) override;
    HttpResponse post(const std::string &url,
                      const std::string &body,
                      const std::map<std::string, std::string> &headers = {}) override;

private:
    std::shared_ptr<ISecureClient> _client;
    std::string _host;
    uint16_t _port;
    static constexpr uint32_t DEFAULT_TIMEOUT = 5000; // 5 seconds

    std::string readResponse();
    HttpResponse parseResponse(const std::string &rawResponse);
    void writeHeaders(const std::map<std::string, std::string> &headers);
};

#endif // SECURE_HTTP_CLIENT_H