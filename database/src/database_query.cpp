#include "database/database_query.h"
#include <iostream>
#include <sstream>

namespace alt_las {
namespace database {

// DatabaseQuery sınıfı
DatabaseQuery::DatabaseQuery(std::shared_ptr<DatabaseConnection> connection)
    : connection_(connection) {
}

DatabaseQuery::~DatabaseQuery() {
}

std::shared_ptr<DatabaseConnection> DatabaseQuery::getConnection() const {
    std::lock_guard<std::mutex> lock(connectionMutex_);
    return connection_;
}

void DatabaseQuery::setConnection(std::shared_ptr<DatabaseConnection> connection) {
    std::lock_guard<std::mutex> lock(connectionMutex_);
    connection_ = connection;
}

std::string DatabaseQuery::getTypeString(QueryType type) {
    switch (type) {
        case QueryType::SELECT:
            return "SELECT";
        case QueryType::INSERT:
            return "INSERT";
        case QueryType::UPDATE:
            return "UPDATE";
        case QueryType::DELETE:
            return "DELETE";
        case QueryType::CREATE:
            return "CREATE";
        case QueryType::DROP:
            return "DROP";
        case QueryType::ALTER:
            return "ALTER";
        case QueryType::CUSTOM:
            return "CUSTOM";
        default:
            return "UNKNOWN";
    }
}

std::string DatabaseQuery::getParamString(const QueryParam& param) {
    if (std::holds_alternative<int>(param)) {
        return std::to_string(std::get<int>(param));
    } else if (std::holds_alternative<double>(param)) {
        return std::to_string(std::get<double>(param));
    } else if (std::holds_alternative<std::string>(param)) {
        return "'" + std::get<std::string>(param) + "'";
    } else if (std::holds_alternative<bool>(param)) {
        return std::get<bool>(param) ? "TRUE" : "FALSE";
    } else if (std::holds_alternative<std::nullptr_t>(param)) {
        return "NULL";
    } else {
        return "UNKNOWN";
    }
}

// SQLQuery sınıfı
SQLQuery::SQLQuery(std::shared_ptr<DatabaseConnection> connection, const std::string& queryText, QueryType queryType)
    : DatabaseQuery(connection),
      queryText_(queryText),
      queryType_(queryType) {
}

SQLQuery::~SQLQuery() {
}

std::string SQLQuery::getQueryText() const {
    std::lock_guard<std::mutex> lock(queryTextMutex_);
    return queryText_;
}

QueryType SQLQuery::getQueryType() const {
    std::lock_guard<std::mutex> lock(queryTypeMutex_);
    return queryType_;
}

std::vector<QueryParam> SQLQuery::getQueryParams() const {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    return params_;
}

void SQLQuery::setQueryText(const std::string& queryText) {
    std::lock_guard<std::mutex> lock(queryTextMutex_);
    queryText_ = queryText;
}

void SQLQuery::setQueryType(QueryType queryType) {
    std::lock_guard<std::mutex> lock(queryTypeMutex_);
    queryType_ = queryType;
}

void SQLQuery::addParam(const QueryParam& param) {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_.push_back(param);
}

void SQLQuery::clearParams() {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_.clear();
}

void SQLQuery::setParams(const std::vector<QueryParam>& params) {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_ = params;
}

void SQLQuery::setQuery(const std::string& queryText, const std::vector<QueryParam>& params, QueryType queryType) {
    {
        std::lock_guard<std::mutex> lock(queryTextMutex_);
        queryText_ = queryText;
    }
    
    {
        std::lock_guard<std::mutex> lock(paramsMutex_);
        params_ = params;
    }
    
    {
        std::lock_guard<std::mutex> lock(queryTypeMutex_);
        queryType_ = queryType;
    }
}

std::string SQLQuery::prepareQuery() const {
    std::lock_guard<std::mutex> lockText(queryTextMutex_);
    std::lock_guard<std::mutex> lockParams(paramsMutex_);
    
    std::string query = queryText_;
    
    // Parametreleri yerleştir
    for (size_t i = 0; i < params_.size(); i++) {
        std::string placeholder = "?" + std::to_string(i + 1);
        std::string value = getParamString(params_[i]);
        
        size_t pos = query.find(placeholder);
        if (pos != std::string::npos) {
            query.replace(pos, placeholder.length(), value);
        }
    }
    
    return query;
}

// MongoDBQuery sınıfı
MongoDBQuery::MongoDBQuery(std::shared_ptr<DatabaseConnection> connection, const std::string& collection, QueryType queryType)
    : DatabaseQuery(connection),
      collection_(collection),
      queryType_(queryType),
      limit_(0),
      skip_(0) {
}

MongoDBQuery::~MongoDBQuery() {
}

std::string MongoDBQuery::getQueryText() const {
    std::stringstream ss;
    
    // Sorgu türüne göre sorgu metni oluştur
    switch (getQueryType()) {
        case QueryType::SELECT:
            ss << "db." << getCollection() << ".find(";
            
            // Filtreler
            {
                std::lock_guard<std::mutex> lock(filtersMutex_);
                if (!filters_.empty()) {
                    ss << "{";
                    
                    bool first = true;
                    for (const auto& pair : filters_) {
                        if (!first) {
                            ss << ", ";
                        }
                        
                        ss << "\"" << pair.first << "\": " << getParamString(pair.second);
                        first = false;
                    }
                    
                    ss << "}";
                } else {
                    ss << "{}";
                }
            }
            
            // Projeksiyonlar
            {
                std::lock_guard<std::mutex> lock(projectionsMutex_);
                if (!projections_.empty()) {
                    ss << ", {";
                    
                    bool first = true;
                    for (const auto& pair : projections_) {
                        if (!first) {
                            ss << ", ";
                        }
                        
                        ss << "\"" << pair.first << "\": " << (pair.second ? "1" : "0");
                        first = false;
                    }
                    
                    ss << "}";
                }
            }
            
            ss << ")";
            
            // Sıralamalar
            {
                std::lock_guard<std::mutex> lock(sortsMutex_);
                if (!sorts_.empty()) {
                    ss << ".sort({";
                    
                    bool first = true;
                    for (const auto& pair : sorts_) {
                        if (!first) {
                            ss << ", ";
                        }
                        
                        ss << "\"" << pair.first << "\": " << (pair.second ? "1" : "-1");
                        first = false;
                    }
                    
                    ss << "})";
                }
            }
            
            // Limit
            {
                std::lock_guard<std::mutex> lock(limitMutex_);
                if (limit_ > 0) {
                    ss << ".limit(" << limit_ << ")";
                }
            }
            
            // Skip
            {
                std::lock_guard<std::mutex> lock(skipMutex_);
                if (skip_ > 0) {
                    ss << ".skip(" << skip_ << ")";
                }
            }
            
            break;
        
        case QueryType::INSERT:
            ss << "db." << getCollection() << ".insert(";
            
            // Belgeler
            {
                std::lock_guard<std::mutex> lock(documentsMutex_);
                if (!documents_.empty()) {
                    ss << "{";
                    
                    bool first = true;
                    for (const auto& pair : documents_) {
                        if (!first) {
                            ss << ", ";
                        }
                        
                        ss << "\"" << pair.first << "\": " << getParamString(pair.second);
                        first = false;
                    }
                    
                    ss << "}";
                } else {
                    ss << "{}";
                }
            }
            
            ss << ")";
            break;
        
        case QueryType::UPDATE:
            ss << "db." << getCollection() << ".update(";
            
            // Filtreler
            {
                std::lock_guard<std::mutex> lock(filtersMutex_);
                if (!filters_.empty()) {
                    ss << "{";
                    
                    bool first = true;
                    for (const auto& pair : filters_) {
                        if (!first) {
                            ss << ", ";
                        }
                        
                        ss << "\"" << pair.first << "\": " << getParamString(pair.second);
                        first = false;
                    }
                    
                    ss << "}";
                } else {
                    ss << "{}";
                }
            }
            
            // Güncellemeler
            {
                std::lock_guard<std::mutex> lock(updatesMutex_);
                if (!updates_.empty()) {
                    ss << ", {$set: {";
                    
                    bool first = true;
                    for (const auto& pair : updates_) {
                        if (!first) {
                            ss << ", ";
                        }
                        
                        ss << "\"" << pair.first << "\": " << getParamString(pair.second);
                        first = false;
                    }
                    
                    ss << "}}";
                } else {
                    ss << ", {$set: {}}";
                }
            }
            
            ss << ")";
            break;
        
        case QueryType::DELETE:
            ss << "db." << getCollection() << ".remove(";
            
            // Filtreler
            {
                std::lock_guard<std::mutex> lock(filtersMutex_);
                if (!filters_.empty()) {
                    ss << "{";
                    
                    bool first = true;
                    for (const auto& pair : filters_) {
                        if (!first) {
                            ss << ", ";
                        }
                        
                        ss << "\"" << pair.first << "\": " << getParamString(pair.second);
                        first = false;
                    }
                    
                    ss << "}";
                } else {
                    ss << "{}";
                }
            }
            
            ss << ")";
            break;
        
        case QueryType::CREATE:
            ss << "db.createCollection(\"" << getCollection() << "\")";
            break;
        
        case QueryType::DROP:
            ss << "db." << getCollection() << ".drop()";
            break;
        
        default:
            ss << "UNKNOWN QUERY TYPE";
            break;
    }
    
    return ss.str();
}

QueryType MongoDBQuery::getQueryType() const {
    std::lock_guard<std::mutex> lock(queryTypeMutex_);
    return queryType_;
}

std::vector<QueryParam> MongoDBQuery::getQueryParams() const {
    std::vector<QueryParam> params;
    
    // Filtreler
    {
        std::lock_guard<std::mutex> lock(filtersMutex_);
        for (const auto& pair : filters_) {
            params.push_back(pair.second);
        }
    }
    
    // Güncellemeler
    {
        std::lock_guard<std::mutex> lock(updatesMutex_);
        for (const auto& pair : updates_) {
            params.push_back(pair.second);
        }
    }
    
    // Belgeler
    {
        std::lock_guard<std::mutex> lock(documentsMutex_);
        for (const auto& pair : documents_) {
            params.push_back(pair.second);
        }
    }
    
    return params;
}

std::string MongoDBQuery::getCollection() const {
    std::lock_guard<std::mutex> lock(collectionMutex_);
    return collection_;
}

void MongoDBQuery::setCollection(const std::string& collection) {
    std::lock_guard<std::mutex> lock(collectionMutex_);
    collection_ = collection;
}

void MongoDBQuery::setQueryType(QueryType queryType) {
    std::lock_guard<std::mutex> lock(queryTypeMutex_);
    queryType_ = queryType;
}

void MongoDBQuery::addFilter(const std::string& key, const QueryParam& value) {
    std::lock_guard<std::mutex> lock(filtersMutex_);
    filters_[key] = value;
}

void MongoDBQuery::clearFilters() {
    std::lock_guard<std::mutex> lock(filtersMutex_);
    filters_.clear();
}

std::unordered_map<std::string, QueryParam> MongoDBQuery::getFilters() const {
    std::lock_guard<std::mutex> lock(filtersMutex_);
    return filters_;
}

void MongoDBQuery::addProjection(const std::string& field, bool include) {
    std::lock_guard<std::mutex> lock(projectionsMutex_);
    projections_[field] = include;
}

void MongoDBQuery::clearProjections() {
    std::lock_guard<std::mutex> lock(projectionsMutex_);
    projections_.clear();
}

std::unordered_map<std::string, bool> MongoDBQuery::getProjections() const {
    std::lock_guard<std::mutex> lock(projectionsMutex_);
    return projections_;
}

void MongoDBQuery::addSort(const std::string& field, bool ascending) {
    std::lock_guard<std::mutex> lock(sortsMutex_);
    sorts_[field] = ascending;
}

void MongoDBQuery::clearSorts() {
    std::lock_guard<std::mutex> lock(sortsMutex_);
    sorts_.clear();
}

std::unordered_map<std::string, bool> MongoDBQuery::getSorts() const {
    std::lock_guard<std::mutex> lock(sortsMutex_);
    return sorts_;
}

void MongoDBQuery::setLimit(int limit) {
    std::lock_guard<std::mutex> lock(limitMutex_);
    limit_ = limit;
}

int MongoDBQuery::getLimit() const {
    std::lock_guard<std::mutex> lock(limitMutex_);
    return limit_;
}

void MongoDBQuery::setSkip(int skip) {
    std::lock_guard<std::mutex> lock(skipMutex_);
    skip_ = skip;
}

int MongoDBQuery::getSkip() const {
    std::lock_guard<std::mutex> lock(skipMutex_);
    return skip_;
}

void MongoDBQuery::addUpdate(const std::string& key, const QueryParam& value) {
    std::lock_guard<std::mutex> lock(updatesMutex_);
    updates_[key] = value;
}

void MongoDBQuery::clearUpdates() {
    std::lock_guard<std::mutex> lock(updatesMutex_);
    updates_.clear();
}

std::unordered_map<std::string, QueryParam> MongoDBQuery::getUpdates() const {
    std::lock_guard<std::mutex> lock(updatesMutex_);
    return updates_;
}

void MongoDBQuery::addDocument(const std::string& key, const QueryParam& value) {
    std::lock_guard<std::mutex> lock(documentsMutex_);
    documents_[key] = value;
}

void MongoDBQuery::clearDocuments() {
    std::lock_guard<std::mutex> lock(documentsMutex_);
    documents_.clear();
}

std::unordered_map<std::string, QueryParam> MongoDBQuery::getDocuments() const {
    std::lock_guard<std::mutex> lock(documentsMutex_);
    return documents_;
}

// QueryBuilder sınıfı
std::shared_ptr<SQLQuery> QueryBuilder::select(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns, const std::string& where, const std::string& orderBy, int limit, int offset) {
    std::stringstream ss;
    
    // SELECT kısmı
    ss << "SELECT ";
    
    // Sütunlar
    if (columns.empty()) {
        ss << "*";
    } else {
        for (size_t i = 0; i < columns.size(); i++) {
            if (i > 0) {
                ss << ", ";
            }
            
            ss << columns[i];
        }
    }
    
    // FROM kısmı
    ss << " FROM " << table;
    
    // WHERE kısmı
    if (!where.empty()) {
        ss << " WHERE " << where;
    }
    
    // ORDER BY kısmı
    if (!orderBy.empty()) {
        ss << " ORDER BY " << orderBy;
    }
    
    // LIMIT kısmı
    if (limit > 0) {
        ss << " LIMIT " << limit;
    }
    
    // OFFSET kısmı
    if (offset > 0) {
        ss << " OFFSET " << offset;
    }
    
    return std::make_shared<SQLQuery>(connection, ss.str(), QueryType::SELECT);
}

std::shared_ptr<SQLQuery> QueryBuilder::insert(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns, const std::vector<QueryParam>& values) {
    std::stringstream ss;
    
    // INSERT INTO kısmı
    ss << "INSERT INTO " << table;
    
    // Sütunlar
    if (!columns.empty()) {
        ss << " (";
        
        for (size_t i = 0; i < columns.size(); i++) {
            if (i > 0) {
                ss << ", ";
            }
            
            ss << columns[i];
        }
        
        ss << ")";
    }
    
    // VALUES kısmı
    ss << " VALUES (";
    
    for (size_t i = 0; i < values.size(); i++) {
        if (i > 0) {
            ss << ", ";
        }
        
        ss << "?" << (i + 1);
    }
    
    ss << ")";
    
    auto query = std::make_shared<SQLQuery>(connection, ss.str(), QueryType::INSERT);
    query->setParams(values);
    
    return query;
}

std::shared_ptr<SQLQuery> QueryBuilder::update(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns, const std::vector<QueryParam>& values, const std::string& where) {
    std::stringstream ss;
    
    // UPDATE kısmı
    ss << "UPDATE " << table << " SET ";
    
    // Sütunlar ve değerler
    for (size_t i = 0; i < columns.size(); i++) {
        if (i > 0) {
            ss << ", ";
        }
        
        ss << columns[i] << " = ?" << (i + 1);
    }
    
    // WHERE kısmı
    if (!where.empty()) {
        ss << " WHERE " << where;
    }
    
    auto query = std::make_shared<SQLQuery>(connection, ss.str(), QueryType::UPDATE);
    query->setParams(values);
    
    return query;
}

std::shared_ptr<SQLQuery> QueryBuilder::remove(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::string& where) {
    std::stringstream ss;
    
    // DELETE FROM kısmı
    ss << "DELETE FROM " << table;
    
    // WHERE kısmı
    if (!where.empty()) {
        ss << " WHERE " << where;
    }
    
    return std::make_shared<SQLQuery>(connection, ss.str(), QueryType::DELETE);
}

std::shared_ptr<SQLQuery> QueryBuilder::createTable(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns) {
    std::stringstream ss;
    
    // CREATE TABLE kısmı
    ss << "CREATE TABLE " << table << " (";
    
    // Sütunlar
    for (size_t i = 0; i < columns.size(); i++) {
        if (i > 0) {
            ss << ", ";
        }
        
        ss << columns[i];
    }
    
    ss << ")";
    
    return std::make_shared<SQLQuery>(connection, ss.str(), QueryType::CREATE);
}

std::shared_ptr<SQLQuery> QueryBuilder::dropTable(std::shared_ptr<DatabaseConnection> connection, const std::string& table) {
    std::stringstream ss;
    
    // DROP TABLE kısmı
    ss << "DROP TABLE " << table;
    
    return std::make_shared<SQLQuery>(connection, ss.str(), QueryType::DROP);
}

std::shared_ptr<SQLQuery> QueryBuilder::alterTable(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::string& action, const std::string& column, const std::string& definition) {
    std::stringstream ss;
    
    // ALTER TABLE kısmı
    ss << "ALTER TABLE " << table << " " << action << " " << column;
    
    // Sütun tanımı
    if (!definition.empty()) {
        ss << " " << definition;
    }
    
    return std::make_shared<SQLQuery>(connection, ss.str(), QueryType::ALTER);
}

std::shared_ptr<MongoDBQuery> QueryBuilder::createMongoDBQuery(std::shared_ptr<DatabaseConnection> connection, const std::string& collection, QueryType queryType) {
    return std::make_shared<MongoDBQuery>(connection, collection, queryType);
}

} // namespace database
} // namespace alt_las
