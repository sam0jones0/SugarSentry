#include "esp32_secure_client.h"

ESP32SecureClient::ESP32SecureClient()
{
    _client.setInsecure(); // For now, we're not using certificate validation
}

bool ESP32SecureClient::connect(const char *host, uint16_t port)
{
    return _client.connect(host, port);
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