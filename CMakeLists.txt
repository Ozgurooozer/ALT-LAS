cmake_minimum_required(VERSION 3.20)
project(ALT-LAS VERSION 1.0.0 LANGUAGES CXX)

# Ana exe sürüm bilgisi
set(ALT_LAS_VERSION_MAJOR 1)
set(ALT_LAS_VERSION_MINOR 0)
set(ALT_LAS_VERSION_PATCH 0)
set(ALT_LAS_VERSION "${ALT_LAS_VERSION_MAJOR}.${ALT_LAS_VERSION_MINOR}.${ALT_LAS_VERSION_PATCH}")
set(ALT_LAS_VERSION_STRING "ALT-LAS v${ALT_LAS_VERSION}")

# C++17 standardını kullan
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UNICODE tanımları
add_definitions(-DUNICODE -D_UNICODE)

# UI framework seçeneği
# NOT: wxWidgets kaldırıldı, Rust Druid arayüzü kullanılıyor
# option(USE_WIN32_API "Use Win32 API instead of wxWidgets" ON) - Artık kullanılmıyor

# wxWidgets kaldırıldı, Rust Druid arayüzü kullanılıyor
# wxWidgets kütüphanesini bul (eğer Win32 API kullanılmıyorsa)
# if(NOT USE_WIN32_API)
#     include(wxWidgets.cmake)
#     include_directories(${wxWidgets_INCLUDE_DIRS})
# endif()

# Derleme modunu ayarla
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Optimizasyon bayrakları
if(MSVC)
    # MSVC için optimizasyon bayrakları (sadece Release modunda)
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "/W4 /O2 /Ob2 /Oi /Ot /GL")

    # Link-time optimizasyonu etkinleştir (sadece Release modunda)
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "/LTCG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/LTCG")

    # LNK4075 uyarısını gidermek için /INCREMENTAL:NO ekle
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /INCREMENTAL:NO")

    # Debug modunda /INCREMENTAL kullan
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL")
else()
    # GCC/Clang için optimizasyon bayrakları (sadece Release modunda)
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -pedantic -O3 -march=native -flto")
    # Link-time optimizasyonu etkinleştir (sadece Release modunda)
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "-flto")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-flto")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto")
endif()

# Derleme çıktı dizinlerini ayarla
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# OpenCV yapılandırması
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/opencv-install")
find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    message(STATUS "OpenCV found at ${OpenCV_DIR}")
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
else()
    # OpenCV bulunamadı, sahte bir OpenCV hedefi oluştur
    message(WARNING "OpenCV not found. Creating a stub OpenCV target.")
    add_library(opencv_core INTERFACE)
    add_library(opencv_imgproc INTERFACE)
    add_library(opencv_highgui INTERFACE)
    add_library(opencv_videoio INTERFACE)
    add_library(opencv_imgcodecs INTERFACE)
    add_library(opencv_objdetect INTERFACE)
    add_library(opencv_dnn INTERFACE)

    # Sahte include dizini
    set(OPENCV_STUB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/opencv-install/include")
    file(MAKE_DIRECTORY ${OPENCV_STUB_INCLUDE_DIR})
    file(MAKE_DIRECTORY "${OPENCV_STUB_INCLUDE_DIR}/opencv2")

    # Sahte opencv2/opencv.hpp dosyası oluştur
    file(WRITE "${OPENCV_STUB_INCLUDE_DIR}/opencv2/opencv.hpp" "// Stub OpenCV header\n")

    # Sahte include dizinini ekle
    include_directories(${OPENCV_STUB_INCLUDE_DIR})

    # OpenCV_LIBS değişkenini ayarla
    set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_videoio opencv_imgcodecs opencv_objdetect opencv_dnn)

    message(STATUS "Created stub OpenCV libraries: ${OpenCV_LIBS}")
endif()

# GTest yapılandırması
set(GTEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest)
set(GTEST_INCLUDE_DIR ${GTEST_ROOT}/googletest/include)
set(GTEST_LIBRARY_DIR ${GTEST_ROOT}/build/lib)

