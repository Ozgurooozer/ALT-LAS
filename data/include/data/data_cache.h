#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>
#include <any>

namespace alt_las {
namespace data {

/**
 * @brief Önbellek girdisi
 */
struct CacheEntry {
    std::any data;                                      // Önbellek verisi
    std::chrono::system_clock::time_point createdAt;    // Oluşturulma zamanı
    std::chrono::system_clock::time_point expiresAt;    // Sona erme zamanı
    std::chrono::seconds ttl;                           // Yaşam süresi
    std::string tags;                                   // Etiketler
};

/**
 * @brief Veri önbellekleme sınıfı
 * 
 * Bu sınıf, veri önbellekleme işlemlerini temsil eder.
 */
class DataCache {
public:
    /**
     * @brief Singleton örneğini döndürür
     * 
     * @return DataCache örneği
     */
    static DataCache& getInstance();

    /**
     * @brief Yıkıcı
     */
    ~DataCache();

    /**
     * @brief Önbelleğe veri ekler
     * 
     * @param key Anahtar
     * @param data Veri
     * @param ttl Yaşam süresi (saniye)
     * @param tags Etiketler
     */
    template <typename T>
    void set(const std::string& key, const T& data, std::chrono::seconds ttl = std::chrono::seconds(0), const std::string& tags = "");

    /**
     * @brief Önbellekten veri alır
     * 
     * @param key Anahtar
     * @param defaultValue Varsayılan değer
     * @return Veri
     */
    template <typename T>
    T get(const std::string& key, const T& defaultValue = T());

    /**
     * @brief Önbellekten veri siler
     * 
     * @param key Anahtar
     * @return Silme başarılıysa true, değilse false
     */
    bool remove(const std::string& key);

    /**
     * @brief Önbellekte anahtar varlığını kontrol eder
     * 
     * @param key Anahtar
     * @return Anahtar varsa true, yoksa false
     */
    bool has(const std::string& key);

    /**
     * @brief Önbelleği temizler
     */
    void clear();

    /**
     * @brief Süresi dolmuş girdileri temizler
     * 
     * @return Temizlenen girdi sayısı
     */
    size_t clearExpired();

    /**
     * @brief Etiketlere göre girdileri temizler
     * 
     * @param tags Etiketler
     * @return Temizlenen girdi sayısı
     */
    size_t clearByTags(const std::string& tags);

    /**
     * @brief Önbellek boyutunu döndürür
     * 
     * @return Önbellek boyutu
     */
    size_t size() const;

    /**
     * @brief Önbellek kapasitesini döndürür
     * 
     * @return Önbellek kapasitesi
     */
    size_t capacity() const;

    /**
     * @brief Önbellek kapasitesini ayarlar
     * 
     * @param capacity Önbellek kapasitesi
     */
    void setCapacity(size_t capacity);

    /**
     * @brief Varsayılan yaşam süresini döndürür
     * 
     * @return Varsayılan yaşam süresi
     */
    std::chrono::seconds getDefaultTtl() const;

    /**
     * @brief Varsayılan yaşam süresini ayarlar
     * 
     * @param ttl Varsayılan yaşam süresi
     */
    void setDefaultTtl(std::chrono::seconds ttl);

    /**
     * @brief Otomatik temizleme aralığını döndürür
     * 
     * @return Otomatik temizleme aralığı
     */
    std::chrono::seconds getCleanupInterval() const;

    /**
     * @brief Otomatik temizleme aralığını ayarlar
     * 
     * @param interval Otomatik temizleme aralığı
     */
    void setCleanupInterval(std::chrono::seconds interval);

    /**
     * @brief Otomatik temizlemeyi başlatır
     * 
     * @return Başlatma başarılıysa true, değilse false
     */
    bool startAutoCleanup();

    /**
     * @brief Otomatik temizlemeyi durdurur
     * 
     * @return Durdurma başarılıysa true, değilse false
     */
    bool stopAutoCleanup();

