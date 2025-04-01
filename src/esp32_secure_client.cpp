#include "esp32_secure_client.h"
#include <Arduino.h>
#include <dexcom_constants.h>

ESP32SecureClient::ESP32SecureClient()
{
}

bool ESP32SecureClient::connect(const char *host, uint16_t port)
{
    Serial.printf("Attempting to connect to %s:%d\n", host, port);
    if (_rootCA)
    {
        Serial.println("Using provided root CA");
        _client.setCACert(_rootCA);
    }
    else
    {
        _client.setInsecure();
        Serial.println("Falling back to insecure");
    }

    // Single connection attempt
    if (_client.connect(host, port))
    {
        Serial.println("TCP connection established");
        if (_client.connected())
        {
            Serial.println("SSL/TLS handshake completed successfully");
            return true;
        }
        else
        {
            Serial.println("SSL/TLS handshake failed");
        }
    }
    
    // Log the error
    char error_buffer[100];
    _client.lastError(error_buffer, sizeof(error_buffer));
    Serial.printf("Connection failed. Error: %s\n", error_buffer);
    
    return false;
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

void ESP32SecureClient::println(const std::string& data)
{
    _client.println(data.c_str());
}

void ESP32SecureClient::println()
{
    _client.println();
}

std::string ESP32SecureClient::readStringUntil(char terminator)
{
    String result = _client.readStringUntil(terminator);
    return std::string(result.c_str());
}

void ESP32SecureClient::setTimeout(uint32_t timeout)
{
    _client.setTimeout(timeout);
}
