FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    unittests
    AST_tests.cpp
    Lexer_tests.cpp
    Render_tests.cpp
    Source_tests.cpp
    Unit_tests.cpp
    Error_tests.cpp
    Tokens_tests.cpp
)

target_link_libraries(
    unittests
    GTest::gtest_main
    vimformat
)

include(GoogleTest)

gtest_discover_tests(
    unittests
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    PROPERTIES TIMEOUT 10
)
