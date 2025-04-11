#include "database/database_connection.h"
#include <iostream>
#include <sstream>

namespace alt_las {
namespace database {

// DatabaseConnectionFactory statik üyeleri
std::unordered_map<DatabaseType, std::function<std::shared_ptr<DatabaseConnection>(const ConnectionParams&)>> DatabaseConnectionFactory::connectionCreators_;
std::mutex DatabaseConnectionFactory::creatorsMutex_;

// DatabaseConnection sınıfı
DatabaseConnection::DatabaseConnection(const ConnectionParams& params)
    : params_(params) {
}

DatabaseConnection::~DatabaseConnection() {
}

ConnectionParams DatabaseConnection::getParams() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    return params_;
}

void DatabaseConnection::setParams(const ConnectionParams& params) {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_ = params;
}

std::string DatabaseConnection::getStatusString(ConnectionStatus status) {
    switch (status) {
        case ConnectionStatus::DISCONNECTED:
            return "Disconnected";
        case ConnectionStatus::CONNECTING:
            return "Connecting";
        case ConnectionStatus::CONNECTED:
            return "Connected";
        case ConnectionStatus::ERROR:
            return "Error";
        default:
            return "Unknown";
    }
}

std::string DatabaseConnection::getTypeString(DatabaseType type) {
    switch (type) {
        case DatabaseType::SQLITE:
            return "SQLite";
        case DatabaseType::MYSQL:
            return "MySQL";
        case DatabaseType::POSTGRESQL:
            return "PostgreSQL";
        case DatabaseType::MONGODB:
            return "MongoDB";
        case DatabaseType::CUSTOM:
            return "Custom";
        default:
            return "Unknown";
    }
}

// SQLiteConnection sınıfı
SQLiteConnection::SQLiteConnection(const ConnectionParams& params)
    : DatabaseConnection(params),
      db_(nullptr),
      status_(ConnectionStatus::DISCONNECTED) {
}

SQLiteConnection::~SQLiteConnection() {
    close();
}

bool SQLiteConnection::open() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTING;
    }
    
    // TODO: SQLite bağlantı kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTED;
    }
    
    return true;
}

bool SQLiteConnection::close() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    // Bağlantı durumunu kontrol et
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        if (status_ != ConnectionStatus::CONNECTED) {
            return true;
        }
    }
    
    // TODO: SQLite bağlantı kapatma kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::DISCONNECTED;
    }
    
    return true;
}

ConnectionStatus SQLiteConnection::getStatus() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    return status_;
}

std::string SQLiteConnection::createConnectionUrl() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // SQLite bağlantı URL'si oluştur
    std::stringstream ss;
    ss << "sqlite://" << params_.database;
    
    return ss.str();
}

std::string SQLiteConnection::getConnectionInfo() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // SQLite bağlantı bilgilerini oluştur
    std::stringstream ss;
    ss << "SQLite Database: " << params_.database;
    
    return ss.str();
}

std::string SQLiteConnection::getLastError() const {
    std::lock_guard<std::mutex> lock(errorMutex_);
    return lastError_;
}

// MySQLConnection sınıfı
MySQLConnection::MySQLConnection(const ConnectionParams& params)
    : DatabaseConnection(params),
      mysql_(nullptr),
      status_(ConnectionStatus::DISCONNECTED) {
}

MySQLConnection::~MySQLConnection() {
    close();
}

bool MySQLConnection::open() {
    std::lock_guard<std::mutex> lock(mysqlMutex_);
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTING;
    }
    
    // TODO: MySQL bağlantı kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTED;
    }
    
    return true;
}

bool MySQLConnection::close() {
    std::lock_guard<std::mutex> lock(mysqlMutex_);
    
    // Bağlantı durumunu kontrol et
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        if (status_ != ConnectionStatus::CONNECTED) {
            return true;
        }
    }
    
    // TODO: MySQL bağlantı kapatma kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::DISCONNECTED;
    }
    
    return true;
}

ConnectionStatus MySQLConnection::getStatus() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    return status_;
}

std::string MySQLConnection::createConnectionUrl() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // MySQL bağlantı URL'si oluştur
    std::stringstream ss;
    ss << "mysql://" << params_.username << ":" << params_.password << "@" << params_.host << ":" << params_.port << "/" << params_.database;
    
    return ss.str();
}

std::string MySQLConnection::getConnectionInfo() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // MySQL bağlantı bilgilerini oluştur
    std::stringstream ss;
    ss << "MySQL Host: " << params_.host << ":" << params_.port << ", Database: " << params_.database << ", Username: " << params_.username;
    
    return ss.str();
}

std::string MySQLConnection::getLastError() const {
    std::lock_guard<std::mutex> lock(errorMutex_);
    return lastError_;
}

// PostgreSQLConnection sınıfı
PostgreSQLConnection::PostgreSQLConnection(const ConnectionParams& params)
    : DatabaseConnection(params),
      pg_(nullptr),
      status_(ConnectionStatus::DISCONNECTED) {
}

PostgreSQLConnection::~PostgreSQLConnection() {
    close();
}

