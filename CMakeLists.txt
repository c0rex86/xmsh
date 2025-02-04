cmake_minimum_required(VERSION 3.10)
project(xmsh VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(xmsh_server src/server/main.cpp src/server/server.cpp)
add_executable(xmsh_client src/client/main.cpp src/client/client.cpp)

target_include_directories(xmsh_server PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(xmsh_client PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(xmsh_server PRIVATE OpenSSL::SSL OpenSSL::Crypto Threads::Threads)
target_link_libraries(xmsh_client PRIVATE OpenSSL::SSL OpenSSL::Crypto Threads::Threads) 