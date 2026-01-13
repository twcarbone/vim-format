add_library(
    vimformat
    STATIC
    ASTVisitor.cpp
    Analyzer.cpp
    Context.cpp
    Controller.cpp
    Exceptions.cpp
    ExprDisplayVisitor.cpp
    HybridParser.cpp
    Lexer.cpp
    Node.cpp
    NodeVisitor.cpp
    Parser.cpp
    PrattParser.cpp
    Scope.cpp
    Settings.cpp
    Source.cpp
    Symbol.cpp
    Token.cpp
    TokenSpec.cpp
    ast.cpp
)

add_executable(
    vim-format
    main.cpp
)

target_include_directories(vimformat PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(vim-format PUBLIC vimformat)
