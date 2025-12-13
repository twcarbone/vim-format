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
    void stmt_list(Node* parent);
    void stmt(Node* parent);
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

    void consume(Node* parent, const Token::Type type);
};

#endif  // PARSER_H
