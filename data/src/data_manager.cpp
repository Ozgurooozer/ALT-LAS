#include "data/data_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace alt_las {
namespace data {

DataManager& DataManager::getInstance() {
    static DataManager instance;
    return instance;
}

DataManager::DataManager()
    : serializer_(DataSerializer::getInstance()),
      validator_(DataValidator::getInstance()),
      cache_(DataCache::getInstance()),
      dataDirectory_("data"),
      dataExtension_(".json") {
    
    // Veri dizinini oluştur
    createDataDirectory();
    
    // Önbellek otomatik temizlemeyi başlat
    cache_.startAutoCleanup();
}

DataManager::~DataManager() {
    // Önbellek otomatik temizlemeyi durdur
    cache_.stopAutoCleanup();
}

DataSerializer& DataManager::getSerializer() {
    return serializer_;
}

DataValidator& DataManager::getValidator() {
    return validator_;
}

DataCache& DataManager::getCache() {
    return cache_;
}

bool DataManager::removeData(const std::string& key) {
    // Veri yolunu oluştur
    std::string filePath = createDataPath(key);
    
    // Dosyayı sil
    bool success = std::filesystem::remove(filePath);
    
    // Önbellekten sil
    cache_.remove(key);
    
    return success;
}

bool DataManager::hasData(const std::string& key) {
    // Önbellekte kontrol et
    if (cache_.has(key)) {
        return true;
    }
    
    // Veri yolunu oluştur
    std::string filePath = createDataPath(key);
    
    // Dosya varlığını kontrol et
    return std::filesystem::exists(filePath);
}

std::string DataManager::getDataDirectory() const {
    std::lock_guard<std::mutex> lock(directoryMutex_);
    return dataDirectory_;
}

void DataManager::setDataDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(directoryMutex_);
    dataDirectory_ = directory;
    
    // Veri dizinini oluştur
    createDataDirectory();
}

std::string DataManager::getDataExtension() const {
    std::lock_guard<std::mutex> lock(directoryMutex_);
    return dataExtension_;
}

void DataManager::setDataExtension(const std::string& extension) {
    std::lock_guard<std::mutex> lock(directoryMutex_);
    dataExtension_ = extension;
}

std::string DataManager::createDataPath(const std::string& key) const {
    std::lock_guard<std::mutex> lock(directoryMutex_);
    
    // Veri yolunu oluştur
    std::string filePath = dataDirectory_ + "/" + key + dataExtension_;
    
    return filePath;
}

std::string DataManager::generateDataReport() const {
    std::stringstream ss;
    
    // Başlık
    ss << "=== Data Management Report ===" << std::endl;
    ss << std::endl;
    
    // Veri dizini
    ss << "Data Directory: " << getDataDirectory() << std::endl;
    ss << "Data Extension: " << getDataExtension() << std::endl;
    ss << std::endl;
    
    // Önbellek istatistikleri
    ss << "Cache Statistics:" << std::endl;
    
    auto stats = cache_.getStats();
    
    ss << "  Size: " << cache_.size() << std::endl;
    ss << "  Capacity: " << cache_.capacity() << std::endl;
    ss << "  Default TTL: " << cache_.getDefaultTtl().count() << " seconds" << std::endl;
    ss << "  Cleanup Interval: " << cache_.getCleanupInterval().count() << " seconds" << std::endl;
    ss << "  Auto Cleanup: " << (cache_.isAutoCleanupRunning() ? "Running" : "Stopped") << std::endl;
    ss << std::endl;
    
    ss << "  Hits: " << stats["hit"] << std::endl;
    ss << "  Misses: " << stats["miss"] << std::endl;
    ss << "  Sets: " << stats["set"] << std::endl;
    ss << "  Removes: " << stats["remove"] << std::endl;
    ss << "  Clears: " << stats["clear"] << std::endl;
    ss << "  Clear Expired: " << stats["clearExpired"] << std::endl;
    ss << "  Clear By Tags: " << stats["clearByTags"] << std::endl;
    ss << "  Expired: " << stats["expired"] << std::endl;
    ss << "  Errors: " << stats["error"] << std::endl;
    
    return ss.str();
}

bool DataManager::saveDataReport(const std::string& filePath) const {
    // Raporu oluştur
    std::string report = generateDataReport();
    
    // Dosyayı aç
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open report file: " << filePath << std::endl;
        return false;
    }
    
    // Raporu dosyaya yaz
    file << report;
    
    // Dosyayı kapat
    file.close();
    
    return true;
}

void DataManager::printDataReport() const {
    // Raporu oluştur
    std::string report = generateDataReport();
    
    // Raporu konsola yazdır
    std::cout << report;
}

bool DataManager::createDataDirectory() const {
    std::lock_guard<std::mutex> lock(directoryMutex_);
    
    // Veri dizini varlığını kontrol et
    if (!std::filesystem::exists(dataDirectory_)) {
        // Veri dizinini oluştur
        return std::filesystem::create_directories(dataDirectory_);
    }
    
    return true;
}

} // namespace data
} // namespace alt_las
