#ifndef ESP32_SECURE_CLIENT_H
#define ESP32_SECURE_CLIENT_H

#include "i_secure_client.h"
#include <WiFiClientSecure.h>

class ESP32SecureClient : public ISecureClient
{
public:
    ESP32SecureClient();
    virtual ~ESP32SecureClient() override = default;

    bool connect(const char *host, uint16_t port) override;
    size_t write(const uint8_t *buf, size_t size) override;
    size_t write(const char *buf) override;
    int available() override;
    int read() override;
    int read(uint8_t *buf, size_t size) override;
    void stop() override;
    bool connected() override;
    void setTimeout(uint32_t timeout) override;

    // New methods
    void println(const std::string &data) override;
    void println() override;
    std::string readStringUntil(char terminator) override;

    void setCACert(const char *rootCA);

private:
    WiFiClientSecure _client;
    const char *_rootCA;
};

#endif // ESP32_SECURE_CLIENT_H