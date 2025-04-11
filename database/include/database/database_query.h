#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <variant>
#include "database/database_connection.h"

namespace alt_las {
namespace database {

/**
 * @brief Sorgu parametresi
 */
using QueryParam = std::variant<int, double, std::string, bool, std::nullptr_t>;

/**
 * @brief Sorgu türü
 */
enum class QueryType {
    SELECT, // Seçme sorgusu
    INSERT, // Ekleme sorgusu
    UPDATE, // Güncelleme sorgusu
    DELETE, // Silme sorgusu
    CREATE, // Oluşturma sorgusu
    DROP,   // Silme sorgusu
    ALTER,  // Değiştirme sorgusu
    CUSTOM  // Özel sorgu
};

/**
 * @brief Veritabanı sorgu sınıfı
 * 
 * Bu sınıf, veritabanı sorgu işlemlerini temsil eder.
 */
class DatabaseQuery {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param connection Veritabanı bağlantısı
     */
    DatabaseQuery(std::shared_ptr<DatabaseConnection> connection);

    /**
     * @brief Yıkıcı
     */
    virtual ~DatabaseQuery();

    /**
     * @brief Sorgu metnini döndürür
     * 
     * @return Sorgu metni
     */
    virtual std::string getQueryText() const = 0;

    /**
     * @brief Sorgu türünü döndürür
     * 
     * @return Sorgu türü
     */
    virtual QueryType getQueryType() const = 0;

    /**
     * @brief Sorgu parametrelerini döndürür
     * 
     * @return Sorgu parametreleri
     */
    virtual std::vector<QueryParam> getQueryParams() const = 0;

    /**
     * @brief Sorgu türünü string olarak döndürür
     * 
     * @param type Sorgu türü
     * @return Sorgu türü string'i
     */
    static std::string getTypeString(QueryType type);

    /**
     * @brief Sorgu parametresini string olarak döndürür
     * 
     * @param param Sorgu parametresi
     * @return Sorgu parametresi string'i
     */
    static std::string getParamString(const QueryParam& param);

    /**
     * @brief Veritabanı bağlantısını döndürür
     * 
     * @return Veritabanı bağlantısı
     */
    std::shared_ptr<DatabaseConnection> getConnection() const;

    /**
     * @brief Veritabanı bağlantısını ayarlar
     * 
     * @param connection Veritabanı bağlantısı
     */
    void setConnection(std::shared_ptr<DatabaseConnection> connection);

protected:
    std::shared_ptr<DatabaseConnection> connection_; // Veritabanı bağlantısı
    mutable std::mutex connectionMutex_; // Bağlantı mutex'i
};

/**
 * @brief SQL sorgu sınıfı
 */
class SQLQuery : public DatabaseQuery {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param connection Veritabanı bağlantısı
     * @param queryText Sorgu metni
     * @param queryType Sorgu türü
     */
    SQLQuery(std::shared_ptr<DatabaseConnection> connection, const std::string& queryText, QueryType queryType = QueryType::CUSTOM);

    /**
     * @brief Yıkıcı
     */
    ~SQLQuery();

    /**
     * @brief Sorgu metnini döndürür
     * 
     * @return Sorgu metni
     */
    std::string getQueryText() const override;

    /**
     * @brief Sorgu türünü döndürür
     * 
     * @return Sorgu türü
     */
    QueryType getQueryType() const override;

    /**
     * @brief Sorgu parametrelerini döndürür
     * 
     * @return Sorgu parametreleri
     */
    std::vector<QueryParam> getQueryParams() const override;

    /**
     * @brief Sorgu metnini ayarlar
     * 
     * @param queryText Sorgu metni
     */
    void setQueryText(const std::string& queryText);

    /**
     * @brief Sorgu türünü ayarlar
     * 
     * @param queryType Sorgu türü
     */
    void setQueryType(QueryType queryType);

    /**
     * @brief Sorgu parametresi ekler
     * 
     * @param param Sorgu parametresi
     */
    void addParam(const QueryParam& param);

    /**
     * @brief Sorgu parametrelerini temizler
     */
    void clearParams();

    /**
     * @brief Sorgu parametrelerini ayarlar
     * 
     * @param params Sorgu parametreleri
     */
    void setParams(const std::vector<QueryParam>& params);

