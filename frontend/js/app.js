// Main application logic

document.addEventListener('DOMContentLoaded', function() {
    // Check authentication on protected pages
    const protectedPages = ['dashboard.html', 'transfer.html', 'history.html'];
    const currentPage = window.location.pathname.split('/').pop();
    
    if (protectedPages.includes(currentPage) && !api.isAuthenticated()) {
        window.location.href = 'login.html';
        return;
    }

    // Initialize page-specific functionality
    if (currentPage === 'dashboard.html') {
        initDashboard();
    } else if (currentPage === 'transfer.html') {
        initTransfer();
    } else if (currentPage === 'history.html') {
        initHistory();
    }

    // Setup logout functionality for all pages
    setupLogout();
    
    // Update user info in header
    updateUserInfo();
});

// Dashboard functionality
function initDashboard() {
    loadAccounts();
    loadRecentTransactions();
    setupCreateAccountModal();
}

async function loadAccounts() {
    const accountsList = document.getElementById('accountsList');
    
    try {
        accountsList.innerHTML = '<div class="loading">Loading accounts...</div>';
        const accounts = await api.getAccounts();
        
        if (!accounts || accounts.length === 0) {
            accountsList.innerHTML = '<div class="no-data">No accounts found. Create your first account!</div>';
            return;
        }

        accountsList.innerHTML = accounts.map(account => `
            <div class="account-card">
                <div class="account-number">${account.account_number}</div>
                <div class="account-type">${account.account_type} Account</div>
                <div class="account-balance">$${account.balance.toFixed(2)}</div>
            </div>
        `).join('');
    } catch (error) {
        accountsList.innerHTML = '<div class="no-data">Error loading accounts</div>';
        console.error('Error loading accounts:', error);
    }
}

async function loadRecentTransactions() {
    const recentTransactions = document.getElementById('recentTransactions');
    
    try {
        recentTransactions.innerHTML = '<div class="loading">Loading transactions...</div>';
        
        // Get user's accounts first
        const accounts = await api.getAccounts();
        if (!accounts || accounts.length === 0) {
            recentTransactions.innerHTML = '<div class="no-data">No transactions found</div>';
            return;
        }

        // Get transactions for the first account (or all accounts)
        const transactions = await api.getTransactions(accounts[0].id);
        
        if (!transactions || transactions.length === 0) {
            recentTransactions.innerHTML = '<div class="no-data">No recent transactions</div>';
            return;
        }

        // Show only the 5 most recent transactions
        const recentTxns = transactions.slice(0, 5);
        
        recentTransactions.innerHTML = `
            <div class="transaction-table">
                <table>
                    <thead>
                        <tr>
                            <th>Date</th>
                            <th>Type</th>
                            <th>Amount</th>
                            <th>Status</th>
                        </tr>
                    </thead>
                    <tbody>
                        ${recentTxns.map(txn => `
                            <tr>
                                <td>${formatDate(txn.timestamp)}</td>
                                <td>${txn.transaction_type}</td>
                                <td>$${txn.amount.toFixed(2)}</td>
                                <td><span class="status-${txn.status}">${txn.status}</span></td>
                            </tr>
                        `).join('')}
                    </tbody>
                </table>
            </div>
        `;
    } catch (error) {
        recentTransactions.innerHTML = '<div class="no-data">Error loading transactions</div>';
        console.error('Error loading transactions:', error);
    }
}

function setupCreateAccountModal() {
    const createAccountBtn = document.getElementById('createAccountBtn');
    const modal = document.getElementById('createAccountModal');
    const closeBtn = modal.querySelector('.close');
    const form = document.getElementById('createAccountForm');
    const message = document.getElementById('createAccountMessage');

    createAccountBtn.addEventListener('click', () => {
        modal.style.display = 'block';
    });

    closeBtn.addEventListener('click', () => {
        modal.style.display = 'none';
        form.reset();
        message.style.display = 'none';
    });

    window.addEventListener('click', (e) => {
        if (e.target === modal) {
            modal.style.display = 'none';
        }
    });

    form.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const accountType = document.getElementById('accountType').value;
        const initialDeposit = parseFloat(document.getElementById('initialDeposit').value);

        try {
            showMessage(message, 'Creating account...', 'info');
            const response = await api.createAccount(accountType, initialDeposit);
            
            if (response.success) {
                showMessage(message, 'Account created successfully!', 'success');
                setTimeout(() => {
                    modal.style.display = 'none';
                    form.reset();
                    loadAccounts(); // Reload accounts
                }, 2000);
            } else {
                showMessage(message, response.message || 'Failed to create account', 'error');
            }
        } catch (error) {
            showMessage(message, error.message || 'Transfer failed', 'error');
        }
    });
}

// History functionality
function initHistory() {
    loadHistoryAccounts();
    loadTransactionHistory();
    setupHistoryFilters();
}

async function loadHistoryAccounts() {
    const accountFilter = document.getElementById('accountFilter');
    
    try {
        const accounts = await api.getAccounts();
        
        if (accounts && accounts.length > 0) {
            accountFilter.innerHTML = '<option value="">All Accounts</option>' +
                accounts.map(account => 
                    `<option value="${account.id}">${account.account_number} (${account.account_type})</option>`
                ).join('');
        }
    } catch (error) {
        console.error('Error loading accounts for filter:', error);
    }
}

