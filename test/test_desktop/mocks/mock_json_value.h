#pragma once

#include "i_json_value.h"
#include <memory>
#include <optional>
#include <string>

class MockJsonValue : public IJsonValue {
public:
    std::optional<std::string> getString(const std::string& key) const override {
        if (key == "Trend") return "Flat";
        if (key == "WT") return "Date(1609459200000)";
        return std::nullopt;
    }
    
    std::optional<int> getInt(const std::string& key) const override {
        if (key == "Value") return 120;
        return std::nullopt;
    }
    
    std::optional<double> getDouble(const std::string& key) const override {
        return std::nullopt;
    }
    
    std::optional<bool> getBool(const std::string& key) const override {
        return std::nullopt;
    }
};
