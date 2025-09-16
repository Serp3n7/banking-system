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
