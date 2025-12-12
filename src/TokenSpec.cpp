#include <regex>

#include "TokenSpec.h"

TokenSpec::TokenSpec()
{
    push("^let\\b", Token::Type::CMD_LET);
    push("^echo\\b", Token::Type::CMD_ECHO);
    push("^set\\b", Token::Type::CMD_SET);

    push("^if\\b", Token::Type::IF);
    push("^elseif\\b", Token::Type::ELSEIF);
    push("^else\\b", Token::Type::ELSE);
    push("^endif\\b", Token::Type::ENDIF);
    push("^while\\b", Token::Type::WHILE);
    push("^endwhile\\b", Token::Type::ENDWHILE);
    push("^for\\b", Token::Type::FOR);
    push("^in\\b", Token::Type::IN);
    push("^endfor\\b", Token::Type::ENDFOR);
    push("^break\\b", Token::Type::BREAK);
    push("^continue\\b", Token::Type::CONTINUE);

    push("^\\+=", Token::Type::ASSIGN_ADD);
    push("^-=", Token::Type::ASSIGN_MINUS);
    push("^\\*=", Token::Type::ASSIGN_MUL);
    push("^/=", Token::Type::ASSIGN_DIV);
    push("^=", Token::Type::ASSIGN_EQ);
    push("^%=", Token::Type::ASSIGN_MODULO);
    push("^\\.\\.=", Token::Type::ASSIGN_CAT_NEW);
    push("^\\.=", Token::Type::ASSIGN_CAT_OLD);

    push("^\\?\\?", Token::Type::OP_FALSEY);

    push("^\\+", Token::Type::GEN_PLUS);
    push("^-", Token::Type::GEN_MINUS);
    push("^\\?", Token::Type::GEN_QUESTION);

    push("^\\*", Token::Type::OP_MUL);
    push("^/", Token::Type::OP_DIV);
    push("^\\%", Token::Type::OP_MODULO);
    push("^==", Token::Type::OP_EQUAL);
    push("^!=", Token::Type::OP_NEQUAL);
    push("^<<", Token::Type::OP_LSHIFT);
    push("^>>", Token::Type::OP_RSHIFT);
    push("^>=", Token::Type::OP_GTE);
    push("^<=", Token::Type::OP_LTE);
    push("^>", Token::Type::OP_GT);
    push("^<", Token::Type::OP_LT);
    push("^=~", Token::Type::OP_MATCH);
    push("^!~", Token::Type::OP_NMATCH);
    push("^isnot\\b", Token::Type::OP_ISNOT);
    push("^is\\b", Token::Type::OP_IS);
    push("^\\|\\|", Token::Type::OP_OR);
    push("^&&", Token::Type::OP_AND);
    push("^#", Token::Type::OP_MATCH_CASE);
    push("^:", Token::Type::OP_TERNARY_ELSE);
    push("^\\.\\.", Token::Type::OP_CAT_NEW);
    push("^\\.", Token::Type::OP_CAT_OLD);
    push("^!", Token::Type::OP_LOGICAL_NOT);
    push("^&", Token::Type::OP_OPTION);

    push("^\\(", Token::Type::L_PAREN);
    push("^\\)", Token::Type::R_PAREN);
    push("^\\[", Token::Type::L_BRACKET);
    push("^\\]", Token::Type::R_BRACKET);
    push("^,", Token::Type::COMMA);

    push("^\"[^\"]*\"", Token::Type::STRING);
    push("^'[^']*'", Token::Type::STRING);

    push("^\\d\\.\\d+[Ee][\\+-]?\\d+", Token::Type::FLOAT);
    push("^\\d+\\.\\d+", Token::Type::FLOAT);

    push("^0[Xx]\\d+", Token::Type::INTEGER);
    push("^0[Oo]\\d+", Token::Type::INTEGER);
    push("^0[Bb][01]+", Token::Type::INTEGER);
    push("^\\d+", Token::Type::INTEGER);

    push("^\t", Token::Type::TAB);
    push("^ ", Token::Type::SPACE);
    push("^\n", Token::Type::NEWLINE);

    push("^[a-zA-Z_][a-zA-Z0-9_]*", Token::Type::GEN_NAME);

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
