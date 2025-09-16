#ifndef ROUTES_H
#define ROUTES_H

#include <crow.h>
#include "database.h"

class Routes {
private:
    Database* db;
    
public:
    Routes(Database* database);
    void setupRoutes(crow::SimpleApp& app);
    
    // Route handlers
    crow::response handleLogin(const crow::request& req);
    crow::response handleRegister(const crow::request& req);
    crow::response handleGetBalance(const std::string& account_id, const crow::request& req);
    crow::response handleTransfer(const crow::request& req);
    crow::response handleGetTransactions(const std::string& account_id, const crow::request& req);
    crow::response handleCreateAccount(const crow::request& req);
    crow::response handleGetAccounts(const crow::request& req);
};

#endif
