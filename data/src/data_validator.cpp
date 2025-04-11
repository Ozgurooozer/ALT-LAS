#include "data/data_validator.h"
#include <iostream>

namespace alt_las {
namespace data {

DataValidator& DataValidator::getInstance() {
    static DataValidator instance;
    return instance;
}

DataValidator::DataValidator() {
    // Varsayılan doğrulama kurallarını oluştur
    createDefaultRules();
}

DataValidator::~DataValidator() {
}

ValidationResult DataValidator::validate(const nlohmann::json& data, const nlohmann::json& schema) {
    ValidationResult result;
    result.valid = true;
    
    // Veriyi şemaya göre doğrula
    validateAgainstSchema(data, schema, result);
    
    return result;
}

ValidationResult DataValidator::validate(const nlohmann::json& data, const std::string& schemaName) {
    // Şemayı al
    nlohmann::json schema = getSchema(schemaName);
    
    // Şema bulunamazsa, geçersiz sonuç döndür
    if (schema.is_null()) {
        ValidationResult result;
        result.valid = false;
        result.errors.push_back("Schema not found: " + schemaName);
        return result;
    }
    
    // Veriyi şemaya göre doğrula
    return validate(data, schema);
}

void DataValidator::addSchema(const std::string& name, const nlohmann::json& schema) {
    std::lock_guard<std::mutex> lock(schemasMutex_);
    schemas_[name] = schema;
}

bool DataValidator::removeSchema(const std::string& name) {
    std::lock_guard<std::mutex> lock(schemasMutex_);
    
    auto it = schemas_.find(name);
    if (it != schemas_.end()) {
        schemas_.erase(it);
        return true;
    }
    
    return false;
}

bool DataValidator::hasSchema(const std::string& name) const {
    std::lock_guard<std::mutex> lock(schemasMutex_);
    return schemas_.find(name) != schemas_.end();
}

nlohmann::json DataValidator::getSchema(const std::string& name) const {
    std::lock_guard<std::mutex> lock(schemasMutex_);
    
    auto it = schemas_.find(name);
    if (it != schemas_.end()) {
        return it->second;
    }
    
    return nlohmann::json();
}

void DataValidator::addRule(const std::string& name, const std::string& description, std::function<bool(const nlohmann::json&)> validator) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    
    ValidationRule rule;
    rule.name = name;
    rule.description = description;
    rule.validator = validator;
    
    rules_[name] = rule;
}

bool DataValidator::removeRule(const std::string& name) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    
    auto it = rules_.find(name);
    if (it != rules_.end()) {
        rules_.erase(it);
        return true;
    }
    
    return false;
}

bool DataValidator::hasRule(const std::string& name) const {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    return rules_.find(name) != rules_.end();
}

ValidationRule DataValidator::getRule(const std::string& name) const {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    
    auto it = rules_.find(name);
    if (it != rules_.end()) {
        return it->second;
    }
    
    return ValidationRule();
}

std::vector<ValidationRule> DataValidator::getAllRules() const {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    
    std::vector<ValidationRule> result;
    
    for (const auto& pair : rules_) {
        result.push_back(pair.second);
    }
    
    return result;
}

