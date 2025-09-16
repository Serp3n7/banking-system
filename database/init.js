// MongoDB initialization script
// Run this script to set up the database collections and indexes

use banking_system;

// Create users collection
db.createCollection("users");
db.users.createIndex({ "username": 1 }, { unique: true });
db.users.createIndex({ "email": 1 }, { unique: true });

// Create accounts collection
db.createCollection("accounts");
db.accounts.createIndex({ "user_id": 1 });
db.accounts.createIndex({ "account_number": 1 }, { unique: true });

// Create transactions collection
db.createCollection("transactions");
db.transactions.createIndex({ "from_account": 1 });
db.transactions.createIndex({ "to_account": 1 });
db.transactions.createIndex({ "timestamp": -1 });

// Insert sample data for testing
db.users.insertOne({
    username: "testuser",
    email: "test@example.com",
    password_hash: "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
    created_at: new Date(),
    accounts: []
});

print("Database initialized successfully!");
