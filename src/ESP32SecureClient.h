#ifndef ESP32_SECURE_CLIENT_H
#define ESP32_SECURE_CLIENT_H

#include "ISecureClient.h"
#include <WiFiClientSecure.h>

class ESP32SecureClient : public ISecureClient
{
public:
    ESP32SecureClient();
    bool connect(const char *host, uint16_t port) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int available() override;
    int read() override;
    int read(uint8_t *buf, size_t size) override;
    void stop() override;
    bool connected() override;

private:
    WiFiClientSecure client;
};

#endif // ESP32_SECURE_CLIENT_H