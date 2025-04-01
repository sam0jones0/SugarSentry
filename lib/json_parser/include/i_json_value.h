#ifndef I_JSON_VALUE_H
#define I_JSON_VALUE_H

#include <string>
#include <vector>
#include <memory>
#include <optional>

/**
 * @brief Interface representing a JSON value
 */
class IJsonValue {
public:
    virtual ~IJsonValue() = default;

    /**
     * @brief Get a string value
     * @param key The key to look up
     * @return Optional string value
     */
    virtual std::optional<std::string> getString(const std::string& key) const = 0;

    /**
     * @brief Get an integer value
     * @param key The key to look up
     * @return Optional integer value
     */
    virtual std::optional<int> getInt(const std::string& key) const = 0;

    /**
     * @brief Get a double value
     * @param key The key to look up
     * @return Optional double value
     */
    virtual std::optional<double> getDouble(const std::string& key) const = 0;

    /**
     * @brief Get a boolean value
     * @param key The key to look up
     * @return Optional boolean value
     */
    virtual std::optional<bool> getBool(const std::string& key) const = 0;
};

/**
 * @brief Interface for basic JSON parsing operations
 */
class IJsonParser {
public:
    virtual ~IJsonParser() = default;

    /**
     * @brief Parse a JSON string into an object
     * @param jsonString The JSON string to parse
     * @return Shared pointer to IJsonValue, null if parsing fails
     */
    virtual std::shared_ptr<IJsonValue> parseObject(const std::string& jsonString) = 0;

    /**
     * @brief Parse a JSON string into an array
     * @param jsonString The JSON string to parse
     * @return Vector of IJsonValue objects, empty if parsing fails
     */
    virtual std::vector<std::shared_ptr<IJsonValue>> parseArray(const std::string& jsonString) = 0;
};

#endif // I_JSON_VALUE_H
