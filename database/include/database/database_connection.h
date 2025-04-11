#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <vector>

namespace alt_las {
namespace database {

/**
 * @brief Veritabanı türü
 */
enum class DatabaseType {
    SQLITE,     // SQLite veritabanı
    MYSQL,      // MySQL veritabanı
    POSTGRESQL, // PostgreSQL veritabanı
    MONGODB,    // MongoDB veritabanı
    CUSTOM      // Özel veritabanı
};

/**
 * @brief Veritabanı bağlantı durumu
 */
enum class ConnectionStatus {
    DISCONNECTED, // Bağlantı kesildi
    CONNECTING,   // Bağlanıyor
    CONNECTED,    // Bağlandı
    ERROR         // Hata
};

/**
 * @brief Veritabanı bağlantı parametreleri
 */
struct ConnectionParams {
    DatabaseType type;      // Veritabanı türü
    std::string host;       // Sunucu adresi
    int port;               // Sunucu portu
    std::string database;   // Veritabanı adı
    std::string username;   // Kullanıcı adı
    std::string password;   // Parola
    std::string options;    // Ek seçenekler
};

/**
 * @brief Veritabanı bağlantı sınıfı
 * 
 * Bu sınıf, veritabanı bağlantı işlemlerini temsil eder.
 */
class DatabaseConnection {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param params Bağlantı parametreleri
     */
    DatabaseConnection(const ConnectionParams& params);

    /**
     * @brief Yıkıcı
     */
    virtual ~DatabaseConnection();

    /**
     * @brief Bağlantı açar
     * 
     * @return Bağlantı başarılıysa true, değilse false
     */
    virtual bool open() = 0;

    /**
     * @brief Bağlantı kapatır
     * 
     * @return Kapatma başarılıysa true, değilse false
     */
    virtual bool close() = 0;

    /**
     * @brief Bağlantı durumunu döndürür
     * 
     * @return Bağlantı durumu
     */
    virtual ConnectionStatus getStatus() const = 0;

    /**
     * @brief Bağlantı parametrelerini döndürür
     * 
     * @return Bağlantı parametreleri
     */
    ConnectionParams getParams() const;

    /**
     * @brief Bağlantı parametrelerini ayarlar
     * 
     * @param params Bağlantı parametreleri
     */
    void setParams(const ConnectionParams& params);

    /**
     * @brief Bağlantı durumunu string olarak döndürür
     * 
     * @param status Bağlantı durumu
     * @return Bağlantı durumu string'i
     */
    static std::string getStatusString(ConnectionStatus status);

    /**
     * @brief Veritabanı türünü string olarak döndürür
     * 
     * @param type Veritabanı türü
     * @return Veritabanı türü string'i
     */
    static std::string getTypeString(DatabaseType type);

    /**
     * @brief Bağlantı URL'sini oluşturur
     * 
     * @return Bağlantı URL'si
     */
    virtual std::string createConnectionUrl() const = 0;

    /**
     * @brief Bağlantı bilgilerini döndürür
     * 
     * @return Bağlantı bilgileri
     */
    virtual std::string getConnectionInfo() const = 0;

    /**
     * @brief Bağlantı hatası döndürür
     * 
     * @return Bağlantı hatası
     */
    virtual std::string getLastError() const = 0;

protected:
    ConnectionParams params_; // Bağlantı parametreleri
    mutable std::mutex paramsMutex_; // Parametreler mutex'i
};

/**
 * @brief SQLite veritabanı bağlantı sınıfı
 */
class SQLiteConnection : public DatabaseConnection {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param params Bağlantı parametreleri
     */
    SQLiteConnection(const ConnectionParams& params);

    /**
     * @brief Yıkıcı
     */
    ~SQLiteConnection();

    /**
     * @brief Bağlantı açar
     * 
     * @return Bağlantı başarılıysa true, değilse false
     */
    bool open() override;

    /**
     * @brief Bağlantı kapatır
     * 
     * @return Kapatma başarılıysa true, değilse false
     */
    bool close() override;

    /**
     * @brief Bağlantı durumunu döndürür
     * 
     * @return Bağlantı durumu
     */
    ConnectionStatus getStatus() const override;

    /**
     * @brief Bağlantı URL'sini oluşturur
     * 
     * @return Bağlantı URL'si
     */
    std::string createConnectionUrl() const override;

    /**
     * @brief Bağlantı bilgilerini döndürür
     * 
     * @return Bağlantı bilgileri
     */
    std::string getConnectionInfo() const override;

    /**
     * @brief Bağlantı hatası döndürür
     * 
     * @return Bağlantı hatası
     */
    std::string getLastError() const override;

private:
    void* db_; // SQLite veritabanı işaretçisi
    mutable std::mutex dbMutex_; // Veritabanı mutex'i
    ConnectionStatus status_; // Bağlantı durumu
    mutable std::mutex statusMutex_; // Durum mutex'i
    std::string lastError_; // Son hata
    mutable std::mutex errorMutex_; // Hata mutex'i
};

/**
 * @brief MySQL veritabanı bağlantı sınıfı
 */
class MySQLConnection : public DatabaseConnection {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param params Bağlantı parametreleri
     */
    MySQLConnection(const ConnectionParams& params);

    /**
     * @brief Yıkıcı
     */
    ~MySQLConnection();

    /**
     * @brief Bağlantı açar
     * 
     * @return Bağlantı başarılıysa true, değilse false
     */
    bool open() override;

    /**
     * @brief Bağlantı kapatır
     * 
     * @return Kapatma başarılıysa true, değilse false
     */
    bool close() override;

    /**
     * @brief Bağlantı durumunu döndürür
     * 
     * @return Bağlantı durumu
     */
    ConnectionStatus getStatus() const override;

