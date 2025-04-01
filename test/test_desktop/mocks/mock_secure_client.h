#pragma once

#include <gmock/gmock.h>
#include <string>
#include "i_secure_client.h"

class MockSecureClient : public ISecureClient
{
public:
    MOCK_METHOD(bool, connect, (const char* host, uint16_t port), (override));
    MOCK_METHOD(size_t, write, (const uint8_t* buf, size_t size), (override));
    MOCK_METHOD(size_t, write, (const char* buf), (override));
    MOCK_METHOD(int, available, (), (override));
    MOCK_METHOD(int, read, (), (override));
    MOCK_METHOD(int, read, (uint8_t* buf, size_t size), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, connected, (), (override));
    MOCK_METHOD(void, setTimeout, (uint32_t timeout), (override));
    MOCK_METHOD(void, println, (const std::string& data), (override));
    MOCK_METHOD(void, println, (), (override));
    MOCK_METHOD(std::string, readStringUntil, (char terminator), (override));
};
