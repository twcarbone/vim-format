#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "Context.h"
#include "Node.h"
#include "Token.h"

class Lexer;

class Parser
{
public:
    Parser();
    Parser(const Context& context, std::vector<Token*> tokens);
    ~Parser();

    void parse();

    Node* root() const;

private:
    Node* m_pRoot;
    Token* m_pCurrToken;
    Source m_cSource;

    size_t m_nCurrTokenIdx { 0 };

    std::vector<Token*> m_lTokens;

    void program();
    void stmt_list(Node* parent);
    void stmt(Node* parent);
    void select_stmt(Node* parent);
    void iteration_stmt(Node* parent);
    void function_stmt(Node* parent);
    void arg_list(Node* parent);
    void list_expr(Node* parent);
    void expr1(Node* parent);
    void expr2(Node* parent);
    void expr3(Node* parent);
    void expr4(Node* parent);
    void expr5(Node* parent);
    void expr6(Node* parent);
    void expr7(Node* parent);
    void expr8(Node* parent);
    void expr9(Node* parent);
    void expr10(Node* parent);
    void expr11(Node* parent);

    void next();
    void consume(Node* parent, const Token::Type type);
    bool consume_optional(Node* parent, const Token::Type type);
};

#endif  // PARSER_H
