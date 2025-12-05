#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "Token.h"

class Tokenizer;
class Token;

struct Node
{
    Token cToken { Token() };
    Node* pBody { nullptr };
};

class Parser
{
public:
    Parser();
    ~Parser();

    Node* parse(const std::string& program);

private:
    Tokenizer* m_pTokenizer;
    Token* m_pLookAheadToken;

    Node* Program();
    Node* StatementList();
    Node* Statement();
    Node* ExpressionStatement();
    Node* Expression();
    Node* Literal();
    Node* IntegerLiteral();
    Node* StringLiteral();

    const Token* consume(Token::Type type);
};

#endif  // PARSER_H
