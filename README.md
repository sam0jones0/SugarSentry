# SugarSentry

ESP32-based ambient blood glucose visualisation device with e-ink display. Inspired by gagebenne/pydexcom and lmarzen/esp32-weather-epd.

> ❗ **Project Status: Development in Progress**
>
> *   ✅ Core Dexcom API communication (authentication, fetching readings).
> *   ❌ E-ink display integration.
> *   ❌ Sensor (Temp/Humidity/Proximity) integration.
> *   ❌ Graphing functionality.
> *   ❌ LED feedback logic.
> *   ❌ Power optimization implementation.

## Description

SugarSentry aims to be a stylish, bedside/desktop device that displays blood sugar levels from the Dexcom API, shows historical data in a graph, and also monitors room temperature and humidity. The device uses an e-ink display for low power consumption and extended battery life. For nighttime use, it features a proximity-activated RGB LED that dimly indicates the current blood sugar level.

## Key Features

*(Note: Most features are planned and not yet implemented; see Project Status above for details)*

- 🔄 Displays real-time blood glucose levels from Dexcom API
- 🔄 Shows historical glucose data in graph form
- 🔄 Monitors room temperature and humidity (BME280 sensor)
- 🔄 Detects proximity (VCNL4040 sensor)
- 🔄 Highly power-efficient with e-ink display
- 🔄 Battery-powered with recharging circuits for flexibility
- 🔄 Proximity-activated RGB LED for nighttime glucose level indication

## Hardware

- Adafruit HUZZAH32 – ESP32 Feather Board
- 7.5-inch (800x480) E-Paper Display
- BME280 temperature/humidity sensor
- VCNL4040 proximity and ambient light sensor
- RGB LED
- LiPo battery

## Development

- Written in C++
- Compatible with Arduino IDE and PlatformIO
- Core networking and API logic established
- Focus on modularity, testability, power efficiency, and memory optimisation

## License

This project is licensed under the [MIT license](LICENSE).

## Acknowledgements

This project was inspired by:
- [gagebenne/pydexcom](https://github.com/gagebenne/pydexcom)
- [lmarzen/esp32-weather-epd](https://github.com/lmarzen/esp32-weather-epd)
