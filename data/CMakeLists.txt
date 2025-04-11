# Veri yönetimi için CMakeLists.txt

# Veri yönetimi kaynak dosyaları
set(DATA_SOURCES
    src/data_serializer.cpp
    src/data_validator.cpp
    src/data_cache.cpp
    src/data_manager.cpp
)

# Veri yönetimi başlık dosyaları
set(DATA_HEADERS
    include/data/data_serializer.h
    include/data/data_validator.h
    include/data/data_cache.h
    include/data/data_manager.h
)

# Veri yönetimi kütüphanesi
add_library(alt_las_data STATIC ${DATA_SOURCES} ${DATA_HEADERS})

# Kütüphane için include dizinleri
target_include_directories(alt_las_data PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${NLOHMANN_JSON_INCLUDE_DIRS}
)

# Kütüphane için bağımlılıklar
target_link_libraries(alt_las_data
    alt_las_common
    ${CMAKE_THREAD_LIBS_INIT}
)

# Veri yönetimi testleri
if(BUILD_TESTS)
    # Veri yönetimi test kaynak dosyaları
    set(DATA_TEST_SOURCES
        tests/data_serializer_test.cpp
        tests/data_validator_test.cpp
        tests/data_cache_test.cpp
        tests/data_manager_test.cpp
    )

    # Veri yönetimi test hedefi
    add_executable(test_alt_las_data ${DATA_TEST_SOURCES})

    # Test hedefi için include dizinleri
    target_include_directories(test_alt_las_data PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIRS}
        ${NLOHMANN_JSON_INCLUDE_DIRS}
    )

    # Test hedefi için bağımlılıklar
    target_link_libraries(test_alt_las_data
        alt_las_data
        alt_las_common
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )

    # Test hedefini test listesine ekle
    add_test(NAME test_alt_las_data COMMAND test_alt_las_data)
endif()