    /**
     * @brief Sorgu metnini ve parametrelerini ayarlar
     * 
     * @param queryText Sorgu metni
     * @param params Sorgu parametreleri
     * @param queryType Sorgu türü
     */
    void setQuery(const std::string& queryText, const std::vector<QueryParam>& params, QueryType queryType = QueryType::CUSTOM);

    /**
     * @brief Sorgu metnini hazırlar
     * 
     * @return Hazırlanmış sorgu metni
     */
    std::string prepareQuery() const;

private:
    std::string queryText_; // Sorgu metni
    mutable std::mutex queryTextMutex_; // Sorgu metni mutex'i
    QueryType queryType_; // Sorgu türü
    mutable std::mutex queryTypeMutex_; // Sorgu türü mutex'i
    std::vector<QueryParam> params_; // Sorgu parametreleri
    mutable std::mutex paramsMutex_; // Parametreler mutex'i
};

/**
 * @brief MongoDB sorgu sınıfı
 */
class MongoDBQuery : public DatabaseQuery {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param connection Veritabanı bağlantısı
     * @param collection Koleksiyon adı
     * @param queryType Sorgu türü
     */
    MongoDBQuery(std::shared_ptr<DatabaseConnection> connection, const std::string& collection, QueryType queryType = QueryType::SELECT);

    /**
     * @brief Yıkıcı
     */
    ~MongoDBQuery();

    /**
     * @brief Sorgu metnini döndürür
     * 
     * @return Sorgu metni
     */
    std::string getQueryText() const override;

    /**
     * @brief Sorgu türünü döndürür
     * 
     * @return Sorgu türü
     */
    QueryType getQueryType() const override;

    /**
     * @brief Sorgu parametrelerini döndürür
     * 
     * @return Sorgu parametreleri
     */
    std::vector<QueryParam> getQueryParams() const override;

    /**
     * @brief Koleksiyon adını döndürür
     * 
     * @return Koleksiyon adı
     */
    std::string getCollection() const;

    /**
     * @brief Koleksiyon adını ayarlar
     * 
     * @param collection Koleksiyon adı
     */
    void setCollection(const std::string& collection);

    /**
     * @brief Sorgu türünü ayarlar
     * 
     * @param queryType Sorgu türü
     */
    void setQueryType(QueryType queryType);

    /**
     * @brief Filtre ekler
     * 
     * @param key Anahtar
     * @param value Değer
     */
    void addFilter(const std::string& key, const QueryParam& value);

    /**
     * @brief Filtreleri temizler
     */
    void clearFilters();

    /**
     * @brief Filtreleri döndürür
     * 
     * @return Filtreler
     */
    std::unordered_map<std::string, QueryParam> getFilters() const;

    /**
     * @brief Projeksiyon ekler
     * 
     * @param field Alan adı
     * @param include Dahil edilecek mi?
     */
    void addProjection(const std::string& field, bool include = true);

    /**
     * @brief Projeksiyonları temizler
     */
    void clearProjections();

    /**
     * @brief Projeksiyonları döndürür
     * 
     * @return Projeksiyonlar
     */
    std::unordered_map<std::string, bool> getProjections() const;

    /**
     * @brief Sıralama ekler
     * 
     * @param field Alan adı
     * @param ascending Artan sıralama mı?
     */
    void addSort(const std::string& field, bool ascending = true);

    /**
     * @brief Sıralamaları temizler
     */
    void clearSorts();

    /**
     * @brief Sıralamaları döndürür
     * 
     * @return Sıralamalar
     */
    std::unordered_map<std::string, bool> getSorts() const;

    /**
     * @brief Limit ayarlar
     * 
     * @param limit Limit
     */
    void setLimit(int limit);

    /**
     * @brief Limiti döndürür
     * 
     * @return Limit
     */
    int getLimit() const;

    /**
     * @brief Atlama sayısını ayarlar
     * 
     * @param skip Atlama sayısı
     */
    void setSkip(int skip);

    /**
     * @brief Atlama sayısını döndürür
     * 
     * @return Atlama sayısı
     */
    int getSkip() const;

    /**
     * @brief Güncelleme ekler
     * 
     * @param key Anahtar
     * @param value Değer
     */
    void addUpdate(const std::string& key, const QueryParam& value);

    /**
     * @brief Güncellemeleri temizler
     */
    void clearUpdates();

    /**
     * @brief Güncellemeleri döndürür
     * 
     * @return Güncellemeler
     */
    std::unordered_map<std::string, QueryParam> getUpdates() const;

