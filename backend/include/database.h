#ifndef DATABASE_H
#define DATABASE_H

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <string>
#include <vector>

struct User {
    std::string id;
    std::string username;
    std::string email;
    std::string password_hash;
    std::vector<std::string> accounts;
};

struct Account {
    std::string id;
    std::string user_id;
    std::string account_number;
    std::string account_type;
    double balance;
    std::string status;
};

struct Transaction {
    std::string id;
    std::string from_account;
    std::string to_account;
    double amount;
    std::string transaction_type;
    std::string description;
    std::string timestamp;
    std::string status;
};

class Database {
private:
    mongocxx::client client;
    mongocxx::database db;
    
public:
    Database();
    
    // User operations
    bool createUser(const User& user);
    User getUserByUsername(const std::string& username);
    User getUserById(const std::string& id);
    
    // Account operations
    bool createAccount(const Account& account);
    std::vector<Account> getAccountsByUserId(const std::string& user_id);
    Account getAccountById(const std::string& account_id);
    Account getAccountByNumber(const std::string& account_number);
    bool updateAccountBalance(const std::string& account_id, double new_balance);
    
    // Transaction operations
    bool createTransaction(const Transaction& transaction);
    std::vector<Transaction> getTransactionsByAccountId(const std::string& account_id);
    
    // Utility
    std::string generateAccountNumber();
};

#endif
