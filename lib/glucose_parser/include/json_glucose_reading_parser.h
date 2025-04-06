#ifndef JSON_GLUCOSE_READING_PARSER_H
#define JSON_GLUCOSE_READING_PARSER_H

#include "i_glucose_reading_parser.h"
#include "i_json_parser.h"
#include <memory>

class JsonGlucoseReadingParser : public IGlucoseReadingParser
{
public:
    /**
     * @brief Constructs a JsonGlucoseReadingParser with a JSON parser
     * 
     * @param jsonParser The JSON parser implementation to use
     */
    explicit JsonGlucoseReadingParser(std::shared_ptr<IJsonParser> jsonParser);

    /**
     * @brief Parse JSON formatted response into glucose readings
     *
     * @param response JSON string containing glucose readings
     * @return std::vector<GlucoseReading> Vector of parsed glucose readings
     * @throws ArgumentError if JSON is invalid or cannot be parsed
     */
    std::vector<GlucoseReading> parse(const std::string &response) override;

private:
    std::shared_ptr<IJsonParser> _jsonParser;

};

#endif // JSON_GLUCOSE_READING_PARSER_H
