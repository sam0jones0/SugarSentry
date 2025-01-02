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

## Project Structure and Organization

```
project-root/
├── doc/
│   ├── ComponentsGPIO.md
│   ├── ProjectGoal.md
│   ├── SoftwareEng.md
│   ├── fritzing/
│   │   └── SugarSentry.fzz
│   └── schematics/
│       ├── Adafruit HUZZAH32 ESP32 Feather Pinout.pdf
│       └── Adafruit HUZZAH32 ESP32 Feather Pinout.png
├── include/
│   ├── config.h
│   └── README
├── lib/
│   ├── README
│   ├── debug_print/
│   │   └── debug_print.h
│   ├── dexcom_client/
│   │   ├── include/
│   │   │   ├── dexcom_client.h
│   │   │   ├── dexcom_constants.h
│   │   │   ├── dexcom_errors.h
│   │   │   ├── dexcom_utils.h
│   │   │   └── glucose_reading.h
│   │   └── src/
│   │       ├── dexcom_client.cpp
│   │       ├── dexcom_constants.cpp
│   │       └── dexcom_utils.cpp
│   └── i_secure_client/
│       └── i_secure_client.h
├── src/
│   ├── esp32_secure_client.cpp
│   ├── esp32_secure_client.h
│   └── main.cpp
└── test/
    ├── README
    ├── test_desktop/
    │   ├── test_main.cpp
    │   ├── integration/
    │   ├── mocks/
    │   │   ├── mock_secure_client.cpp
    │   │   └── mock_secure_client.h
    │   └── unit/
    │       ├── test_dexcom_client.cpp
    │       ├── test_dexcom_utils.cpp
    │       └── test_glucose_reading.cpp
    └── test_embedded/
```

----

The user is professional software engineer who is familiar with Python, Javascript, Scala, Java. However, they are not too familiar with C/C++.

The user is rewriting a Python library into C++ for use within the embedded project. All questions will pertain to that goal. The user may copy Python code in and ask for it to be translated into C++.

