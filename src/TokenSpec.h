#ifndef TOKENSPEC_H
#define TOKENSPEC_H

#include <map>
#include <regex>
#include <string>
#include <string_view>
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

    std::map<std::string, Token::Type> m_mSpec;
    std::vector<std::string> m_lSpecKeys;

    bool startswith(std::string_view str, std::string_view prefix) const;
    void push(const std::string& re, const Token::Type type);
};

#endif  // TOKENSPEC_H
