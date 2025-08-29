#include "DatabaseManager.h"
#include <QUuid>
#include <QCryptographicHash>
#include <QSqlDriver>

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent) {
    connectionName = generateConnectionName();
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connectToDatabase(const DatabaseConfig& config) {
    this->config = config;
    
    // Remove existing connection if any
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    
    // Create new connection
    database = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    database.setHostName(config.hostName);
    database.setDatabaseName(config.databaseName);
    database.setUserName(config.userName);
    database.setPassword(config.password);
    database.setPort(config.port);
    
    // Set connection options for better reliability
    database.setConnectOptions("MYSQL_OPT_RECONNECT=1;MYSQL_OPT_CONNECT_TIMEOUT=5");
    
    bool connected = database.open();
    
    if (connected) {
        qDebug() << "Database connected successfully to" << config.databaseName;
        emit connectionStatusChanged(true);
        
        // Test the connection with a simple query
        QSqlQuery testQuery("SELECT 1", database);
        if (!testQuery.exec()) {
            qDebug() << "Connection test failed:" << testQuery.lastError().text();
            connected = false;
        }
    } else {
        QString error = database.lastError().text();
        qDebug() << "Database connection failed:" << error;
        emit errorOccurred(error);
        emit connectionStatusChanged(false);
    }
    
    return connected;
}

bool DatabaseManager::isConnected() const {
    return database.isOpen() && database.isValid();
}

void DatabaseManager::disconnect() {
    if (database.isOpen()) {
        database.close();
        emit connectionStatusChanged(false);
    }
    
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

QString DatabaseManager::getLastError() const {
    return database.lastError().text();
}

bool DatabaseManager::testConnection() {
    if (!isConnected()) {
        return false;
    }
    
    QSqlQuery query("SELECT 1", database);
    return query.exec();
}

QueryResult DatabaseManager::executeQuery(const QString& query, const QMap<QString, QVariant>& params) {
    if (!isConnected()) {
        return QueryResult(false, "数据库未连接");
    }
    
    QSqlQuery sqlQuery(database);
    
    if (!params.isEmpty()) {
        sqlQuery.prepare(query);
        // Bind parameters
        for (auto it = params.begin(); it != params.end(); ++it) {
            sqlQuery.bindValue(it.key(), it.value());
        }
    } else {
        sqlQuery.prepare(query);
    }
    
    if (!sqlQuery.exec()) {
        QString error = sqlQuery.lastError().text();
        qDebug() << "Query execution failed:" << error;
        qDebug() << "Query:" << query;
        emit errorOccurred(error);
        return QueryResult(false, error);
    }
    
    QueryResult result(true);
    QList<QMap<QString, QVariant>> data;
    
    while (sqlQuery.next()) {
        QMap<QString, QVariant> row;
        QSqlRecord record = sqlQuery.record();
        
        for (int i = 0; i < record.count(); ++i) {
            row[record.fieldName(i)] = sqlQuery.value(i);
        }
        
        data.append(row);
    }
    
    result.setData(data);
    return result;
}

bool DatabaseManager::executeUpdate(const QString& query, const QMap<QString, QVariant>& params) {
    if (!isConnected()) {
        emit errorOccurred("数据库未连接");
        return false;
    }
    
    QSqlQuery sqlQuery(database);
    
    if (!params.isEmpty()) {
        sqlQuery.prepare(query);
        // Bind parameters
        for (auto it = params.begin(); it != params.end(); ++it) {
            sqlQuery.bindValue(it.key(), it.value());
        }
    } else {
        sqlQuery.prepare(query);
    }
    
    if (!sqlQuery.exec()) {
        QString error = sqlQuery.lastError().text();
        qDebug() << "Update execution failed:" << error;
        qDebug() << "Query:" << query;
        emit errorOccurred(error);
        return false;
    }
    
    return true;
}

int DatabaseManager::executeInsert(const QString& query, const QMap<QString, QVariant>& params) {
    if (!executeUpdate(query, params)) {
        return -1;
    }
    
    // Get last insert ID
    QSqlQuery idQuery("SELECT LAST_INSERT_ID()", database);
    if (idQuery.exec() && idQuery.next()) {
        return idQuery.value(0).toInt();
    }
    
    return -1;
}

bool DatabaseManager::beginTransaction() {
    if (!isConnected()) {
        return false;
    }
    return database.transaction();
}

bool DatabaseManager::commitTransaction() {
    if (!isConnected()) {
        return false;
    }
    return database.commit();
}

bool DatabaseManager::rollbackTransaction() {
    if (!isConnected()) {
        return false;
    }
    return database.rollback();
}

QString DatabaseManager::escapeString(const QString& str) const {
    QString escaped = str;
    escaped.replace("'", "''");
    escaped.replace("\\", "\\\\");
    return escaped;
}

QVariant DatabaseManager::getLastInsertId() const {
    QSqlQuery query("SELECT LAST_INSERT_ID()", database);
    if (query.exec() && query.next()) {
        return query.value(0);
    }
    return QVariant();
}

QString DatabaseManager::generateConnectionName() {
    return QUuid::createUuid().toString();
}

bool DatabaseManager::initializeDatabase() {
    return createTables();
}

bool DatabaseManager::createTables() {
    if (!isConnected()) {
        return false;
    }
    
    // This would create tables if they don't exist
    // For now, we assume tables are already created via SQL scripts
    return true;
}

bool DatabaseManager::dropTables() {
    if (!isConnected()) {
        return false;
    }
    
    // Implementation for dropping tables if needed
    return true;
}