# 🏦 Advanced Banking System

A complete full-stack banking web application built with modern technologies for secure financial transactions and account management.

## 🚀 Features

### Core Banking Features
- **User Authentication & Registration** - Secure login system with password hashing
- **Multi-Account Management** - Support for Savings and Checking accounts
- **Money Transfers** - Secure transfer between accounts with real-time validation
- **Transaction History** - Complete transaction tracking with filtering options
- **Balance Management** - Real-time balance updates and checking
- **Account Creation** - Dynamic account creation with initial deposit

### Security Features
- **Password Hashing** - SHA-256 encryption for password security
- **Token-Based Authentication** - JWT-like tokens for session management
- **Input Validation** - Server-side and client-side validation
- **SQL Injection Protection** - Parameterized queries and input sanitization
- **CORS Protection** - Cross-origin request security

### User Experience
- **Responsive Design** - Works seamlessly on all devices
- **Modern UI/UX** - Clean, professional interface with animations
- **Real-time Updates** - Live balance and transaction updates
- **Form Validation** - Comprehensive client-side validation
- **Error Handling** - Graceful error messages and recovery

## 🛠️ Technology Stack

### Backend
- **C++17** - Modern C++ with advanced features
- **Crow Framework** - Lightweight HTTP web framework
- **MongoDB C++ Driver** - Native database connectivity
- **OpenSSL** - Cryptographic library for security

### Frontend
- **HTML5** - Modern markup with semantic elements
- **CSS3** - Advanced styling with Grid and Flexbox
- **JavaScript ES6+** - Modern JavaScript features
- **Responsive Design** - Mobile-first approach

### Database
- **MongoDB** - NoSQL database for flexible data storage
- **Indexes** - Optimized database performance
- **Data Validation** - Schema validation and constraints

## 📁 Project Structure

```
banking-system/
├── backend/
│   ├── src/
│   │   ├── main.cpp          # Application entry point
│   │   ├── database.cpp      # Database operations
│   │   ├── auth.cpp          # Authentication logic
│   │   ├── routes.cpp        # API route handlers
│   │   └── utils.cpp         # Utility functions
│   ├── include/
│   │   ├── database.h        # Database class definitions
│   │   ├── auth.h           # Authentication headers
│   │   ├── routes.h         # Route handler headers
│   │   └── utils.h          # Utility headers
│   └── CMakeLists.txt       # Build configuration
├── frontend/
│   ├── index.html           # Landing page
│   ├── login.html           # Authentication page
│   ├── dashboard.html       # Main dashboard
│   ├── transfer.html        # Money transfer page
│   ├── history.html         # Transaction history
│   ├── css/
│   │   └── styles.css       # Complete styling
│   └── js/
│       ├── api.js           # API communication
│       ├── auth.js          # Authentication logic
│       └── app.js           # Main application logic
├── database/
│   └── init.js              # Database initialization
├── setup.sh                 # Automated setup script
└── README.md               # Project documentation
```

## 🚀 Quick Start

### 1. System Requirements
- Ubuntu 20.04+ (WSL supported)
- MongoDB 6.0+
- CMake 3.10+
- GCC with C++17 support

### 2. Automated Setup
```bash
# Make setup script executable
chmod +x setup.sh

# Run automated setup
./setup.sh
```

### 3. Manual Setup (Alternative)

#### Install Dependencies
```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install build tools
sudo apt install -y build-essential cmake pkg-config

# Install MongoDB
sudo apt install -y mongodb-org

# Install C++ MongoDB driver
sudo apt install -y libmongocxx-dev libbsoncxx-dev

# Install Crow framework
sudo apt install -y libcrow-dev

# Install OpenSSL
sudo apt install -y libssl-dev
```

#### Build Backend
```bash
cd backend
mkdir build && cd build
cmake ..
make
```

#### Initialize Database
```bash
# Start MongoDB
sudo systemctl start mongod

# Initialize database schema
mongo < ../database/init.js
```

