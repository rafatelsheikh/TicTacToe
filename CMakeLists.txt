cmake_minimum_required(VERSION 3.10)
project(TicTacToe
        VERSION 1.0
        LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# where to put build output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# list your source files
set(SOURCES
    src/exceptions.cpp
    src/Model.cpp
    src/Human.cpp
    src/AI.cpp
    src/Controller.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# (optional) Show all warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
)
