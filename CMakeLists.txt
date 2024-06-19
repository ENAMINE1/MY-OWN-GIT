cmake_minimum_required(VERSION 3.10)

# Set the project name
project(git_c)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCES
    src/CPP/main.cpp
    src/CPP/git_init.cpp
    src/CPP/hash_object.cpp
    src/CPP/ls_tree.cpp
    src/CPP/cat_file.cpp
    src/CPP/write_tree.cpp
    src/CPP/utils.cpp
    src/CPP/git_add.cpp
)

# Add header files
set(HEADERS
    src/Headers/git_init.h
    src/Headers/hash_object.h
    src/Headers/ls_tree.h
    src/Headers/cat_file.h
    src/Headers/write_tree.h
    src/Headers/utils.h
    src/Headers/git_add.h
)

# Include directories
include_directories(src/Headers)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL ZLIB::ZLIB)