async function loadTransactionHistory() {
    const tableBody = document.getElementById('transactionTableBody');
    const noTransactions = document.getElementById('noTransactions');
    
    try {
        tableBody.innerHTML = '<tr><td colspan="6" class="loading">Loading transactions...</td></tr>';
        
        // Get user's accounts first
        const accounts = await api.getAccounts();
        if (!accounts || accounts.length === 0) {
            tableBody.innerHTML = '';
            noTransactions.style.display = 'block';
            return;
        }

        // Get transactions for all accounts (for now, just the first one)
        const transactions = await api.getTransactions(accounts[0].id);
        
        if (!transactions || transactions.length === 0) {
            tableBody.innerHTML = '';
            noTransactions.style.display = 'block';
            return;
        }

        noTransactions.style.display = 'none';
        tableBody.innerHTML = transactions.map(txn => `
            <tr>
                <td>${formatDate(txn.timestamp)}</td>
                <td>${txn.transaction_type}</td>
                <td>${txn.from_account === accounts[0].id ? 'To: ' + txn.to_account : 'From: ' + txn.from_account}</td>
                <td>${txn.description || '-'}</td>
                <td>${txn.amount.toFixed(2)}</td>
                <td><span class="status-${txn.status}">${txn.status}</span></td>
            </tr>
        `).join('');
    } catch (error) {
        tableBody.innerHTML = '<tr><td colspan="6" class="no-data">Error loading transactions</td></tr>';
        console.error('Error loading transaction history:', error);
    }
}

function setupHistoryFilters() {
    const applyFiltersBtn = document.getElementById('applyFilters');
    
    applyFiltersBtn.addEventListener('click', () => {
        // In a real implementation, you would filter the transactions
        // based on the selected criteria
        loadTransactionHistory();
    });
}

// Utility functions
function setupLogout() {
    const logoutBtn = document.getElementById('logoutBtn');
    if (logoutBtn) {
        logoutBtn.addEventListener('click', (e) => {
            e.preventDefault();
            api.clearToken();
            window.location.href = 'login.html';
        });
    }
}

function updateUserInfo() {
    const welcomeUser = document.getElementById('welcomeUser');
    if (welcomeUser) {
        const userData = api.getUserData();
        if (userData) {
            welcomeUser.textContent = `Welcome, ${userData.username}`;
        }
    }
}

function formatDate(dateString) {
    const date = new Date(dateString);
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString([], {hour: '2-digit', minute:'2-digit'});
}

function showMessage(element, message, type) {
    element.textContent = message;
    element.className = `message ${type}`;
    element.style.display = 'block';
    
    if (type === 'success' || type === 'info') {
        setTimeout(() => {
            element.style.display = 'none';
        }, 5000);
    }
}.message || 'Failed to create account', 'error');
        }
    });
}

// Transfer functionality
function initTransfer() {
    loadTransferAccounts();
    setupTransferForm();
}

async function loadTransferAccounts() {
    const fromAccountSelect = document.getElementById('fromAccount');
    
    try {
        const accounts = await api.getAccounts();
        
        if (!accounts || accounts.length === 0) {
            fromAccountSelect.innerHTML = '<option value="">No accounts available</option>';
            return;
        }

        fromAccountSelect.innerHTML = '<option value="">Select account...</option>' +
            accounts.map(account => 
                `<option value="${account.id}" data-balance="${account.balance}">
                    ${account.account_number} (${account.account_type}) - $${account.balance.toFixed(2)}
                </option>`
            ).join('');

        // Update balance when account is selected
        fromAccountSelect.addEventListener('change', (e) => {
            const selectedOption = e.target.selectedOptions[0];
            const balance = selectedOption ? selectedOption.dataset.balance : '0.00';
            document.getElementById('availableBalance').textContent = `$${parseFloat(balance).toFixed(2)}`;
        });
    } catch (error) {
        fromAccountSelect.innerHTML = '<option value="">Error loading accounts</option>';
        console.error('Error loading accounts:', error);
    }
}

function setupTransferForm() {
    const form = document.getElementById('transferForm');
    const amountInput = document.getElementById('amount');
    const summaryAmount = document.getElementById('summaryAmount');
    const summaryTotal = document.getElementById('summaryTotal');
    const message = document.getElementById('transferMessage');

    // Update summary when amount changes
    amountInput.addEventListener('input', (e) => {
        const amount = parseFloat(e.target.value) || 0;
        summaryAmount.textContent = `$${amount.toFixed(2)}`;
        summaryTotal.textContent = `$${amount.toFixed(2)}`;
    });

    form.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const fromAccount = document.getElementById('fromAccount').value;
        const toAccountNumber = document.getElementById('toAccountNumber').value.trim();
        const amount = parseFloat(document.getElementById('amount').value);
        const description = document.getElementById('description').value.trim();

        // Validation
        if (!fromAccount) {
            showMessage(message, 'Please select a source account', 'error');
            return;
        }

        if (!toAccountNumber) {
            showMessage(message, 'Please enter recipient account number', 'error');
            return;
        }

        if (!amount || amount <= 0) {
            showMessage(message, 'Please enter a valid amount', 'error');
            return;
        }

        const availableBalance = parseFloat(document.getElementById('availableBalance').textContent.replace('$', ''));
        if (amount > availableBalance) {
            showMessage(message, 'Insufficient balance', 'error');
            return;
        }

        try {
            showMessage(message, 'Processing transfer...', 'info');
            const response = await api.transfer(fromAccount, toAccountNumber, amount, description);
            
            if (response.success) {
                showMessage(message, 'Transfer completed successfully!', 'success');
                form.reset();
                document.getElementById('availableBalance').textContent = '$0.00';
                document.getElementById('summaryAmount').textContent = '$0.00';
                document.getElementById('summaryTotal').textContent = '$0.00';
            } else {
                showMessage(message, response.message || 'Transfer failed', 'error');
            }
        } catch (error) {
            showMessage(message, error
