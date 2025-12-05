#include <regex>

#include "TokenSpec.h"

TokenSpec::TokenSpec()
{
    // Keyword
    push("^let", Token::Type::KEYWORD_NONE);

    // Operator
    push("^\\=", Token::Type::OPERATOR_ASSIGNMENT);

    // Special
    push("^;", Token::Type::SPECIAL_SEMICOLON);

    // Literal
    push("^\\d+", Token::Type::LITERAL_INTEGER);

    // Identifier
    push("^[a-zA-Z_]", Token::Type::IDENTIFIER_NONE);

    // Other
    push("^\\s+", Token::Type::WHITESPACE);
}

TokenSpec::~TokenSpec()
{
}

Token* TokenSpec::match(const std::string& asText)
{
    for (auto it = m_lSpec.cbegin(); it != m_lSpec.cend(); ++it)
    {
        const std::regex& lcRe = it->first;
        const Token::Type& leTokenType = it->second;

        std::smatch lcMatch;
        if (std::regex_search(asText, lcMatch, lcRe))
        {
            return new Token(leTokenType, lcMatch.str());
        }
    }

    return nullptr;
}

void TokenSpec::push(const std::string& asRe, const Token::Type aeType)
{
    m_lSpec.push_back(std::pair<std::regex, Token::Type> { asRe, aeType });
}
