Task 42

Refactored SecureHttpClient::readResponse to separate HTTP headers and body processing. The method now returns a RawHttpResponse struct containing distinct headersStr and bodyStr fields instead of a combined string. This separation maintains the original header parsing approach (line-by-line) that worked in the previous implementation, while ensuring the body can be accurately extracted after the header/body boundary. The parseResponse method was updated to work with this new structure, allowing for more precise HTTP response handling. Tests failing for the time being.

Task 41

Removed unused static buffer size constant and JsonDocument type alias from ArduinoJsonParser header.

----

Task 40

Successfully cleaned up unnecessary #include directives from multiple files after the removal of IJsonValue, ArduinoJsonValue, and related functionality. Specifically:

----

Task 39

Removed GlucoseReading(const IJsonValue&) constructor and corresponding tests, with all tests passing.

----

Task 38: Removed the `IJsonValue` interface and related code/tests.

----

Task 34-37: Refined `IJsonParser`: removed unused `parseObject`/`parseArray`, improved `parseJsonArray` memory efficiency (`DynamicJsonDocument`), removed redundant `JsonGlucoseReadingParser::parseJsonObject`.

----

Task 32-33: Refactored `JsonGlucoseReadingParser::parse` to use `IJsonParser::parseJsonArray` callback; updated tests for callback mechanism, error handling, and size limits.

----

Task 29-31: Introduced `IJsonParser::parseJsonArray` callback method and `GlucoseReading(JsonObjectConst)` constructor for efficient direct parsing.

----

Task 28: Moved `IJsonParser` interface definition to `i_json_parser.h`.

----

Task 27: Updated `main.cpp` with correct dependency injection for `DexcomClient`.

----

Task 26: Removed redundant error checks in `DexcomClient::post`.

----

Task 25: Added tests for `DexcomUtils::stringToTrendDirection`, fixed null handling.

----

Task 23-24: Added `DexcomClient::getGlucoseReadings` tests for invalid parameters and session refresh on failure.

----

Task 20-22: Removed internal retry logic from `DexcomClient::post` and `ESP32SecureClient::connect`.

----

Task 15-18: Implemented comprehensive `DexcomClient` tests: fixture, construction (success/failure), `getGlucoseReadings`.

----

Task 10, 12-14: Standardized mocks for `ISecureClient`, `IJsonParser`, `IHttpClient`, `IGlucoseReadingParser` using GMock.

----

Task 8-9: Added `SecureHttpClient` tests: constructor, connection, GET/POST, error handling.

----

Task 7: Added tests for (now removed) `GlucoseReading(IJsonValue)` constructor error handling.

----

Task 6: Migrated desktop tests to GoogleTest.

----

Task 3-5: Major refactor: Introduced `IHttpClient`, `IJsonParser`, `IGlucoseReadingParser` interfaces and implementations (`SecureHttpClient`, `ArduinoJsonParser`, `JsonGlucoseReadingParser`) to separate concerns (Network, JSON, Parsing) from `DexcomClient`.

----

Task 2

Improve the original DexcomClient code by following strict software engineering practices

Focus on dependency injection, testability, and separation of concerns

Make the code more maintainable and easier to test

CHANGES MADE SO FAR:

    Separated HTTP handling from DexcomClient by creating:
        IHttpClient interface for high-level HTTP operations
        HttpRequest and HttpResponse structs for clean data handling
        SecureHttpClient implementation using the existing ISecureClient

    Created proper test infrastructure:
        MockSecureClient for testing
        Comprehensive tests for HTTP functionality
        Test fixtures and setup/teardown handling

----

Task 1 

Understand the background of the project

"""
Project Goal: Develop a portable device that displays blood sugar levels from a Dexcom API, shows historical data in a graph, and monitors room temperature, humidity, and proximity. The device uses an e-ink display for low power consumption and long battery life.

Main Development Board:
- Adafruit HUZZAH32 – ESP32 Feather Board

Components:
- 7.5-inch (800x480) E-Paper Display
- BME280 temperature/humidity sensor
- VCNL4040 proximity and ambient light sensor
- LiPo battery (for portability)

GPIO/Integration Requirements:
- SPI interface for E-Paper Display (6-8 pins)
- I2C for BME280 and VCNL4040 sensors (2 pins, shared)
- 1 additional pin for VCNL4040 interrupt
- 1-2 pins for user input buttons
- 1 pin for status LED

Board Specifications (Adafruit HUZZAH32):
- ESP32 dual-core processor
- 4MB Flash
- 520KB SRAM
- Wi-Fi and Bluetooth connectivity
- Built-in LiPo battery charging
- USB connectivity for programming and debugging
- Breadboard-friendly form factor

Power Management:
- Utilizes ESP32's low power modes for extended battery life
- Built-in LiPo charging capability

Connectivity:
- Wi-Fi for API calls to Dexcom
- Bluetooth available for potential future expansions

Development Environment:
- Compatible with Arduino IDE and PlatformIO

Additional Advantages:
- Compact form factor suitable for portable device design
- Good community support and available libraries
- Cost-effective for prototype development


## Development Setup

platformIO in vscode on Windows 10

Written in C++

```ini
[env:featheresp32]
platform = espressif32
board = esp32dev
framework = arduino
build_unflags = -std=gnu++11
build_flags = -std=gnu++17
monitor_speed = 115200
upload_speed = 921600
lib_deps = 
    bblanchon/ArduinoJson @ ^6.18.5
test_ignore = test_desktop

[env:native]
platform = native
build_unflags = -std=gnu++11
build_flags = -std=gnu++17
test_ignore = test_embedded
```

----

The user is professional software engineer who is familiar with Python, Javascript, Scala, Java. However, they are not too familiar with C/C++.

The user is rewriting a Python library into C++ for use within the embedded project. All questions will pertain to that goal. The user may copy Python code in and ask for it to be translated into C++.

Memory/CPU contraints are a concern and code should be optimised for this purpose.
"""
