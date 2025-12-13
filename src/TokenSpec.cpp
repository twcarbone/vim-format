#include <regex>
#include <string_view>

#include "TokenSpec.h"

TokenSpec::TokenSpec()
{
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

    push("^[a-zA-Z_][a-zA-Z0-9_]*", Token::Type::GEN_NAME);

    m_mSpec.insert({"let", Token::Type::CMD_LET});
    m_mSpec.insert({"echo", Token::Type::CMD_ECHO});
    m_mSpec.insert({"set", Token::Type::CMD_SET});
    m_mSpec.insert({"if", Token::Type::IF});
    m_mSpec.insert({"elseif", Token::Type::ELSEIF});
    m_mSpec.insert({"else", Token::Type::ELSE});
    m_mSpec.insert({"endif", Token::Type::ENDIF});
    m_mSpec.insert({"while", Token::Type::WHILE});
    m_mSpec.insert({"endwhile", Token::Type::ENDWHILE});
    m_mSpec.insert({"for", Token::Type::FOR});
    m_mSpec.insert({"in", Token::Type::IN});
    m_mSpec.insert({"endfor", Token::Type::ENDFOR});
    m_mSpec.insert({"break", Token::Type::BREAK});
    m_mSpec.insert({"continue", Token::Type::CONTINUE});
    m_mSpec.insert({"is", Token::Type::OP_IS});
    m_mSpec.insert({"isnot", Token::Type::OP_ISNOT});

    m_lSpecKeys.reserve(m_mSpec.size());

    for (const auto& [key, value] : m_mSpec)
    {
        m_lSpecKeys.push_back(key);
    }
}

TokenSpec::~TokenSpec()
{
}

Token* TokenSpec::match(const std::string& asText)
{
    if (startswith(asText, " "))
    {
        return new Token(Token::Type::SPACE, " ");
    }
    else if (startswith(asText, "\n"))
    {
        return new Token(Token::Type::NEWLINE, "\n");
    }
    else if (startswith(asText, "\t"))
    {
        return new Token(Token::Type::TAB, "\t");
    }

    for (const std::string& lsKey : m_lSpecKeys)
    {
        if (startswith(asText, lsKey + " "))
        {
            return new Token(m_mSpec.at(lsKey), lsKey);
        }
    }

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

bool TokenSpec::startswith(std::string_view asStr, std::string_view asPrefix) const
{
    return asStr.substr(0, asPrefix.size()) == asPrefix;
}

void TokenSpec::push(const std::string& asRe, const Token::Type aeType)
{
    m_lSpec.push_back(std::pair<std::regex, Token::Type> { asRe, aeType });
}
