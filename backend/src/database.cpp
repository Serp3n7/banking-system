#include "database.h"
#include "utils.h"
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

Database::Database() : client{mongocxx::uri{"mongodb://localhost:27017"}}, db{client["banking_system"]} {}

bool Database::createUser(const User& user) {
    try {
        auto collection = db["users"];
        
        document doc{};
        doc << "username" << user.username
            << "email" << user.email
            << "password_hash" << user.password_hash
            << "created_at" << Utils::getCurrentTimestamp()
            << "accounts" << bsoncxx::builder::stream::open_array
            << bsoncxx::builder::stream::close_array;
            
        auto result = collection.insert_one(doc.view());
        return result.has_value();
    } catch (const std::exception& e) {
        std::cerr << "Error creating user: " << e.what() << std::endl;
        return false;
    }
}

User Database::getUserByUsername(const std::string& username) {
    User user;
    try {
        auto collection = db["users"];
        auto filter = document{} << "username" << username << finalize;
        auto result = collection.find_one(filter.view());
        
        if (result) {
            auto doc = result->view();
            user.id = doc["_id"].get_oid().value.to_string();
            user.username = doc["username"].get_utf8().value.to_string();
            user.email = doc["email"].get_utf8().value.to_string();
            user.password_hash = doc["password_hash"].get_utf8().value.to_string();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting user: " << e.what() << std::endl;
    }
    return user;
}

bool Database::createAccount(const Account& account) {
    try {
        auto collection = db["accounts"];
        
        document doc{};
        doc << "user_id" << account.user_id
            << "account_number" << account.account_number
            << "account_type" << account.account_type
            << "balance" << account.balance
            << "status" << account.status
            << "created_at" << Utils::getCurrentTimestamp();
            
        auto result = collection.insert_one(doc.view());
        return result.has_value();
    } catch (const std::exception& e) {
        std::cerr << "Error creating account: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Account> Database::getAccountsByUserId(const std::string& user_id) {
    std::vector<Account> accounts;
    try {
        auto collection = db["accounts"];
        auto filter = document{} << "user_id" << user_id << finalize;
        auto cursor = collection.find(filter.view());
        
        for (auto&& doc : cursor) {
            Account account;
            account.id = doc["_id"].get_oid().value.to_string();
            account.user_id = doc["user_id"].get_utf8().value.to_string();
            account.account_number = doc["account_number"].get_utf8().value.to_string();
            account.account_type = doc["account_type"].get_utf8().value.to_string();
            account.balance = doc["balance"].get_double().value;
            account.status = doc["status"].get_utf8().value.to_string();
            accounts.push_back(account);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting accounts: " << e.what() << std::endl;
    }
    return accounts;
}

Account Database::getAccountByNumber(const std::string& account_number) {
    Account account;
    try {
        auto collection = db["accounts"];
        auto filter = document{} << "account_number" << account_number << finalize;
        auto result = collection.find_one(filter.view());
        
        if (result) {
            auto doc = result->view();
            account.id = doc["_id"].get_oid().value.to_string();
            account.user_id = doc["user_id"].get_utf8().value.to_string();
            account.account_number = doc["account_number"].get_utf8().value.to_string();
            account.account_type = doc["account_type"].get_utf8().value.to_string();
            account.balance = doc["balance"].get_double().value;
            account.status = doc["status"].get_utf8().value.to_string();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting account by number: " << e.what() << std::endl;
    }
    return account;
}

Account Database::getAccountById(const std::string& account_id) {
    Account account;
    try {
        auto collection = db["accounts"];
        auto filter = document{} << "_id" << bsoncxx::oid{account_id} << finalize;
        auto result = collection.find_one(filter.view());
        
        if (result) {
            auto doc = result->view();
            account.id = doc["_id"].get_oid().value.to_string();
            account.user_id = doc["user_id"].get_utf8().value.to_string();
            account.account_number = doc["account_number"].get_utf8().value.to_string();
            account.account_type = doc["account_type"].get_utf8().value.to_string();
            account.balance = doc["balance"].get_double().value;
            account.status = doc["status"].get_utf8().value.to_string();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting account by ID: " << e.what() << std::endl;
    }
    return account;
}

bool Database::updateAccountBalance(const std::string& account_id, double new_balance) {
    try {
        auto collection = db["accounts"];
        auto filter = document{} << "_id" << bsoncxx::oid{account_id} << finalize;
        auto update = document{} << "$set" << open_document 
                                << "balance" << new_balance 
                                << close_document << finalize;
        
        auto result = collection.update_one(filter.view(), update.view());
        return result && result->modified_count() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error updating balance: " << e.what() << std::endl;
        return false;
    }
}

bool Database::createTransaction(const Transaction& transaction) {
    try {
        auto collection = db["transactions"];
        
        document doc{};
        doc << "from_account" << transaction.from_account
            << "to_account" << transaction.to_account
            << "amount" << transaction.amount
            << "transaction_type" << transaction.transaction_type
            << "description" << transaction.description
            << "timestamp" << transaction.timestamp
            << "status" << transaction.status;
            
        auto result = collection.insert_one(doc.view());
        return result.has_value();
    } catch (const std::exception& e) {
        std::cerr << "Error creating transaction: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Transaction> Database::getTransactionsByAccountId(const std::string& account_id) {
    std::vector<Transaction> transactions;
    try {
        auto collection = db["transactions"];
        auto filter = document{} << "$or" << bsoncxx::builder::stream::open_array
                                << open_document << "from_account" << account_id << close_document
                                << open_document << "to_account" << account_id << close_document
                                << bsoncxx::builder::stream::close_array << finalize;
        
        auto cursor = collection.find(filter.view());
        
        for (auto&& doc : cursor) {
            Transaction transaction;
            transaction.id = doc["_id"].get_oid().value.to_string();
            transaction.from_account = doc["from_account"].get_utf8().value.to_string();
            transaction.to_account = doc["to_account"].get_utf8().value.to_string();
            transaction.amount = doc["amount"].get_double().value;
            transaction.transaction_type = doc["transaction_type"].get_utf8().value.to_string();
            transaction.description = doc["description"].get_utf8().value.to_string();
            transaction.timestamp = doc["timestamp"].get_utf8().value.to_string();
            transaction.status = doc["status"].get_utf8().value.to_string();
            transactions.push_back(transaction);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting transactions: " << e.what() << std::endl;
    }
    return transactions;
}

std::string Database::generateAccountNumber() {
    return "ACC" + Utils::generateRandomId(10);
}
