#include "DatabaseConnection.h"
#include <iostream>
#include <stdexcept>

DatabaseConnection::DatabaseConnection(const std::string& host, const std::string& username,
                                     const std::string& password, const std::string& database,
                                     unsigned int port)
    : host(host), username(username), password(password), database(database), port(port), connection(nullptr) {
    connection = mysql_init(nullptr);
    if (!connection) {
        throw std::runtime_error("Failed to initialize MySQL connection");
    }
}

DatabaseConnection::~DatabaseConnection() {
    disconnect();
}

bool DatabaseConnection::connect() {
    if (!connection) {
        connection = mysql_init(nullptr);
        if (!connection) {
            return false;
        }
    }
    
    bool reconnect = true;
    mysql_options(connection, MYSQL_OPT_RECONNECT, &reconnect);
    mysql_options(connection, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    
    if (!mysql_real_connect(connection, host.c_str(), username.c_str(),
                           password.c_str(), database.c_str(), port, nullptr, 0)) {
        std::cerr << "Connection failed: " << mysql_error(connection) << std::endl;
        return false;
    }
    
    return true;
}

void DatabaseConnection::disconnect() {
    if (connection) {
        mysql_close(connection);
        connection = nullptr;
    }
}

bool DatabaseConnection::isConnected() {
    if (!connection) return false;
    return mysql_ping(connection) == 0;
}

bool DatabaseConnection::reconnect() {
    disconnect();
    return connect();
}

MYSQL_RES* DatabaseConnection::executeQuery(const std::string& query) {
    if (!connection || !isConnected()) {
        if (!reconnect()) {
            return nullptr;
        }
    }
    
    if (mysql_query(connection, query.c_str()) != 0) {
        std::cerr << "Query failed: " << mysql_error(connection) << std::endl;
        return nullptr;
    }
    
    return mysql_store_result(connection);
}

bool DatabaseConnection::executeUpdate(const std::string& query) {
    if (!connection || !isConnected()) {
        if (!reconnect()) {
            return false;
        }
    }
    
    if (mysql_query(connection, query.c_str()) != 0) {
        std::cerr << "Update failed: " << mysql_error(connection) << std::endl;
        return false;
    }
    
    return true;
}

bool DatabaseConnection::beginTransaction() {
    return executeUpdate("START TRANSACTION");
}

bool DatabaseConnection::commit() {
    return executeUpdate("COMMIT");
}

bool DatabaseConnection::rollback() {
    return executeUpdate("ROLLBACK");
}

std::string DatabaseConnection::escapeString(const std::string& str) {
    if (!connection) return str;
    
    char* escaped = new char[str.length() * 2 + 1];
    mysql_real_escape_string(connection, escaped, str.c_str(), str.length());
    std::string result(escaped);
    delete[] escaped;
    return result;
}

unsigned long DatabaseConnection::getLastInsertId() {
    if (!connection) return 0;
    return mysql_insert_id(connection);
}

std::string DatabaseConnection::getError() {
    if (!connection) return "No connection";
    return std::string(mysql_error(connection));
}

// ConnectionPool implementation
ConnectionPool::ConnectionPool(const std::string& host, const std::string& username,
                              const std::string& password, const std::string& database,
                              unsigned int port, size_t maxConnections)
    : host(host), username(username), password(password), database(database),
      port(port), maxConnections(maxConnections) {
    initializePool();
}

ConnectionPool::~ConnectionPool() {
    std::lock_guard<std::mutex> lock(poolMutex);
    while (!pool.empty()) {
        pool.pop();
    }
}

void ConnectionPool::initializePool() {
    std::lock_guard<std::mutex> lock(poolMutex);
    for (size_t i = 0; i < maxConnections; ++i) {
        auto conn = std::make_unique<DatabaseConnection>(host, username, password, database, port);
        if (conn->connect()) {
            pool.push(std::move(conn));
        }
    }
}

std::unique_ptr<DatabaseConnection> ConnectionPool::getConnection() {
    std::lock_guard<std::mutex> lock(poolMutex);
    if (pool.empty()) {
        auto conn = std::make_unique<DatabaseConnection>(host, username, password, database, port);
        if (conn->connect()) {
            return conn;
        }
        return nullptr;
    }
    
    auto conn = std::move(pool.front());
    pool.pop();
    
    if (!conn->isConnected()) {
        conn->reconnect();
    }
    
    return conn;
}

void ConnectionPool::returnConnection(std::unique_ptr<DatabaseConnection> conn) {
    if (conn && conn->isConnected()) {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (pool.size() < maxConnections) {
            pool.push(std::move(conn));
        }
    }
}