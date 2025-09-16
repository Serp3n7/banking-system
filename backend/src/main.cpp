#include <crow.h>
#include <iostream>
#include <mongocxx/instance.hpp>
#include "database.h"
#include "routes.h"

int main() {
    // Initialize MongoDB driver
    mongocxx::instance inst{};
    
    // Create database connection
    Database database;
    
    // Create routes handler
    Routes routes(&database);
    
    // Create Crow app
    crow::SimpleApp app;
    
    // Enable CORS
    app.get_middleware<crow::CORSHandler>().global()
        .headers("Content-Type", "Authorization")
        .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method)
        .origin("*");
    
    // Setup routes
    routes.setupRoutes(app);
    
    // Serve static files
    app.route_dynamic("/")
    ([](const crow::request& req) {
        return crow::load_text("../frontend/index.html");
    });
    
    std::cout << "Banking System Server starting on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();
    
    return 0;
}
