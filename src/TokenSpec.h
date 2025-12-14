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
    const std::vector<std::pair<std::regex, Token::Type> > m_lReSpec;

    const std::vector<std::string> m_lDelimitedSpecKeys;
    const std::map<std::string, Token::Type> m_mDelimitedSpec;

    const std::vector<std::string> m_lFixedWidthSpecKeys;
    const std::map<std::string, Token::Type> m_mFixedWidthSpec;

    static bool startswith(std::string_view str, std::string_view prefix);
};

#endif  // TOKENSPEC_H
