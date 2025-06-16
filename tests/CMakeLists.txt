file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(runTests ${TEST_SOURCES})
target_link_libraries(runTests PRIVATE GTest::gtest_main tictactoe_lib)

include(GoogleTest)
gtest_discover_tests(runTests)
