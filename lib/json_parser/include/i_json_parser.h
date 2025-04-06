#ifndef I_JSON_PARSER_H
#define I_JSON_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <ArduinoJson.h>
#include "i_json_value.h"

/**
 * @brief Interface for basic JSON parsing operations
 */
class IJsonParser {
public:
    virtual ~IJsonParser() = default;

    /**
     * @brief Parse a JSON string into an array and process each element directly
     * @param jsonString The JSON string to parse
     * @param elementProcessor Function to process each array element
     * @return True if parsing was successful, false otherwise
     */
    virtual bool parseJsonArray(const std::string& jsonString, 
                               std::function<bool(ArduinoJson::JsonObjectConst)> elementProcessor) = 0;
};

#endif // I_JSON_PARSER_H
