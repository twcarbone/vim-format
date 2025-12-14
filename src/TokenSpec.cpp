#include <regex>
#include <string_view>

#include "TokenSpec.h"

TokenSpec::TokenSpec() :
    m_lDelimitedSpecKeys {
        // clang-format off
        "let",
        "echo",
        "set",
        "if",
        "elseif",
        "else",
        "endif",
        "while",
        "endwhile",
        "for",
        "in",
        "endfor",
        "break",
        "continue",
        "is",
        "isnot",
        // clang-format on
    },
    m_mDelimitedSpec {
        { "let", Token::Type::CMD_LET },
        { "echo", Token::Type::CMD_ECHO },
        { "set", Token::Type::CMD_SET },
        { "if", Token::Type::IF },
        { "elseif", Token::Type::ELSEIF },
        { "else", Token::Type::ELSE },
        { "endif", Token::Type::ENDIF },
        { "while", Token::Type::WHILE },
        { "endwhile", Token::Type::ENDWHILE },
        { "for", Token::Type::FOR },
        { "in", Token::Type::IN },
        { "endfor", Token::Type::ENDFOR },
        { "break", Token::Type::BREAK },
        { "continue", Token::Type::CONTINUE },
        { "is", Token::Type::OP_IS },
        { "isnot", Token::Type::OP_ISNOT },
    },
    m_lFixedWidthSpecKeys {
        // clang-format off
        " ",
        "\n",
        "\t",
        "||",
        ",",
        "]",
        "[",
        "??",
        "?",
        ">>",
        ">=",
        ">",
        "=~",
        "==",
        "=",
        "<=",
        "<<",
        "<",
        ":",
        "/=",
        "/",
        ".=",
        "..=",
        "..",
        ".",
        "-=",
        "-",
        "+=",
        "+",
        "*=",
        "*",
        ")",
        "(",
        "&&",
        "&",
        "%=",
        "%",
        "#",
        "!~",
        "!=",
        "!",
        // clang-format on
    },
    m_mFixedWidthSpec {
        { "!", Token::Type::OP_LOGICAL_NOT },  { "!", Token::Type::OP_LOGICAL_NOT },
        { "!=", Token::Type::OP_NEQUAL },      { "!~", Token::Type::OP_NMATCH },
        { "#", Token::Type::OP_MATCH_CASE },   { "%", Token::Type::OP_MODULO },
        { "%=", Token::Type::ASSIGN_MODULO },  { "&", Token::Type::OP_OPTION },
        { "&&", Token::Type::OP_AND },         { "(", Token::Type::L_PAREN },
        { ")", Token::Type::R_PAREN },         { "*", Token::Type::OP_MUL },
        { "*=", Token::Type::ASSIGN_MUL },     { "+", Token::Type::GEN_PLUS },
        { "+=", Token::Type::ASSIGN_ADD },     { "-", Token::Type::GEN_MINUS },
        { "-=", Token::Type::ASSIGN_MINUS },   { ".", Token::Type::OP_CAT_OLD },
        { "..", Token::Type::OP_CAT_NEW },     { "..=", Token::Type::ASSIGN_CAT_NEW },
        { ".=", Token::Type::ASSIGN_CAT_OLD }, { "/", Token::Type::OP_DIV },
        { "/=", Token::Type::ASSIGN_DIV },     { ":", Token::Type::OP_TERNARY_ELSE },
        { "<", Token::Type::OP_LT },           { "<<", Token::Type::OP_LSHIFT },
        { "<=", Token::Type::OP_LTE },         { "=", Token::Type::ASSIGN_EQ },
        { "==", Token::Type::OP_EQUAL },       { "=~", Token::Type::OP_MATCH },
        { ">", Token::Type::OP_GT },           { ">=", Token::Type::OP_GTE },
        { ">>", Token::Type::OP_RSHIFT },      { "?", Token::Type::GEN_QUESTION },
        { "??", Token::Type::OP_FALSEY },      { "[", Token::Type::L_BRACKET },
        { "]", Token::Type::R_BRACKET },       { ",", Token::Type::COMMA },
        { "||", Token::Type::OP_OR },          { " ", Token::Type::SPACE },
        { "\n", Token::Type::NEWLINE },        { "\t", Token::Type::TAB },
    }
{
    push("^\"[^\"]*\"", Token::Type::STRING);
    push("^'[^']*'", Token::Type::STRING);
    push("^\\d\\.\\d+[Ee][\\+-]?\\d+", Token::Type::FLOAT);
    push("^\\d+\\.\\d+", Token::Type::FLOAT);
    push("^0[Xx]\\d+", Token::Type::INTEGER);
    push("^0[Oo]\\d+", Token::Type::INTEGER);
    push("^0[Bb][01]+", Token::Type::INTEGER);
    push("^\\d+", Token::Type::INTEGER);
    push("^[a-zA-Z_][a-zA-Z0-9_]*", Token::Type::GEN_NAME);
}

TokenSpec::~TokenSpec()
{
}

Token* TokenSpec::match(const std::string& asText)
{
    for (const std::string& lsKey : m_lDelimitedSpecKeys)
    {
        if (startswith(asText, lsKey + " "))
        {
            return new Token(m_mDelimitedSpec.at(lsKey), lsKey);
        }
    }

    for (const std::string& lsKey : m_lFixedWidthSpecKeys)
    {
        if (startswith(asText, lsKey))
        {
            return new Token(m_mFixedWidthSpec.at(lsKey), lsKey);
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
