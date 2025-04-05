#ifndef I_DEXCOM_RESPONSE_PARSER_H
#define I_DEXCOM_RESPONSE_PARSER_H

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "glucose_reading.h"
#include "dexcom_errors.h"

/**
 * @brief Interface for Dexcom JSON response parsing operations
 */
class IDexcomResponseParser {
public:
    virtual ~IDexcomResponseParser() = default;

    /**
     * @brief Parse a JSON string into a GlucoseReading object
     * @param jsonString The JSON string to parse
     * @return Optional GlucoseReading, empty if parsing fails
     */
    virtual std::optional<GlucoseReading> parseGlucoseReading(const std::string& jsonString) = 0;

    /**
     * @brief Parse a JSON string containing multiple glucose readings
     * @param jsonString The JSON string to parse
     * @return Vector of GlucoseReading objects
     */
    virtual std::vector<GlucoseReading> parseGlucoseReadings(const std::string& jsonString) = 0;

    /**
     * @brief Parse an error response from the Dexcom API
     * @param jsonString The JSON string to parse
     * @return Optional DexcomError, empty if no error is found
     */
    virtual std::unique_ptr<DexcomError> parseErrorResponse(const std::string& jsonString) = 0;
};

#endif // I_DEXCOM_RESPONSE_PARSER_H
