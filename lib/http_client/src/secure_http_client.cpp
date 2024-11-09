#include "secure_http_client.h"
#include "debug_print.h"
#include <sstream>

SecureHttpClient::SecureHttpClient(std::shared_ptr<ISecureClient> client)
    : _client(std::move(client)), _port(0)
{
    _client->setTimeout(DEFAULT_TIMEOUT);
}

SecureHttpClient::~SecureHttpClient()
{
    disconnect();
}

bool SecureHttpClient::connect(const std::string &host, uint16_t port)
{
    _host = host;
    _port = port;
    return _client->connect(host.c_str(), port);
}

void SecureHttpClient::disconnect()
{
    if (isConnected())
    {
        _client->stop();
    }
}

bool SecureHttpClient::isConnected() const
{
    return _client->connected();
}

HttpResponse SecureHttpClient::send(const HttpRequest &request)
{
    if (!isConnected())
    {
        if (!connect(_host, _port))
        {
            return HttpResponse{500, "Failed to connect", {}};
        }
    }

    // Write request line
    _client->println(request.method + " " + request.url + " HTTP/1.1");

    // Write headers
    _client->println("Host: " + _host);
    writeHeaders(request.headers);

    // Write body if present
    if (request.body)
    {
        _client->println("Content-Length: " + std::to_string(request.body->length()));
        _client->println();
        _client->println(*request.body);
    }
    else
    {
        _client->println();
    }

    return parseResponse(readResponse());
}

HttpResponse SecureHttpClient::get(const std::string &url,
                                   const std::map<std::string, std::string> &headers)
{
    HttpRequest request{
        url,
        "GET",
        headers,
        std::nullopt};
    return send(request);
}

HttpResponse SecureHttpClient::post(const std::string &url,
                                    const std::string &body,
                                    const std::map<std::string, std::string> &headers)
{
    HttpRequest request{
        url,
        "POST",
        headers,
        body};
    return send(request);
}

std::string SecureHttpClient::readResponse()
{
    std::string response;
    bool headers_complete = false;

    // Read headers
    while (_client->connected() && !headers_complete)
    {
        std::string line = _client->readStringUntil('\n');
        response += line;

        // Check for empty line that separates headers from body
        if (line == "\r" || line == "\r\n")
        {
            headers_complete = true;
        }

        // Prevent infinite loop if no proper header termination
        if (response.length() > 16384)
        { // 16KB max header size
            break;
        }
    }

    // Read body if there's data available
    int content_length = 0;
    auto it = response.find("Content-Length: ");
    if (it != std::string::npos)
    {
        size_t end = response.find("\r\n", it);
        if (end != std::string::npos)
        {
            content_length = std::stoi(response.substr(it + 16, end - (it + 16)));
        }
    }

    // Read exact content length if specified
    if (content_length > 0)
    {
        while (_client->available() && content_length > 0)
        {
            char c = static_cast<char>(_client->read());
            response += c;
            content_length--;
        }
    }
    else
    {
        // Read any remaining data
        while (_client->available())
        {
            char c = static_cast<char>(_client->read());
            response += c;
        }
    }

    return response;
}

HttpResponse SecureHttpClient::parseResponse(const std::string &rawResponse)
{
    HttpResponse response;
    std::istringstream responseStream(rawResponse);
    std::string line;

    // Parse status line
    if (std::getline(responseStream, line))
    {
        if (line.length() > 12)
        {
            response.statusCode = std::stoi(line.substr(9, 3));
        }
        else
        {
            response.statusCode = 500;
        }
    }

    // Parse headers
    while (std::getline(responseStream, line) && line != "\r")
    {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2); // Skip ": "
            if (!value.empty() && value.back() == '\r')
            {
                value.pop_back();
            }
            response.headers[key] = value;
        }
    }

    // Parse body
    std::string body;
    while (std::getline(responseStream, line))
    {
        body += line;
        if (!responseStream.eof())
        {
            body += "\n";
        }
    }

    // Trim trailing whitespace from body
    while (!body.empty() && (body.back() == '\n' || body.back() == '\r' || body.back() == ' '))
    {
        body.pop_back();
    }

    response.body = body;
    return response;
}

void SecureHttpClient::writeHeaders(const std::map<std::string, std::string> &headers)
{
    for (const auto &[key, value] : headers)
    {
        _client->println(key + ": " + value);
    }
}