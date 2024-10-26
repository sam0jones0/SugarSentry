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

// Optional: Add debug levels
enum class DebugLevel {
    NONE,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    VERBOSE
};

#ifdef DEBUG_LEVEL
constexpr DebugLevel CURRENT_DEBUG_LEVEL = DEBUG_LEVEL;
#else
constexpr DebugLevel CURRENT_DEBUG_LEVEL = DebugLevel::INFO;
#endif

#define DEBUG_ERROR(x) if(CURRENT_DEBUG_LEVEL >= DebugLevel::ERROR) { DEBUG_PRINT("[ERROR] " << x); }
#define DEBUG_WARN(x) if(CURRENT_DEBUG_LEVEL >= DebugLevel::WARN) { DEBUG_PRINT("[WARN] " << x); }
#define DEBUG_INFO(x) if(CURRENT_DEBUG_LEVEL >= DebugLevel::INFO) { DEBUG_PRINT("[INFO] " << x); }
#define DEBUG_DBG(x) if(CURRENT_DEBUG_LEVEL >= DebugLevel::DEBUG) { DEBUG_PRINT("[DEBUG] " << x); }
#define DEBUG_VERBOSE(x) if(CURRENT_DEBUG_LEVEL >= DebugLevel::VERBOSE) { DEBUG_PRINT("[VERBOSE] " << x); }

#define DEBUG_ERROR_F(format, ...) if(CURRENT_DEBUG_LEVEL >= DebugLevel::ERROR) { DEBUG_PRINTF("[ERROR] " format, __VA_ARGS__); }
#define DEBUG_WARN_F(format, ...) if(CURRENT_DEBUG_LEVEL >= DebugLevel::WARN) { DEBUG_PRINTF("[WARN] " format, __VA_ARGS__); }
#define DEBUG_INFO_F(format, ...) if(CURRENT_DEBUG_LEVEL >= DebugLevel::INFO) { DEBUG_PRINTF("[INFO] " format, __VA_ARGS__); }
#define DEBUG_DBG_F(format, ...) if(CURRENT_DEBUG_LEVEL >= DebugLevel::DEBUG) { DEBUG_PRINTF("[DEBUG] " format, __VA_ARGS__); }
#define DEBUG_VERBOSE_F(format, ...) if(CURRENT_DEBUG_LEVEL >= DebugLevel::VERBOSE) { DEBUG_PRINTF("[VERBOSE] " format, __VA_ARGS__); }

#endif // DEBUG_H