    /**
     * @brief Bağlantı URL'sini oluşturur
     * 
     * @return Bağlantı URL'si
     */
    std::string createConnectionUrl() const override;

    /**
     * @brief Bağlantı bilgilerini döndürür
     * 
     * @return Bağlantı bilgileri
     */
    std::string getConnectionInfo() const override;

    /**
     * @brief Bağlantı hatası döndürür
     * 
     * @return Bağlantı hatası
     */
    std::string getLastError() const override;

private:
    void* mysql_; // MySQL veritabanı işaretçisi
    mutable std::mutex mysqlMutex_; // Veritabanı mutex'i
    ConnectionStatus status_; // Bağlantı durumu
    mutable std::mutex statusMutex_; // Durum mutex'i
    std::string lastError_; // Son hata
    mutable std::mutex errorMutex_; // Hata mutex'i
};

/**
 * @brief PostgreSQL veritabanı bağlantı sınıfı
 */
class PostgreSQLConnection : public DatabaseConnection {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param params Bağlantı parametreleri
     */
    PostgreSQLConnection(const ConnectionParams& params);

    /**
     * @brief Yıkıcı
     */
    ~PostgreSQLConnection();

    /**
     * @brief Bağlantı açar
     * 
     * @return Bağlantı başarılıysa true, değilse false
     */
    bool open() override;

    /**
     * @brief Bağlantı kapatır
     * 
     * @return Kapatma başarılıysa true, değilse false
     */
    bool close() override;

    /**
     * @brief Bağlantı durumunu döndürür
     * 
     * @return Bağlantı durumu
     */
    ConnectionStatus getStatus() const override;

    /**
     * @brief Bağlantı URL'sini oluşturur
     * 
     * @return Bağlantı URL'si
     */
    std::string createConnectionUrl() const override;

    /**
     * @brief Bağlantı bilgilerini döndürür
     * 
     * @return Bağlantı bilgileri
     */
    std::string getConnectionInfo() const override;

    /**
     * @brief Bağlantı hatası döndürür
     * 
     * @return Bağlantı hatası
     */
    std::string getLastError() const override;

private:
    void* pg_; // PostgreSQL veritabanı işaretçisi
    mutable std::mutex pgMutex_; // Veritabanı mutex'i
    ConnectionStatus status_; // Bağlantı durumu
    mutable std::mutex statusMutex_; // Durum mutex'i
    std::string lastError_; // Son hata
    mutable std::mutex errorMutex_; // Hata mutex'i
};

/**
 * @brief MongoDB veritabanı bağlantı sınıfı
 */
class MongoDBConnection : public DatabaseConnection {
public:
    /**
     * @brief Parametreli yapıcı
     * 
     * @param params Bağlantı parametreleri
     */
    MongoDBConnection(const ConnectionParams& params);

    /**
     * @brief Yıkıcı
     */
    ~MongoDBConnection();

    /**
     * @brief Bağlantı açar
     * 
     * @return Bağlantı başarılıysa true, değilse false
     */
    bool open() override;

    /**
     * @brief Bağlantı kapatır
     * 
     * @return Kapatma başarılıysa true, değilse false
     */
    bool close() override;

    /**
     * @brief Bağlantı durumunu döndürür
     * 
     * @return Bağlantı durumu
     */
    ConnectionStatus getStatus() const override;

    /**
     * @brief Bağlantı URL'sini oluşturur
     * 
     * @return Bağlantı URL'si
     */
    std::string createConnectionUrl() const override;

    /**
     * @brief Bağlantı bilgilerini döndürür
     * 
     * @return Bağlantı bilgileri
     */
    std::string getConnectionInfo() const override;

    /**
     * @brief Bağlantı hatası döndürür
     * 
     * @return Bağlantı hatası
     */
    std::string getLastError() const override;

private:
    void* client_; // MongoDB istemci işaretçisi
    mutable std::mutex clientMutex_; // İstemci mutex'i
    ConnectionStatus status_; // Bağlantı durumu
    mutable std::mutex statusMutex_; // Durum mutex'i
    std::string lastError_; // Son hata
    mutable std::mutex errorMutex_; // Hata mutex'i
};

/**
 * @brief Veritabanı bağlantı fabrikası
 */
class DatabaseConnectionFactory {
public:
    /**
     * @brief Veritabanı bağlantısı oluşturur
     * 
     * @param params Bağlantı parametreleri
     * @return Veritabanı bağlantısı
     */
    static std::shared_ptr<DatabaseConnection> createConnection(const ConnectionParams& params);

    /**
     * @brief Özel bağlantı oluşturucu ekler
     * 
     * @param type Veritabanı türü
     * @param creator Bağlantı oluşturucu fonksiyonu
     */
    static void addConnectionCreator(DatabaseType type, std::function<std::shared_ptr<DatabaseConnection>(const ConnectionParams&)> creator);

    /**
     * @brief Özel bağlantı oluşturucu siler
     * 
     * @param type Veritabanı türü
     * @return Silme başarılıysa true, değilse false
     */
    static bool removeConnectionCreator(DatabaseType type);

    /**
     * @brief Özel bağlantı oluşturucu varlığını kontrol eder
     * 
     * @param type Veritabanı türü
     * @return Bağlantı oluşturucu varsa true, yoksa false
     */
    static bool hasConnectionCreator(DatabaseType type);

private:
    static std::unordered_map<DatabaseType, std::function<std::shared_ptr<DatabaseConnection>(const ConnectionParams&)>> connectionCreators_; // Bağlantı oluşturucular
    static std::mutex creatorsMutex_; // Oluşturucular mutex'i
};

} // namespace database
} // namespace alt_las
