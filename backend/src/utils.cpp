#include "utils.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>
#include <algorithm>

std::string Utils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

std::string Utils::generateRandomId(int length) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, sizeof(charset) - 2);
    
    for (int i = 0; i < length; ++i) {
        result += charset[distribution(generator)];
    }
    
    return result;
}

bool Utils::isValidEmail(const std::string& email) {
    const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

bool Utils::isValidAmount(double amount) {
    return amount > 0 && amount <= 1000000; // Max transfer limit
}

std::string Utils::sanitizeInput(const std::string& input) {
    std::string sanitized = input;
    // Remove potentially dangerous characters
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(), 
                                   [](char c) { return c == '<' || c == '>' || c == '&' || c == '"' || c == '\''; }), 
                   sanitized.end());
    return sanitized;
}
