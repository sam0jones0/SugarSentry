#ifndef ARDUINO_JSON_PARSER_H
#define ARDUINO_JSON_PARSER_H

#include "i_json_parser.h"
#include "../../dexcom_client/include/dexcom_constants.h"
#include <ArduinoJson.h>

/**
 * @brief ArduinoJson implementation of IJsonParser
 */
class ArduinoJsonParser : public IJsonParser {
public:
    bool parseJsonArray(const std::string& jsonString, 
                       std::function<bool(ArduinoJson::JsonObjectConst)> elementProcessor) override;

private:
};

#endif // ARDUINO_JSON_PARSER_H
