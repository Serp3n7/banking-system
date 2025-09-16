// API configuration
const API_BASE_URL = 'http://localhost:8080/api';

class API {
    constructor() {
        this.token = localStorage.getItem('auth_token');
    }

    // Set authorization token
    setToken(token) {
        this.token = token;
        localStorage.setItem('auth_token', token);
    }

    // Clear authorization token
    clearToken() {
        this.token = null;
        localStorage.removeItem('auth_token');
        localStorage.removeItem('user_data');
    }

    // Get authorization headers
    getHeaders(includeAuth = true) {
        const headers = {
            'Content-Type': 'application/json',
        };
        
        if (includeAuth && this.token) {
            headers['Authorization'] = this.token;
        }
        
        return headers;
    }

    // Generic API request method
    async request(endpoint, options = {}) {
        const url = `${API_BASE_URL}${endpoint}`;
        
        const config = {
            method: options.method || 'GET',
            headers: this.getHeaders(options.includeAuth !== false),
            ...options
        };

        if (config.method !== 'GET' && options.data) {
            config.body = JSON.stringify(options.data);
        }

        try {
            const response = await fetch(url, config);
            const data = await response.json().catch(() => ({}));

            if (!response.ok) {
                throw new Error(data.message || `HTTP error! status: ${response.status}`);
            }

            return data;
        } catch (error) {
            console.error('API request failed:', error);
            throw error;
        }
    }

    // Authentication methods
    async login(username, password) {
        const response = await this.request('/login', {
            method: 'POST',
            data: { username, password },
            includeAuth: false
        });
        
        if (response.success && response.token) {
            this.setToken(response.token);
            localStorage.setItem('user_data', JSON.stringify({
                id: response.user_id,
                username: response.username
            }));
        }
        
        return response;
    }

    async register(username, email, password) {
        return await this.request('/register', {
            method: 'POST',
            data: { username, email, password },
            includeAuth: false
        });
    }

    // Account methods
    async getAccounts() {
        return await this.request('/accounts');
    }

    async createAccount(accountType, initialDeposit) {
        return await this.request('/accounts', {
            method: 'POST',
            data: { 
                account_type: accountType, 
                initial_deposit: initialDeposit 
            }
        });
    }

    async getBalance(accountId) {
        return await this.request(`/balance/${accountId}`);
    }

    // Transaction methods
    async transfer(fromAccount, toAccountNumber, amount, description = '') {
        return await this.request('/transfer', {
            method: 'POST',
            data: {
                from_account: fromAccount,
                to_account_number: toAccountNumber,
                amount: parseFloat(amount),
                description: description
            }
        });
    }

    async getTransactions(accountId) {
        return await this.request(`/transactions/${accountId}`);
    }

    // Utility methods
    isAuthenticated() {
        return !!this.token;
    }

    getUserData() {
        const userData = localStorage.getItem('user_data');
        return userData ? JSON.parse(userData) : null;
    }
}

// Create global API instance
const api = new API();
