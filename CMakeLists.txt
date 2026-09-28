cmake_minimum_required(VERSION 3.10)
project(HttpServer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# Отключаем поддержку OpenSSL в httplib
add_compile_definitions(CPPHTTPLIB_OPENSSL_SUPPORT=0)

# Подключаем nlohmann/json вручную
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE /opt/vcpkg/installed/x64-linux/include)

add_executable(my_server src/main.cpp)

target_include_directories(my_server PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/httplib
)

find_package(OpenSSL REQUIRED)
target_link_libraries(my_server
    Threads::Threads
    nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
)