#ifndef ISECURE_CLIENT_H
#define ISECURE_CLIENT_H

#include <cstdint>
#include <cstddef>

class ISecureClient
{
public:
    virtual ~ISecureClient() = default;
    virtual bool connect(const char *host, uint16_t port) = 0;
    virtual size_t write(const uint8_t *buf, size_t size) = 0;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int read(uint8_t *buf, size_t size) = 0;
    virtual void stop() = 0;
    virtual bool connected() = 0;
};

#endif // ISECURE_CLIENT_H