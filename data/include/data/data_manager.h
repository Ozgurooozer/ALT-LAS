#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <nlohmann/json.hpp>
#include "data/data_serializer.h"
#include "data/data_validator.h"
#include "data/data_cache.h"

namespace alt_las {
namespace data {

/**
 * @brief Veri yönetim sınıfı
 * 
 * Bu sınıf, veri yönetim işlemlerini temsil eder.
 */
class DataManager {
public:
    /**
     * @brief Singleton örneğini döndürür
     * 
     * @return DataManager örneği
     */
    static DataManager& getInstance();

    /**
     * @brief Yıkıcı
     */
    ~DataManager();

    /**
     * @brief Veri serileştiricisini döndürür
     * 
     * @return Veri serileştiricisi
     */
    DataSerializer& getSerializer();

    /**
     * @brief Veri doğrulayıcısını döndürür
     * 
     * @return Veri doğrulayıcısı
     */
    DataValidator& getValidator();

    /**
     * @brief Veri önbelleğini döndürür
     * 
     * @return Veri önbelleği
     */
    DataCache& getCache();

    /**
     * @brief Veriyi kaydeder
     * 
     * @param key Anahtar
     * @param data Veri
     * @param format Serileştirme formatı
     * @return Kaydetme başarılıysa true, değilse false
     */
    template <typename T>
    bool saveData(const std::string& key, const T& data, SerializationFormat format = SerializationFormat::JSON);

    /**
     * @brief Veriyi yükler
     * 
     * @param key Anahtar
     * @param defaultValue Varsayılan değer
     * @param format Serileştirme formatı
     * @return Veri
     */
    template <typename T>
    T loadData(const std::string& key, const T& defaultValue = T(), SerializationFormat format = SerializationFormat::JSON);

    /**
     * @brief Veriyi siler
     * 
     * @param key Anahtar
     * @return Silme başarılıysa true, değilse false
     */
    bool removeData(const std::string& key);

    /**
     * @brief Veri varlığını kontrol eder
     * 
     * @param key Anahtar
     * @return Veri varsa true, yoksa false
     */
    bool hasData(const std::string& key);

    /**
     * @brief Veriyi doğrular
     * 
     * @param data Veri
     * @param schemaName Şema adı
     * @return Doğrulama sonucu
     */
    template <typename T>
    ValidationResult validateData(const T& data, const std::string& schemaName);

    /**
     * @brief Veriyi önbelleğe ekler
     * 
     * @param key Anahtar
     * @param data Veri
     * @param ttl Yaşam süresi (saniye)
     * @param tags Etiketler
     */
    template <typename T>
    void cacheData(const std::string& key, const T& data, std::chrono::seconds ttl = std::chrono::seconds(0), const std::string& tags = "");

    /**
     * @brief Önbellekten veri alır
     * 
     * @param key Anahtar
     * @param defaultValue Varsayılan değer
     * @return Veri
     */
    template <typename T>
    T getCachedData(const std::string& key, const T& defaultValue = T());

    /**
     * @brief Veri dizinini döndürür
     * 
     * @return Veri dizini
     */
    std::string getDataDirectory() const;

    /**
     * @brief Veri dizinini ayarlar
     * 
     * @param directory Veri dizini
     */
    void setDataDirectory(const std::string& directory);

    /**
     * @brief Veri uzantısını döndürür
     * 
     * @return Veri uzantısı
     */
    std::string getDataExtension() const;

    /**
     * @brief Veri uzantısını ayarlar
     * 
     * @param extension Veri uzantısı
     */
    void setDataExtension(const std::string& extension);

    /**
     * @brief Veri yolu oluşturur
     * 
     * @param key Anahtar
     * @return Veri yolu
     */
    std::string createDataPath(const std::string& key) const;

    /**
     * @brief Veri yönetim raporunu oluşturur
     * 
     * @return Veri yönetim raporu
     */
    std::string generateDataReport() const;

    /**
     * @brief Veri yönetim raporunu dosyaya kaydeder
     * 
     * @param filePath Dosya yolu
     * @return Kaydetme başarılıysa true, değilse false
     */
    bool saveDataReport(const std::string& filePath) const;

    /**
     * @brief Veri yönetim raporunu konsola yazdırır
     */
    void printDataReport() const;

private:
    /**
     * @brief Yapıcı
     */
    DataManager();

    /**
     * @brief Kopyalama yapıcısı (engellendi)
     */
    DataManager(const DataManager&) = delete;

    /**
     * @brief Atama operatörü (engellendi)
     */
    DataManager& operator=(const DataManager&) = delete;

    /**
     * @brief Veri dizinini oluşturur
     * 
     * @return Oluşturma başarılıysa true, değilse false
     */
    bool createDataDirectory() const;

    DataSerializer& serializer_; // Veri serileştiricisi
    DataValidator& validator_; // Veri doğrulayıcısı
    DataCache& cache_; // Veri önbelleği
    std::string dataDirectory_; // Veri dizini
    std::string dataExtension_; // Veri uzantısı
    mutable std::mutex directoryMutex_; // Dizin mutex'i
};

template <typename T>
bool DataManager::saveData(const std::string& key, const T& data, SerializationFormat format) {
    // Veri yolunu oluştur
    std::string filePath = createDataPath(key);
    
    // Veriyi serileştir ve dosyaya kaydet
    bool success = serializer_.serializeToFile(data, filePath, format);
    
    // Veriyi önbelleğe ekle
    if (success) {
        cacheData(key, data);
    }
    
    return success;
}

template <typename T>
T DataManager::loadData(const std::string& key, const T& defaultValue, SerializationFormat format) {
    // Önbellekten veri al
    if (cache_.has(key)) {
        return getCachedData(key, defaultValue);
    }
    
    // Veri yolunu oluştur
    std::string filePath = createDataPath(key);
    
    // Veriyi dosyadan deserileştir
    T data = serializer_.deserializeFromFile<T>(filePath, format);
    
    // Veriyi önbelleğe ekle
    cacheData(key, data);
    
    return data;
}

template <typename T>
ValidationResult DataManager::validateData(const T& data, const std::string& schemaName) {
    // Veriyi JSON formatına dönüştür
    nlohmann::json jsonData = data;
    
    // Veriyi doğrula
    return validator_.validate(jsonData, schemaName);
}

template <typename T>
void DataManager::cacheData(const std::string& key, const T& data, std::chrono::seconds ttl, const std::string& tags) {
    // Veriyi önbelleğe ekle
    cache_.set(key, data, ttl, tags);
}

template <typename T>
T DataManager::getCachedData(const std::string& key, const T& defaultValue) {
    // Önbellekten veri al
    return cache_.get(key, defaultValue);
}

} // namespace data
} // namespace alt_las
