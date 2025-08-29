#include "DatabaseService.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <openssl/sha.h>

DatabaseService::DatabaseService(const std::string& host, const std::string& username,
                                const std::string& password, const std::string& database,
                                unsigned int port, size_t maxConnections) {
    connectionPool = std::make_shared<ConnectionPool>(host, username, password, database, port, maxConnections);
    userDAO = std::make_unique<UserDAO>(connectionPool);
    productDAO = std::make_unique<ProductDAO>(connectionPool);
    orderDAO = std::make_unique<OrderDAO>(connectionPool);
}

DatabaseService::~DatabaseService() = default;

bool DatabaseService::initializeDatabase() {
    return createTables();
}

bool DatabaseService::createTables() {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::vector<std::string> createTableQueries = {
        // 用户表
        R"(CREATE TABLE IF NOT EXISTS users (
            user_id INT AUTO_INCREMENT PRIMARY KEY,
            username VARCHAR(50) UNIQUE NOT NULL,
            email VARCHAR(100) UNIQUE NOT NULL,
            password_hash VARCHAR(255) NOT NULL,
            first_name VARCHAR(50),
            last_name VARCHAR(50),
            phone VARCHAR(20),
            is_active BOOLEAN DEFAULT TRUE,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            INDEX idx_username (username),
            INDEX idx_email (email),
            INDEX idx_active (is_active)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // 产品分类表
        R"(CREATE TABLE IF NOT EXISTS categories (
            category_id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            description TEXT,
            parent_id INT,
            is_active BOOLEAN DEFAULT TRUE,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            FOREIGN KEY (parent_id) REFERENCES categories(category_id) ON DELETE SET NULL,
            INDEX idx_parent (parent_id),
            INDEX idx_active (is_active)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // 产品表
        R"(CREATE TABLE IF NOT EXISTS products (
            product_id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(200) NOT NULL,
            description TEXT,
            price DECIMAL(10,2) NOT NULL,
            category_id INT,
            sku VARCHAR(50) UNIQUE NOT NULL,
            stock_quantity INT DEFAULT 0,
            weight DECIMAL(8,2) DEFAULT 0.00,
            dimensions VARCHAR(100),
            image_url VARCHAR(500),
            is_active BOOLEAN DEFAULT TRUE,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            FOREIGN KEY (category_id) REFERENCES categories(category_id) ON DELETE SET NULL,
            INDEX idx_category (category_id),
            INDEX idx_sku (sku),
            INDEX idx_price (price),
            INDEX idx_stock (stock_quantity),
            INDEX idx_active (is_active)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // 订单表
        R"(CREATE TABLE IF NOT EXISTS orders (
            order_id INT AUTO_INCREMENT PRIMARY KEY,
            user_id INT NOT NULL,
            status ENUM('PENDING', 'CONFIRMED', 'PROCESSING', 'SHIPPED', 'DELIVERED', 'CANCELLED', 'REFUNDED') DEFAULT 'PENDING',
            total_amount DECIMAL(10,2) NOT NULL,
            shipping_cost DECIMAL(10,2) DEFAULT 0.00,
            tax_amount DECIMAL(10,2) DEFAULT 0.00,
            shipping_address TEXT NOT NULL,
            billing_address TEXT NOT NULL,
            payment_method VARCHAR(50),
            tracking_number VARCHAR(100),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            shipped_at TIMESTAMP NULL,
            delivered_at TIMESTAMP NULL,
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            INDEX idx_user (user_id),
            INDEX idx_status (status),
            INDEX idx_created (created_at),
            INDEX idx_tracking (tracking_number)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // 订单项表
        R"(CREATE TABLE IF NOT EXISTS order_items (
            order_item_id INT AUTO_INCREMENT PRIMARY KEY,
            order_id INT NOT NULL,
            product_id INT NOT NULL,
            quantity INT NOT NULL,
            unit_price DECIMAL(10,2) NOT NULL,
            total_price DECIMAL(10,2) NOT NULL,
            FOREIGN KEY (order_id) REFERENCES orders(order_id) ON DELETE CASCADE,
            FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE CASCADE,
            INDEX idx_order (order_id),
            INDEX idx_product (product_id)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // 购物车表
        R"(CREATE TABLE IF NOT EXISTS shopping_cart (
            cart_id INT AUTO_INCREMENT PRIMARY KEY,
            user_id INT NOT NULL,
            product_id INT NOT NULL,
            quantity INT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE CASCADE,
            UNIQUE KEY unique_user_product (user_id, product_id),
            INDEX idx_user (user_id)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // 用户地址表
        R"(CREATE TABLE IF NOT EXISTS user_addresses (
            address_id INT AUTO_INCREMENT PRIMARY KEY,
            user_id INT NOT NULL,
            address_type ENUM('SHIPPING', 'BILLING') NOT NULL,
            first_name VARCHAR(50) NOT NULL,
            last_name VARCHAR(50) NOT NULL,
            company VARCHAR(100),
            address_line1 VARCHAR(200) NOT NULL,
            address_line2 VARCHAR(200),
            city VARCHAR(100) NOT NULL,
            state VARCHAR(100) NOT NULL,
            postal_code VARCHAR(20) NOT NULL,
            country VARCHAR(100) NOT NULL,
            phone VARCHAR(20),
            is_default BOOLEAN DEFAULT FALSE,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            INDEX idx_user (user_id),
            INDEX idx_type (address_type)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)"
    };
    
    if (!conn->beginTransaction()) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    for (const auto& query : createTableQueries) {
        if (!conn->executeUpdate(query)) {
            std::cerr << "Failed to create table: " << conn->getError() << std::endl;
            conn->rollback();
            connectionPool->returnConnection(std::move(conn));
            return false;
        }
    }
    
    // 插入默认分类
    std::string insertDefaultCategory = 
        "INSERT IGNORE INTO categories (category_id, name, description) VALUES (1, 'General', 'Default category')";
    conn->executeUpdate(insertDefaultCategory);
    
    bool result = conn->commit();
    connectionPool->returnConnection(std::move(conn));
    
    if (result) {
        std::cout << "Database tables created successfully!" << std::endl;
    }
    
    return result;
}

