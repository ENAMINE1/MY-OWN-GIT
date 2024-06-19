cmake_minimum_required(VERSION 3.10)
project(GitClone)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

add_executable(git_clone server.cpp)
target_link_libraries(git_clone OpenSSL::SSL OpenSSL::Crypto ${ZLIB_LIBRARIES})
