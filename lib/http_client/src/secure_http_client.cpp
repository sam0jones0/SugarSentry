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
    DEBUG_PRINTF(">> %s %s HTTP/1.1\n", request.method.c_str(), request.url.c_str());
    _client->println(request.method + " " + request.url + " HTTP/1.1");

    // Write headers
    DEBUG_PRINTF(">> Host: %s\n", _host.c_str());
    _client->println("Host: " + _host);
    writeHeaders(request.headers);

    // Write body if present
    if (request.body)
    {
        DEBUG_PRINTF(">> Content-Length: %zu\n", request.body->length());
        _client->println("Content-Length: " + std::to_string(request.body->length()));
        DEBUG_PRINT(">> [blank line]");
        _client->println();
        DEBUG_PRINTF(">> Body:\n%s\n", request.body->c_str());
        _client->println(*request.body);
    }
    else
    {
        DEBUG_PRINT(">> [blank line]");
        _client->println();
    }

    RawHttpResponse rawResponse = readResponse();
    return parseResponse(rawResponse);
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

RawHttpResponse SecureHttpClient::readResponse()
{
    RawHttpResponse response;

    // Read headers line by line
    while (_client->connected())
    {
        std::string line = _client->readStringUntil('\n');
        response.headersStr += line;

        // Check for empty line that separates headers from body
        if (line == "\r" || line == "\r\n")
        {
            break; // Headers complete, don't read the body
        }

        // Prevent infinite loop if no proper header termination
        if (response.headersStr.length() > 8192)
        { // 8KB max header size
            break;
        }
    }

    // Leave bodyStr empty - don't attempt to read the body
    
    return response;
}

HttpResponse SecureHttpClient::parseResponse(const RawHttpResponse &rawResponse)
{
    HttpResponse response;
    
    // Parse status line and headers
    size_t pos = 0;
    size_t end_line = rawResponse.headersStr.find("\r\n", pos);
    
    // Extract status line (first line in headers)
    if (end_line != std::string::npos) {
        std::string status_line = rawResponse.headersStr.substr(pos, end_line - pos);
        
        // Parse status code from status line (typically "HTTP/1.1 200 OK")
        if (status_line.length() > 12) {
            try {
                response.statusCode = std::stoi(status_line.substr(9, 3));
            } catch (...) {
                response.statusCode = 500; // Default to 500 on parsing error
            }
        } else {
            response.statusCode = 500; // Default to 500 for malformed status line
        }
        
        pos = end_line + 2; // Move past this line and the \r\n
    } else {
        response.statusCode = 500; // Default to 500 for completely malformed response
        pos = rawResponse.headersStr.length(); // Skip header parsing
    }
    
    // Parse headers (each line until empty line)
    while (pos < rawResponse.headersStr.length()) {
        end_line = rawResponse.headersStr.find("\r\n", pos);
        if (end_line == std::string::npos) {
            break; // No more lines
        }
        
        std::string line = rawResponse.headersStr.substr(pos, end_line - pos);
        pos = end_line + 2; // Move past this line and the \r\n
        
        // Empty line indicates end of headers
        if (line.empty()) {
            break;
        }
        
        // Parse header: "Key: Value"
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            // Skip the ": " after the colon - at least one space is expected
            size_t value_start = colon_pos + 1;
            while (value_start < line.length() && line[value_start] == ' ') {
                value_start++;
            }
            std::string value = line.substr(value_start);
            
            response.headers[key] = value;
        }
    }

    // The body is directly from rawResponse.bodyStr
    response.body = rawResponse.bodyStr;
    
    return response;
}

void SecureHttpClient::writeHeaders(const std::map<std::string, std::string> &headers)
{
    for (const auto &[key, value] : headers)
    {
        DEBUG_PRINTF(">> %s: %s\n", key.c_str(), value.c_str());
        _client->println(key + ": " + value);
    }
}
