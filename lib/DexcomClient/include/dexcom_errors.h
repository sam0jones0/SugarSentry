#ifndef DEXCOM_ERRORS_H
#define DEXCOM_ERRORS_H

#include <Arduino.h>

// Base enum for all Dexcom error types
enum class DexcomErrorEnum {
    // Base class, no specific errors
};

// Account error enum
enum class AccountErrorEnum : DexcomErrorEnum {
    FAILED_AUTHENTICATION,
    MAX_ATTEMPTS
};

// Session error enum
enum class SessionErrorEnum : DexcomErrorEnum {
    NOT_FOUND,
    INVALID
};

// Argument error enum
enum class ArgumentErrorEnum : DexcomErrorEnum {
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
    GLUCOSE_READING_INVALID
};

// Base class for all Dexcom errors
class DexcomError : public std::exception {
protected:
    String _message;

public:
    DexcomError(const String& message) : _message(message) {}
    virtual const char* what() const noexcept override {
        return _message.c_str();
    }
};

// Specific error classes
class AccountError : public DexcomError {
public:
    AccountError(AccountErrorEnum error);
};

class SessionError : public DexcomError {
public:
    SessionError(SessionErrorEnum error);
};

class ArgumentError : public DexcomError {
public:
    ArgumentError(ArgumentErrorEnum error);
};

String getErrorMessage(AccountErrorEnum error);
String getErrorMessage(SessionErrorEnum error);
String getErrorMessage(ArgumentErrorEnum error);

#endif // DEXCOM_ERRORS_H