bool DatabaseService::dropTables() {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::vector<std::string> dropTableQueries = {
        "DROP TABLE IF EXISTS order_items",
        "DROP TABLE IF EXISTS shopping_cart",
        "DROP TABLE IF EXISTS user_addresses",
        "DROP TABLE IF EXISTS orders",
        "DROP TABLE IF EXISTS products",
        "DROP TABLE IF EXISTS categories",
        "DROP TABLE IF EXISTS users"
    };
    
    if (!conn->beginTransaction()) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    for (const auto& query : dropTableQueries) {
        if (!conn->executeUpdate(query)) {
            std::cerr << "Failed to drop table: " << conn->getError() << std::endl;
            conn->rollback();
            connectionPool->returnConnection(std::move(conn));
            return false;
        }
    }
    
    bool result = conn->commit();
    connectionPool->returnConnection(std::move(conn));
    
    if (result) {
        std::cout << "Database tables dropped successfully!" << std::endl;
    }
    
    return result;
}

bool DatabaseService::registerUser(const std::string& username, const std::string& email,
                                  const std::string& password, const std::string& firstName,
                                  const std::string& lastName, const std::string& phone) {
    // 验证输入
    if (!validateUsername(username) || !validateEmail(email) || password.length() < 8) {
        return false;
    }
    
    // 检查用户是否已存在
    if (userDAO->userExists(username, email)) {
        return false;
    }
    
    // 创建用户
    User user(username, email, hashPassword(password));
    user.setFirstName(firstName);
    user.setLastName(lastName);
    user.setPhone(phone);
    
    return userDAO->createUser(user);
}

std::unique_ptr<User> DatabaseService::loginUser(const std::string& username, const std::string& password) {
    return userDAO->authenticateUser(username, password);
}

bool DatabaseService::createProduct(const std::string& name, const std::string& description,
                                   double price, int categoryId, int stockQuantity) {
    if (name.empty() || price < 0) {
        return false;
    }
    
    Product product(name, description, price, categoryId);
    product.setStockQuantity(stockQuantity);
    
    return productDAO->createProduct(product);
}

int DatabaseService::placeOrder(int userId, const std::vector<std::pair<int, int>>& productQuantities,
                               const std::string& shippingAddress, const std::string& billingAddress,
                               const std::string& paymentMethod) {
    if (productQuantities.empty() || shippingAddress.empty() || billingAddress.empty()) {
        return 0;
    }
    
    // 验证用户存在
    auto user = userDAO->getUserById(userId);
    if (!user) {
        return 0;
    }
    
    // 创建订单
    Order order(userId, shippingAddress, billingAddress);
    order.setPaymentMethod(paymentMethod);
    
    // 添加订单项并计算总价
    double totalAmount = 0.0;
    for (const auto& item : productQuantities) {
        int productId = item.first;
        int quantity = item.second;
        
        auto product = productDAO->getProductById(productId);
        if (!product || !product->getIsActive() || product->getStockQuantity() < quantity) {
            return 0; // 产品不存在、不活跃或库存不足
        }
        
        OrderItem orderItem(0, productId, quantity, product->getPrice());
        order.addItem(orderItem);
        totalAmount += orderItem.getTotalPrice();
    }
    
    // 设置运费和税费（简化计算）
    double shippingCost = totalAmount > 100.0 ? 0.0 : 10.0; // 满100免运费
    double taxAmount = totalAmount * 0.08; // 8%税率
    
    order.setShippingCost(shippingCost);
    order.setTaxAmount(taxAmount);
    order.calculateTotalAmount();
    
    // 创建订单
    int orderId = orderDAO->createOrder(order);
    
    if (orderId > 0) {
        // 减少库存
        for (const auto& item : productQuantities) {
            productDAO->decreaseStock(item.first, item.second);
        }
    }
    
    return orderId;
}