    /**
     * @brief Belge ekler
     * 
     * @param key Anahtar
     * @param value Değer
     */
    void addDocument(const std::string& key, const QueryParam& value);

    /**
     * @brief Belgeleri temizler
     */
    void clearDocuments();

    /**
     * @brief Belgeleri döndürür
     * 
     * @return Belgeler
     */
    std::unordered_map<std::string, QueryParam> getDocuments() const;

private:
    std::string collection_; // Koleksiyon adı
    mutable std::mutex collectionMutex_; // Koleksiyon mutex'i
    QueryType queryType_; // Sorgu türü
    mutable std::mutex queryTypeMutex_; // Sorgu türü mutex'i
    std::unordered_map<std::string, QueryParam> filters_; // Filtreler
    mutable std::mutex filtersMutex_; // Filtreler mutex'i
    std::unordered_map<std::string, bool> projections_; // Projeksiyonlar
    mutable std::mutex projectionsMutex_; // Projeksiyonlar mutex'i
    std::unordered_map<std::string, bool> sorts_; // Sıralamalar
    mutable std::mutex sortsMutex_; // Sıralamalar mutex'i
    int limit_; // Limit
    mutable std::mutex limitMutex_; // Limit mutex'i
    int skip_; // Atlama sayısı
    mutable std::mutex skipMutex_; // Atlama mutex'i
    std::unordered_map<std::string, QueryParam> updates_; // Güncellemeler
    mutable std::mutex updatesMutex_; // Güncellemeler mutex'i
    std::unordered_map<std::string, QueryParam> documents_; // Belgeler
    mutable std::mutex documentsMutex_; // Belgeler mutex'i
};

/**
 * @brief Sorgu oluşturucu sınıfı
 */
class QueryBuilder {
public:
    /**
     * @brief SELECT sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param table Tablo adı
     * @param columns Sütun adları
     * @param where WHERE koşulu
     * @param orderBy ORDER BY koşulu
     * @param limit LIMIT değeri
     * @param offset OFFSET değeri
     * @return SQL sorgusu
     */
    static std::shared_ptr<SQLQuery> select(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns = {"*"}, const std::string& where = "", const std::string& orderBy = "", int limit = 0, int offset = 0);

    /**
     * @brief INSERT sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param table Tablo adı
     * @param columns Sütun adları
     * @param values Değerler
     * @return SQL sorgusu
     */
    static std::shared_ptr<SQLQuery> insert(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns, const std::vector<QueryParam>& values);

    /**
     * @brief UPDATE sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param table Tablo adı
     * @param columns Sütun adları
     * @param values Değerler
     * @param where WHERE koşulu
     * @return SQL sorgusu
     */
    static std::shared_ptr<SQLQuery> update(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns, const std::vector<QueryParam>& values, const std::string& where = "");

    /**
     * @brief DELETE sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param table Tablo adı
     * @param where WHERE koşulu
     * @return SQL sorgusu
     */
    static std::shared_ptr<SQLQuery> remove(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::string& where = "");

    /**
     * @brief CREATE TABLE sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param table Tablo adı
     * @param columns Sütun tanımları
     * @return SQL sorgusu
     */
    static std::shared_ptr<SQLQuery> createTable(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::vector<std::string>& columns);

    /**
     * @brief DROP TABLE sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param table Tablo adı
     * @return SQL sorgusu
     */
    static std::shared_ptr<SQLQuery> dropTable(std::shared_ptr<DatabaseConnection> connection, const std::string& table);

    /**
     * @brief ALTER TABLE sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param table Tablo adı
     * @param action Eylem
     * @param column Sütun adı
     * @param definition Sütun tanımı
     * @return SQL sorgusu
     */
    static std::shared_ptr<SQLQuery> alterTable(std::shared_ptr<DatabaseConnection> connection, const std::string& table, const std::string& action, const std::string& column, const std::string& definition = "");

    /**
     * @brief MongoDB sorgusu oluşturur
     * 
     * @param connection Veritabanı bağlantısı
     * @param collection Koleksiyon adı
     * @param queryType Sorgu türü
     * @return MongoDB sorgusu
     */
    static std::shared_ptr<MongoDBQuery> createMongoDBQuery(std::shared_ptr<DatabaseConnection> connection, const std::string& collection, QueryType queryType = QueryType::SELECT);
};

} // namespace database
} // namespace alt_las
