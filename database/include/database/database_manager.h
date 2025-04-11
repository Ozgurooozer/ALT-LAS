#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include "database/database_connection.h"
#include "database/database_query.h"
#include "database/database_result.h"

namespace alt_las {
namespace database {

/**
 * @brief Veritabanı yönetim sınıfı
 * 
 * Bu sınıf, veritabanı yönetim işlemlerini temsil eder.
 */
class DatabaseManager {
public:
    /**
     * @brief Singleton örneğini döndürür
     * 
     * @return DatabaseManager örneği
     */
    static DatabaseManager& getInstance();

    /**
     * @brief Yıkıcı
     */
    ~DatabaseManager();

    /**
     * @brief Veritabanı bağlantısı oluşturur
     * 
     * @param params Bağlantı parametreleri
     * @return Veritabanı bağlantısı
     */
    std::shared_ptr<DatabaseConnection> createConnection(const ConnectionParams& params);

    /**
     * @brief Veritabanı bağlantısı ekler
     * 
     * @param name Bağlantı adı
     * @param connection Veritabanı bağlantısı
     * @return Ekleme başarılıysa true, değilse false
     */
    bool addConnection(const std::string& name, std::shared_ptr<DatabaseConnection> connection);

    /**
     * @brief Veritabanı bağlantısı siler
     * 
     * @param name Bağlantı adı
     * @return Silme başarılıysa true, değilse false
     */
    bool removeConnection(const std::string& name);

    /**
     * @brief Veritabanı bağlantısı varlığını kontrol eder
     * 
     * @param name Bağlantı adı
     * @return Bağlantı varsa true, yoksa false
     */
    bool hasConnection(const std::string& name) const;

    /**
     * @brief Veritabanı bağlantısını döndürür
     * 
     * @param name Bağlantı adı
     * @return Veritabanı bağlantısı
     */
    std::shared_ptr<DatabaseConnection> getConnection(const std::string& name) const;

    /**
     * @brief Tüm veritabanı bağlantılarını döndürür
     * 
     * @return Veritabanı bağlantıları
     */
    std::unordered_map<std::string, std::shared_ptr<DatabaseConnection>> getConnections() const;

    /**
     * @brief Veritabanı sorgusu oluşturur
     * 
     * @param connectionName Bağlantı adı
     * @param queryText Sorgu metni
     * @param queryType Sorgu türü
     * @return Veritabanı sorgusu
     */
    std::shared_ptr<SQLQuery> createQuery(const std::string& connectionName, const std::string& queryText, QueryType queryType = QueryType::CUSTOM);

    /**
     * @brief Veritabanı sorgusu çalıştırır
     * 
     * @param query Veritabanı sorgusu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> executeQuery(std::shared_ptr<DatabaseQuery> query);

    /**
     * @brief Veritabanı sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param queryText Sorgu metni
     * @param queryType Sorgu türü
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> executeQuery(const std::string& connectionName, const std::string& queryText, QueryType queryType = QueryType::CUSTOM);

    /**
     * @brief SELECT sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param table Tablo adı
     * @param columns Sütun adları
     * @param where WHERE koşulu
     * @param orderBy ORDER BY koşulu
     * @param limit LIMIT değeri
     * @param offset OFFSET değeri
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> select(const std::string& connectionName, const std::string& table, const std::vector<std::string>& columns = {"*"}, const std::string& where = "", const std::string& orderBy = "", int limit = 0, int offset = 0);

    /**
     * @brief INSERT sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param table Tablo adı
     * @param columns Sütun adları
     * @param values Değerler
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> insert(const std::string& connectionName, const std::string& table, const std::vector<std::string>& columns, const std::vector<QueryParam>& values);

    /**
     * @brief UPDATE sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param table Tablo adı
     * @param columns Sütun adları
     * @param values Değerler
     * @param where WHERE koşulu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> update(const std::string& connectionName, const std::string& table, const std::vector<std::string>& columns, const std::vector<QueryParam>& values, const std::string& where = "");

    /**
     * @brief DELETE sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param table Tablo adı
     * @param where WHERE koşulu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> remove(const std::string& connectionName, const std::string& table, const std::string& where = "");

    /**
     * @brief CREATE TABLE sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param table Tablo adı
     * @param columns Sütun tanımları
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> createTable(const std::string& connectionName, const std::string& table, const std::vector<std::string>& columns);

    /**
     * @brief DROP TABLE sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param table Tablo adı
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> dropTable(const std::string& connectionName, const std::string& table);

    /**
     * @brief ALTER TABLE sorgusu çalıştırır
     * 
     * @param connectionName Bağlantı adı
     * @param table Tablo adı
     * @param action Eylem
     * @param column Sütun adı
     * @param definition Sütun tanımı
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> alterTable(const std::string& connectionName, const std::string& table, const std::string& action, const std::string& column, const std::string& definition = "");

    /**
     * @brief MongoDB sorgusu oluşturur
     * 
     * @param connectionName Bağlantı adı
     * @param collection Koleksiyon adı
     * @param queryType Sorgu türü
     * @return MongoDB sorgusu
     */
    std::shared_ptr<MongoDBQuery> createMongoDBQuery(const std::string& connectionName, const std::string& collection, QueryType queryType = QueryType::SELECT);

    /**
     * @brief MongoDB sorgusu çalıştırır
     * 
     * @param query MongoDB sorgusu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> executeMongoDBQuery(std::shared_ptr<MongoDBQuery> query);

    /**
     * @brief Veritabanı yönetim raporunu oluşturur
     * 
     * @return Veritabanı yönetim raporu
     */
    std::string generateDatabaseReport() const;

    /**
     * @brief Veritabanı yönetim raporunu dosyaya kaydeder
     * 
     * @param filePath Dosya yolu
     * @return Kaydetme başarılıysa true, değilse false
     */
    bool saveDatabaseReport(const std::string& filePath) const;

    /**
     * @brief Veritabanı yönetim raporunu konsola yazdırır
     */
    void printDatabaseReport() const;

private:
    /**
     * @brief Yapıcı
     */
    DatabaseManager();

    /**
     * @brief Kopyalama yapıcısı (engellendi)
     */
    DatabaseManager(const DatabaseManager&) = delete;

    /**
     * @brief Atama operatörü (engellendi)
     */
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    /**
     * @brief SQLite veritabanı sorgusu çalıştırır
     * 
     * @param query Veritabanı sorgusu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> executeSQLiteQuery(std::shared_ptr<SQLQuery> query);

    /**
     * @brief MySQL veritabanı sorgusu çalıştırır
     * 
     * @param query Veritabanı sorgusu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> executeMySQLQuery(std::shared_ptr<SQLQuery> query);

    /**
     * @brief PostgreSQL veritabanı sorgusu çalıştırır
     * 
     * @param query Veritabanı sorgusu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> executePostgreSQLQuery(std::shared_ptr<SQLQuery> query);

    /**
     * @brief MongoDB veritabanı sorgusu çalıştırır
     * 
     * @param query Veritabanı sorgusu
     * @return Veritabanı sonucu
     */
    std::shared_ptr<DatabaseResult> executeMongoDBQueryInternal(std::shared_ptr<MongoDBQuery> query);

    std::unordered_map<std::string, std::shared_ptr<DatabaseConnection>> connections_; // Veritabanı bağlantıları
    mutable std::mutex connectionsMutex_; // Bağlantılar mutex'i
};

} // namespace database
} // namespace alt_las
