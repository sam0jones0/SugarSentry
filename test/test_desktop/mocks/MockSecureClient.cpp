#include "MockSecureClient.h"
#include <cstring>
#include <algorithm>

bool MockSecureClient::connect(const char *host, uint16_t port)
{
    return isConnected;
}

size_t MockSecureClient::write(const uint8_t *buf, size_t size)
{
    // just return the size as if all data was written
    return size;
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
}

bool MockSecureClient::connected()
{
    return isConnected;
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