#ifndef MOCK_SECURE_CLIENT_H
#define MOCK_SECURE_CLIENT_H

#include "i_secure_client.h"
#include <string>
#include <vector>

class MockSecureClient : public ISecureClient
{
public:
    bool connect(const char *host, uint16_t port) override;
    size_t write(const uint8_t *buf, size_t size) override;
    size_t write(const char *buf) override;
    int available() override;
    int read() override;
    int read(uint8_t *buf, size_t size) override;
    void stop() override;
    bool connected() override;
    void setTimeout(uint32_t timeout) override;

    // Test control functions
    void setConnected(bool isConnected);
    void setNextReadData(const std::string &data);
    std::string getLastWrittenData() const;

private:
    bool isConnected = false;
    std::string readData;
    size_t readIndex = 0;
    uint32_t timeout = 0;
    std::vector<uint8_t> writtenData;
};

#endif // MOCK_SECURE_CLIENT_H