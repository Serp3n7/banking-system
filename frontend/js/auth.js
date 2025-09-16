// Authentication handling for login page

document.addEventListener('DOMContentLoaded', function() {
    // Check if user is already logged in
    if (api.isAuthenticated()) {
        window.location.href = 'dashboard.html';
        return;
    }

    // Tab switching
    const tabBtns = document.querySelectorAll('.tab-btn');
    const authForms = document.querySelectorAll('.auth-form');

    tabBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            const tab = btn.getAttribute('data-tab');
            
            // Update active tab
            tabBtns.forEach(b => b.classList.remove('active'));
            btn.classList.add('active');
            
            // Show corresponding form
            authForms.forEach(form => {
                form.classList.remove('active');
                if (form.id === `${tab}Form`) {
                    form.classList.add('active');
                }
            });
        });
    });

    // Login form handler
    const loginForm = document.getElementById('loginForm');
    const loginMessage = document.getElementById('loginMessage');

    loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const username = document.getElementById('loginUsername').value.trim();
        const password = document.getElementById('loginPassword').value;

        if (!username || !password) {
            showMessage(loginMessage, 'Please fill in all fields', 'error');
            return;
        }

        try {
            showMessage(loginMessage, 'Logging in...', 'info');
            const response = await api.login(username, password);
            
            if (response.success) {
                showMessage(loginMessage, 'Login successful! Redirecting...', 'success');
                setTimeout(() => {
                    window.location.href = 'dashboard.html';
                }, 1500);
            } else {
                showMessage(loginMessage, response.message || 'Login failed', 'error');
            }
        } catch (error) {
            showMessage(loginMessage, error.message || 'Login failed', 'error');
        }
    });

    // Register form handler
    const registerForm = document.getElementById('registerForm');
    const registerMessage = document.getElementById('registerMessage');

    registerForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const username = document.getElementById('registerUsername').value.trim();
        const email = document.getElementById('registerEmail').value.trim();
        const password = document.getElementById('registerPassword').value;
        const confirmPassword = document.getElementById('confirmPassword').value;

        // Validation
        if (!username || !email || !password || !confirmPassword) {
            showMessage(registerMessage, 'Please fill in all fields', 'error');
            return;
        }

        if (password !== confirmPassword) {
            showMessage(registerMessage, 'Passwords do not match', 'error');
            return;
        }

        if (password.length < 6) {
            showMessage(registerMessage, 'Password must be at least 6 characters long', 'error');
            return;
        }

        if (!isValidEmail(email)) {
            showMessage(registerMessage, 'Please enter a valid email address', 'error');
            return;
        }

        try {
            showMessage(registerMessage, 'Creating account...', 'info');
            const response = await api.register(username, email, password);
            
            if (response.success) {
                showMessage(registerMessage, 'Account created successfully! You can now log in.', 'success');
                
                // Switch to login tab after successful registration
                setTimeout(() => {
                    document.querySelector('[data-tab="login"]').click();
                    document.getElementById('loginUsername').value = username;
                }, 2000);
            } else {
                showMessage(registerMessage, response.message || 'Registration failed', 'error');
            }
        } catch (error) {
            showMessage(registerMessage, error.message || 'Registration failed', 'error');
        }
    });
});

// Utility functions
function showMessage(element, message, type) {
    element.textContent = message;
    element.className = `message ${type}`;
    element.style.display = 'block';
    
    if (type === 'success' || type === 'info') {
        setTimeout(() => {
            element.style.display = 'none';
        }, 5000);
    }
}

function isValidEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
}
