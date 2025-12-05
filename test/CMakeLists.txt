FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    unittests
    Tokenizer_tests.cpp
)

target_link_libraries(
    unittests
    GTest::gtest_main
    vimformat
)

include(GoogleTest)

gtest_discover_tests(unittests)