bool DatabaseService::processOrderPayment(int orderId, const std::string& paymentMethod) {
    auto order = orderDAO->getOrderById(orderId);
    if (!order || order->getStatus() != OrderStatus::PENDING) {
        return false;
    }
    
    // 这里应该集成实际的支付处理逻辑
    // 简化处理：直接确认订单
    order->setPaymentMethod(paymentMethod);
    
    return orderDAO->confirmOrder(orderId);
}

bool DatabaseService::fulfillOrder(int orderId, const std::string& trackingNumber) {
    auto order = orderDAO->getOrderById(orderId);
    if (!order) {
        return false;
    }
    
    // 根据当前状态决定下一步操作
    switch (order->getStatus()) {
        case OrderStatus::CONFIRMED:
            return orderDAO->processOrder(orderId);
        case OrderStatus::PROCESSING:
            if (!trackingNumber.empty()) {
                return orderDAO->shipOrder(orderId, trackingNumber);
            }
            return false;
        case OrderStatus::SHIPPED:
            return orderDAO->deliverOrder(orderId);
        default:
            return false;
    }
}

DatabaseService::DatabaseStats DatabaseService::getDatabaseStats() {
    DatabaseStats stats = {};
    
    stats.totalUsers = userDAO->getUserCount();
    stats.activeUsers = static_cast<int>(userDAO->getActiveUsers().size());
    stats.totalProducts = productDAO->getProductCount();
    stats.activeProducts = productDAO->getActiveProductCount();
    stats.totalOrders = orderDAO->getOrderCount();
    stats.pendingOrders = orderDAO->getOrderCountByStatus(OrderStatus::PENDING);
    stats.totalRevenue = orderDAO->getTotalRevenue();
    stats.lowStockProducts = static_cast<int>(productDAO->getLowStockProducts(10).size());
    
    return stats;
}

bool DatabaseService::backupDatabase(const std::string& backupPath) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    // 这里应该实现数据库备份逻辑
    // 可以使用mysqldump命令或者MySQL的备份API
    std::cout << "Database backup functionality not implemented yet." << std::endl;
    
    connectionPool->returnConnection(std::move(conn));
    return false;
}

bool DatabaseService::optimizeTables() {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::vector<std::string> tables = {
        "users", "categories", "products", "orders", "order_items", 
        "shopping_cart", "user_addresses"
    };
    
    for (const auto& table : tables) {
        std::string query = "OPTIMIZE TABLE " + table;
        if (!conn->executeUpdate(query)) {
            std::cerr << "Failed to optimize table " << table << ": " << conn->getError() << std::endl;
        }
    }
    
    connectionPool->returnConnection(std::move(conn));
    return true;
}

bool DatabaseService::cleanupOldData(int daysOld) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    if (!conn->beginTransaction()) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    // 清理旧的已完成订单（保留重要数据）
    std::stringstream query;
    query << "DELETE FROM order_items WHERE order_id IN ("
          << "SELECT order_id FROM orders WHERE status IN ('DELIVERED', 'CANCELLED') "
          << "AND created_at < DATE_SUB(NOW(), INTERVAL " << daysOld << " DAY))";
    
    if (!conn->executeUpdate(query.str())) {
        conn->rollback();
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    // 清理空的购物车项目
    std::string cleanupCart = "DELETE FROM shopping_cart WHERE created_at < DATE_SUB(NOW(), INTERVAL 30 DAY)";
    conn->executeUpdate(cleanupCart);
    
    bool result = conn->commit();
    connectionPool->returnConnection(std::move(conn));
    
    if (result) {
        std::cout << "Old data cleanup completed successfully!" << std::endl;
    }
    
    return result;
}

std::string DatabaseService::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool DatabaseService::validateEmail(const std::string& email) {
    const std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailPattern);
}

bool DatabaseService::validateUsername(const std::string& username) {
    return !username.empty() && username.length() >= 3 && username.length() <= 50;
}