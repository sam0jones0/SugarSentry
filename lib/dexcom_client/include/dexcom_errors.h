#ifndef DEXCOM_ERRORS_H
#define DEXCOM_ERRORS_H

#include <exception>
#include <string>

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

// Forward declarations
std::string getErrorMessage(DexcomErrors::AccountError error);
std::string getErrorMessage(DexcomErrors::SessionError error);
std::string getErrorMessage(DexcomErrors::ArgumentError error);

class DexcomError : public std::exception
{
protected:
    std::string _message;

public:
    DexcomError(const std::string &message) : _message(message) {}
    virtual const char *what() const noexcept override
    {
        return _message.c_str();
    }
};

class AccountError : public DexcomError
{
public:
    AccountError(DexcomErrors::AccountError error) : DexcomError(getErrorMessage(error)) {}
};

class SessionError : public DexcomError
{
public:
    SessionError(DexcomErrors::SessionError error) : DexcomError(getErrorMessage(error)) {}
};

class ArgumentError : public DexcomError
{
public:
    ArgumentError(DexcomErrors::ArgumentError error) : DexcomError(getErrorMessage(error)) {}
};

// Function implementations
inline std::string getErrorMessage(DexcomErrors::AccountError error)
{
    switch (error)
    {
    case DexcomErrors::AccountError::FAILED_AUTHENTICATION:
        return "Failed authentication";
    case DexcomErrors::AccountError::MAX_ATTEMPTS:
        return "Maximum attempts reached";
    default:
        return "Unknown account error";
    }
}

inline std::string getErrorMessage(DexcomErrors::SessionError error)
{
    switch (error)
    {
    case DexcomErrors::SessionError::NOT_FOUND:
        return "Session not found";
    case DexcomErrors::SessionError::INVALID:
        return "Invalid session";
    default:
        return "Unknown session error";
    }
}

inline std::string getErrorMessage(DexcomErrors::ArgumentError error)
{
    switch (error)
    {
    case DexcomErrors::ArgumentError::MINUTES_INVALID:
        return "Invalid minutes";
    case DexcomErrors::ArgumentError::MAX_COUNT_INVALID:
        return "Invalid max count";
    case DexcomErrors::ArgumentError::USERNAME_INVALID:
        return "Invalid username";
    case DexcomErrors::ArgumentError::TOO_MANY_USER_ID_PROVIDED:
        return "Too many user IDs provided";
    case DexcomErrors::ArgumentError::NONE_USER_ID_PROVIDED:
        return "No user ID provided";
    case DexcomErrors::ArgumentError::PASSWORD_INVALID:
        return "Invalid password";
    case DexcomErrors::ArgumentError::ACCOUNT_ID_INVALID:
        return "Invalid account ID";
    case DexcomErrors::ArgumentError::ACCOUNT_ID_DEFAULT:
        return "Default account ID used";
    case DexcomErrors::ArgumentError::SESSION_ID_INVALID:
        return "Invalid session ID";
    case DexcomErrors::ArgumentError::SESSION_ID_DEFAULT:
        return "Default session ID used";
    case DexcomErrors::ArgumentError::GLUCOSE_READING_INVALID:
        return "Invalid glucose reading";
    case DexcomErrors::ArgumentError::RESPONSE_TOO_LARGE:
        return "Response too large";
    default:
        return "Unknown argument error";
    }
}

#endif // DEXCOM_ERRORS_H