bool PostgreSQLConnection::open() {
    std::lock_guard<std::mutex> lock(pgMutex_);
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTING;
    }
    
    // TODO: PostgreSQL bağlantı kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTED;
    }
    
    return true;
}

bool PostgreSQLConnection::close() {
    std::lock_guard<std::mutex> lock(pgMutex_);
    
    // Bağlantı durumunu kontrol et
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        if (status_ != ConnectionStatus::CONNECTED) {
            return true;
        }
    }
    
    // TODO: PostgreSQL bağlantı kapatma kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::DISCONNECTED;
    }
    
    return true;
}

ConnectionStatus PostgreSQLConnection::getStatus() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    return status_;
}

std::string PostgreSQLConnection::createConnectionUrl() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // PostgreSQL bağlantı URL'si oluştur
    std::stringstream ss;
    ss << "postgresql://" << params_.username << ":" << params_.password << "@" << params_.host << ":" << params_.port << "/" << params_.database;
    
    return ss.str();
}

std::string PostgreSQLConnection::getConnectionInfo() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // PostgreSQL bağlantı bilgilerini oluştur
    std::stringstream ss;
    ss << "PostgreSQL Host: " << params_.host << ":" << params_.port << ", Database: " << params_.database << ", Username: " << params_.username;
    
    return ss.str();
}

std::string PostgreSQLConnection::getLastError() const {
    std::lock_guard<std::mutex> lock(errorMutex_);
    return lastError_;
}

// MongoDBConnection sınıfı
MongoDBConnection::MongoDBConnection(const ConnectionParams& params)
    : DatabaseConnection(params),
      client_(nullptr),
      status_(ConnectionStatus::DISCONNECTED) {
}

MongoDBConnection::~MongoDBConnection() {
    close();
}

bool MongoDBConnection::open() {
    std::lock_guard<std::mutex> lock(clientMutex_);
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTING;
    }
    
    // TODO: MongoDB bağlantı kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::CONNECTED;
    }
    
    return true;
}

bool MongoDBConnection::close() {
    std::lock_guard<std::mutex> lock(clientMutex_);
    
    // Bağlantı durumunu kontrol et
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        if (status_ != ConnectionStatus::CONNECTED) {
            return true;
        }
    }
    
    // TODO: MongoDB bağlantı kapatma kodu eklenecek
    
    // Bağlantı durumunu ayarla
    {
        std::lock_guard<std::mutex> statusLock(statusMutex_);
        status_ = ConnectionStatus::DISCONNECTED;
    }
    
    return true;
}

ConnectionStatus MongoDBConnection::getStatus() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    return status_;
}

std::string MongoDBConnection::createConnectionUrl() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // MongoDB bağlantı URL'si oluştur
    std::stringstream ss;
    ss << "mongodb://" << params_.username << ":" << params_.password << "@" << params_.host << ":" << params_.port << "/" << params_.database;
    
    return ss.str();
}

std::string MongoDBConnection::getConnectionInfo() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    
    // MongoDB bağlantı bilgilerini oluştur
    std::stringstream ss;
    ss << "MongoDB Host: " << params_.host << ":" << params_.port << ", Database: " << params_.database << ", Username: " << params_.username;
    
    return ss.str();
}

std::string MongoDBConnection::getLastError() const {
    std::lock_guard<std::mutex> lock(errorMutex_);
    return lastError_;
}

// DatabaseConnectionFactory sınıfı
std::shared_ptr<DatabaseConnection> DatabaseConnectionFactory::createConnection(const ConnectionParams& params) {
    // Özel bağlantı oluşturucu kontrol et
    {
        std::lock_guard<std::mutex> lock(creatorsMutex_);
        
        auto it = connectionCreators_.find(params.type);
        if (it != connectionCreators_.end()) {
            return it->second(params);
        }
    }
    
    // Varsayılan bağlantı oluşturucular
    switch (params.type) {
        case DatabaseType::SQLITE:
            return std::make_shared<SQLiteConnection>(params);
        case DatabaseType::MYSQL:
            return std::make_shared<MySQLConnection>(params);
        case DatabaseType::POSTGRESQL:
            return std::make_shared<PostgreSQLConnection>(params);
        case DatabaseType::MONGODB:
            return std::make_shared<MongoDBConnection>(params);
        default:
            return nullptr;
    }
}

void DatabaseConnectionFactory::addConnectionCreator(DatabaseType type, std::function<std::shared_ptr<DatabaseConnection>(const ConnectionParams&)> creator) {
    std::lock_guard<std::mutex> lock(creatorsMutex_);
    connectionCreators_[type] = creator;
}

bool DatabaseConnectionFactory::removeConnectionCreator(DatabaseType type) {
    std::lock_guard<std::mutex> lock(creatorsMutex_);
    
    auto it = connectionCreators_.find(type);
    if (it != connectionCreators_.end()) {
        connectionCreators_.erase(it);
        return true;
    }
    
    return false;
}

bool DatabaseConnectionFactory::hasConnectionCreator(DatabaseType type) {
    std::lock_guard<std::mutex> lock(creatorsMutex_);
    return connectionCreators_.find(type) != connectionCreators_.end();
}

} // namespace database
} // namespace alt_las