void DataValidator::createDefaultRules() {
    // E-posta doğrulama kuralı
    addRule("email", "Validates email format", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string email = data.get<std::string>();
        std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        
        return std::regex_match(email, emailRegex);
    });
    
    // URL doğrulama kuralı
    addRule("url", "Validates URL format", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string url = data.get<std::string>();
        std::regex urlRegex(R"(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))");
        
        return std::regex_match(url, urlRegex);
    });
    
    // Tarih doğrulama kuralı
    addRule("date", "Validates date format (YYYY-MM-DD)", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string date = data.get<std::string>();
        std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
        
        return std::regex_match(date, dateRegex);
    });
    
    // Zaman doğrulama kuralı
    addRule("time", "Validates time format (HH:MM:SS)", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string time = data.get<std::string>();
        std::regex timeRegex(R"(\d{2}:\d{2}:\d{2})");
        
        return std::regex_match(time, timeRegex);
    });
    
    // Tarih-zaman doğrulama kuralı
    addRule("datetime", "Validates datetime format (YYYY-MM-DD HH:MM:SS)", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string datetime = data.get<std::string>();
        std::regex datetimeRegex(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})");
        
        return std::regex_match(datetime, datetimeRegex);
    });
    
    // Telefon numarası doğrulama kuralı
    addRule("phone", "Validates phone number format", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string phone = data.get<std::string>();
        std::regex phoneRegex(R"(\+?[0-9]{10,15})");
        
        return std::regex_match(phone, phoneRegex);
    });
    
    // Posta kodu doğrulama kuralı
    addRule("zipcode", "Validates zip code format", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string zipcode = data.get<std::string>();
        std::regex zipcodeRegex(R"([0-9]{5}(-[0-9]{4})?)");
        
        return std::regex_match(zipcode, zipcodeRegex);
    });
    
    // Kredi kartı numarası doğrulama kuralı
    addRule("creditcard", "Validates credit card number format", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string creditcard = data.get<std::string>();
        std::regex creditcardRegex(R"([0-9]{13,19})");
        
        return std::regex_match(creditcard, creditcardRegex);
    });
    
    // IP adresi doğrulama kuralı
    addRule("ipv4", "Validates IPv4 address format", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string ipv4 = data.get<std::string>();
        std::regex ipv4Regex(R"((\d{1,3}\.){3}\d{1,3})");
        
        return std::regex_match(ipv4, ipv4Regex);
    });
    
    // IPv6 adresi doğrulama kuralı
    addRule("ipv6", "Validates IPv6 address format", [](const nlohmann::json& data) {
        if (!data.is_string()) {
            return false;
        }
        
        std::string ipv6 = data.get<std::string>();
        std::regex ipv6Regex(R"(([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4})");
        
        return std::regex_match(ipv6, ipv6Regex);
    });
}

void DataValidator::validateAgainstSchema(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    // Veri tipini doğrula
    if (schema.contains("type")) {
        if (!validateType(data, schema, result, path)) {
            return;
        }
    }
    
    // Veri değerini doğrula
    if (schema.contains("enum") || schema.contains("const") || schema.contains("minimum") || schema.contains("maximum") || schema.contains("exclusiveMinimum") || schema.contains("exclusiveMaximum") || schema.contains("multipleOf")) {
        if (!validateValue(data, schema, result, path)) {
            return;
        }
    }
    
    // Veri uzunluğunu doğrula
    if (schema.contains("minLength") || schema.contains("maxLength")) {
        if (!validateLength(data, schema, result, path)) {
            return;
        }
    }
    
    // Veri formatını doğrula
    if (schema.contains("format")) {
        if (!validateFormat(data, schema, result, path)) {
            return;
        }
    }
    
    // Veri desenini doğrula
    if (schema.contains("pattern")) {
        if (!validatePattern(data, schema, result, path)) {
            return;
        }
    }
    
    // Veri özelliklerini doğrula
    if (schema.contains("properties") || schema.contains("required") || schema.contains("additionalProperties")) {
        if (!validateProperties(data, schema, result, path)) {
            return;
        }
    }
    
    // Veri öğelerini doğrula
    if (schema.contains("items") || schema.contains("minItems") || schema.contains("maxItems") || schema.contains("uniqueItems")) {
        if (!validateItems(data, schema, result, path)) {
            return;
        }
    }
    
    // Veri kuralını doğrula
    if (schema.contains("rule")) {
        if (!validateRule(data, schema, result, path)) {
            return;
        }
    }
}

bool DataValidator::validateType(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    std::string type = schema["type"];
    
    bool valid = false;
    
    if (type == "null") {
        valid = data.is_null();
    } else if (type == "boolean") {
        valid = data.is_boolean();
    } else if (type == "number") {
        valid = data.is_number();
    } else if (type == "integer") {
        valid = data.is_number_integer();
    } else if (type == "string") {
        valid = data.is_string();
    } else if (type == "array") {
        valid = data.is_array();
    } else if (type == "object") {
        valid = data.is_object();
    }
    
    if (!valid) {
        result.valid = false;
        result.errors.push_back(path + ": Expected type '" + type + "'");
        
        if (!path.empty()) {
            result.fieldResults[path] = false;
        }
    }
    
    return valid;
}

