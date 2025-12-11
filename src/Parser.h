#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "Node.h"
#include "Token.h"

class Tokenizer;

class Parser
{
public:
    Parser();
    ~Parser();

    void parse(const std::string& program);

    Node* root() const;

private:
    Node* m_pRoot;
    Token* m_pCurrToken;
    Tokenizer* m_pTokenizer;

    void program();
    void statement(Node* parent);
    void statement_list(Node* parent);
    void multiplicative_expr(Node* parent);
    void additive_expr(Node* parent);
    void factor(Node* parent);

    void consume(Node* parent, const Token::Type type);
};

#endif  // PARSER_H
