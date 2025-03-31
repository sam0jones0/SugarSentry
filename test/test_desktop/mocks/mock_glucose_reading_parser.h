#pragma once

#include "i_glucose_reading_parser.h"
#include "glucose_reading.h"
#include <vector>
#include <string>

class MockGlucoseReadingParser : public IGlucoseReadingParser
{
public:
    std::vector<GlucoseReading> parse(const std::string &response) override
    {
        // By default, return an empty vector for empty or invalid responses
        if (response.empty() || response == "[]" || response == "invalid") {
            return {};
        }
        
        // For other responses, return a vector with mock readings
        std::vector<GlucoseReading> readings;
        
        // Create a couple of mock readings
        // GlucoseReading(value, trend string, timestamp string)
        readings.push_back(GlucoseReading(120, "Flat", "Date(1609459200000)"));
        
        // Add a second reading if needed for more complex tests
        readings.push_back(GlucoseReading(130, "SingleUp", "Date(1609459260000)"));
        
        return readings;
    }
};
