#include "mock_secure_client.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>

bool MockSecureClient::connect(const char *host, uint16_t port)
{
    return isConnected;
}

size_t MockSecureClient::write(const uint8_t *buf, size_t size)
{
    // Store the complete data chunk
    std::string chunk(reinterpret_cast<const char*>(buf), size);
    writtenData.insert(writtenData.end(), buf, buf + size);
    return size;
}

size_t MockSecureClient::write(const char *buf)
{
    size_t len = strlen(buf);
    return write(reinterpret_cast<const uint8_t *>(buf), len);
}

int MockSecureClient::available()
{
    return readData.length() - readIndex;
}

int MockSecureClient::read()
{
    if (readIndex < readData.length())
    {
        return readData[readIndex++];
    }
    return -1;
}

int MockSecureClient::read(uint8_t *buf, size_t size)
{
    size_t bytesToRead = std::min(size, readData.length() - readIndex);
    if (bytesToRead > 0)
    {
        std::memcpy(buf, readData.c_str() + readIndex, bytesToRead);
        readIndex += bytesToRead;
        return bytesToRead;
    }
    return 0;
}

void MockSecureClient::stop()
{
    isConnected = false;
    readData.clear();
    readIndex = 0;
    writtenData.clear();
}

bool MockSecureClient::connected()
{
    // Return false if we've read all the data, even if technically still "connected"
    return isConnected && (readIndex < readData.length());
}

void MockSecureClient::setTimeout(uint32_t timeout)
{
    this->timeout = timeout;
}

void MockSecureClient::println(const std::string &data)
{
    // Write the data and CRLF as a single chunk
    std::string complete = data + "\r\n";
    write(reinterpret_cast<const uint8_t *>(complete.c_str()), complete.length());
}

void MockSecureClient::println()
{
    write(reinterpret_cast<const uint8_t *>("\r\n"), 2);
}

std::string MockSecureClient::readStringUntil(char terminator)
{
    std::string result;
    while (readIndex < readData.length())
    {
        char c = readData[readIndex++];
        result += c;
        if (c == terminator)
        {
            // If we read a \n, make sure we've included any preceding \r
            if (terminator == '\n' && result.length() > 1 && result[result.length()-2] != '\r')
            {
                // If there was no \r before the \n, back up one character
                readIndex--;
                result.pop_back();
                break;
            }
            break;
        }
    }
    return result;
}

// Test control functions
void MockSecureClient::setConnected(bool connected)
{
    isConnected = connected;
    // Reset read position when connection state changes
    if (!connected) {
        readIndex = 0;
    }
}

void MockSecureClient::setNextReadData(const std::string &data)
{
    // Ensure proper line endings in the response
    std::string processedData = data;
    size_t pos = 0;
    while ((pos = processedData.find('\n', pos)) != std::string::npos) {
        if (pos == 0 || processedData[pos - 1] != '\r') {
            processedData.insert(pos, "\r");
            pos += 2;
        } else {
            pos++;
        }
    }

    // Add proper HTTP response format if not already present
    if (processedData.find("HTTP/1.1") == std::string::npos) {
        std::stringstream ss;
        ss << "HTTP/1.1 200 OK\r\n"
           << "Content-Type: application/json\r\n"
           << "Content-Length: " << processedData.length() << "\r\n"
           << "\r\n"
           << processedData;
        processedData = ss.str();
    }

    // Ensure the response ends with a newline
    if (!processedData.empty() && processedData.back() != '\n') {
        processedData += "\r\n";
    }

    readData = processedData;
    readIndex = 0;

    // Log the mock response for debugging
    std::cout << "Mock response data:\n" << readData << std::endl;
}

std::string MockSecureClient::getLastWrittenData() const
{
    return std::string(writtenData.begin(), writtenData.end());
}
