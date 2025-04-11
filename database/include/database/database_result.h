#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <variant>
#include <any>

namespace alt_las {
namespace database {

/**
 * @brief Sonuç değeri
 */
using ResultValue = std::variant<int, double, std::string, bool, std::nullptr_t>;

/**
 * @brief Sonuç satırı
 */
using ResultRow = std::unordered_map<std::string, ResultValue>;

/**
 * @brief Veritabanı sonuç sınıfı
 * 
 * Bu sınıf, veritabanı sorgu sonuçlarını temsil eder.
 */
class DatabaseResult {
public:
    /**
     * @brief Yapıcı
     */
    DatabaseResult();

    /**
     * @brief Yıkıcı
     */
    virtual ~DatabaseResult();

    /**
     * @brief Sonuç satırlarını döndürür
     * 
     * @return Sonuç satırları
     */
    virtual std::vector<ResultRow> getRows() const = 0;

    /**
     * @brief Sonuç satır sayısını döndürür
     * 
     * @return Sonuç satır sayısı
     */
    virtual size_t getRowCount() const = 0;

    /**
     * @brief Sonuç sütun sayısını döndürür
     * 
     * @return Sonuç sütun sayısı
     */
    virtual size_t getColumnCount() const = 0;

    /**
     * @brief Sonuç sütun adlarını döndürür
     * 
     * @return Sonuç sütun adları
     */
    virtual std::vector<std::string> getColumnNames() const = 0;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param column Sütun indeksi
     * @return Sonuç değeri
     */
    virtual ResultValue getValue(size_t row, size_t column) const = 0;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param columnName Sütun adı
     * @return Sonuç değeri
     */
    virtual ResultValue getValue(size_t row, const std::string& columnName) const = 0;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param column Sütun indeksi
     * @param defaultValue Varsayılan değer
     * @return Sonuç değeri
     */
    template <typename T>
    T getValue(size_t row, size_t column, const T& defaultValue) const;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param columnName Sütun adı
     * @param defaultValue Varsayılan değer
     * @return Sonuç değeri
     */
    template <typename T>
    T getValue(size_t row, const std::string& columnName, const T& defaultValue) const;

    /**
     * @brief Sonuç satırını döndürür
     * 
     * @param row Satır indeksi
     * @return Sonuç satırı
     */
    virtual ResultRow getRow(size_t row) const = 0;

    /**
     * @brief Sonuç sütununu döndürür
     * 
     * @param column Sütun indeksi
     * @return Sonuç sütunu
     */
    virtual std::vector<ResultValue> getColumn(size_t column) const = 0;

    /**
     * @brief Sonuç sütununu döndürür
     * 
     * @param columnName Sütun adı
     * @return Sonuç sütunu
     */
    virtual std::vector<ResultValue> getColumn(const std::string& columnName) const = 0;

    /**
     * @brief Sonuç değerini string olarak döndürür
     * 
     * @param value Sonuç değeri
     * @return Sonuç değeri string'i
     */
    static std::string getValueString(const ResultValue& value);

    /**
     * @brief Sonuç başarılı mı?
     * 
     * @return Sonuç başarılıysa true, değilse false
     */
    virtual bool isSuccess() const = 0;

    /**
     * @brief Sonuç hatası döndürür
     * 
     * @return Sonuç hatası
     */
    virtual std::string getError() const = 0;

    /**
     * @brief Etkilenen satır sayısını döndürür
     * 
     * @return Etkilenen satır sayısı
     */
    virtual size_t getAffectedRows() const = 0;

    /**
     * @brief Son eklenen ID'yi döndürür
     * 
     * @return Son eklenen ID
     */
    virtual int64_t getLastInsertId() const = 0;

    /**
     * @brief Sonuç metnini döndürür
     * 
     * @return Sonuç metni
     */
    virtual std::string toString() const = 0;

    /**
     * @brief Sonuç JSON metnini döndürür
     * 
     * @return Sonuç JSON metni
     */
    virtual std::string toJson() const = 0;

    /**
     * @brief Sonuç CSV metnini döndürür
     * 
     * @return Sonuç CSV metni
     */
    virtual std::string toCsv() const = 0;

    /**
     * @brief Sonuç XML metnini döndürür
     * 
     * @return Sonuç XML metni
     */
    virtual std::string toXml() const = 0;
};

/**
 * @brief SQL sonuç sınıfı
 */
class SQLResult : public DatabaseResult {
public:
    /**
     * @brief Yapıcı
     */
    SQLResult();

    /**
     * @brief Parametreli yapıcı
     * 
     * @param success Başarılı mı?
     * @param error Hata
     */
    SQLResult(bool success, const std::string& error = "");

    /**
     * @brief Yıkıcı
     */
    ~SQLResult();

    /**
     * @brief Sonuç satırlarını döndürür
     * 
     * @return Sonuç satırları
     */
    std::vector<ResultRow> getRows() const override;

    /**
     * @brief Sonuç satır sayısını döndürür
     * 
     * @return Sonuç satır sayısı
     */
    size_t getRowCount() const override;

