#include <cstdlib>
#include <regex>
#include <string>
#include <string_view>

#include "TokenSpec.h"

TokenSpec::TokenSpec() :
    m_lReSpec {
        { std::regex { "^[a-zA-Z_][a-zA-Z0-9_]*" }, Token::Type::GEN_NAME },
    },
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
        "function",
        "endfunction",
        "range",
        "abort",
        "dict",
        "closure",
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
        { "function", Token::Type::FUNCTION },
        { "endfunction", Token::Type::ENDFUNCTION },
        { "range", Token::Type::FN_RANGE },
        { "abort", Token::Type::FN_ABORT },
        { "dict", Token::Type::FN_DICT },
        { "closure", Token::Type::FN_CLOSURE },
    },
    m_lFixedWidthSpecKeys {
        // clang-format off
        " ",
        "\n",
        "\t",
        "||",
        ",",
        "[",
        "]",
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
        "...",
        "..",
        ".",
        "-=",
        "-",
        "+=",
        "+",
        "*=",
        "*",
        "(",
        ")",
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
        // clang-format off
        { "!", Token::Type::GEN_EXCLAMATION },
        { "...", Token::Type::FN_ELLIPSES },
        { "!=", Token::Type::OP_NEQUAL },
        { "!~", Token::Type::OP_NMATCH },
        { "#", Token::Type::OP_MATCH_CASE },
        { "%", Token::Type::OP_MODULO },
        { "%=", Token::Type::ASSIGN_MODULO },
        { "&", Token::Type::OP_OPTION },
        { "&&", Token::Type::OP_AND },
        { "(", Token::Type::L_PAREN },
        { ")", Token::Type::R_PAREN },
        { "*", Token::Type::OP_MUL },
        { "*=", Token::Type::ASSIGN_MUL },
        { "+", Token::Type::GEN_PLUS },
        { "+=", Token::Type::ASSIGN_ADD },
        { "-", Token::Type::GEN_MINUS },
        { "-=", Token::Type::ASSIGN_MINUS },
        { ".", Token::Type::OP_CAT_OLD },
        { "..", Token::Type::OP_CAT_NEW },
        { "..=", Token::Type::ASSIGN_CAT_NEW },
        { ".=", Token::Type::ASSIGN_CAT_OLD },
        { "/", Token::Type::OP_DIV },
        { "/=", Token::Type::ASSIGN_DIV },
        { ":", Token::Type::GEN_COLON },
        { "<", Token::Type::OP_LT },
        { "<<", Token::Type::OP_LSHIFT },
        { "<=", Token::Type::OP_LTE },
        { "=", Token::Type::ASSIGN_EQ },
        { "==", Token::Type::OP_EQUAL },
        { "=~", Token::Type::OP_MATCH },
        { ">", Token::Type::OP_GT },
        { ">=", Token::Type::OP_GTE },
        { ">>", Token::Type::OP_RSHIFT },
        { "?", Token::Type::GEN_QUESTION },
        { "??", Token::Type::OP_FALSEY },
        { "[", Token::Type::L_BRACKET },
        { "]", Token::Type::R_BRACKET },
        { ",", Token::Type::COMMA },
        { "||", Token::Type::OP_OR },
        { " ", Token::Type::SPACE },
        { "\n", Token::Type::NEWLINE },
        { "\t", Token::Type::TAB },
        // clang-format on
    }
{
}

TokenSpec::~TokenSpec()
{
}