# nlohmann/json kütüphanesini ekle
set(NLOHMANN_JSON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/json/include")
include_directories(${NLOHMANN_JSON_INCLUDE_DIRS})

# GTest kütüphanelerinin tam yollarını belirt
if(WIN32)
    set(GTEST_LIBRARY ${GTEST_LIBRARY_DIR}/Release/gtest.lib)
    set(GTEST_MAIN_LIBRARY ${GTEST_LIBRARY_DIR}/Release/gtest_main.lib)
else()
    set(GTEST_LIBRARY ${GTEST_LIBRARY_DIR}/libgtest.a)
    set(GTEST_MAIN_LIBRARY ${GTEST_LIBRARY_DIR}/libgtest_main.a)
endif()

# nlohmann/json kütüphanesini ekle
set(NLOHMANN_JSON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/json/include")
include_directories(${NLOHMANN_JSON_INCLUDE_DIRS})

# Alt projeleri ekle
add_subdirectory(common)
add_subdirectory(screen_capture)
add_subdirectory(automation)
add_subdirectory(integration)
add_subdirectory(main_app)
add_subdirectory(simple_test)
add_subdirectory(simple_win32_app)
add_subdirectory(ui/debug_ui)
add_subdirectory(ui/ai_chat_interface)
add_subdirectory(ui/rust_druid_interface)
add_subdirectory(mod_system)
# Şimdilik plugin_system'i devre dışı bırakıyoruz
# add_subdirectory(plugin_system)

# Veri yönetimi ve veritabanı entegrasyonu
add_subdirectory(data)
add_subdirectory(database)

# Güvenlik ve hata yönetimi
add_subdirectory(security)
add_subdirectory(error)



# Ana exe oluştur
add_executable(alt_las_main src/main.cpp)
target_link_libraries(alt_las_main
    PRIVATE
    common
    screen_capture
    automation
    integration
    main_app
)

# Ana exe için sürüm bilgisi ekle
target_compile_definitions(alt_las_main
    PRIVATE
    ALT_LAS_VERSION_MAJOR=${ALT_LAS_VERSION_MAJOR}
    ALT_LAS_VERSION_MINOR=${ALT_LAS_VERSION_MINOR}
    ALT_LAS_VERSION_PATCH=${ALT_LAS_VERSION_PATCH}
    ALT_LAS_VERSION_STRING="${ALT_LAS_VERSION_STRING}"
)

# Kurulum paketi oluşturma - Temel ayarlar
set(CPACK_PACKAGE_NAME "ALT-LAS")
set(CPACK_PACKAGE_VENDOR "ALT-LAS")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Advanced Learning and Task Automation System")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ALT-LAS")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/installer/README.txt")

# Windows kurulum paketi ayarları
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "ALT-LAS")
    set(CPACK_NSIS_DISPLAY_NAME "ALT-LAS")
    set(CPACK_NSIS_HELP_LINK "https://www.alt-las.com/support")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://www.alt-las.com")
    set(CPACK_NSIS_CONTACT "info@alt-las.com")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    # Gerçek bir .ico dosyası kullanılmalıdır
    # Geçici olarak varsayılan simgeler kullanılacak
    # set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/installer/alt-las.ico")
    # set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/installer/alt-las.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\alt_las.exe")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "alt_las.exe")
    set(CPACK_NSIS_MENU_LINKS
        "docs/index.html" "Documentation"
        "https://www.alt-las.com" "ALT-LAS Website")
endif()

# CPack'i dahil et
include(CPack)

# Python ve Rust modülleri için yapılandırma
# NOT: Bu modüller ayrı olarak derlenir ve entegre edilir

# Derleme seçenekleri
option(BUILD_TESTS "Build test programs" ON)
option(USE_CUDA "Use CUDA for acceleration" OFF)

# GTest yapılandırması
if(BUILD_TESTS)
    # GTest'in external dizininde olup olmadığını kontrol et
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest")
        set(GTEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest)
        set(GTEST_INCLUDE_DIR ${GTEST_ROOT}/googletest/include ${GTEST_ROOT}/googlemock/include)
        set(GTEST_LIBRARY_DIR ${GTEST_ROOT}/build/lib/Release)
        set(GTEST_LIBRARIES gtest gtest_main gmock gmock_main)

        # GTest include dizinlerini ekle
        include_directories(${GTEST_INCLUDE_DIR})
        link_directories(${GTEST_LIBRARY_DIR})

        message(STATUS "GTest found at ${GTEST_ROOT}")
        message(STATUS "GTest include directories: ${GTEST_INCLUDE_DIR}")
        message(STATUS "GTest library directories: ${GTEST_LIBRARY_DIR}")
    endif()

    # Test programlarını derle
    enable_testing()
    add_subdirectory(tests)
endif()

# CUDA desteği
if(USE_CUDA)
    # Geçici olarak devre dışı bırakıldı
    # find_package(CUDA 11.8 REQUIRED)
    # add_definitions(-DUSE_CUDA)
    # include_directories(${CUDA_INCLUDE_DIRS})
endif()

# Kurulum hedefleri
install(TARGETS simple_test alt_las_main
    RUNTIME DESTINATION bin
)

# Dokümantasyon dosyalarını kopyala
install(FILES README.md LICENSE
    DESTINATION share/alt_las
)

# Sürüm bilgisi
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ALT-LASConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
