#ifndef GLUCOSE_READING_H
#define GLUCOSE_READING_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <time.h>
#include "dexcom_constants.h"
#include "dexcom_errors.h"

class GlucoseReading
{
public:
    GlucoseReading(const JsonObject &jsonGlucoseReading);

    int getValue() const;
    int getMgDl() const;
    float getMmolL() const;
    int getTrend() const;
    String getTrendDirection() const;
    String getTrendDescription() const;
    String getTrendArrow() const;
    time_t getDateTime() const;
    JsonObject getJson() const;

    String toString() const;

private:
    int _value;
    String _trendDirection;
    int _trend;
    time_t _datetime;
    JsonObject _json;

    void parseJsonReading(const JsonObject &jsonGlucoseReading);
};

#endif // GLUCOSE_READING_H