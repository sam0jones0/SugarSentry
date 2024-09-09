# SugarSentry

> ❗ **Project Status: In Progress**
> 
> This project is currently under active development and is not yet ready for production use.

ESP32-based portable blood glucose monitor with e-ink display. Inspired by gagebenne/pydexcom and lmarzen/esp32-weather-epd.

## Description

SugarSentry is a portable device that displays blood sugar levels from the Dexcom API, shows historical data in a graph, and also monitors room temperature and humidity (because why not). The device uses an e-ink display for low power consumption and long battery life. For nighttime use, it features a proximity-activated RGB LED that dimly indicates the current blood sugar level.

## Key Features

- 🔄 Displays real-time blood glucose levels from Dexcom API
- 🔄 Shows historical glucose data in graph form
- 🔄 Monitors room temperature and humidity (BME280 sensor)
- 🔄 Detects proximity (VCNL4040 sensor)
- 🔄 Low power consumption with e-ink display
- 🔄 Portable design with LiPo battery
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
- Developed with focus on low power consumption and memory optimization

## License

This project is licensed under the [MIT license](LICENSE).

## Acknowledgements

This project was inspired by:
- [gagebenne/pydexcom](https://github.com/gagebenne/pydexcom)
- [lmarzen/esp32-weather-epd](https://github.com/lmarzen/esp32-weather-epd)