bool DataValidator::validateValue(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    bool valid = true;
    
    // Enum doğrulama
    if (schema.contains("enum")) {
        bool enumValid = false;
        
        for (const auto& value : schema["enum"]) {
            if (data == value) {
                enumValid = true;
                break;
            }
        }
        
        if (!enumValid) {
            result.valid = false;
            result.errors.push_back(path + ": Value not in enum");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // Const doğrulama
    if (schema.contains("const")) {
        if (data != schema["const"]) {
            result.valid = false;
            result.errors.push_back(path + ": Value not equal to const");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // Minimum doğrulama
    if (schema.contains("minimum") && data.is_number()) {
        if (data.get<double>() < schema["minimum"].get<double>()) {
            result.valid = false;
            result.errors.push_back(path + ": Value less than minimum");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // Maximum doğrulama
    if (schema.contains("maximum") && data.is_number()) {
        if (data.get<double>() > schema["maximum"].get<double>()) {
            result.valid = false;
            result.errors.push_back(path + ": Value greater than maximum");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // ExclusiveMinimum doğrulama
    if (schema.contains("exclusiveMinimum") && data.is_number()) {
        if (data.get<double>() <= schema["exclusiveMinimum"].get<double>()) {
            result.valid = false;
            result.errors.push_back(path + ": Value less than or equal to exclusiveMinimum");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // ExclusiveMaximum doğrulama
    if (schema.contains("exclusiveMaximum") && data.is_number()) {
        if (data.get<double>() >= schema["exclusiveMaximum"].get<double>()) {
            result.valid = false;
            result.errors.push_back(path + ": Value greater than or equal to exclusiveMaximum");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // MultipleOf doğrulama
    if (schema.contains("multipleOf") && data.is_number()) {
        double value = data.get<double>();
        double multipleOf = schema["multipleOf"].get<double>();
        
        if (std::fmod(value, multipleOf) != 0) {
            result.valid = false;
            result.errors.push_back(path + ": Value not multiple of " + std::to_string(multipleOf));
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    return valid;
}

bool DataValidator::validateLength(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    bool valid = true;
    
    // MinLength doğrulama
    if (schema.contains("minLength") && data.is_string()) {
        std::string value = data.get<std::string>();
        int minLength = schema["minLength"].get<int>();
        
        if (value.length() < minLength) {
            result.valid = false;
            result.errors.push_back(path + ": String length less than minLength");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // MaxLength doğrulama
    if (schema.contains("maxLength") && data.is_string()) {
        std::string value = data.get<std::string>();
        int maxLength = schema["maxLength"].get<int>();
        
        if (value.length() > maxLength) {
            result.valid = false;
            result.errors.push_back(path + ": String length greater than maxLength");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    return valid;
}

bool DataValidator::validateFormat(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    bool valid = true;
    
    // Format doğrulama
    if (schema.contains("format") && data.is_string()) {
        std::string format = schema["format"].get<std::string>();
        
        // Kural varlığını kontrol et
        if (hasRule(format)) {
            ValidationRule rule = getRule(format);
            
            if (!rule.validator(data)) {
                result.valid = false;
                result.errors.push_back(path + ": Invalid format '" + format + "'");
                
                if (!path.empty()) {
                    result.fieldResults[path] = false;
                }
                
                valid = false;
            }
        } else {
            result.valid = false;
            result.errors.push_back(path + ": Unknown format '" + format + "'");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    return valid;
}

bool DataValidator::validatePattern(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    bool valid = true;
    
    // Pattern doğrulama
    if (schema.contains("pattern") && data.is_string()) {
        std::string value = data.get<std::string>();
        std::string pattern = schema["pattern"].get<std::string>();
        
        try {
            std::regex regex(pattern);
            
            if (!std::regex_match(value, regex)) {
                result.valid = false;
                result.errors.push_back(path + ": String does not match pattern");
                
                if (!path.empty()) {
                    result.fieldResults[path] = false;
                }
                
                valid = false;
            }
        } catch (const std::regex_error& e) {
            result.valid = false;
            result.errors.push_back(path + ": Invalid pattern: " + e.what());
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    return valid;
}

bool DataValidator::validateProperties(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    bool valid = true;
    
    // Nesne tipini kontrol et
    if (!data.is_object()) {
        return valid;
    }
    
    // Required doğrulama
    if (schema.contains("required")) {
        for (const auto& requiredProperty : schema["required"]) {
            std::string propertyName = requiredProperty.get<std::string>();
            
            if (!data.contains(propertyName)) {
                result.valid = false;
                result.errors.push_back(path + ": Missing required property '" + propertyName + "'");
                
                if (!path.empty()) {
                    result.fieldResults[path + "." + propertyName] = false;
                }
                
                valid = false;
            }
        }
    }
    
    // Properties doğrulama
    if (schema.contains("properties")) {
        for (const auto& property : data.items()) {
            std::string propertyName = property.key();
            
            if (schema["properties"].contains(propertyName)) {
                std::string propertyPath = path.empty() ? propertyName : path + "." + propertyName;
                
                validateAgainstSchema(property.value(), schema["properties"][propertyName], result, propertyPath);
            } else if (schema.contains("additionalProperties")) {
                if (schema["additionalProperties"].is_boolean()) {
                    if (!schema["additionalProperties"].get<bool>()) {
                        result.valid = false;
                        result.errors.push_back(path + ": Additional property '" + propertyName + "' not allowed");
                        
                        if (!path.empty()) {
                            result.fieldResults[path + "." + propertyName] = false;
                        }
                        
                        valid = false;
                    }
                } else {
                    std::string propertyPath = path.empty() ? propertyName : path + "." + propertyName;
                    
                    validateAgainstSchema(property.value(), schema["additionalProperties"], result, propertyPath);
                }
            }
        }
    }
    
    return valid;
}

bool DataValidator::validateItems(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    bool valid = true;
    
    // Dizi tipini kontrol et
    if (!data.is_array()) {
        return valid;
    }
    
    // MinItems doğrulama
    if (schema.contains("minItems")) {
        int minItems = schema["minItems"].get<int>();
        
        if (data.size() < minItems) {
            result.valid = false;
            result.errors.push_back(path + ": Array length less than minItems");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // MaxItems doğrulama
    if (schema.contains("maxItems")) {
        int maxItems = schema["maxItems"].get<int>();
        
        if (data.size() > maxItems) {
            result.valid = false;
            result.errors.push_back(path + ": Array length greater than maxItems");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // UniqueItems doğrulama
    if (schema.contains("uniqueItems") && schema["uniqueItems"].get<bool>()) {
        std::set<nlohmann::json> uniqueItems;
        
        for (const auto& item : data) {
            uniqueItems.insert(item);
        }
        
        if (uniqueItems.size() != data.size()) {
            result.valid = false;
            result.errors.push_back(path + ": Array items not unique");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    // Items doğrulama
    if (schema.contains("items")) {
        if (schema["items"].is_object()) {
            // Tüm öğeler için aynı şema
            for (size_t i = 0; i < data.size(); i++) {
                std::string itemPath = path.empty() ? "[" + std::to_string(i) + "]" : path + "[" + std::to_string(i) + "]";
                
                validateAgainstSchema(data[i], schema["items"], result, itemPath);
            }
        } else if (schema["items"].is_array()) {
            // Her öğe için farklı şema
            for (size_t i = 0; i < std::min(data.size(), schema["items"].size()); i++) {
                std::string itemPath = path.empty() ? "[" + std::to_string(i) + "]" : path + "[" + std::to_string(i) + "]";
                
                validateAgainstSchema(data[i], schema["items"][i], result, itemPath);
            }
            
            // AdditionalItems doğrulama
            if (data.size() > schema["items"].size()) {
                if (schema.contains("additionalItems")) {
                    if (schema["additionalItems"].is_boolean()) {
                        if (!schema["additionalItems"].get<bool>()) {
                            result.valid = false;
                            result.errors.push_back(path + ": Additional items not allowed");
                            
                            if (!path.empty()) {
                                result.fieldResults[path] = false;
                            }
                            
                            valid = false;
                        }
                    } else {
                        for (size_t i = schema["items"].size(); i < data.size(); i++) {
                            std::string itemPath = path.empty() ? "[" + std::to_string(i) + "]" : path + "[" + std::to_string(i) + "]";
                            
                            validateAgainstSchema(data[i], schema["additionalItems"], result, itemPath);
                        }
                    }
                }
            }
        }
    }
    
    return valid;
}

bool DataValidator::validateRule(const nlohmann::json& data, const nlohmann::json& schema, ValidationResult& result, const std::string& path) {
    bool valid = true;
    
    // Rule doğrulama
    if (schema.contains("rule")) {
        std::string ruleName = schema["rule"].get<std::string>();
        
        // Kural varlığını kontrol et
        if (hasRule(ruleName)) {
            ValidationRule rule = getRule(ruleName);
            
            if (!rule.validator(data)) {
                result.valid = false;
                result.errors.push_back(path + ": Failed rule '" + ruleName + "'");
                
                if (!path.empty()) {
                    result.fieldResults[path] = false;
                }
                
                valid = false;
            }
        } else {
            result.valid = false;
            result.errors.push_back(path + ": Unknown rule '" + ruleName + "'");
            
            if (!path.empty()) {
                result.fieldResults[path] = false;
            }
            
            valid = false;
        }
    }
    
    return valid;
}

} // namespace data
} // namespace alt_las
