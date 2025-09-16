#!/bin/bash

# Banking System Setup Script (Fixed MongoDB Key Issue)

echo "🏦 Setting up Banking System..."

# Update system
echo "📦 Updating system packages..."
sudo apt update && sudo apt upgrade -y

# Install development tools
echo "🔧 Installing development tools..."
sudo apt install -y build-essential cmake pkg-config git curl wget gnupg

# Install MongoDB (Fixed GPG key method)
echo "🗄️ Installing MongoDB..."

# Remove old MongoDB key if exists
sudo apt-key del 7EACA395

# Import MongoDB GPG key to proper location
curl -fsSL https://www.mongodb.org/static/pgp/server-6.0.asc | sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/mongodb-6.gpg

# Add MongoDB repository
echo "deb [ arch=amd64,arm64 signed-by=/etc/apt/trusted.gpg.d/mongodb-6.gpg ] https://repo.mongodb.org/apt/ubuntu focal/mongodb-org/6.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-6.0.list

# Update package list
sudo apt update

# Install MongoDB
sudo apt install -y mongodb-org

# Install MongoDB C++ driver
echo "📚 Installing MongoDB C++ driver..."
sudo apt install -y libmongocxx-dev libbsoncxx-dev

# Install Crow framework dependencies
echo "🐦 Installing Crow framework..."
# First install Crow dependencies
sudo apt install -y libboost-system-dev libboost-filesystem-dev libboost-thread-dev libboost-regex-dev

# Install Crow from source if not available in repos
if ! dpkg -s libcrow-dev >/dev/null 2>&1; then
    echo "Installing Crow from source..."
    cd /tmp
    git clone https://github.com/CrowCpp/Crow.git
    cd Crow
    mkdir build
    cd build
    cmake .. -DCROW_BUILD_EXAMPLES=OFF
    make -j$(nproc)
    sudo make install
    sudo ldconfig
    cd ~
fi

# Install OpenSSL for encryption
echo "🔒 Installing OpenSSL..."
sudo apt install -y libssl-dev

# Start MongoDB service
echo "🚀 Starting MongoDB service..."
sudo systemctl start mongod
sudo systemctl enable mongod

# Verify MongoDB is running
echo "🔍 Verifying MongoDB installation..."
if sudo systemctl is-active --quiet mongod; then
    echo "✅ MongoDB is running successfully"
else
    echo "⚠️ MongoDB failed to start, trying to fix..."
    sudo systemctl status mongod
    sudo journalctl -u mongod --no-pager
fi

# Create project directory
echo "📁 Creating project structure..."
mkdir -p banking-system/{frontend/{css,js},backend/{src,include,build},database}
cd banking-system

# Initialize git repository
echo "📚 Initializing Git repository..."
git init
cat > .gitignore << 'EOF'
# Build directories
build/
*.o
*.so
*.a

# IDE files
.vscode/
.idea/
*.swp
*.swo

# OS files
.DS_Store
Thumbs.db

# Logs
*.log

# Temporary files
tmp/
temp/
EOF

