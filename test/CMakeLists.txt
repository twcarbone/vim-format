FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    unittests
    Parser_tests.cpp
    Lexer_tests.cpp
)

target_link_libraries(
    unittests
    GTest::gtest_main
    vimformat
)

include(GoogleTest)

gtest_discover_tests(unittests)