### 4. Run Application
```bash
# Start the C++ server (from backend/build/)
./banking_server

# Serve frontend (new terminal)
cd frontend
python3 -m http.server 3000

# Access application
# Open: http://localhost:3000
```

## 📊 API Documentation

### Authentication Endpoints
- `POST /api/register` - Create new user account
- `POST /api/login` - User authentication

### Account Management
- `GET /api/accounts` - Retrieve user accounts
- `POST /api/accounts` - Create new bank account
- `GET /api/balance/:id` - Get account balance

### Transactions
- `POST /api/transfer` - Transfer money between accounts
- `GET /api/transactions/:id` - Get transaction history

### Request/Response Format
```json
// Login Request
{
    "username": "testuser",
    "password": "password123"
}

// Login Response
{
    "success": true,
    "token": "auth_token_here",
    "user_id": "user_id_here",
    "username": "testuser"
}

// Transfer Request
{
    "from_account": "account_id",
    "to_account_number": "ACC1234567890",
    "amount": 100.50,
    "description": "Payment description"
}
```

## 🔒 Security Implementation

### Password Security
- SHA-256 hashing algorithm
- No plaintext password storage
- Secure password verification

### Authentication
- Token-based session management
- 24-hour token expiration
- Automatic token cleanup

### Input Validation
- Server-side validation for all inputs
- Email format validation
- Amount range validation
- Account number verification

### Database Security
- Parameterized queries prevent injection
- Unique constraints on critical fields
- Indexed fields for performance

## 🎨 UI/UX Features

### Visual Design
- Modern gradient backgrounds
- Card-based layout system
- Consistent color scheme
- Professional typography

### Interactions
- Smooth hover animations
- Form validation feedback
- Loading states
- Success/error messages

### Responsive Design
- Mobile-first approach
- Flexible grid layouts
- Scalable components
- Touch-friendly interface

## 🧪 Testing

### Default Test Account
```
Username: testuser
Password: password
```

### Test Scenarios
1. **User Registration** - Create new account with validation
2. **Login Process** - Authentication with error handling
3. **Account Creation** - Multiple account types with initial deposit
4. **Money Transfer** - Between accounts with balance verification
5. **Transaction History** - View and filter transaction records

## 🚀 Deployment Options

### Local Development
- Use provided setup scripts
- Python HTTP server for frontend
- Local MongoDB instance

### Production Deployment
- Docker containerization
- Nginx reverse proxy
- SSL/TLS certificates
- MongoDB Atlas cloud database

## 🔧 Customization

### Adding Features
- Extend API endpoints in `routes.cpp`
- Add frontend pages and JavaScript modules
- Update database schemas in `database.cpp`

### Styling Changes
- Modify `styles.css` for visual updates
- Update color scheme variables
- Customize responsive breakpoints

### Security Enhancements
- Implement rate limiting
- Add two-factor authentication
- Enhanced password policies
- Audit logging

## 🤝 Contributing

1. Fork the repository
2. Create feature branch: `git checkout -b feature-name`
3. Commit changes: `git commit -am 'Add feature'`
4. Push to branch: `git push origin feature-name`
5. Submit pull request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🎯 Future Enhancements

### Planned Features
- [ ] Account statements generation
- [ ] Email notifications
- [ ] Mobile application
- [ ] Loan management system
- [ ] Investment portfolio tracking
- [ ] Multi-currency support
- [ ] Admin dashboard
- [ ] Advanced reporting
- [ ] API rate limiting
- [ ] Two-factor authentication

### Technical Improvements
- [ ] Docker containerization
- [ ] Automated testing suite
- [ ] CI/CD pipeline
- [ ] Performance monitoring
- [ ] Database backup system
- [ ] Load balancing
- [ ] Microservices architecture

## 📞 Support

For questions, issues, or contributions:
- Create GitHub Issues for bugs
- Submit Pull Requests for features
- Review documentation for setup help

---

**Made with ❤️ for educational purposes**

This project demonstrates modern full-stack development practices with a focus on security, performance, and user experience.
