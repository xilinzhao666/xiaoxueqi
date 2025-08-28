#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H

#include <mysql/mysql.h>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <queue>

class DatabaseConnection {
private:
    MYSQL* connection;
    std::string host;
    std::string username;
    std::string password;
    std::string database;
    unsigned int port;
    
public:
    DatabaseConnection(const std::string& host, const std::string& username, 
                      const std::string& password, const std::string& database, 
                      unsigned int port = 3306);
    ~DatabaseConnection();
    
    bool connect();
    void disconnect();
    bool isConnected();
    bool reconnect();
    
    MYSQL_RES* executeQuery(const std::string& query);
    bool executeUpdate(const std::string& query);
    bool beginTransaction();
    bool commit();
    bool rollback();
    
    std::string escapeString(const std::string& str);
    unsigned long getLastInsertId();
    std::string getError();
    
    MYSQL* getConnection() { return connection; }
};

class ConnectionPool {
private:
    std::queue<std::unique_ptr<DatabaseConnection>> pool;
    std::mutex poolMutex;
    std::string host, username, password, database;
    unsigned int port;
    size_t maxConnections;
    
public:
    ConnectionPool(const std::string& host, const std::string& username,
                  const std::string& password, const std::string& database,
                  unsigned int port = 3306, size_t maxConnections = 10);
    ~ConnectionPool();
    
    std::unique_ptr<DatabaseConnection> getConnection();
    void returnConnection(std::unique_ptr<DatabaseConnection> conn);
    void initializePool();
};

#endif // DATABASE_CONNECTION_H