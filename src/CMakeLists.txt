add_library(
    vimformat
    STATIC
    Context.cpp
    Exceptions.cpp
    Lexer.cpp
    Node.cpp
    NodeVisitor.cpp
    Parser.cpp
    Settings.cpp
    Token.cpp
    TokenSpec.cpp
)

add_executable(
    vim-format
    main.cpp
)

target_include_directories(vimformat PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(vim-format PUBLIC vimformat)
