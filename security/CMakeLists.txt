# Güvenlik için CMakeLists.txt

# Güvenlik kaynak dosyaları
set(SECURITY_SOURCES
    src/security_policy.cpp
    src/security_checker.cpp
    src/security_logger.cpp
    src/security_manager.cpp
)

# Güvenlik başlık dosyaları
set(SECURITY_HEADERS
    include/security/security_policy.h
    include/security/security_checker.h
    include/security/security_logger.h
    include/security/security_manager.h
)

# Güvenlik kütüphanesi
add_library(alt_las_security STATIC ${SECURITY_SOURCES} ${SECURITY_HEADERS})

# Kütüphane için include dizinleri
target_include_directories(alt_las_security PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${NLOHMANN_JSON_INCLUDE_DIRS}
)

# Kütüphane için bağımlılıklar
target_link_libraries(alt_las_security
    alt_las_common
    ${CMAKE_THREAD_LIBS_INIT}
)

# Güvenlik testleri
if(BUILD_TESTS)
    # Güvenlik test kaynak dosyaları
    set(SECURITY_TEST_SOURCES
        tests/security_policy_test.cpp
        tests/security_checker_test.cpp
        tests/security_logger_test.cpp
        tests/security_manager_test.cpp
    )

    # Güvenlik test hedefi
    add_executable(test_alt_las_security ${SECURITY_TEST_SOURCES})

    # Test hedefi için include dizinleri
    target_include_directories(test_alt_las_security PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIRS}
        ${NLOHMANN_JSON_INCLUDE_DIRS}
    )

    # Test hedefi için bağımlılıklar
    target_link_libraries(test_alt_las_security
        alt_las_security
        alt_las_common
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )

    # Test hedefini test listesine ekle
    add_test(NAME test_alt_las_security COMMAND test_alt_las_security)
endif()
