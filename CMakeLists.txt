cmake_minimum_required(VERSION 3.10)
project(TicTacToe VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# Fetch SQLiteCpp
FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG 3.3.1
)
set(SQLITECPP_INTERNAL_SQLITE ON CACHE BOOL "Build SQLite3 internally")

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Make both available
FetchContent_MakeAvailable(SQLiteCpp googletest)

# Enable testing
enable_testing()

# Include your source and tests folders
add_subdirectory(src)
add_subdirectory(tests)
