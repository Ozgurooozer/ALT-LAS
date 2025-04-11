#include "database/database_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace alt_las {
namespace database {

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
    // Tüm bağlantıları kapat
    std::unordered_map<std::string, std::shared_ptr<DatabaseConnection>> connections;
    
    {
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        connections = connections_;
    }
    
    for (auto& pair : connections) {
        auto connection = pair.second;
        if (connection) {
            connection->close();
        }
    }
}

std::shared_ptr<DatabaseConnection> DatabaseManager::createConnection(const ConnectionParams& params) {
    // Veritabanı bağlantısı oluştur
    auto connection = DatabaseConnectionFactory::createConnection(params);
    
    // Bağlantıyı aç
    if (connection) {
        connection->open();
    }
    
    return connection;
}

bool DatabaseManager::addConnection(const std::string& name, std::shared_ptr<DatabaseConnection> connection) {
    std::lock_guard<std::mutex> lock(connectionsMutex_);
    
    // Bağlantı adını kontrol et
    if (connections_.find(name) != connections_.end()) {
        return false;
    }
    
    // Bağlantıyı ekle
    connections_[name] = connection;
    
    return true;
}

bool DatabaseManager::removeConnection(const std::string& name) {
    std::lock_guard<std::mutex> lock(connectionsMutex_);
    
    // Bağlantıyı bul
    auto it = connections_.find(name);
    if (it == connections_.end()) {
        return false;
    }
    
    // Bağlantıyı kapat
    auto connection = it->second;
    if (connection) {
        connection->close();
    }
    
    // Bağlantıyı sil
    connections_.erase(it);
    
    return true;
}

bool DatabaseManager::hasConnection(const std::string& name) const {
    std::lock_guard<std::mutex> lock(connectionsMutex_);
    return connections_.find(name) != connections_.end();
}

std::shared_ptr<DatabaseConnection> DatabaseManager::getConnection(const std::string& name) const {
    std::lock_guard<std::mutex> lock(connectionsMutex_);
    
    // Bağlantıyı bul
    auto it = connections_.find(name);
    if (it == connections_.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::unordered_map<std::string, std::shared_ptr<DatabaseConnection>> DatabaseManager::getConnections() const {
    std::lock_guard<std::mutex> lock(connectionsMutex_);
    return connections_;
}

std::shared_ptr<SQLQuery> DatabaseManager::createQuery(const std::string& connectionName, const std::string& queryText, QueryType queryType) {
    // Bağlantıyı al
    auto connection = getConnection(connectionName);
    if (!connection) {
        return nullptr;
    }
    
    // Sorguyu oluştur
    return std::make_shared<SQLQuery>(connection, queryText, queryType);
}

std::shared_ptr<DatabaseResult> DatabaseManager::executeQuery(std::shared_ptr<DatabaseQuery> query) {
    // Sorguyu kontrol et
    if (!query) {
        auto result = std::make_shared<SQLResult>(false, "Invalid query");
        return result;
    }
    
    // Bağlantıyı al
    auto connection = query->getConnection();
    if (!connection) {
        auto result = std::make_shared<SQLResult>(false, "Invalid connection");
        return result;
    }
    
    // Bağlantı durumunu kontrol et
    if (connection->getStatus() != ConnectionStatus::CONNECTED) {
        // Bağlantıyı aç
        if (!connection->open()) {
            auto result = std::make_shared<SQLResult>(false, "Failed to open connection: " + connection->getLastError());
            return result;
        }
    }
    
    // Sorgu türüne göre çalıştır
    if (auto sqlQuery = std::dynamic_pointer_cast<SQLQuery>(query)) {
        // Veritabanı türüne göre çalıştır
        auto params = connection->getParams();
        
        switch (params.type) {
            case DatabaseType::SQLITE:
                return executeSQLiteQuery(sqlQuery);
            case DatabaseType::MYSQL:
                return executeMySQLQuery(sqlQuery);
            case DatabaseType::POSTGRESQL:
                return executePostgreSQLQuery(sqlQuery);
            default:
                auto result = std::make_shared<SQLResult>(false, "Unsupported database type");
                return result;
        }
    } else if (auto mongoDBQuery = std::dynamic_pointer_cast<MongoDBQuery>(query)) {
        return executeMongoDBQueryInternal(mongoDBQuery);
    } else {
        auto result = std::make_shared<SQLResult>(false, "Unsupported query type");
        return result;
    }
}

std::shared_ptr<DatabaseResult> DatabaseManager::executeQuery(const std::string& connectionName, const std::string& queryText, QueryType queryType) {
    // Sorguyu oluştur
    auto query = createQuery(connectionName, queryText, queryType);
    if (!query) {
        auto result = std::make_shared<SQLResult>(false, "Failed to create query");
        return result;
    }
    
    // Sorguyu çalıştır
    return executeQuery(query);
}

} // namespace database
} // namespace alt_las
