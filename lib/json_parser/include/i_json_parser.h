#ifndef I_JSON_PARSER_H
#define I_JSON_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "i_json_value.h"

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

#endif // I_JSON_PARSER_H
