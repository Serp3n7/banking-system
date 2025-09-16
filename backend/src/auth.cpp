#include "auth.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
#include <map>
#include <chrono>

std::string Auth::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool Auth::verifyPassword(const std::string& password, const std::string& hash) {
    return hashPassword(password) == hash;
}

// Simple token storage (in production, use Redis or database)
static std::map<std::string, std::pair<std::string, std::chrono::time_point<std::chrono::system_clock>>> token_store;

std::string Auth::generateToken(const std::string& user_id) {
    std::string token;
    unsigned char buffer[32];
    RAND_bytes(buffer, sizeof(buffer));
    
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i];
    }
    token = ss.str();
    
    auto expiry = std::chrono::system_clock::now() + std::chrono::hours(24);
    token_store[token] = {user_id, expiry};
    
    return token;
}

bool Auth::verifyToken(const std::string& token, std::string& user_id) {
    auto it = token_store.find(token);
    if (it != token_store.end()) {
        if (std::chrono::system_clock::now() < it->second.second) {
            user_id = it->second.first;
            return true;
        } else {
            token_store.erase(it);
        }
    }
    return false;
}

std::string Auth::generateJWT(const std::string& user_id) {
    // Simplified JWT implementation
    return generateToken(user_id);
}

bool Auth::verifyJWT(const std::string& token, std::string& user_id) {
    return verifyToken(token, user_id);
}