    /**
     * @brief Sonuç sütun sayısını döndürür
     * 
     * @return Sonuç sütun sayısı
     */
    size_t getColumnCount() const override;

    /**
     * @brief Sonuç sütun adlarını döndürür
     * 
     * @return Sonuç sütun adları
     */
    std::vector<std::string> getColumnNames() const override;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param column Sütun indeksi
     * @return Sonuç değeri
     */
    ResultValue getValue(size_t row, size_t column) const override;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param columnName Sütun adı
     * @return Sonuç değeri
     */
    ResultValue getValue(size_t row, const std::string& columnName) const override;

    /**
     * @brief Sonuç satırını döndürür
     * 
     * @param row Satır indeksi
     * @return Sonuç satırı
     */
    ResultRow getRow(size_t row) const override;

    /**
     * @brief Sonuç sütununu döndürür
     * 
     * @param column Sütun indeksi
     * @return Sonuç sütunu
     */
    std::vector<ResultValue> getColumn(size_t column) const override;

    /**
     * @brief Sonuç sütununu döndürür
     * 
     * @param columnName Sütun adı
     * @return Sonuç sütunu
     */
    std::vector<ResultValue> getColumn(const std::string& columnName) const override;

    /**
     * @brief Sonuç başarılı mı?
     * 
     * @return Sonuç başarılıysa true, değilse false
     */
    bool isSuccess() const override;

    /**
     * @brief Sonuç hatası döndürür
     * 
     * @return Sonuç hatası
     */
    std::string getError() const override;

    /**
     * @brief Etkilenen satır sayısını döndürür
     * 
     * @return Etkilenen satır sayısı
     */
    size_t getAffectedRows() const override;

    /**
     * @brief Son eklenen ID'yi döndürür
     * 
     * @return Son eklenen ID
     */
    int64_t getLastInsertId() const override;

    /**
     * @brief Sonuç metnini döndürür
     * 
     * @return Sonuç metni
     */
    std::string toString() const override;

    /**
     * @brief Sonuç JSON metnini döndürür
     * 
     * @return Sonuç JSON metni
     */
    std::string toJson() const override;

    /**
     * @brief Sonuç CSV metnini döndürür
     * 
     * @return Sonuç CSV metni
     */
    std::string toCsv() const override;

    /**
     * @brief Sonuç XML metnini döndürür
     * 
     * @return Sonuç XML metni
     */
    std::string toXml() const override;

    /**
     * @brief Sonuç satırı ekler
     * 
     * @param row Sonuç satırı
     */
    void addRow(const ResultRow& row);

    /**
     * @brief Sonuç satırlarını ayarlar
     * 
     * @param rows Sonuç satırları
     */
    void setRows(const std::vector<ResultRow>& rows);

    /**
     * @brief Sonuç sütun adlarını ayarlar
     * 
     * @param columnNames Sonuç sütun adları
     */
    void setColumnNames(const std::vector<std::string>& columnNames);

    /**
     * @brief Sonuç başarı durumunu ayarlar
     * 
     * @param success Başarı durumu
     */
    void setSuccess(bool success);

    /**
     * @brief Sonuç hatasını ayarlar
     * 
     * @param error Hata
     */
    void setError(const std::string& error);

    /**
     * @brief Etkilenen satır sayısını ayarlar
     * 
     * @param affectedRows Etkilenen satır sayısı
     */
    void setAffectedRows(size_t affectedRows);

    /**
     * @brief Son eklenen ID'yi ayarlar
     * 
     * @param lastInsertId Son eklenen ID
     */
    void setLastInsertId(int64_t lastInsertId);

private:
    std::vector<ResultRow> rows_; // Sonuç satırları
    mutable std::mutex rowsMutex_; // Satırlar mutex'i
    std::vector<std::string> columnNames_; // Sütun adları
    mutable std::mutex columnNamesMutex_; // Sütun adları mutex'i
    bool success_; // Başarılı mı?
    mutable std::mutex successMutex_; // Başarı mutex'i
    std::string error_; // Hata
    mutable std::mutex errorMutex_; // Hata mutex'i
    size_t affectedRows_; // Etkilenen satır sayısı
    mutable std::mutex affectedRowsMutex_; // Etkilenen satır sayısı mutex'i
    int64_t lastInsertId_; // Son eklenen ID
    mutable std::mutex lastInsertIdMutex_; // Son eklenen ID mutex'i
};

/**
 * @brief MongoDB sonuç sınıfı
 */
class MongoDBResult : public DatabaseResult {
public:
    /**
     * @brief Yapıcı
     */
    MongoDBResult();

    /**
     * @brief Parametreli yapıcı
     * 
     * @param success Başarılı mı?
     * @param error Hata
     */
    MongoDBResult(bool success, const std::string& error = "");

    /**
     * @brief Yıkıcı
     */
    ~MongoDBResult();

    /**
     * @brief Sonuç satırlarını döndürür
     * 
     * @return Sonuç satırları
     */
    std::vector<ResultRow> getRows() const override;

