#include "routes.h"
#include "auth.h"
#include "utils.h"
#include <crow/json.h>
#include <iostream>

Routes::Routes(Database* database) : db(database) {}

void Routes::setupRoutes(crow::SimpleApp& app) {
    // Authentication routes
    CROW_ROUTE(app, "/api/login").methods("POST"_method)
    ([this](const crow::request& req) {
        return this->handleLogin(req);
    });
    
    CROW_ROUTE(app, "/api/register").methods("POST"_method)
    ([this](const crow::request& req) {
        return this->handleRegister(req);
    });
    
    // Account routes
    CROW_ROUTE(app, "/api/accounts").methods("GET"_method)
    ([this](const crow::request& req) {
        return this->handleGetAccounts(req);
    });
    
    CROW_ROUTE(app, "/api/accounts").methods("POST"_method)
    ([this](const crow::request& req) {
        return this->handleCreateAccount(req);
    });
    
    CROW_ROUTE(app, "/api/balance/<string>")
    ([this](const std::string& account_id, const crow::request& req) {
        return this->handleGetBalance(account_id, req);
    });
    
    // Transaction routes
    CROW_ROUTE(app, "/api/transfer").methods("POST"_method)
    ([this](const crow::request& req) {
        return this->handleTransfer(req);
    });
    
    CROW_ROUTE(app, "/api/transactions/<string>")
    ([this](const std::string& account_id, const crow::request& req) {
        return this->handleGetTransactions(account_id, req);
    });
}

