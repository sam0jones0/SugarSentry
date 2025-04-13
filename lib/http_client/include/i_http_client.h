#ifndef I_HTTP_CLIENT_H
#define I_HTTP_CLIENT_H

#include <string>
#include <map>
#include <optional>
#include <cstdint> // Add this for uint16_t

struct HttpResponse
{
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
    int contentLength = 0; // Add this member
};

struct HttpRequest
{
    std::string url;
    std::string method;
    std::map<std::string, std::string> headers;
    std::optional<std::string> body;
};

class IHttpClient
{
public:
    virtual ~IHttpClient() = default;

    virtual bool connect(const std::string &host, std::uint16_t port) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;

    virtual HttpResponse send(const HttpRequest &request) = 0;

    // Convenience methods
    virtual HttpResponse get(const std::string &url,
                             const std::map<std::string, std::string> &headers = {}) = 0;

    virtual HttpResponse post(const std::string &url,
                              const std::string &body,
                              const std::map<std::string, std::string> &headers = {}) = 0;
};

#endif // I_HTTP_CLIENT_H
