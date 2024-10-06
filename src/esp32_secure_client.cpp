#include "esp32_secure_client.h"
#include <Arduino.h>

ESP32SecureClient::ESP32SecureClient()
{
}

bool ESP32SecureClient::connect(const char *host, uint16_t port)
{
    Serial.printf("Attempting to connect to %s:%d\n", host, port);
    if (_rootCA)
    {
        _client.setCACert(_rootCA);
    }
    else
    {
        _client.setInsecure(); // Fallback to insecure if no cert provided
        Serial.println("Falling back to insecure");
    }
    return _client.connect(host, port);
}

void ESP32SecureClient::setCACert(const char *rootCA)
{
    _rootCA = rootCA;
}

size_t ESP32SecureClient::write(const uint8_t *buf, size_t size)
{
    return _client.write(buf, size);
}

size_t ESP32SecureClient::write(const char *buf)
{
    return _client.write(reinterpret_cast<const uint8_t *>(buf), strlen(buf));
}

int ESP32SecureClient::available()
{
    return _client.available();
}

int ESP32SecureClient::read()
{
    return _client.read();
}

int ESP32SecureClient::read(uint8_t *buf, size_t size)
{
    return _client.read(buf, size);
}

void ESP32SecureClient::stop()
{
    _client.stop();
}

bool ESP32SecureClient::connected()
{
    return _client.connected();
}

void ESP32SecureClient::setTimeout(uint32_t timeout)
{
    _client.setTimeout(timeout);
}
