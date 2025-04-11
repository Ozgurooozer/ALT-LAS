#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <nlohmann/json.hpp>

namespace alt_las {
namespace data {

/**
 * @brief Serileştirme formatı
 */
enum class SerializationFormat {
    JSON,       // JSON formatı
    XML,        // XML formatı
    BINARY,     // İkili format
    CSV,        // CSV formatı
    CUSTOM      // Özel format
};

/**
 * @brief Veri serileştirme sınıfı
 * 
 * Bu sınıf, veri serileştirme işlemlerini temsil eder.
 */
class DataSerializer {
public:
    /**
     * @brief Singleton örneğini döndürür
     * 
     * @return DataSerializer örneği
     */
    static DataSerializer& getInstance();

    /**
     * @brief Yıkıcı
     */
    ~DataSerializer();

    /**
     * @brief Veriyi serileştirir
     * 
     * @param data Serileştirilecek veri
     * @param format Serileştirme formatı
     * @return Serileştirilmiş veri
     */
    template <typename T>
    std::string serialize(const T& data, SerializationFormat format = SerializationFormat::JSON);

    /**
     * @brief Veriyi deserileştirir
     * 
     * @param serializedData Deserileştirilecek veri
     * @param format Serileştirme formatı
     * @return Deserileştirilmiş veri
     */
    template <typename T>
    T deserialize(const std::string& serializedData, SerializationFormat format = SerializationFormat::JSON);

    /**
     * @brief Veriyi dosyaya serileştirir
     * 
     * @param data Serileştirilecek veri
     * @param filePath Dosya yolu
     * @param format Serileştirme formatı
     * @return Serileştirme başarılıysa true, değilse false
     */
    template <typename T>
    bool serializeToFile(const T& data, const std::string& filePath, SerializationFormat format = SerializationFormat::JSON);

    /**
     * @brief Veriyi dosyadan deserileştirir
     * 
     * @param filePath Dosya yolu
     * @param format Serileştirme formatı
     * @return Deserileştirilmiş veri
     */
    template <typename T>
    T deserializeFromFile(const std::string& filePath, SerializationFormat format = SerializationFormat::JSON);

    /**
     * @brief Özel serileştirici ekler
     * 
     * @param name Serileştirici adı
     * @param serializer Serileştirici fonksiyonu
     */
    void addCustomSerializer(const std::string& name, std::function<std::string(const nlohmann::json&)> serializer);

    /**
     * @brief Özel deserileştirici ekler
     * 
     * @param name Deserileştirici adı
     * @param deserializer Deserileştirici fonksiyonu
     */
    void addCustomDeserializer(const std::string& name, std::function<nlohmann::json(const std::string&)> deserializer);

    /**
     * @brief Özel serileştirici siler
     * 
     * @param name Serileştirici adı
     * @return Silme başarılıysa true, değilse false
     */
    bool removeCustomSerializer(const std::string& name);

    /**
     * @brief Özel deserileştirici siler
     * 
     * @param name Deserileştirici adı
     * @return Silme başarılıysa true, değilse false
     */
    bool removeCustomDeserializer(const std::string& name);

    /**
     * @brief Özel serileştirici varlığını kontrol eder
     * 
     * @param name Serileştirici adı
     * @return Serileştirici varsa true, yoksa false
     */
    bool hasCustomSerializer(const std::string& name) const;

    /**
     * @brief Özel deserileştirici varlığını kontrol eder
     * 
     * @param name Deserileştirici adı
     * @return Deserileştirici varsa true, yoksa false
     */
    bool hasCustomDeserializer(const std::string& name) const;

    /**
     * @brief Serileştirme formatını string olarak döndürür
     * 
     * @param format Serileştirme formatı
     * @return Serileştirme formatı string'i
     */
    std::string getFormatString(SerializationFormat format) const;

private:
    /**
     * @brief Yapıcı
     */
    DataSerializer();

