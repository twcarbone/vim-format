#ifndef TOKENSPEC_H
#define TOKENSPEC_H

#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "Token.h"

class TokenSpec
{
public:
    TokenSpec();
    ~TokenSpec();

    Token* match(const std::string& text);

private:
    std::vector<std::pair<std::regex, Token::Type> > m_lSpec;

    void push(const std::string& re, const Token::Type type);
};

#endif  // TOKENSPEC_H
