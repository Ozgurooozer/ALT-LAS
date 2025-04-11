#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <regex>
#include <nlohmann/json.hpp>

namespace alt_las {
namespace data {

/**
 * @brief Doğrulama sonucu
 */
struct ValidationResult {
    bool valid;                                 // Doğrulama başarılı mı?
    std::vector<std::string> errors;            // Doğrulama hataları
    std::unordered_map<std::string, bool> fieldResults; // Alan doğrulama sonuçları
};

/**
 * @brief Doğrulama kuralı
 */
struct ValidationRule {
    std::string name;                           // Kural adı
    std::string description;                    // Kural açıklaması
    std::function<bool(const nlohmann::json&)> validator; // Doğrulama fonksiyonu
};

/**
 * @brief Veri doğrulama sınıfı
 * 
 * Bu sınıf, veri doğrulama işlemlerini temsil eder.
 */
class DataValidator {
public:
    /**
     * @brief Singleton örneğini döndürür
     * 
     * @return DataValidator örneği
     */
    static DataValidator& getInstance();

    /**
     * @brief Yıkıcı
     */
    ~DataValidator();

    /**
     * @brief Veriyi doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @return Doğrulama sonucu
     */
    ValidationResult validate(const nlohmann::json& data, const nlohmann::json& schema);

    /**
     * @brief Veriyi doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schemaName Doğrulama şeması adı
     * @return Doğrulama sonucu
     */
    ValidationResult validate(const nlohmann::json& data, const std::string& schemaName);

    /**
     * @brief Doğrulama şeması ekler
     * 
     * @param name Şema adı
     * @param schema Doğrulama şeması
     */
    void addSchema(const std::string& name, const nlohmann::json& schema);

    /**
     * @brief Doğrulama şeması siler
     * 
     * @param name Şema adı
     * @return Silme başarılıysa true, değilse false
     */
    bool removeSchema(const std::string& name);

    /**
     * @brief Doğrulama şeması varlığını kontrol eder
     * 
     * @param name Şema adı
     * @return Şema varsa true, yoksa false
     */
    bool hasSchema(const std::string& name) const;

    /**
     * @brief Doğrulama şemasını döndürür
     * 
     * @param name Şema adı
     * @return Doğrulama şeması
     */
    nlohmann::json getSchema(const std::string& name) const;

    /**
     * @brief Doğrulama kuralı ekler
     * 
     * @param name Kural adı
     * @param description Kural açıklaması
     * @param validator Doğrulama fonksiyonu
     */
    void addRule(const std::string& name, const std::string& description, std::function<bool(const nlohmann::json&)> validator);

    /**
     * @brief Doğrulama kuralı siler
     * 
     * @param name Kural adı
     * @return Silme başarılıysa true, değilse false
     */
    bool removeRule(const std::string& name);

    /**
     * @brief Doğrulama kuralı varlığını kontrol eder
     * 
     * @param name Kural adı
     * @return Kural varsa true, yoksa false
     */
    bool hasRule(const std::string& name) const;

    /**
     * @brief Doğrulama kuralını döndürür
     * 
     * @param name Kural adı
     * @return Doğrulama kuralı
     */
    ValidationRule getRule(const std::string& name) const;

    /**
     * @brief Tüm doğrulama kurallarını döndürür
     * 
     * @return Doğrulama kuralları
     */
    std::vector<ValidationRule> getAllRules() const;

    /**
     * @brief Varsayılan doğrulama kurallarını oluşturur
     */
    void createDefaultRules();

private:
    /**
     * @brief Yapıcı
     */
    DataValidator();

    /**
     * @brief Kopyalama yapıcısı (engellendi)
     */
    DataValidator(const DataValidator&) = delete;

    /**
     * @brief Atama operatörü (engellendi)
     */
    DataValidator& operator=(const DataValidator&) = delete;

    /**
     * @brief Veriyi şemaya göre doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     */
    void validateAgainstSchema(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path = "");

    /**
     * @brief Veri tipini doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validateType(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    /**
     * @brief Veri değerini doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validateValue(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    /**
     * @brief Veri uzunluğunu doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validateLength(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    /**
     * @brief Veri formatını doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validateFormat(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    /**
     * @brief Veri desenini doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validatePattern(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    /**
     * @brief Veri özelliklerini doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validateProperties(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    /**
     * @brief Veri öğelerini doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validateItems(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    /**
     * @brief Veri kuralını doğrular
     * 
     * @param data Doğrulanacak veri
     * @param schema Doğrulama şeması
     * @param result Doğrulama sonucu
     * @param path Doğrulama yolu
     * @return Doğrulama başarılıysa true, değilse false
     */
    bool validateRule(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path);

    std::unordered_map<std::string, nlohmann::json> schemas_; // Doğrulama şemaları
    mutable std::mutex schemasMutex_; // Şemalar mutex'i
    std::unordered_map<std::string, ValidationRule> rules_; // Doğrulama kuralları
    mutable std::mutex rulesMutex_; // Kurallar mutex'i
};

} // namespace data
} // namespace alt_las
