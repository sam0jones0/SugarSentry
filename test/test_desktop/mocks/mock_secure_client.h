#pragma once

#include "i_secure_client.h"
#include <string>
#include <cstring>

// Google Mock style interface
class MockSecureClient : public ISecureClient
{
public:
    bool connect(const char* host, uint16_t port) override { return true; }
    size_t write(const uint8_t* buf, size_t size) override { return size; }
    size_t write(const char* buf) override { return strlen(buf); }
    int available() override { return 0; }
    int read() override { return -1; }
    int read(uint8_t* buf, size_t size) override { return 0; }
    void stop() override {}
    bool connected() override { return true; }
    void setTimeout(uint32_t timeout) override {}
    void println(const std::string& data) override {}
    void println() override {}
    std::string readStringUntil(char terminator) override { return ""; }
};
