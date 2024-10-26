#ifndef DEBUG_H
#define DEBUG_H

#ifdef ARDUINO
#include <Arduino.h>
#define DEBUG_PRINT(x) Serial.println(x)
#define DEBUG_PRINTF(format, ...) Serial.printf(format, __VA_ARGS__)
#define PLATFORM_DELAY(ms) delay(ms)
#else
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdio>
#define DEBUG_PRINT(x) std::cout << x << std::endl
#define DEBUG_PRINTF(format, ...) printf(format, __VA_ARGS__)
#define PLATFORM_DELAY(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#endif

#endif // DEBUG_H