# ESP32 Development with VSCode: Getting Started Guide

## Table of Contents
1. [VSCode Setup](#vscode-setup)
2. [Project Structure](#project-structure)
3. [Coding Practices for Testability](#coding-practices-for-testability)
4. [Testing Strategies](#testing-strategies)
5. [QEMU Setup and Usage](#qemu-setup-and-usage)
6. [Workflow and Best Practices](#workflow-and-best-practices)

## VSCode Setup

### Installing VSCode and PlatformIO
1. Download and install VSCode from [https://code.visualstudio.com/](https://code.visualstudio.com/)
2. Open VSCode and navigate to the Extensions view (Ctrl+Shift+X)
3. Search for "PlatformIO IDE" and install it
4. Restart VSCode after installation

### Creating a New ESP32 Project
1. Click on the PlatformIO icon in the left sidebar
2. Choose "New Project" from the PlatformIO Home
3. Select "Espressif ESP32 Dev Module" (or your specific board) as the board
4. Choose "Arduino" as the framework
5. Select a location for your project

### Configuring Your Project
Edit `platformio.ini` in your project root:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps = 
    ; Add your library dependencies here
test_ignore = test_desktop
```

## Project Structure

```
project_root/
├── src/
│   ├── main.cpp
│   ├── wifi_manager.cpp
│   ├── wifi_manager.h
│   ├── dexcom_api.cpp
│   ├── dexcom_api.h
│   ├── display_manager.cpp
│   ├── display_manager.h
│   ├── sensor_manager.cpp
│   └── sensor_manager.h
├── lib/
│   └── README
├── include/
│   └── project_config.h
├── test/
│   ├── test_desktop/
│   │   ├── test_dexcom_api.cpp
│   │   └── test_data_processing.cpp
│   └── test_embedded/
│       ├── test_wifi_manager.cpp
│       └── test_sensor_manager.cpp
├── platformio.ini
├── README.md
└── .gitignore
```

### Key Components:
- `src/`: Main source files
- `lib/`: Project-specific libraries
- `include/`: Project-wide header files
- `test/`: Test files (separated into desktop and embedded tests)
- `platformio.ini`: PlatformIO configuration
- `README.md`: Project documentation
- `.gitignore`: Git ignore file

## Coding Practices for Testability

### 1. Use Interfaces and Dependency Injection
Create interfaces for hardware-dependent components:

```cpp
// In sensor_interface.h
class ISensor {
public:
    virtual float readTemperature() = 0;
    virtual ~ISensor() = default;
};

// In real_sensor.h
class RealSensor : public ISensor {
public:
    float readTemperature() override {
        // Actual hardware interaction
    }
};

// In mock_sensor.h
class MockSensor : public ISensor {
public:
    float readTemperature() override {
        return 25.0; // Mock value for testing
    }
};
```

Use dependency injection in your classes:

```cpp
// In temperature_monitor.h
class TemperatureMonitor {
private:
    ISensor& sensor;
public:
    TemperatureMonitor(ISensor& s) : sensor(s) {}
    bool isOverheating() {
        return sensor.readTemperature() > 30.0;
    }
};
```

### 2. Separate Hardware-Specific Code
Keep hardware-specific code in separate files and use conditional compilation when necessary:

```cpp
#ifdef USE_HARDWARE
#include "real_sensor.h"
#else
#include "mock_sensor.h"
#endif

ISensor* createSensor() {
#ifdef USE_HARDWARE
    return new RealSensor();
#else
    return new MockSensor();
#endif
}
```

### 3. Use Configuration Files
Keep configurable parameters in a separate header file:

```cpp
// In project_config.h
#define TEMPERATURE_THRESHOLD 30.0
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"
```

## Testing Strategies

### Unit Testing with Catch2
Use Catch2 for unit testing. Add it to your `platformio.ini`:

```ini
[env:native]
platform = native
lib_deps = catchorg/Catch2@^2.13.6
build_flags = -std=c++11
```

Example test file (`test/test_desktop/test_temperature_monitor.cpp`):

```cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "temperature_monitor.h"
#include "mock_sensor.h"

TEST_CASE("Temperature monitoring") {
    MockSensor mockSensor;
    TemperatureMonitor monitor(mockSensor);
    REQUIRE(monitor.isOverheating() == false);
}
```

### Integration Testing with QEMU
For integration tests that require simulated hardware, use QEMU. These tests will be in the `test/test_embedded/` directory.

Example (`test/test_embedded/test_wifi_manager.cpp`):

```cpp
#include <unity.h>
#include "wifi_manager.h"

void test_wifi_connection() {
    WifiManager wifiManager;
    TEST_ASSERT_TRUE(wifiManager.connect());
    TEST_ASSERT_TRUE(wifiManager.isConnected());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_connection);
    UNITY_END();
}

void loop() {}
```

## QEMU Setup and Usage

### Installing QEMU for ESP32
1. Clone the Espressif QEMU repository:
   ```
   git clone https://github.com/espressif/qemu.git
   ```
2. Follow the build instructions in the repository's README

### Running Tests with QEMU
Add a custom test environment in `platformio.ini`:

```ini
[env:esp32-qemu]
platform = espressif32
board = esp32dev
framework = arduino
test_transport = custom
test_ignore = test_desktop
debug_tool = qemu
debug_init_break =
```

Run QEMU tests:
```
pio test -e esp32-qemu
```

### Debugging with QEMU
1. Set breakpoints in your code
2. Start debugging using the VSCode debug panel
3. Select the "PlatformIO Debug (esp32-qemu)" configuration

## Workflow and Best Practices

1. **Development Cycle**:
   - Write code in `src/` and `include/`
   - Create unit tests in `test/test_desktop/`
   - Run desktop tests frequently: `pio test -e native`
   - Create integration tests in `test/test_embedded/`
   - Run QEMU tests: `pio test -e esp32-qemu`
   - Test on actual hardware

2. **Version Control**:
   - Use Git for version control
   - Commit frequently with meaningful messages
   - Use branches for features and bug fixes

3. **Code Style**:
   - Use consistent naming conventions (e.g., camelCase for functions, PascalCase for classes)
   - Apply consistent indentation (use VSCode's auto-formatting)

4. **Documentation**:
   - Keep `README.md` updated with project overview and setup instructions
   - Use inline comments for complex logic
   - Write function and class documentation in header files

5. **Continuous Integration**:
   - Set up a CI pipeline (e.g., GitHub Actions) to run tests automatically

6. **Dependency Management**:
   - Manage libraries through PlatformIO's Library Manager or `lib_deps` in `platformio.ini`

7. **Regular Testing**:
   - Run unit tests before committing changes
   - Perform integration tests with QEMU regularly
   - Test on actual hardware before major releases

By following these practices and utilizing the tools mentioned, you'll have a robust development environment for your ESP32 project, enabling efficient coding, thorough testing, and easier maintenance.

## GitHub Actions Setup

GitHub Actions provides an excellent, integrated CI/CD solution for your ESP32 project. Here's how to set it up and use it effectively:

### Basic Workflow Setup

Create a file named `.github/workflows/esp32_ci.yml` in your repository with the following content:

```yaml
name: ESP32 CI

on:
  pull_request:
    branches: [ main ]
  push:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * 0'  # Runs at 2 AM every Sunday

jobs:
  desktop-tests:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.x'
    - uses: actions/cache@v2
      with:
        path: ~/.platformio
        key: ${{ runner.os }}-pio
    - uses: actions/cache@v2
      with:
        path: ~/.cache/pip
        key: ${{ runner.os }}-pip-${{ hashFiles('**/requirements.txt') }}
        restore-keys: |
          ${{ runner.os }}-pip-
    - name: Install PlatformIO
      run: |
        pip install platformio
    - name: Run desktop tests
      run: |
        pio test -e native
    - name: Run cppcheck
      run: |
        sudo apt-get install -y cppcheck
        cppcheck --enable=all --error-exitcode=1 src/

  embedded-tests:
    runs-on: ubuntu-latest
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    needs: desktop-tests
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.x'
    - uses: actions/cache@v2
      with:
        path: ~/.platformio
        key: ${{ runner.os }}-pio
    - uses: actions/cache@v2
      with:
        path: ~/.cache/pip
        key: ${{ runner.os }}-pip-${{ hashFiles('**/requirements.txt') }}
        restore-keys: |
          ${{ runner.os }}-pip-
    - name: Install PlatformIO
      run: |
        pip install platformio
    - name: Install QEMU
      run: |
        sudo apt-get update
        sudo apt-get install -y qemu-system-xtensa
    - name: Run embedded tests
      run: |
        pio test -e esp32-qemu

  build:
    runs-on: ubuntu-latest
    needs: [desktop-tests, embedded-tests]
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.x'
    - uses: actions/cache@v2
      with:
        path: ~/.platformio
        key: ${{ runner.os }}-pio
    - uses: actions/cache@v2
      with:
        path: ~/.cache/pip
        key: ${{ runner.os }}-pip-${{ hashFiles('**/requirements.txt') }}
        restore-keys: |
          ${{ runner.os }}-pip-
    - name: Install PlatformIO
      run: |
        pip install platformio
    - name: Build project
      run: |
        pio run -e esp32dev
    - uses: actions/upload-artifact@v2
      with:
        name: firmware
        path: .pio/build/esp32dev/firmware.bin
```

### Workflow Logic

The CI pipeline is set up to do the following:

1. For Pull Requests:
   - Run desktop tests and static analysis on every push to a PR targeting the main branch.

2. For Pushes to Main:
   - Run desktop tests and static analysis first.
   - If desktop tests pass, run embedded tests using QEMU.
   - If both desktop and embedded tests pass, proceed to build the project.

3. Scheduled Runs:
   - The entire workflow runs at 2 AM every Sunday to catch any issues that might arise from updated dependencies.

This ensures that:
- Quick feedback is provided on PRs with desktop tests and static analysis.
- Main branch integrity is maintained with comprehensive testing.
- Builds are only created when all tests pass, and only for pushes to main.
- Regular checks are performed to catch potential issues from dependency updates.

### Workflow Optimizations

1. Caching:
   The workflow uses caching for PlatformIO and pip dependencies to speed up subsequent runs:

   ```yaml
   - uses: actions/cache@v2
     with:
       path: ~/.platformio
       key: ${{ runner.os }}-pio
   - uses: actions/cache@v2
     with:
       path: ~/.cache/pip
       key: ${{ runner.os }}-pip-${{ hashFiles('**/requirements.txt') }}
       restore-keys: |
         ${{ runner.os }}-pip-
   ```

2. Artifacts:
   Build outputs are saved for easy access:

   ```yaml
   - uses: actions/upload-artifact@v2
     with:
       name: firmware
       path: .pio/build/esp32dev/firmware.bin
   ```

### Additional CI/CD Best Practices

1. Code Quality Checks:
   Static analysis is performed using cppcheck:

   ```yaml
   - name: Run cppcheck
     run: |
       sudo apt-get install -y cppcheck
       cppcheck --enable=all --error-exitcode=1 src/
   ```

2. Scheduled Runs:
   Periodic checks are added to catch issues from updated dependencies:

   ```yaml
   on:
     schedule:
       - cron: '0 2 * * 0'  # Runs at 2 AM every Sunday
   ```

3. Branch Protection:
   - Set up branch protection rules in GitHub repository settings.
   - Require status checks to pass before merging to main.

4. Notifications:
   - Configure GitHub notifications for failed workflows.
   - Consider integrating with Slack or other team communication tools.

5. Environment Variables:
   - Use GitHub secrets for sensitive information.
   - Access in workflows using `${{ secrets.SECRET_NAME }}`.

6. Documentation:
   - Keep `README.md` updated with badge showing CI status.
   - Document the CI/CD process for team members.

7. Versioning:
   - Implement semantic versioning.
   - Use GitHub Actions to automate version bumps and tagging.

By implementing these practices, you'll create a robust, efficient CI/CD pipeline for your ESP32 project, ensuring code quality and streamlining the development process.