#include "data/data_cache.h"
#include <iostream>
#include <algorithm>

namespace alt_las {
namespace data {

DataCache& DataCache::getInstance() {
    static DataCache instance;
    return instance;
}

DataCache::DataCache()
    : capacity_(1000),
      defaultTtl_(std::chrono::seconds(3600)),
      cleanupInterval_(std::chrono::seconds(300)),
      autoCleanupRunning_(false) {
    
    // İstatistikleri sıfırla
    resetStats();
}

DataCache::~DataCache() {
    // Otomatik temizlemeyi durdur
    stopAutoCleanup();
}

bool DataCache::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Anahtarı kontrol et
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return false;
    }
    
    // Önbellekten sil
    cache_.erase(it);
    
    // İstatistikleri güncelle
    {
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_["remove"]++;
    }
    
    return true;
}

bool DataCache::has(const std::string& key) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Anahtarı kontrol et
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return false;
    }
    
    // Sona erme zamanını kontrol et
    if (it->second.expiresAt != std::chrono::system_clock::time_point::max() && it->second.expiresAt <= std::chrono::system_clock::now()) {
        // Süresi dolmuş girdiyi sil
        cache_.erase(it);
        
        // İstatistikleri güncelle
        {
            std::lock_guard<std::mutex> statsLock(statsMutex_);
            stats_["expired"]++;
        }
        
        return false;
    }
    
    return true;
}

void DataCache::clear() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Önbelleği temizle
    cache_.clear();
    
    // İstatistikleri güncelle
    {
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_["clear"]++;
    }
}

size_t DataCache::clearExpired() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    size_t count = 0;
    auto now = std::chrono::system_clock::now();
    
    // Süresi dolmuş girdileri temizle
    for (auto it = cache_.begin(); it != cache_.end();) {
        if (it->second.expiresAt != std::chrono::system_clock::time_point::max() && it->second.expiresAt <= now) {
            it = cache_.erase(it);
            count++;
        } else {
            ++it;
        }
    }
    
    // İstatistikleri güncelle
    {
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_["expired"] += count;
        stats_["clearExpired"]++;
    }
    
    return count;
}

size_t DataCache::clearByTags(const std::string& tags) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    size_t count = 0;
    
    // Etiketlere göre girdileri temizle
    for (auto it = cache_.begin(); it != cache_.end();) {
        if (it->second.tags.find(tags) != std::string::npos) {
            it = cache_.erase(it);
            count++;
        } else {
            ++it;
        }
    }
    
    // İstatistikleri güncelle
    {
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_["clearByTags"]++;
    }
    
    return count;
}

size_t DataCache::size() const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    return cache_.size();
}

size_t DataCache::capacity() const {
    return capacity_;
}

void DataCache::setCapacity(size_t capacity) {
    capacity_ = capacity;
    
    // Önbellek kapasitesini kontrol et
    checkCapacity();
}

std::chrono::seconds DataCache::getDefaultTtl() const {
    return defaultTtl_;
}

void DataCache::setDefaultTtl(std::chrono::seconds ttl) {
    defaultTtl_ = ttl;
}

std::chrono::seconds DataCache::getCleanupInterval() const {
    return cleanupInterval_;
}

void DataCache::setCleanupInterval(std::chrono::seconds interval) {
    cleanupInterval_ = interval;
}

bool DataCache::startAutoCleanup() {
    std::lock_guard<std::mutex> lock(autoCleanupMutex_);
    
    // Eğer otomatik temizleme zaten çalışıyorsa, false döndür
    if (isAutoCleanupRunning()) {
        return false;
    }
    
    // Otomatik temizleme bayrağını ayarla
    autoCleanupRunning_ = true;
    
    // Otomatik temizleme iş parçacığını başlat
    autoCleanupThread_ = std::thread([this]() {
        while (isAutoCleanupRunning()) {
            // Süresi dolmuş girdileri temizle
            clearExpired();
            
            // Temizleme aralığı kadar bekle
            std::this_thread::sleep_for(cleanupInterval_);
        }
    });
    
    return true;
}

bool DataCache::stopAutoCleanup() {
    std::lock_guard<std::mutex> lock(autoCleanupMutex_);
    
    // Eğer otomatik temizleme çalışmıyorsa, true döndür
    if (!isAutoCleanupRunning()) {
        return true;
    }
    
    // Otomatik temizleme bayrağını temizle
    autoCleanupRunning_ = false;
    
    // Otomatik temizleme iş parçacığını bekle
    if (autoCleanupThread_.joinable()) {
        autoCleanupThread_.join();
    }
    
    return true;
}

bool DataCache::isAutoCleanupRunning() const {
    return autoCleanupRunning_;
}

std::unordered_map<std::string, size_t> DataCache::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

void DataCache::resetStats() {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // İstatistikleri sıfırla
    stats_["hit"] = 0;
    stats_["miss"] = 0;
    stats_["set"] = 0;
    stats_["remove"] = 0;
    stats_["clear"] = 0;
    stats_["clearExpired"] = 0;
    stats_["clearByTags"] = 0;
    stats_["expired"] = 0;
    stats_["error"] = 0;
}

void DataCache::autoCleanup() {
    // Süresi dolmuş girdileri temizle
    clearExpired();
}

void DataCache::checkCapacity() {
    // Eğer önbellek kapasitesi aşılmışsa, en eski girdileri sil
    if (cache_.size() > capacity_) {
        // Girdileri oluşturulma zamanına göre sırala
        std::vector<std::pair<std::string, std::chrono::system_clock::time_point>> entries;
        
        for (const auto& pair : cache_) {
            entries.push_back(std::make_pair(pair.first, pair.second.createdAt));
        }
        
        // Oluşturulma zamanına göre sırala (en eski en başta)
        std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
        
        // Kapasiteyi aşan girdileri sil
        size_t removeCount = cache_.size() - capacity_;
        
        for (size_t i = 0; i < removeCount && i < entries.size(); i++) {
            cache_.erase(entries[i].first);
        }
        
        // İstatistikleri güncelle
        {
            std::lock_guard<std::mutex> statsLock(statsMutex_);
            stats_["capacityExceeded"]++;
        }
    }
}

} // namespace data
} // namespace alt_las