    /**
     * @brief Otomatik temizlemenin çalışıp çalışmadığını kontrol eder
     * 
     * @return Otomatik temizleme çalışıyorsa true, değilse false
     */
    bool isAutoCleanupRunning() const;

    /**
     * @brief Önbellek istatistiklerini döndürür
     * 
     * @return Önbellek istatistikleri
     */
    std::unordered_map<std::string, size_t> getStats() const;

    /**
     * @brief Önbellek istatistiklerini sıfırlar
     */
    void resetStats();

private:
    /**
     * @brief Yapıcı
     */
    DataCache();

    /**
     * @brief Kopyalama yapıcısı (engellendi)
     */
    DataCache(const DataCache&) = delete;

    /**
     * @brief Atama operatörü (engellendi)
     */
    DataCache& operator=(const DataCache&) = delete;

    /**
     * @brief Otomatik temizleme işlemi
     */
    void autoCleanup();

    /**
     * @brief Önbellek kapasitesini kontrol eder
     */
    void checkCapacity();

    std::unordered_map<std::string, CacheEntry> cache_; // Önbellek
    mutable std::mutex cacheMutex_; // Önbellek mutex'i
    size_t capacity_; // Önbellek kapasitesi
    std::chrono::seconds defaultTtl_; // Varsayılan yaşam süresi
    std::chrono::seconds cleanupInterval_; // Otomatik temizleme aralığı
    bool autoCleanupRunning_; // Otomatik temizleme çalışıyor mu?
    std::thread autoCleanupThread_; // Otomatik temizleme iş parçacığı
    mutable std::mutex autoCleanupMutex_; // Otomatik temizleme mutex'i
    std::unordered_map<std::string, size_t> stats_; // Önbellek istatistikleri
    mutable std::mutex statsMutex_; // İstatistikler mutex'i
};

template <typename T>
void DataCache::set(const std::string& key, const T& data, std::chrono::seconds ttl, const std::string& tags) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Yaşam süresini ayarla
    if (ttl.count() == 0) {
        ttl = defaultTtl_;
    }
    
    // Önbellek girdisi oluştur
    CacheEntry entry;
    entry.data = data;
    entry.createdAt = std::chrono::system_clock::now();
    entry.ttl = ttl;
    
    // Sona erme zamanını ayarla
    if (ttl.count() > 0) {
        entry.expiresAt = entry.createdAt + ttl;
    } else {
        entry.expiresAt = std::chrono::system_clock::time_point::max();
    }
    
    entry.tags = tags;
    
    // Önbelleğe ekle
    cache_[key] = entry;
    
    // Önbellek kapasitesini kontrol et
    checkCapacity();
    
    // İstatistikleri güncelle
    {
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_["set"]++;
    }
}

template <typename T>
T DataCache::get(const std::string& key, const T& defaultValue) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Anahtarı kontrol et
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        // İstatistikleri güncelle
        {
            std::lock_guard<std::mutex> statsLock(statsMutex_);
            stats_["miss"]++;
        }
        
        return defaultValue;
    }
    
    // Sona erme zamanını kontrol et
    if (it->second.expiresAt != std::chrono::system_clock::time_point::max() && it->second.expiresAt <= std::chrono::system_clock::now()) {
        // Süresi dolmuş girdiyi sil
        cache_.erase(it);
        
        // İstatistikleri güncelle
        {
            std::lock_guard<std::mutex> statsLock(statsMutex_);
            stats_["expired"]++;
            stats_["miss"]++;
        }
        
        return defaultValue;
    }
    
    // Veriyi döndür
    try {
        // İstatistikleri güncelle
        {
            std::lock_guard<std::mutex> statsLock(statsMutex_);
            stats_["hit"]++;
        }
        
        return std::any_cast<T>(it->second.data);
    } catch (const std::bad_any_cast& e) {
        // İstatistikleri güncelle
        {
            std::lock_guard<std::mutex> statsLock(statsMutex_);
            stats_["error"]++;
            stats_["miss"]++;
        }
        
        return defaultValue;
    }
}

} // namespace data
} // namespace alt_las
