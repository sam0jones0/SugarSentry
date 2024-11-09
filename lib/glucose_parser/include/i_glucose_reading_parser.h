#ifndef I_GLUCOSE_READING_PARSER_H
#define I_GLUCOSE_READING_PARSER_H

#include <vector>
#include <string>
#include "glucose_reading.h"

class IGlucoseReadingParser
{
public:
    virtual ~IGlucoseReadingParser() = default;

    /**
     * @brief Parse a string response into a vector of glucose readings
     *
     * @param response The raw response string containing glucose readings data
     * @return std::vector<GlucoseReading> Vector of parsed glucose readings
     * @throws ArgumentError if the response is invalid or cannot be parsed
     */
    virtual std::vector<GlucoseReading> parse(const std::string &response) = 0;
};

#endif // I_GLUCOSE_READING_PARSER_H