crow::response Routes::handleLogin(const crow::request& req) {
    try {
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            return crow::response(400, "Invalid JSON");
        }
        
        std::string username = json_data["username"].s();
        std::string password = json_data["password"].s();
        
        User user = db->getUserByUsername(username);
        if (user.username.empty() || !Auth::verifyPassword(password, user.password_hash)) {
            return crow::response(401, "Invalid credentials");
        }
        
        std::string token = Auth::generateToken(user.id);
        
        crow::json::wvalue response_json;
        response_json["success"] = true;
        response_json["token"] = token;
        response_json["user_id"] = user.id;
        response_json["username"] = user.username;
        
        return crow::response(200, response_json);
    } catch (const std::exception& e) {
        std::cerr << "Login error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response Routes::handleRegister(const crow::request& req) {
    try {
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            return crow::response(400, "Invalid JSON");
        }
        
        std::string username = json_data["username"].s();
        std::string email = json_data["email"].s();
        std::string password = json_data["password"].s();
        
        // Validation
        if (username.empty() || email.empty() || password.empty()) {
            return crow::response(400, "Missing required fields");
        }
        
        if (!Utils::isValidEmail(email)) {
            return crow::response(400, "Invalid email format");
        }
        
        // Check if user exists
        User existing_user = db->getUserByUsername(username);
        if (!existing_user.username.empty()) {
            return crow::response(409, "Username already exists");
        }
        
        // Create user
        User new_user;
        new_user.username = username;
        new_user.email = email;
        new_user.password_hash = Auth::hashPassword(password);
        
        if (db->createUser(new_user)) {
            crow::json::wvalue response_json;
            response_json["success"] = true;
            response_json["message"] = "User created successfully";
            return crow::response(201, response_json);
        } else {
            return crow::response(500, "Failed to create user");
        }
    } catch (const std::exception& e) {
        std::cerr << "Register error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response Routes::handleGetAccounts(const crow::request& req) {
    try {
        // Verify token
        std::string token = req.get_header_value("Authorization");
        if (token.empty()) {
            return crow::response(401, "Missing authorization token");
        }
        
        std::string user_id;
        if (!Auth::verifyToken(token, user_id)) {
            return crow::response(401, "Invalid token");
        }
        
        std::vector<Account> accounts = db->getAccountsByUserId(user_id);
        
        crow::json::wvalue response_json = crow::json::wvalue::list();
        for (size_t i = 0; i < accounts.size(); ++i) {
            response_json[i]["id"] = accounts[i].id;
            response_json[i]["account_number"] = accounts[i].account_number;
            response_json[i]["account_type"] = accounts[i].account_type;
            response_json[i]["balance"] = accounts[i].balance;
            response_json[i]["status"] = accounts[i].status;
        }
        
        return crow::response(200, response_json);
    } catch (const std::exception& e) {
        std::cerr << "Get accounts error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response Routes::handleCreateAccount(const crow::request& req) {
    try {
        // Verify token
        std::string token = req.get_header_value("Authorization");
        if (token.empty()) {
            return crow::response(401, "Missing authorization token");
        }
        
        std::string user_id;
        if (!Auth::verifyToken(token, user_id)) {
            return crow::response(401, "Invalid token");
        }
        
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            return crow::response(400, "Invalid JSON");
        }
        
        std::string account_type = json_data["account_type"].s();
        double initial_deposit = json_data["initial_deposit"].d();
        
        if (!Utils::isValidAmount(initial_deposit)) {
            return crow::response(400, "Invalid initial deposit amount");
        }
        
        Account new_account;
        new_account.user_id = user_id;
        new_account.account_number = db->generateAccountNumber();
        new_account.account_type = account_type;
        new_account.balance = initial_deposit;
        new_account.status = "active";
        
        if (db->createAccount(new_account)) {
            crow::json::wvalue response_json;
            response_json["success"] = true;
            response_json["message"] = "Account created successfully";
            response_json["account_number"] = new_account.account_number;
            return crow::response(201, response_json);
        } else {
            return crow::response(500, "Failed to create account");
        }
    } catch (const std::exception& e) {
        std::cerr << "Create account error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response Routes::handleTransfer(const crow::request& req) {
    try {
        // Verify token
        std::string token = req.get_header_value("Authorization");
        if (token.empty()) {
            return crow::response(401, "Missing authorization token");
        }
        
        std::string user_id;
        if (!Auth::verifyToken(token, user_id)) {
            return crow::response(401, "Invalid token");
        }
        
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            return crow::response(400, "Invalid JSON");
        }
        
        std::string from_account = json_data["from_account"].s();
        std::string to_account_number = json_data["to_account_number"].s();
        double amount = json_data["amount"].d();
        std::string description = json_data["description"].s();
        
        if (!Utils::isValidAmount(amount)) {
            return crow::response(400, "Invalid amount");
        }
        
        // Get accounts
        Account from_acc = db->getAccountById(from_account);
        Account to_acc = db->getAccountByNumber(to_account_number);
        
        if (from_acc.id.empty() || to_acc.id.empty()) {
            return crow::response(404, "Account not found");
        }
        
        if (from_acc.balance < amount) {
            return crow::response(400, "Insufficient balance");
        }
        
        // Perform transfer
        bool success = db->updateAccountBalance(from_acc.id, from_acc.balance - amount) &&
                      db->updateAccountBalance(to_acc.id, to_acc.balance + amount);
        
        if (success) {
            // Create transaction record
            Transaction transaction;
            transaction.from_account = from_account;
            transaction.to_account = to_acc.id;
            transaction.amount = amount;
            transaction.transaction_type = "transfer";
            transaction.description = description;
            transaction.timestamp = Utils::getCurrentTimestamp();
            transaction.status = "completed";
            
            db->createTransaction(transaction);
            
            crow::json::wvalue response_json;
            response_json["success"] = true;
            response_json["message"] = "Transfer completed successfully";
            return crow::response(200, response_json);
        } else {
            return crow::response(500, "Transfer failed");
        }
    } catch (const std::exception& e) {
        std::cerr << "Transfer error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response Routes::handleGetBalance(const std::string& account_id, const crow::request& req) {
    try {
        // Verify token
        std::string token = req.get_header_value("Authorization");
        if (token.empty()) {
            return crow::response(401, "Missing authorization token");
        }
        
        std::string user_id;
        if (!Auth::verifyToken(token, user_id)) {
            return crow::response(401, "Invalid token");
        }
        
        Account account = db->getAccountById(account_id);
        if (account.id.empty()) {
            return crow::response(404, "Account not found");
        }
        
        crow::json::wvalue response_json;
        response_json["account_id"] = account.id;
        response_json["balance"] = account.balance;
        response_json["account_number"] = account.account_number;
        
        return crow::response(200, response_json);
    } catch (const std::exception& e) {
        std::cerr << "Get balance error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}

crow::response Routes::handleGetTransactions(const std::string& account_id, const crow::request& req) {
    try {
        // Verify token
        std::string token = req.get_header_value("Authorization");
        if (token.empty()) {
            return crow::response(401, "Missing authorization token");
        }
        
        std::string user_id;
        if (!Auth::verifyToken(token, user_id)) {
            return crow::response(401, "Invalid token");
        }
        
        std::vector<Transaction> transactions = db->getTransactionsByAccountId(account_id);
        
        crow::json::wvalue response_json = crow::json::wvalue::list();
        for (size_t i = 0; i < transactions.size(); ++i) {
            response_json[i]["id"] = transactions[i].id;
            response_json[i]["from_account"] = transactions[i].from_account;
            response_json[i]["to_account"] = transactions[i].to_account;
            response_json[i]["amount"] = transactions[i].amount;
            response_json[i]["transaction_type"] = transactions[i].transaction_type;
            response_json[i]["description"] = transactions[i].description;
            response_json[i]["timestamp"] = transactions[i].timestamp;
            response_json[i]["status"] = transactions[i].status;
        }
        
        return crow::response(200, response_json);
    } catch (const std::exception& e) {
        std::cerr << "Get transactions error: " << e.what() << std::endl;
        return crow::response(500, "Internal server error");
    }
}
