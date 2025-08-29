#include "User.h"
#include <memory>
#include <sstream>
#include <iostream>
#include <openssl/sha.h>
#include <iomanip>

// User class implementation
User::User() : userId(0), userType(UserType::PATIENT) {}

User::User(const std::string& username, const std::string& password, UserType userType)
    : userId(0), username(username), password(password), userType(userType) {}

std::string User::userTypeToString() const {
    switch (userType) {
        case UserType::DOCTOR: return "Doctor";
        case UserType::PATIENT: return "Patient";
    }
    return "Patient";
}

UserType User::stringToUserType(const std::string& typeStr) {
    if (typeStr == "Doctor") return UserType::DOCTOR;
    return UserType::PATIENT;
}

// UserDAO class implementation
UserDAO::UserDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool UserDAO::createUser(const User& user) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO users (username, password, user_type, email, phone_number) VALUES ("
          << "'" << conn->escapeString(user.getUsername()) << "', "
          << "'" << hashPassword(user.getPassword()) << "', "
          << "'" << user.userTypeToString() << "', "
          << (user.getEmail().empty() ? "NULL" : "'" + conn->escapeString(user.getEmail()) + "'") << ", "
          << (user.getPhoneNumber().empty() ? "NULL" : "'" + conn->escapeString(user.getPhoneNumber()) + "'") << ")";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<User> UserDAO::getUserById(int userId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT user_id, username, password, user_type, email, phone_number, created_at "
          << "FROM users WHERE user_id = " << userId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<User> user = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        user = std::unique_ptr<User>(mapRowToUser(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return user;
}

std::unique_ptr<User> UserDAO::getUserByUsername(const std::string& username) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT user_id, username, password, user_type, email, phone_number, created_at "
          << "FROM users WHERE username = '" << conn->escapeString(username) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<User> user = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        user = std::unique_ptr<User>(mapRowToUser(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return user;
}

std::vector<std::unique_ptr<User>> UserDAO::getAllUsers() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<User>> users;
    if (!conn) return users;
    
    std::string query = "SELECT user_id, username, password, user_type, email, phone_number, created_at "
                       "FROM users ORDER BY created_at DESC";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return users;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        users.push_back(std::unique_ptr<User>(mapRowToUser(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return users;
}

std::vector<std::unique_ptr<User>> UserDAO::getUsersByType(UserType userType) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<User>> users;
    if (!conn) return users;
    
    User tempUser;
    tempUser.setUserType(userType);
    
    std::stringstream query;
    query << "SELECT user_id, username, password, user_type, email, phone_number, created_at "
          << "FROM users WHERE user_type = '" << tempUser.userTypeToString() << "' ORDER BY username";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return users;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        users.push_back(std::unique_ptr<User>(mapRowToUser(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return users;
}

bool UserDAO::updateUser(const User& user) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE users SET "
          << "username = '" << conn->escapeString(user.getUsername()) << "', "
          << "user_type = '" << user.userTypeToString() << "', "
          << "email = " << (user.getEmail().empty() ? "NULL" : "'" + conn->escapeString(user.getEmail()) + "'") << ", "
          << "phone_number = " << (user.getPhoneNumber().empty() ? "NULL" : "'" + conn->escapeString(user.getPhoneNumber()) + "'") << " "
          << "WHERE user_id = " << user.getUserId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool UserDAO::deleteUser(int userId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM users WHERE user_id = " << userId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<User> UserDAO::authenticateUser(const std::string& username, const std::string& password) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::string hashedPassword = hashPassword(password);
    
    std::stringstream query;
    query << "SELECT user_id, username, password, user_type, email, phone_number, created_at "
          << "FROM users WHERE username = '" << conn->escapeString(username) 
          << "' AND password = '" << hashedPassword << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<User> user = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        user = std::unique_ptr<User>(mapRowToUser(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return user;
}

bool UserDAO::changePassword(int userId, const std::string& oldPassword, const std::string& newPassword) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    // First verify old password
    std::string hashedOldPassword = hashPassword(oldPassword);
    std::stringstream verifyQuery;
    verifyQuery << "SELECT user_id FROM users WHERE user_id = " << userId 
                << " AND password = '" << hashedOldPassword << "'";
    
    MYSQL_RES* result = conn->executeQuery(verifyQuery.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    mysql_free_result(result);
    
    if (!row) {
        connectionPool->returnConnection(std::move(conn));
        return false; // Old password doesn't match
    }
    
    // Update with new password
    std::string hashedNewPassword = hashPassword(newPassword);
    std::stringstream updateQuery;
    updateQuery << "UPDATE users SET password = '" << hashedNewPassword 
                << "' WHERE user_id = " << userId;
    
    bool updateResult = conn->executeUpdate(updateQuery.str());
    connectionPool->returnConnection(std::move(conn));
    return updateResult;
}

bool UserDAO::userExists(const std::string& username) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM users WHERE username = '" << conn->escapeString(username) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    bool exists = false;
    if (row && row[0]) {
        exists = std::stoi(row[0]) > 0;
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return exists;
}

bool UserDAO::emailExists(const std::string& email) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM users WHERE email = '" << conn->escapeString(email) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    bool exists = false;
    if (row && row[0]) {
        exists = std::stoi(row[0]) > 0;
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return exists;
}

bool UserDAO::userExists(const std::string& username, const std::string& email) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM users WHERE username = '" << conn->escapeString(username) 
          << "' OR email = '" << conn->escapeString(email) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    bool exists = false;
    if (row && row[0]) {
        exists = std::stoi(row[0]) > 0;
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return exists;
}

std::vector<std::unique_ptr<User>> UserDAO::searchUsers(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<User>> users;
    if (!conn) return users;
    
    std::stringstream query;
    query << "SELECT user_id, username, password, user_type, email, phone_number, created_at "
          << "FROM users WHERE username LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR email LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR phone_number LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY username";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return users;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        users.push_back(std::unique_ptr<User>(mapRowToUser(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return users;
}

std::vector<std::unique_ptr<User>> UserDAO::getActiveUsers() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<User>> users;
    if (!conn) return users;
    
    std::string query = "SELECT user_id, username, password, user_type, email, phone_number, created_at "
                       "FROM users WHERE created_at >= DATE_SUB(NOW(), INTERVAL 30 DAY) ORDER BY created_at DESC";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return users;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        users.push_back(std::unique_ptr<User>(mapRowToUser(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return users;
}

int UserDAO::getUserCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM users";
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = 0;
    if (row && row[0]) {
        count = std::stoi(row[0]);
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return count;
}

int UserDAO::getUserCountByType(UserType userType) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    User tempUser;
    tempUser.setUserType(userType);
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM users WHERE user_type = '" << tempUser.userTypeToString() << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = 0;
    if (row && row[0]) {
        count = std::stoi(row[0]);
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return count;
}

User* UserDAO::mapRowToUser(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    User* user = new User();
    
    if (row[0]) user->setUserId(std::stoi(row[0]));
    if (row[1]) user->setUsername(std::string(row[1], lengths[1]));
    if (row[2]) user->setPassword(std::string(row[2], lengths[2]));
    if (row[3]) user->setUserType(User::stringToUserType(std::string(row[3], lengths[3])));
    if (row[4]) user->setEmail(std::string(row[4], lengths[4]));
    if (row[5]) user->setPhoneNumber(std::string(row[5], lengths[5]));
    if (row[6]) user->setCreatedAt(std::string(row[6], lengths[6]));
    
    return user;
}

std::string UserDAO::hashPassword(const std::string& password) {
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

bool UserDAO::verifyPassword(const std::string& password, const std::string& hash) {
    return hashPassword(password) == hash;
}