#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <map>

class Auth {
public:
    static std::string hashPassword(const std::string& password);
    static bool verifyPassword(const std::string& password, const std::string& hash);
    static std::string generateToken(const std::string& user_id);
    static bool verifyToken(const std::string& token, std::string& user_id);
    static std::string generateJWT(const std::string& user_id);
    static bool verifyJWT(const std::string& token, std::string& user_id);
};

#endif
