#ifndef TOKENSPEC_H
#define TOKENSPEC_H

#include <map>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Source.h"
#include "Token.h"

class TokenSpec
{
public:
    TokenSpec();
    ~TokenSpec();

    Token* match(const Source& source);

private:
    struct Keyword
    {
        std::string sFull = "";
        std::string sAbrv = "";
        Token::Type eTokenType = Token::Type::NONE;
    };

    struct Command : Keyword
    {
    };

    // Punctuators, operators, or other fixed-width symbols
    struct Symbol
    {
        std::string sLexeme = "";
        Token::Type eTokenType = Token::Type::NONE;
    };

    const std::vector<Command> m_lCommands;
    const std::vector<Keyword> m_lKeywords;
    const std::vector<Symbol> m_lSymbols;
    const std::vector<std::pair<std::regex, Token::Type> > m_lReSpec;
};

#endif  // TOKENSPEC_H
