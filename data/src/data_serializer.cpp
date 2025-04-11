#include "data/data_serializer.h"
#include <iostream>
#include <fstream>

namespace alt_las {
namespace data {

DataSerializer& DataSerializer::getInstance() {
    static DataSerializer instance;
    return instance;
}

DataSerializer::DataSerializer() {
}

DataSerializer::~DataSerializer() {
}

void DataSerializer::addCustomSerializer(const std::string& name, std::function<std::string(const nlohmann::json&)> serializer) {
    std::lock_guard<std::mutex> lock(serializersMutex_);
    customSerializers_[name] = serializer;
}

void DataSerializer::addCustomDeserializer(const std::string& name, std::function<nlohmann::json(const std::string&)> deserializer) {
    std::lock_guard<std::mutex> lock(deserializersMutex_);
    customDeserializers_[name] = deserializer;
}

bool DataSerializer::removeCustomSerializer(const std::string& name) {
    std::lock_guard<std::mutex> lock(serializersMutex_);
    
    auto it = customSerializers_.find(name);
    if (it != customSerializers_.end()) {
        customSerializers_.erase(it);
        return true;
    }
    
    return false;
}

bool DataSerializer::removeCustomDeserializer(const std::string& name) {
    std::lock_guard<std::mutex> lock(deserializersMutex_);
    
    auto it = customDeserializers_.find(name);
    if (it != customDeserializers_.end()) {
        customDeserializers_.erase(it);
        return true;
    }
    
    return false;
}

bool DataSerializer::hasCustomSerializer(const std::string& name) const {
    std::lock_guard<std::mutex> lock(serializersMutex_);
    return customSerializers_.find(name) != customSerializers_.end();
}

bool DataSerializer::hasCustomDeserializer(const std::string& name) const {
    std::lock_guard<std::mutex> lock(deserializersMutex_);
    return customDeserializers_.find(name) != customDeserializers_.end();
}

std::string DataSerializer::getFormatString(SerializationFormat format) const {
    switch (format) {
        case SerializationFormat::JSON:
            return "JSON";
        case SerializationFormat::XML:
            return "XML";
        case SerializationFormat::BINARY:
            return "BINARY";
        case SerializationFormat::CSV:
            return "CSV";
        case SerializationFormat::CUSTOM:
            return "CUSTOM";
        default:
            return "UNKNOWN";
    }
}

} // namespace data
} // namespace alt_las
