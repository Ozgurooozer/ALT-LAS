# Veritabanı entegrasyonu için CMakeLists.txt

# Veritabanı entegrasyonu kaynak dosyaları
set(DATABASE_SOURCES
    src/database_connection.cpp
    src/database_query.cpp
    src/database_result.cpp
    src/database_manager.cpp
)

# Veritabanı entegrasyonu başlık dosyaları
set(DATABASE_HEADERS
    include/database/database_connection.h
    include/database/database_query.h
    include/database/database_result.h
    include/database/database_manager.h
)

# Veritabanı entegrasyonu kütüphanesi
add_library(alt_las_database STATIC ${DATABASE_SOURCES} ${DATABASE_HEADERS})

# Kütüphane için include dizinleri
target_include_directories(alt_las_database PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${NLOHMANN_JSON_INCLUDE_DIRS}
)

# Kütüphane için bağımlılıklar
target_link_libraries(alt_las_database
    alt_las_common
    alt_las_data
    ${CMAKE_THREAD_LIBS_INIT}
)

# SQLite bağımlılığı
find_package(SQLite3)
if(SQLite3_FOUND)
    target_include_directories(alt_las_database PUBLIC ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(alt_las_database ${SQLite3_LIBRARIES})
    target_compile_definitions(alt_las_database PUBLIC HAVE_SQLITE3)
endif()

# MySQL bağımlılığı
find_package(MySQL)
if(MySQL_FOUND)
    target_include_directories(alt_las_database PUBLIC ${MySQL_INCLUDE_DIRS})
    target_link_libraries(alt_las_database ${MySQL_LIBRARIES})
    target_compile_definitions(alt_las_database PUBLIC HAVE_MYSQL)
endif()

# PostgreSQL bağımlılığı
find_package(PostgreSQL)
if(PostgreSQL_FOUND)
    target_include_directories(alt_las_database PUBLIC ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(alt_las_database ${PostgreSQL_LIBRARIES})
    target_compile_definitions(alt_las_database PUBLIC HAVE_POSTGRESQL)
endif()

# MongoDB bağımlılığı
find_package(MongoDB)
if(MongoDB_FOUND)
    target_include_directories(alt_las_database PUBLIC ${MongoDB_INCLUDE_DIRS})
    target_link_libraries(alt_las_database ${MongoDB_LIBRARIES})
    target_compile_definitions(alt_las_database PUBLIC HAVE_MONGODB)
endif()

# Veritabanı entegrasyonu testleri
if(BUILD_TESTS)
    # Veritabanı entegrasyonu test kaynak dosyaları
    set(DATABASE_TEST_SOURCES
        tests/database_connection_test.cpp
        tests/database_query_test.cpp
        tests/database_result_test.cpp
        tests/database_manager_test.cpp
    )

    # Veritabanı entegrasyonu test hedefi
    add_executable(test_alt_las_database ${DATABASE_TEST_SOURCES})

    # Test hedefi için include dizinleri
    target_include_directories(test_alt_las_database PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIRS}
        ${NLOHMANN_JSON_INCLUDE_DIRS}
    )

    # Test hedefi için bağımlılıklar
    target_link_libraries(test_alt_las_database
        alt_las_database
        alt_las_data
        alt_las_common
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )

    # SQLite bağımlılığı
    if(SQLite3_FOUND)
        target_include_directories(test_alt_las_database PUBLIC ${SQLite3_INCLUDE_DIRS})
        target_link_libraries(test_alt_las_database ${SQLite3_LIBRARIES})
        target_compile_definitions(test_alt_las_database PUBLIC HAVE_SQLITE3)
    endif()

    # MySQL bağımlılığı
    if(MySQL_FOUND)
        target_include_directories(test_alt_las_database PUBLIC ${MySQL_INCLUDE_DIRS})
        target_link_libraries(test_alt_las_database ${MySQL_LIBRARIES})
        target_compile_definitions(test_alt_las_database PUBLIC HAVE_MYSQL)
    endif()

    # PostgreSQL bağımlılığı
    if(PostgreSQL_FOUND)
        target_include_directories(test_alt_las_database PUBLIC ${PostgreSQL_INCLUDE_DIRS})
        target_link_libraries(test_alt_las_database ${PostgreSQL_LIBRARIES})
        target_compile_definitions(test_alt_las_database PUBLIC HAVE_POSTGRESQL)
    endif()

    # MongoDB bağımlılığı
    if(MongoDB_FOUND)
        target_include_directories(test_alt_las_database PUBLIC ${MongoDB_INCLUDE_DIRS})
        target_link_libraries(test_alt_las_database ${MongoDB_LIBRARIES})
        target_compile_definitions(test_alt_las_database PUBLIC HAVE_MONGODB)
    endif()

    # Test hedefini test listesine ekle
    add_test(NAME test_alt_las_database COMMAND test_alt_las_database)
endif()
