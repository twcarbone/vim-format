#include <regex>

#include "TokenSpec.h"

TokenSpec::TokenSpec()
{
    // Keyword
    push("^let", Token::Type::KEYWORD);

    // Operator
    push("^\\=", Token::Type::OP_ASSIGN);
    push("^\\*", Token::Type::OP_MUL);
    push("^\\/", Token::Type::OP_DIV);
    push("^\\+", Token::Type::PLUS);
    push("^\\-", Token::Type::MINUS);

    // Special
    push("^;", Token::Type::SEMICOLON);
    push("^\\(", Token::Type::L_PAREN);
    push("^\\)", Token::Type::R_PAREN);

    // String constant                  :h string
    push("^\"[^\"]*\"", Token::Type::STRING);

    // Literal string                   :h literal-string
    push("^'[^']*'", Token::Type::STRING);

    // Literal float                    :h floating-point-format
    push("^\\d\\.\\d+[Ee][\\+-]?\\d+", Token::Type::FLOAT);
    push("^\\d+\\.\\d+", Token::Type::FLOAT);

    // Literal integer                  :h expr-number
    push("^0[Xx]\\d+", Token::Type::INTEGER);
    push("^0[Oo]\\d+", Token::Type::INTEGER);
    push("^0[Bb][01]+", Token::Type::INTEGER);
    push("^\\d+", Token::Type::INTEGER);

    // Identifier
    push("^[a-zA-Z_]+", Token::Type::IDENTIFIER);

    // Other
    push("^\t", Token::Type::TAB);
    push("^ ", Token::Type::SPACE);
    push("^\n", Token::Type::NEWLINE);
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
