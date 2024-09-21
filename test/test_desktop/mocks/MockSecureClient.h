#ifndef MOCK_SECURE_CLIENT_H
#define MOCK_SECURE_CLIENT_H

#include "ISecureClient.h"
#include <string>

class MockSecureClient : public ISecureClient
{
public:
    bool connect(const char *host, uint16_t port) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int available() override;
    int read() override;
    int read(uint8_t *buf, size_t size) override;
    void stop() override;
    bool connected() override;

    // Test control functions
    void setConnected(bool isConnected);
    void setNextReadData(const std::string &data);

private:
    bool isConnected = false;
    std::string readData;
    size_t readIndex = 0;
};

#endif // MOCK_SECURE_CLIENT_H