Token* TokenSpec::match(const Source& acSource)
{
    std::string_view lsStr;

    // 1. Look for a fixed-width token
    for (const std::string& lsKey : m_lFixedWidthSpecKeys)
    {
        if (startswith(acSource.remaining_text(), lsKey))
        {
            return new Token(m_mFixedWidthSpec.at(lsKey), lsKey, acSource.pos());
        }
    }

    // 2. Look for a string
    if (startswith_str(acSource.remaining_text(), lsStr))
    {
        return new Token(Token::Type::STRING, std::string { lsStr }, acSource.pos());
    }

    // 3. Look for a float
    if (startswith_float(acSource.remaining_text(), lsStr))
    {
        return new Token(Token::Type::FLOAT, std::string { lsStr }, acSource.pos());
    }

    // 4. Look for an integer
    if (startswith_int(acSource.remaining_text(), lsStr))
    {
        return new Token(Token::Type::INTEGER, std::string { lsStr }, acSource.pos());
    }

    // 5. Look for a delimited token
    for (const std::string& lsKey : m_lDelimitedSpecKeys)
    {
        if (startswith(acSource.remaining_text(), lsKey, "! \n"))
        {
            return new Token(m_mDelimitedSpec.at(lsKey), lsKey, acSource.pos());
        }
    }

    // 6. Match on a regular expression (slow - last resort)
    for (auto it = m_lReSpec.cbegin(); it != m_lReSpec.cend(); ++it)
    {
        const std::regex& lcRe = it->first;
        const Token::Type& leTokenType = it->second;

        std::smatch lcMatch;
        std::string lsText { acSource.remaining_text() };
        if (std::regex_search(lsText, lcMatch, lcRe))
        {
            return new Token(leTokenType, lcMatch.str(), acSource.pos());
        }
    }

    return nullptr;
}

bool TokenSpec::startswith(std::string_view asStr, std::string_view asPrefix, std::string_view asDelim)
{
    size_t lnEnd = asDelim.empty() ? asPrefix.size() : asStr.find_first_of(asDelim);
    return asStr.substr(0, lnEnd) == asPrefix;
}

bool TokenSpec::startswith_str(std::string_view asStr, std::string_view& asPrefix)
{
    // TODO (gh-3): Add support for single-quote string tokenizing
    // TODO (gh-4): Add support for escaped quotes within a string token

    if (asStr.empty() || !startswith(asStr, "\""))
    {
        return false;
    }

    const size_t lnBegin = 0;
    const size_t lnEnd = asStr.find('"', 1);

    asPrefix = asStr.substr(0, lnEnd - lnBegin + 1);

    return true;
}

bool TokenSpec::startswith_int(std::string_view asStr, std::string_view& asOut)
{
    // 1. Start by finding the end any leading digits. Save it for back-tracking.
    const size_t lnDigitsEnd = asStr.find_first_not_of("0123456789");
    asOut = asStr.substr(0, lnDigitsEnd);

    // 2. Zero digits means this is not an integer. More than one digit means this is a
    //    base-10 integer. We can return immediately in either case.
    if (lnDigitsEnd != 1)
    {
        return lnDigitsEnd > 0;
    }

    // 3. Compute the base from the "base character". Return the leading digits if it's
    //    not a valid base character.
    int lnBase;
    switch (asStr[1])
    {
        case 'X':
        case 'x':
            lnBase = 16;
            break;
        case 'O':
        case 'o':
            lnBase = 8;
            break;
        case 'B':
        case 'b':
            lnBase = 2;
            break;
        default:
            return true;
    }

    // 4. Convert the "value" portion (example: 1234 from 0x1234).
    const size_t lnCandidateEnd = asStr.find_first_not_of("0123456789abcdefABCDEF", 2);
    const std::string_view lsValue = asStr.substr(2, lnCandidateEnd - 2);
    const char* pStart = lsValue.data();
    char* pEnd = nullptr;
    long l = std::strtol(pStart, &pEnd, lnBase);

    // 5. Zero valid characters, or one invalid character, followed the "base character".
    //    Return the leading digits.
    if (pEnd == pStart || pEnd != lsValue.end())
    {
        return true;
    }

    asOut = asStr.substr(0, lnCandidateEnd);
    return true;
}

bool TokenSpec::startswith_float(std::string_view asStr, std::string_view& asOut)
{
    // 1. The candidate string is everything until the first non-float character
    const size_t lnCandidateEnd = asStr.find_first_not_of("0123456789eE+-.");
    asOut = asStr.substr(0, lnCandidateEnd);

    // 2. Do the conversion
    const char* pStart = asOut.data();
    char* pEnd = nullptr;
    float f = std::strtof(pStart, &pEnd);

    // 3. The entire candidate needs to be parsed to make a valid float
    if (pEnd == pStart || pEnd != asOut.end())
    {
        return false;
    }

    // 4. It might've been an integer that was parsed
    if (asOut.find('.') == std::string_view::npos)
    {
        return false;
    }

    return true;
}
