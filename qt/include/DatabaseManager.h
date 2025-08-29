#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QList>
#include <QMap>
#include <QDebug>
#include <memory>

// Database connection configuration
struct DatabaseConfig {
    QString hostName;
    QString databaseName;
    QString userName;
    QString password;
    int port;
    
    DatabaseConfig() : hostName("localhost"), databaseName("hospital_db"), 
                      userName("root"), password(""), port(3306) {}
};

// Query result wrapper
class QueryResult {
private:
    bool success;
    QString errorMessage;
    QList<QMap<QString, QVariant>> data;
    
public:
    QueryResult(bool success = false, const QString& error = "") 
        : success(success), errorMessage(error) {}
    
    bool isSuccess() const { return success; }
    QString getError() const { return errorMessage; }
    const QList<QMap<QString, QVariant>>& getData() const { return data; }
    
    void setSuccess(bool s) { success = s; }
    void setError(const QString& error) { errorMessage = error; }
    void setData(const QList<QMap<QString, QVariant>>& d) { data = d; }
    void addRow(const QMap<QString, QVariant>& row) { data.append(row); }
    
    // Convenience methods
    bool isEmpty() const { return data.isEmpty(); }
    int rowCount() const { return data.size(); }
    QMap<QString, QVariant> firstRow() const { 
        return data.isEmpty() ? QMap<QString, QVariant>() : data.first(); 
    }
};

class DatabaseManager : public QObject {
    Q_OBJECT
    
private:
    QSqlDatabase database;
    DatabaseConfig config;
    QString connectionName;
    
public:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();
    
    // Connection management
    bool connectToDatabase(const DatabaseConfig& config);
    bool isConnected() const;
    void disconnect();
    QString getLastError() const;
    
    // Database operations
    bool initializeDatabase();
    bool createTables();
    bool dropTables();
    
    // Query execution
    QueryResult executeQuery(const QString& query, const QMap<QString, QVariant>& params = {});
    bool executeUpdate(const QString& query, const QMap<QString, QVariant>& params = {});
    int executeInsert(const QString& query, const QMap<QString, QVariant>& params = {});
    
    // Transaction management
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    // Utility methods
    QString escapeString(const QString& str) const;
    QVariant getLastInsertId() const;
    
    // Configuration
    void setConfig(const DatabaseConfig& config) { this->config = config; }
    DatabaseConfig getConfig() const { return config; }
    
    // Test connection
    bool testConnection();
    
signals:
    void connectionStatusChanged(bool connected);
    void errorOccurred(const QString& error);
    
private:
    void setupDatabase();
    QString generateConnectionName();
};

#endif // DATABASE_MANAGER_H