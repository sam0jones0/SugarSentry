#ifndef I_JSON_VALUE_H
#define I_JSON_VALUE_H

#include <string>
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

#endif // I_JSON_VALUE_H
