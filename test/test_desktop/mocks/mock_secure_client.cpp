#include "mock_secure_client.h"
#include <cstring>
#include <algorithm>

bool MockSecureClient::connect(const char *host, uint16_t port)
{
    return isConnected;
}

size_t MockSecureClient::write(const uint8_t *buf, size_t size)
{
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
    return isConnected;
}

void MockSecureClient::setTimeout(uint32_t timeout)
{
    this->timeout = timeout;
}

// Test control functions
void MockSecureClient::setConnected(bool connected)
{
    isConnected = connected;
}

void MockSecureClient::setNextReadData(const std::string &data)
{
    readData = data;
    readIndex = 0;
}

std::string MockSecureClient::getLastWrittenData() const
{
    return std::string(writtenData.begin(), writtenData.end());
}