#ifndef DATABASE_SERVICE_H
#define DATABASE_SERVICE_H

#include <memory>
#include "DatabaseConnection.h"
#include "User.h"
#include "Product.h"
#include "Order.h"

class DatabaseService {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    std::unique_ptr<UserDAO> userDAO;
    std::unique_ptr<ProductDAO> productDAO;
    std::unique_ptr<OrderDAO> orderDAO;
    
public:
    DatabaseService(const std::string& host, const std::string& username,
                   const std::string& password, const std::string& database,
                   unsigned int port = 3306, size_t maxConnections = 10);
    ~DatabaseService();
    
    // 初始化数据库
    bool initializeDatabase();
    bool createTables();
    bool dropTables();
    
    // 获取DAO实例
    UserDAO* getUserDAO() { return userDAO.get(); }
    ProductDAO* getProductDAO() { return productDAO.get(); }
    OrderDAO* getOrderDAO() { return orderDAO.get(); }
    
    // 业务逻辑方法
    bool registerUser(const std::string& username, const std::string& email, 
                     const std::string& password, const std::string& firstName = "",
                     const std::string& lastName = "", const std::string& phone = "");
    
    std::unique_ptr<User> loginUser(const std::string& username, const std::string& password);
    
    bool createProduct(const std::string& name, const std::string& description,
                      double price, int categoryId, int stockQuantity = 0);
    
    int placeOrder(int userId, const std::vector<std::pair<int, int>>& productQuantities,
                   const std::string& shippingAddress, const std::string& billingAddress,
                   const std::string& paymentMethod = "");
    
    bool processOrderPayment(int orderId, const std::string& paymentMethod);
    bool fulfillOrder(int orderId, const std::string& trackingNumber = "");
    
    // 统计和报告
    struct DatabaseStats {
        int totalUsers;
        int activeUsers;
        int totalProducts;
        int activeProducts;
        int totalOrders;
        int pendingOrders;
        double totalRevenue;
        int lowStockProducts;
    };
    
    DatabaseStats getDatabaseStats();
    
    // 数据维护
    bool backupDatabase(const std::string& backupPath);
    bool optimizeTables();
    bool cleanupOldData(int daysOld = 365);
    
private:
    std::string hashPassword(const std::string& password);
    bool validateEmail(const std::string& email);
    bool validateUsername(const std::string& username);
};

#endif // DATABASE_SERVICE_H