    /**
     * @brief Kopyalama yapıcısı (engellendi)
     */
    DataSerializer(const DataSerializer&) = delete;

    /**
     * @brief Atama operatörü (engellendi)
     */
    DataSerializer& operator=(const DataSerializer&) = delete;

    /**
     * @brief JSON formatında serileştirir
     * 
     * @param data Serileştirilecek veri
     * @return Serileştirilmiş veri
     */
    template <typename T>
    std::string serializeJson(const T& data);

    /**
     * @brief JSON formatında deserileştirir
     * 
     * @param serializedData Deserileştirilecek veri
     * @return Deserileştirilmiş veri
     */
    template <typename T>
    T deserializeJson(const std::string& serializedData);

    /**
     * @brief XML formatında serileştirir
     * 
     * @param data Serileştirilecek veri
     * @return Serileştirilmiş veri
     */
    template <typename T>
    std::string serializeXml(const T& data);

    /**
     * @brief XML formatında deserileştirir
     * 
     * @param serializedData Deserileştirilecek veri
     * @return Deserileştirilmiş veri
     */
    template <typename T>
    T deserializeXml(const std::string& serializedData);

    /**
     * @brief İkili formatında serileştirir
     * 
     * @param data Serileştirilecek veri
     * @return Serileştirilmiş veri
     */
    template <typename T>
    std::string serializeBinary(const T& data);

    /**
     * @brief İkili formatında deserileştirir
     * 
     * @param serializedData Deserileştirilecek veri
     * @return Deserileştirilmiş veri
     */
    template <typename T>
    T deserializeBinary(const std::string& serializedData);

    /**
     * @brief CSV formatında serileştirir
     * 
     * @param data Serileştirilecek veri
     * @return Serileştirilmiş veri
     */
    template <typename T>
    std::string serializeCsv(const T& data);

    /**
     * @brief CSV formatında deserileştirir
     * 
     * @param serializedData Deserileştirilecek veri
     * @return Deserileştirilmiş veri
     */
    template <typename T>
    T deserializeCsv(const std::string& serializedData);

    /**
     * @brief Özel formatta serileştirir
     * 
     * @param data Serileştirilecek veri
     * @param serializerName Serileştirici adı
     * @return Serileştirilmiş veri
     */
    template <typename T>
    std::string serializeCustom(const T& data, const std::string& serializerName);

    /**
     * @brief Özel formatta deserileştirir
     * 
     * @param serializedData Deserileştirilecek veri
     * @param deserializerName Deserileştirici adı
     * @return Deserileştirilmiş veri
     */
    template <typename T>
    T deserializeCustom(const std::string& serializedData, const std::string& deserializerName);

