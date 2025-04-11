# Hata yönetimi için CMakeLists.txt

# Hata yönetimi kaynak dosyaları
set(ERROR_SOURCES
    src/error_handler.cpp
    src/error_logger.cpp
    src/error_reporter.cpp
    src/error_manager.cpp
)

# Hata yönetimi başlık dosyaları
set(ERROR_HEADERS
    include/error/error_handler.h
    include/error/error_logger.h
    include/error/error_reporter.h
    include/error/error_manager.h
)

# Hata yönetimi kütüphanesi
add_library(alt_las_error STATIC ${ERROR_SOURCES} ${ERROR_HEADERS})

# Kütüphane için include dizinleri
target_include_directories(alt_las_error PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${NLOHMANN_JSON_INCLUDE_DIRS}
)

# Kütüphane için bağımlılıklar
target_link_libraries(alt_las_error
    alt_las_common
    ${CMAKE_THREAD_LIBS_INIT}
)

# Windows için DbgHelp kütüphanesini ekle
if(WIN32)
    target_link_libraries(alt_las_error dbghelp)
endif()

# Hata yönetimi testleri
if(BUILD_TESTS)
    # Hata yönetimi test kaynak dosyaları
    set(ERROR_TEST_SOURCES
        tests/error_handler_test.cpp
        tests/error_logger_test.cpp
        tests/error_reporter_test.cpp
        tests/error_manager_test.cpp
    )

    # Hata yönetimi test hedefi
    add_executable(test_alt_las_error ${ERROR_TEST_SOURCES})

    # Test hedefi için include dizinleri
    target_include_directories(test_alt_las_error PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIRS}
        ${NLOHMANN_JSON_INCLUDE_DIRS}
    )

    # Test hedefi için bağımlılıklar
    target_link_libraries(test_alt_las_error
        alt_las_error
        alt_las_common
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )

    # Windows için DbgHelp kütüphanesini ekle
    if(WIN32)
        target_link_libraries(test_alt_las_error dbghelp)
    endif()

    # Test hedefini test listesine ekle
    add_test(NAME test_alt_las_error COMMAND test_alt_las_error)
endif()