# Create CMakeLists.txt with better error handling
cat > backend/CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(BankingSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Try to find mongocxx
find_package(mongocxx QUIET)
find_package(bsoncxx QUIET)

if(NOT mongocxx_FOUND)
    message(STATUS "mongocxx not found via CMake, trying pkg-config...")
    pkg_check_modules(MONGOCXX REQUIRED libmongocxx)
    pkg_check_modules(BSONCXX REQUIRED libbsoncxx)
endif()

# Find Crow
find_path(CROW_INCLUDE_DIRS crow.h PATH_SUFFIXES crow)
find_library(CROW_LIBRARIES NAMES crow)

if(NOT CROW_INCLUDE_DIRS OR NOT CROW_LIBRARIES)
    message(STATUS "Crow not found via find_library, trying pkg-config...")
    pkg_check_modules(CROW crow)
endif()

# Include directories
include_directories(include)
if(CROW_INCLUDE_DIRS)
    include_directories(${CROW_INCLUDE_DIRS})
endif()

# Create executable
add_executable(banking_server
    src/main.cpp
    src/database.cpp
    src/auth.cpp
    src/routes.cpp
    src/utils.cpp
)

# Link libraries
target_link_libraries(banking_server 
    ${CROW_LIBRARIES}
    crypto
    ssl
    pthread
)

if(mongocxx_FOUND)
    target_link_libraries(banking_server mongo::mongocxx_shared mongo::bsoncxx_shared)
else()
    target_link_libraries(banking_server ${MONGOCXX_LIBRARIES} ${BSONCXX_LIBRARIES})
    target_include_directories(banking_server PRIVATE ${MONGOCXX_INCLUDE_DIRS} ${BSONCXX_INCLUDE_DIRS})
endif()

# Compiler flags
target_compile_options(banking_server PRIVATE ${MONGOCXX_CFLAGS_OTHER} ${BSONCXX_CFLAGS_OTHER})
EOF

# Create README
cat > README.md << 'EOF'
# Advanced Banking System

A full-stack banking application with C++ backend, MongoDB database, and HTML/CSS/JS frontend.

## Features

- User authentication and registration
- Account management (multiple account types)
- Money transfers between accounts
- Transaction history and reporting
- Responsive web interface
- Bank-level security

## Technology Stack

- **Frontend**: HTML5, CSS3, JavaScript (ES6+)
- **Backend**: C++ with Crow framework
- **Database**: MongoDB with C++ driver
- **Security**: OpenSSL for encryption, JWT tokens

## Setup Instructions

1. Run the setup script: `bash setup-fixed.sh`
2. Build the backend: `cd backend && mkdir -p build && cd build && cmake .. && make`
3. Initialize the database: `mongo < ../../database/init.js`
4. Start the server: `./banking_server`
5. Open frontend: `cd ../../../frontend && python3 -m http.server 3000`
6. Access the application at: http://localhost:3000

## Troubleshooting

### MongoDB Issues
- Check if MongoDB is running: `sudo systemctl status mongod`
- View MongoDB logs: `sudo journalctl -u mongod`
- Restart MongoDB: `sudo systemctl restart mongod`

### Build Issues
- Make sure all dependencies are installed
- Try cleaning build directory: `rm -rf build && mkdir build && cd build`
- Check CMake output for missing dependencies

### Crow Framework Issues
If Crow is not found, install manually:
```bash
cd /tmp
git clone https://github.com/CrowCpp/Crow.git
cd Crow
mkdir build && cd build
cmake .. -DCROW_BUILD_EXAMPLES=OFF
make -j$(nproc)
sudo make install
sudo ldconfig
```

## API Endpoints

- `POST /api/login` - User login
- `POST /api/register` - User registration
- `GET /api/accounts` - Get user accounts
- `POST /api/accounts` - Create new account
- `GET /api/balance/:id` - Get account balance
- `POST /api/transfer` - Transfer money
- `GET /api/transactions/:id` - Get transaction history

## Security Features

- Password hashing with SHA-256
- JWT token-based authentication
- Input validation and sanitization
- CORS protection
- SQL injection prevention

## License

MIT License - see LICENSE file for details
EOF

echo "✅ Banking System setup completed!"
echo ""
echo "🔧 Build Instructions:"
echo "   1. cd backend"
echo "   2. mkdir -p build && cd build"
echo "   3. cmake .."
echo "   4. make"
echo ""
echo "🗄️ Database Setup:"
echo "   1. mongo < ../../database/init.js"
echo ""
echo "🚀 Run Application:"
echo "   1. ./banking_server (from backend/build/)"
echo "   2. cd ../../../frontend"
echo "   3. python3 -m http.server 3000"
echo "   4. Open http://localhost:3000"
echo ""
echo "🎉 Happy coding!"

# Test MongoDB connection
echo ""
echo "🧪 Testing MongoDB connection..."
if mongo --eval "db.adminCommand('ismaster')" >/dev/null 2>&1; then
    echo "✅ MongoDB connection test passed"
else
    echo "⚠️ MongoDB connection test failed"
    echo "   Try: sudo systemctl restart mongod"
fi