    std::unordered_map<std::string, std::function<std::string(const nlohmann::json&)>> customSerializers_; // Özel serileştiriciler
    std::unordered_map<std::string, std::function<nlohmann::json(const std::string&)>> customDeserializers_; // Özel deserileştiriciler
    mutable std::mutex serializersMutex_; // Serileştiriciler mutex'i
    mutable std::mutex deserializersMutex_; // Deserileştiriciler mutex'i
};

template <typename T>
std::string DataSerializer::serialize(const T& data, SerializationFormat format) {
    switch (format) {
        case SerializationFormat::JSON:
            return serializeJson(data);
        case SerializationFormat::XML:
            return serializeXml(data);
        case SerializationFormat::BINARY:
            return serializeBinary(data);
        case SerializationFormat::CSV:
            return serializeCsv(data);
        case SerializationFormat::CUSTOM:
            return serializeCustom(data, "default");
        default:
            return serializeJson(data);
    }
}

template <typename T>
T DataSerializer::deserialize(const std::string& serializedData, SerializationFormat format) {
    switch (format) {
        case SerializationFormat::JSON:
            return deserializeJson<T>(serializedData);
        case SerializationFormat::XML:
            return deserializeXml<T>(serializedData);
        case SerializationFormat::BINARY:
            return deserializeBinary<T>(serializedData);
        case SerializationFormat::CSV:
            return deserializeCsv<T>(serializedData);
        case SerializationFormat::CUSTOM:
            return deserializeCustom<T>(serializedData, "default");
        default:
            return deserializeJson<T>(serializedData);
    }
}

template <typename T>
bool DataSerializer::serializeToFile(const T& data, const std::string& filePath, SerializationFormat format) {
    try {
        // Veriyi serileştir
        std::string serializedData = serialize(data, format);
        
        // Dosyayı aç
        std::ofstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Serileştirilmiş veriyi dosyaya yaz
        file.write(serializedData.c_str(), serializedData.size());
        
        // Dosyayı kapat
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

template <typename T>
T DataSerializer::deserializeFromFile(const std::string& filePath, SerializationFormat format) {
    try {
        // Dosyayı aç
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return T();
        }
        
        // Dosya içeriğini oku
        std::string serializedData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Dosyayı kapat
        file.close();
        
        // Veriyi deserileştir
        return deserialize<T>(serializedData, format);
    } catch (const std::exception& e) {
        return T();
    }
}

template <typename T>
std::string DataSerializer::serializeJson(const T& data) {
    try {
        // Veriyi JSON formatına dönüştür
        nlohmann::json jsonData = data;
        
        // JSON verisini string'e dönüştür
        return jsonData.dump();
    } catch (const std::exception& e) {
        return "";
    }
}

template <typename T>
T DataSerializer::deserializeJson(const std::string& serializedData) {
    try {
        // String'i JSON verisine dönüştür
        nlohmann::json jsonData = nlohmann::json::parse(serializedData);
        
        // JSON verisini veri tipine dönüştür
        return jsonData.get<T>();
    } catch (const std::exception& e) {
        return T();
    }
}

template <typename T>
std::string DataSerializer::serializeXml(const T& data) {
    // TODO: XML serileştirme kodu eklenecek
    return "";
}

template <typename T>
T DataSerializer::deserializeXml(const std::string& serializedData) {
    // TODO: XML deserileştirme kodu eklenecek
    return T();
}

template <typename T>
std::string DataSerializer::serializeBinary(const T& data) {
    // TODO: İkili serileştirme kodu eklenecek
    return "";
}

template <typename T>
T DataSerializer::deserializeBinary(const std::string& serializedData) {
    // TODO: İkili deserileştirme kodu eklenecek
    return T();
}

template <typename T>
std::string DataSerializer::serializeCsv(const T& data) {
    // TODO: CSV serileştirme kodu eklenecek
    return "";
}

template <typename T>
T DataSerializer::deserializeCsv(const std::string& serializedData) {
    // TODO: CSV deserileştirme kodu eklenecek
    return T();
}

template <typename T>
std::string DataSerializer::serializeCustom(const T& data, const std::string& serializerName) {
    std::lock_guard<std::mutex> lock(serializersMutex_);
    
    // Serileştiriciyi bul
    auto it = customSerializers_.find(serializerName);
    if (it != customSerializers_.end()) {
        try {
            // Veriyi JSON formatına dönüştür
            nlohmann::json jsonData = data;
            
            // Özel serileştiriciyi çağır
            return it->second(jsonData);
        } catch (const std::exception& e) {
            return "";
        }
    }
    
    return "";
}

template <typename T>
T DataSerializer::deserializeCustom(const std::string& serializedData, const std::string& deserializerName) {
    std::lock_guard<std::mutex> lock(deserializersMutex_);
    
    // Deserileştiriciyi bul
    auto it = customDeserializers_.find(deserializerName);
    if (it != customDeserializers_.end()) {
        try {
            // Özel deserileştiriciyi çağır
            nlohmann::json jsonData = it->second(serializedData);
            
            // JSON verisini veri tipine dönüştür
            return jsonData.get<T>();
        } catch (const std::exception& e) {
            return T();
        }
    }
    
    return T();
}

} // namespace data
} // namespace alt_las