Memory/CPU contraints are a concern and code should be optimised for this purpose.
"""

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


We're breaking apart the original `dexcom_client.cpp`/`.h` files. Specifically, we're extracting and replacing all the HTTP/networking code that was originally mixed into the DexcomClient class.

From the original `dexcom_client.cpp`, this is the key networking code we're replacing:

```cpp
std::string DexcomClient::post(const std::string &endpoint,
                              const std::string &params,
                              const std::string &json)
{
    if (!ensureConnected()) {
        throw SessionError(DexcomErrors::SessionError::INVALID);
    }

    std::string url = "/ShareWebServices/Services/" + endpoint;
    if (!params.empty()) {
        url += "?" + params;
    }

    // Raw HTTP handling
    _client.println("POST " + url + " HTTP/1.1");
    _client.println("Host: " + _base_url);
    _client.println("Content-Type: application/json");
    _client.println("Connection: keep-alive");
    if (!json.empty()) {
        _client.println("Content-Length: " + std::to_string(json.length()));
        _client.println();
        _client.println(json);
    }
    else {
        _client.println();
    }

    // Response handling
    std::string statusLine = _client.readStringUntil('\n');
    int statusCode = parseHttpStatusCode(statusLine);
    // ... more response handling ...
}
```

We're replacing this with our new abstraction layer:
- `IHttpClient` interface
- `SecureHttpClient` implementation
- `HttpRequest`/`HttpResponse` structs

Then the DexcomClient will be simplified to use these new abstractions instead of handling raw HTTP itself. The new DexcomClient would look more like:

```cpp
class DexcomClient {
private:
    std::shared_ptr<IHttpClient> _httpClient;
    // ... other members ...

public:
    DexcomClient(std::shared_ptr<IHttpClient> httpClient, /* other params */)
        : _httpClient(httpClient) {
        // ...
    }

    std::string post(const std::string &endpoint,
                    const std::string &params,
                    const std::string &json) {
        std::string url = "/ShareWebServices/Services/" + endpoint;
        if (!params.empty()) {
            url += "?" + params;
        }

        auto response = _httpClient->post(url, json);
        if (response.statusCode != 200) {
            throw SessionError(DexcomErrors::SessionError::INVALID);
        }
        return response.body;
    }
};
```

----

Task 3

```
The refactoring is now complete. We have successfully:

Separated all concerns into distinct layers:

ISecureClient for low-level socket operations
SecureHttpClient for HTTP protocol handling
IHttpClient for high-level HTTP interface
DexcomClient for business logic
Removed all HTTP handling code from DexcomClient:

No more direct socket operations
No more raw HTTP request/response formatting
Using clean HttpRequest/HttpResponse abstractions
Improved testability:

Created MockHttpClient for testing
Updated tests to use new abstractions
Added comprehensive test coverage
Proper exception testing
Implemented proper dependency injection:

DexcomClient now accepts IHttpClient
SecureHttpClient accepts ISecureClient
All dependencies are passed through constructors
The code is now more maintainable, testable, and follows SOLID principles with clear separation of concerns.
```

----

Task 4

```
Abstract JSON Parsing Logic

Currently, the DexcomClient class directly uses ArduinoJson for JSON parsing within its methods like parseGlucoseReadings() and handleResponse(). This tight coupling makes unit testing difficult and ties the class to a specific JSON library.

Task: Create an interface IJsonParser with methods needed for JSON parsing (e.g., parseObject, parseArray). Implement this interface with a class ArduinoJsonParser that uses ArduinoJson. Inject IJsonParser into DexcomClient via dependency injection. This allows you to mock the parser in tests and replace the JSON library if needed in the future.

All tests are now passing. Let's summarize what we did to fix the issues:

Changed the error handling approach from std::optional<DexcomError> to std::unique_ptr<DexcomError> to properly handle polymorphic types and prevent object slicing.
Added the <memory> header to both interface files since we're using std::unique_ptr.
Updated the test cases to work with unique_ptr instead of optional.
The changes we made ensure that:

Error objects maintain their derived type information through polymorphism
Memory is managed properly using smart pointers
The interface is cleaner and more type-safe
```

----

Task 5 (in progress)

```
Separate Parsing Logic from Network Logic

Task:

    Create a Separate Parser Class:
        Define an interface IGlucoseReadingParser with a method like parse(const std::string& response).
        Implement this interface in a class GlucoseReadingParser that handles parsing JSON strings into GlucoseReading objects.
    Refactor DexcomClient:
        Remove the JSON parsing code from DexcomClient.
        Inject an instance of IGlucoseReadingParser into DexcomClient via constructor or setter injection.
        DexcomClient will use this parser to convert raw JSON responses into domain objects.
    Benefits:
        Single Responsibility: DexcomClient focuses solely on network communication.
        Separation of Concerns: Parsing logic is encapsulated within its own class.
        Testability: You can test parsing logic independently from networking logic.
        Maintainability: Changes to parsing (e.g., handling new JSON formats) won't affect networking code.

I.e.

DexcomClient
   |
   +-- uses --> IHttpClient (for network communication)
   |
   +-- uses --> IGlucoseReadingParser (for parsing readings)
                        |
                        +-- uses --> IJsonParser (for JSON parsing)


Successfully separated parsing logic from network logic by implementing a clean layered architecture:

Interface Layer:

IJsonValue/IJsonParser: Generic JSON operations
IGlucoseReadingParser: Domain-specific parsing
IHttpClient: Network operations
Implementation Layer:

ArduinoJsonParser: Implements IJsonParser using ArduinoJson
JsonGlucoseReadingParser: Implements IGlucoseReadingParser using IJsonParser
GlucoseReading: Domain model using IJsonValue
Benefits:

Clear separation of concerns
Improved testability through interfaces
Flexible architecture allowing different implementations
No direct ArduinoJson dependencies in business logic
The implementation follows the target architecture diagram exactly, with DexcomClient using IHttpClient for network operations and IGlucoseReadingParser for parsing, which in turn uses IJsonParser for low-level JSON operations.

Tests have been written... just need to verify
```