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
    std::string passwordHash;
    UserType userType;
    std::string email;
    std::string phoneNumber;
    std::string createdAt;
    bool isActive;

public:
    // Constructors
    User();
    User(const std::string& username, const std::string& password, UserType userType);
    
    // Getter methods
    int getUserId() const { return userId; }
    std::string getUsername() const { return username; }
    std::string getPasswordHash() const { return passwordHash; }
    UserType getUserType() const { return userType; }
    std::string getEmail() const { return email; }
    std::string getPhoneNumber() const { return phoneNumber; }
    std::string getCreatedAt() const { return createdAt; }
    bool getIsActive() const { return isActive; }
    
    // Setter methods
    void setUserId(int id) { userId = id; }
    void setUsername(const std::string& name) { username = name; }
    void setPasswordHash(const std::string& hash) { passwordHash = hash; }
    void setUserType(UserType type) { userType = type; }
    void setEmail(const std::string& userEmail) { email = userEmail; }
    void setPhoneNumber(const std::string& phone) { phoneNumber = phone; }
    void setCreatedAt(const std::string& created) { createdAt = created; }
    void setIsActive(bool active) { isActive = active; }
    
    // Utility methods
    std::string userTypeToString() const;
    static UserType stringToUserType(const std::string& typeStr);
    bool validatePassword(const std::string& password) const;
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
    std::unique_ptr<User> getUserByEmail(const std::string& email);
    std::vector<std::unique_ptr<User>> getAllUsers();
    std::vector<std::unique_ptr<User>> getActiveUsers();
    
    bool updateUser(const User& user);
    bool deleteUser(int userId);
    bool deactivateUser(int userId);
    bool activateUser(int userId);
    bool setUserActiveStatus(int userId, bool isActive);
    
    // Authentication
    std::unique_ptr<User> authenticateUser(const std::string& username, const std::string& password);
    bool changePassword(int userId, const std::string& oldPassword, const std::string& newPassword);
    
    // Search operations
    std::vector<std::unique_ptr<User>> searchUsers(const std::string& searchTerm);
    bool userExists(const std::string& username);
    bool userExists(const std::string& username, const std::string& email);
    int getUserCount();
    
private:
    User* mapRowToUser(MYSQL_ROW row, unsigned long* lengths);
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
};

#endif // USER_H