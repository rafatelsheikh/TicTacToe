set(LIB_SOURCES
    exceptions.cpp
    Model.cpp
    Human.cpp
    AI.cpp
    Controller.cpp
    database.cpp
)

add_library(tictactoe_lib ${LIB_SOURCES})

target_include_directories(tictactoe_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/SQLiteCpp/include
)

# SQLiteCpp might depend on sqlite3 directly, so make sure it's linked
target_link_libraries(tictactoe_lib PRIVATE SQLiteCpp sqlite3)

add_executable(TicTacToe main.cpp)
target_link_libraries(TicTacToe PRIVATE tictactoe_lib)

target_compile_options(TicTacToe PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
)
