#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <random>

class Utils {
public:
    static std::string getCurrentTimestamp();
    static std::string generateRandomId(int length = 12);
    static bool isValidEmail(const std::string& email);
    static bool isValidAmount(double amount);
    static std::string sanitizeInput(const std::string& input);
};

#endif
