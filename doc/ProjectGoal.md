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
board = featheresp32
framework = arduino
monitor_speed = 115200
upload_speed = 921600
lib_deps = 
    bblanchon/ArduinoJson @ ^6.18.5
test_ignore = test_desktop

[env:native]
platform = native
build_flags = -std=c++11
test_ignore = test_embedded
build_src_filter = +<*> -<.git/> -<.svn/>
```

## Project Structure and Organization

```
project_root/
├── src/
│   ├── main.cpp
├── lib/
│   └── README
├── include/
│   └── project_config.h
├── test/
│   ├── test_desktop/
│   └── test_embedded/
├── platformio.ini
├── README.md
└── .gitignore
```

----

Question:

