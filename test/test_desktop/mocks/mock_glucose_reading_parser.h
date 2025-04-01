#pragma once

#include <gmock/gmock.h>
#include "i_glucose_reading_parser.h"
#include <vector>
#include <string>
#include "glucose_reading.h"

class MockGlucoseReadingParser : public IGlucoseReadingParser {
public:
    MOCK_METHOD(std::vector<GlucoseReading>, parse, (const std::string &response), (override));
};
