#ifndef MOCK_SECURE_CLIENT_H
#define MOCK_SECURE_CLIENT_H

#include "i_secure_client.h"
#include <queue>
#include <string>
#include <cstdint>
#include <string.h>

class MockSecureClient : public ISecureClient
{
public:
    bool connect(const char *host, uint16_t port) override
    {
        last_host = host;
        last_port = port;
        return should_connect;
    }

    size_t write(const uint8_t *buf, size_t size) override
    {
        std::string data(reinterpret_cast<const char *>(buf), size);
        written_data.push(data);
        return size;
    }

    size_t write(const char *buf) override
    {
        written_data.push(std::string(buf));
        return strlen(buf);
    }

    int available() override
    {
        return static_cast<int>(current_read_data.length() - read_position);
    }

    int read() override
    {
        if (read_position >= current_read_data.length())
            return -1;
        return current_read_data[read_position++];
    }

    int read(uint8_t *buf, size_t size) override
    {
        size_t remaining = current_read_data.length() - read_position;
        size_t to_read = std::min(size, remaining);
        if (to_read == 0)
            return 0;

        memcpy(buf, current_read_data.c_str() + read_position, to_read);
        read_position += to_read;
        return static_cast<int>(to_read);
    }

    void stop() override
    {
        is_connected = false;
    }

    bool connected() override
    {
        return is_connected;
    }

    void setTimeout(uint32_t timeout) override
    {
        last_timeout = timeout;
    }

    void println(const std::string &data) override
    {
        written_lines.push(data + "\r\n");
    }

    void println() override
    {
        written_lines.push("\r\n");
    }

    std::string readStringUntil(char terminator) override
    {
        size_t pos = current_read_data.find(terminator, read_position);
        if (pos == std::string::npos)
        {
            if (read_position >= current_read_data.length())
                return "";
            pos = current_read_data.length();
        }

        std::string result = current_read_data.substr(read_position, pos - read_position + 1);
        read_position = pos + 1;
        return result;
    }

    // Test control methods
    void setConnected(bool connected) { is_connected = connected; }
    void setShouldConnect(bool should) { should_connect = should; }
    void setNextReadData(const std::string &data)
    {
        current_read_data = data;
        read_position = 0;
    }

    std::string getLastWrittenData()
    {
        if (written_data.empty())
            return "";
        std::string data = written_data.front();
        written_data.pop();
        return data;
    }

    // Test inspection
    std::string last_host;
    uint16_t last_port{0};
    uint32_t last_timeout{0};

private:
    bool is_connected{false};
    bool should_connect{true};
    std::queue<std::string> written_data;
    std::queue<std::string> written_lines;
    std::string current_read_data;
    size_t read_position{0};
};

#endif // MOCK_SECURE_CLIENT_H