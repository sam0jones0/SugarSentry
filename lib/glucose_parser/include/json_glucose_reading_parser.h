#ifndef JSON_GLUCOSE_READING_PARSER_H
#define JSON_GLUCOSE_READING_PARSER_H

#include "i_glucose_reading_parser.h"
#include "i_json_parser.h"
#include "i_json_value.h"
#include "debug_print.h"
#include "dexcom_errors.h"
#include "dexcom_constants.h"

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

    /**
     * @brief Parse a single JSON object into a GlucoseReading
     *
     * @param jsonObject String containing a single JSON object
     * @return std::optional<GlucoseReading> The parsed reading if successful
     */
    std::optional<GlucoseReading> parseJsonObject(const std::string &jsonObject);
};

#endif // JSON_GLUCOSE_READING_PARSER_H
