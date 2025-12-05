cmake_minimum_required(VERSION 3.22.1)

project(vim-format VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
    vim-format
    main.cpp
    Token.cpp
    Tokenizer.cpp
    TokenSpec.cpp
)
