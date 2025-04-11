#include "database/database_result.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace alt_las {
namespace database {

// DatabaseResult sınıfı
DatabaseResult::DatabaseResult() {
}

DatabaseResult::~DatabaseResult() {
}

std::string DatabaseResult::getValueString(const ResultValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
        return "null";
    } else {
        return "unknown";
    }
}

// SQLResult sınıfı
SQLResult::SQLResult()
    : success_(false),
      affectedRows_(0),
      lastInsertId_(0) {
}

SQLResult::SQLResult(bool success, const std::string& error)
    : success_(success),
      error_(error),
      affectedRows_(0),
      lastInsertId_(0) {
}

SQLResult::~SQLResult() {
}

std::vector<ResultRow> SQLResult::getRows() const {
    std::lock_guard<std::mutex> lock(rowsMutex_);
    return rows_;
}

size_t SQLResult::getRowCount() const {
    std::lock_guard<std::mutex> lock(rowsMutex_);
    return rows_.size();
}

size_t SQLResult::getColumnCount() const {
    std::lock_guard<std::mutex> lock(columnNamesMutex_);
    return columnNames_.size();
}

std::vector<std::string> SQLResult::getColumnNames() const {
    std::lock_guard<std::mutex> lock(columnNamesMutex_);
    return columnNames_;
}

ResultValue SQLResult::getValue(size_t row, size_t column) const {
    std::lock_guard<std::mutex> lockRows(rowsMutex_);
    std::lock_guard<std::mutex> lockColumnNames(columnNamesMutex_);
    
    // Satır indeksini kontrol et
    if (row >= rows_.size()) {
        throw std::out_of_range("Row index out of range");
    }
    
    // Sütun indeksini kontrol et
    if (column >= columnNames_.size()) {
        throw std::out_of_range("Column index out of range");
    }
    
    // Sütun adını al
    std::string columnName = columnNames_[column];
    
    // Değeri al
    auto it = rows_[row].find(columnName);
    if (it != rows_[row].end()) {
        return it->second;
    }
    
    // Değer bulunamazsa, null döndür
    return std::nullptr_t();
}

ResultValue SQLResult::getValue(size_t row, const std::string& columnName) const {
    std::lock_guard<std::mutex> lock(rowsMutex_);
    
    // Satır indeksini kontrol et
    if (row >= rows_.size()) {
        throw std::out_of_range("Row index out of range");
    }
    
    // Değeri al
    auto it = rows_[row].find(columnName);
    if (it != rows_[row].end()) {
        return it->second;
    }
    
    // Değer bulunamazsa, null döndür
    return std::nullptr_t();
}

ResultRow SQLResult::getRow(size_t row) const {
    std::lock_guard<std::mutex> lock(rowsMutex_);
    
    // Satır indeksini kontrol et
    if (row >= rows_.size()) {
        throw std::out_of_range("Row index out of range");
    }
    
    return rows_[row];
}

std::vector<ResultValue> SQLResult::getColumn(size_t column) const {
    std::lock_guard<std::mutex> lockRows(rowsMutex_);
    std::lock_guard<std::mutex> lockColumnNames(columnNamesMutex_);
    
    // Sütun indeksini kontrol et
    if (column >= columnNames_.size()) {
        throw std::out_of_range("Column index out of range");
    }
    
    // Sütun adını al
    std::string columnName = columnNames_[column];
    
    // Sütun değerlerini al
    std::vector<ResultValue> columnValues;
    
    for (const auto& row : rows_) {
        auto it = row.find(columnName);
        if (it != row.end()) {
            columnValues.push_back(it->second);
        } else {
            columnValues.push_back(std::nullptr_t());
        }
    }
    
    return columnValues;
}

std::vector<ResultValue> SQLResult::getColumn(const std::string& columnName) const {
    std::lock_guard<std::mutex> lock(rowsMutex_);
    
    // Sütun değerlerini al
    std::vector<ResultValue> columnValues;
    
    for (const auto& row : rows_) {
        auto it = row.find(columnName);
        if (it != row.end()) {
            columnValues.push_back(it->second);
        } else {
            columnValues.push_back(std::nullptr_t());
        }
    }
    
    return columnValues;
}

bool SQLResult::isSuccess() const {
    std::lock_guard<std::mutex> lock(successMutex_);
    return success_;
}

std::string SQLResult::getError() const {
    std::lock_guard<std::mutex> lock(errorMutex_);
    return error_;
}

size_t SQLResult::getAffectedRows() const {
    std::lock_guard<std::mutex> lock(affectedRowsMutex_);
    return affectedRows_;
}

int64_t SQLResult::getLastInsertId() const {
    std::lock_guard<std::mutex> lock(lastInsertIdMutex_);
    return lastInsertId_;
}

} // namespace database
} // namespace alt_las
