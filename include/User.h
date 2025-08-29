#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

enum class UserType {
    DOCTOR,
    PATIENT
};

class User {
private:
    int userId;
    std::string username;
    std::string password;
    UserType userType;
    std::string email;
    std::string phoneNumber;
    std::string createdAt;

public:
    User();
    User(const std::string& username, const std::string& password, UserType userType);
    
    // Getter methods
    int getUserId() const { return userId; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
    UserType getUserType() const { return userType; }
    std::string getEmail() const { return email; }
    std::string getPhoneNumber() const { return phoneNumber; }
    std::string getCreatedAt() const { return createdAt; }
    
    // Setter methods
    void setUserId(int id) { userId = id; }
    void setUsername(const std::string& name) { username = name; }
    void setPassword(const std::string& pwd) { password = pwd; }
    void setUserType(UserType type) { userType = type; }
    void setEmail(const std::string& userEmail) { email = userEmail; }
    void setPhoneNumber(const std::string& phone) { phoneNumber = phone; }
    void setCreatedAt(const std::string& created) { createdAt = created; }
    
    // Utility methods
    std::string userTypeToString() const;
    static UserType stringToUserType(const std::string& typeStr);
};

class UserDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    UserDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createUser(const User& user);
    std::unique_ptr<User> getUserById(int userId);
    std::unique_ptr<User> getUserByUsername(const std::string& username);
    std::vector<std::unique_ptr<User>> getAllUsers();
    std::vector<std::unique_ptr<User>> getUsersByType(UserType userType);
    
    bool updateUser(const User& user);
    bool deleteUser(int userId);
    
    // Authentication
    std::unique_ptr<User> authenticateUser(const std::string& username, const std::string& password);
    bool changePassword(int userId, const std::string& oldPassword, const std::string& newPassword);
    
    // Validation
    bool userExists(const std::string& username);
    bool emailExists(const std::string& email);
    bool userExists(const std::string& username, const std::string& email);
    
    // Search operations
    std::vector<std::unique_ptr<User>> searchUsers(const std::string& searchTerm);
    std::vector<std::unique_ptr<User>> getActiveUsers();
    int getUserCount();
    int getUserCountByType(UserType userType);
    
private:
    User* mapRowToUser(MYSQL_ROW row, unsigned long* lengths);
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
};

#endif // USER_H