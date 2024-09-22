#ifndef DEXCOM_ERRORS_H
#define DEXCOM_ERRORS_H

#include <Arduino.h>
#include <exception>

namespace DexcomErrors
{
    enum class AccountError : uint8_t
    {
        FAILED_AUTHENTICATION,
        MAX_ATTEMPTS
    };

    enum class SessionError : uint8_t
    {
        NOT_FOUND,
        INVALID,
    };

    enum class ArgumentError : uint8_t
    {
        MINUTES_INVALID,
        MAX_COUNT_INVALID,
        USERNAME_INVALID,
        TOO_MANY_USER_ID_PROVIDED,
        NONE_USER_ID_PROVIDED,
        PASSWORD_INVALID,
        ACCOUNT_ID_INVALID,
        ACCOUNT_ID_DEFAULT,
        SESSION_ID_INVALID,
        SESSION_ID_DEFAULT,
        GLUCOSE_READING_INVALID,
        RESPONSE_TOO_LARGE
    };

}

class DexcomError : public std::exception
{
protected:
    String _message;

public:
    DexcomError(const String &message) : _message(message) {}
    virtual const char *what() const noexcept override
    {
        return _message.c_str();
    }
};

class AccountError : public DexcomError
{
public:
    AccountError(DexcomErrors::AccountError error);
};

class SessionError : public DexcomError
{
public:
    SessionError(DexcomErrors::SessionError error);
};

class ArgumentError : public DexcomError
{
public:
    ArgumentError(DexcomErrors::ArgumentError error);
};

String getErrorMessage(DexcomErrors::AccountError error);
String getErrorMessage(DexcomErrors::SessionError error);
String getErrorMessage(DexcomErrors::ArgumentError error);

#endif // DEXCOM_ERRORS_H