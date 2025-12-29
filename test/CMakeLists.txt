FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    unittests
    Lexer_tests.cpp
    Parser_tests.cpp
    PrattParser_tests.cpp
    Source_tests.cpp
    Translator_tests.cpp
)

target_link_libraries(
    unittests
    GTest::gtest_main
    vimformat
)

include(GoogleTest)

gtest_discover_tests(unittests)