    /**
     * @brief Sonuç satır sayısını döndürür
     * 
     * @return Sonuç satır sayısı
     */
    size_t getRowCount() const override;

    /**
     * @brief Sonuç sütun sayısını döndürür
     * 
     * @return Sonuç sütun sayısı
     */
    size_t getColumnCount() const override;

    /**
     * @brief Sonuç sütun adlarını döndürür
     * 
     * @return Sonuç sütun adları
     */
    std::vector<std::string> getColumnNames() const override;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param column Sütun indeksi
     * @return Sonuç değeri
     */
    ResultValue getValue(size_t row, size_t column) const override;

    /**
     * @brief Sonuç değerini döndürür
     * 
     * @param row Satır indeksi
     * @param columnName Sütun adı
     * @return Sonuç değeri
     */
    ResultValue getValue(size_t row, const std::string& columnName) const override;

    /**
     * @brief Sonuç satırını döndürür
     * 
     * @param row Satır indeksi
     * @return Sonuç satırı
     */
    ResultRow getRow(size_t row) const override;

    /**
     * @brief Sonuç sütununu döndürür
     * 
     * @param column Sütun indeksi
     * @return Sonuç sütunu
     */
    std::vector<ResultValue> getColumn(size_t column) const override;

    /**
     * @brief Sonuç sütununu döndürür
     * 
     * @param columnName Sütun adı
     * @return Sonuç sütunu
     */
    std::vector<ResultValue> getColumn(const std::string& columnName) const override;

    /**
     * @brief Sonuç başarılı mı?
     * 
     * @return Sonuç başarılıysa true, değilse false
     */
    bool isSuccess() const override;

    /**
     * @brief Sonuç hatası döndürür
     * 
     * @return Sonuç hatası
     */
    std::string getError() const override;

    /**
     * @brief Etkilenen satır sayısını döndürür
     * 
     * @return Etkilenen satır sayısı
     */
    size_t getAffectedRows() const override;

    /**
     * @brief Son eklenen ID'yi döndürür
     * 
     * @return Son eklenen ID
     */
    int64_t getLastInsertId() const override;

    /**
     * @brief Sonuç metnini döndürür
     * 
     * @return Sonuç metni
     */
    std::string toString() const override;

    /**
     * @brief Sonuç JSON metnini döndürür
     * 
     * @return Sonuç JSON metni
     */
    std::string toJson() const override;

    /**
     * @brief Sonuç CSV metnini döndürür
     * 
     * @return Sonuç CSV metni
     */
    std::string toCsv() const override;

    /**
     * @brief Sonuç XML metnini döndürür
     * 
     * @return Sonuç XML metni
     */
    std::string toXml() const override;

    /**
     * @brief Belge ekler
     * 
     * @param document Belge
     */
    void addDocument(const ResultRow& document);

    /**
     * @brief Belgeleri ayarlar
     * 
     * @param documents Belgeler
     */
    void setDocuments(const std::vector<ResultRow>& documents);

    /**
     * @brief Sonuç başarı durumunu ayarlar
     * 
     * @param success Başarı durumu
     */
    void setSuccess(bool success);

    /**
     * @brief Sonuç hatasını ayarlar
     * 
     * @param error Hata
     */
    void setError(const std::string& error);

    /**
     * @brief Etkilenen belge sayısını ayarlar
     * 
     * @param affectedDocuments Etkilenen belge sayısı
     */
    void setAffectedDocuments(size_t affectedDocuments);

    /**
     * @brief Son eklenen ID'yi ayarlar
     * 
     * @param lastInsertId Son eklenen ID
     */
    void setLastInsertId(int64_t lastInsertId);

private:
    std::vector<ResultRow> documents_; // Belgeler
    mutable std::mutex documentsMutex_; // Belgeler mutex'i
    bool success_; // Başarılı mı?
    mutable std::mutex successMutex_; // Başarı mutex'i
    std::string error_; // Hata
    mutable std::mutex errorMutex_; // Hata mutex'i
    size_t affectedDocuments_; // Etkilenen belge sayısı
    mutable std::mutex affectedDocumentsMutex_; // Etkilenen belge sayısı mutex'i
    int64_t lastInsertId_; // Son eklenen ID
    mutable std::mutex lastInsertIdMutex_; // Son eklenen ID mutex'i
};

template <typename T>
T DatabaseResult::getValue(size_t row, size_t column, const T& defaultValue) const {
    try {
        ResultValue value = getValue(row, column);
        
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
    } catch (const std::exception& e) {
        // Hata durumunda varsayılan değeri döndür
    }
    
    return defaultValue;
}

template <typename T>
T DatabaseResult::getValue(size_t row, const std::string& columnName, const T& defaultValue) const {
    try {
        ResultValue value = getValue(row, columnName);
        
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
    } catch (const std::exception& e) {
        // Hata durumunda varsayılan değeri döndür
    }
    
    return defaultValue;
}

} // namespace database
} // namespace alt_las
