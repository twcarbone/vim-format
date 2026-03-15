#include <cstdlib>
#include <regex>
#include <string>
#include <string_view>

#include "TokenSpec.h"

TokenSpec::TokenSpec() :
    // clang-format off
    m_lKeywords {
        { "break", "brea", Token::Type::BREAK },
        { "continue", "con", Token::Type::CONTINUE },
        { "echo", "ec", Token::Type::CMD_ECHO },
        { "else", "el", Token::Type::ELSE },
        { "elseif", "elsei", Token::Type::ELSEIF },
        { "endfor", "endfo", Token::Type::ENDFOR },
        { "endfunction", "endf", Token::Type::ENDFUNCTION },
        { "endif", "en", Token::Type::ENDIF },
        { "endwhile", "endwh", Token::Type::ENDWHILE },
        { "function", "fu", Token::Type::FUNCTION },
        { "return", "retu", Token::Type::RETURN },
        { "set", "se", Token::Type::CMD_SET },
        { "while", "wh", Token::Type::WHILE },
        { "abort", "", Token::Type::FN_ABORT },
        { "closure", "", Token::Type::FN_CLOSURE },
        { "dict", "", Token::Type::FN_DICT },
        { "for", "", Token::Type::FOR },
        { "if", "", Token::Type::IF },
        { "in", "", Token::Type::IN },
        { "is", "", Token::Type::OP_IS },
        { "isnot", "", Token::Type::OP_ISNOT },
        { "let", "", Token::Type::CMD_LET },
        { "range", "", Token::Type::FN_RANGE },
    },
    m_lSymbols {
        // These symbols should appear most often. Put them first.
        { " ", Token::Type::SPACE },
        { "\n", Token::Type::NEWLINE },
        { "\t", Token::Type::TAB },
        // All other symbols need to be in decreasing width order so that the full symbol
        // is matched, not multiple narrower symbols (eg, == comes before =).
        { "...", Token::Type::FN_ELLIPSES },
        { "..=", Token::Type::ASSIGN_CAT_NEW },
        { "!=", Token::Type::OP_NEQUAL },
        { "!~", Token::Type::OP_NMATCH },
        { "%=", Token::Type::ASSIGN_MODULO },
        { "&&", Token::Type::OP_AND },
        { "*=", Token::Type::ASSIGN_MUL },
        { "+=", Token::Type::ASSIGN_ADD },
        { "-=", Token::Type::ASSIGN_MINUS },
        { "->", Token::Type::OP_METHOD },
        { "..", Token::Type::OP_CAT_NEW },
        { ".=", Token::Type::ASSIGN_CAT_OLD },
        { "/=", Token::Type::ASSIGN_DIV },
        { "<<", Token::Type::OP_LSHIFT },
        { "<=", Token::Type::OP_LTE },
        { "==", Token::Type::OP_EQUAL },
        { "=~", Token::Type::OP_MATCH },
        { ">=", Token::Type::OP_GTE },
        { ">>", Token::Type::OP_RSHIFT },
        { "??", Token::Type::OP_FALSEY },
        { "||", Token::Type::OP_OR },
        { "!", Token::Type::GEN_EXCLAMATION },
        { "#", Token::Type::OP_MATCH_CASE },
        { "%", Token::Type::OP_MODULO },
        { "&", Token::Type::OP_OPTION },
        { "(", Token::Type::L_PAREN },
        { ")", Token::Type::R_PAREN },
        { "*", Token::Type::OP_MUL },
        { "+", Token::Type::GEN_PLUS },
        { ",", Token::Type::COMMA },
        { "-", Token::Type::GEN_MINUS },
        { ".", Token::Type::GEN_DOT },
        { "/", Token::Type::OP_DIV },
        { ":", Token::Type::GEN_COLON },
        { "<", Token::Type::OP_LT },
        { "=", Token::Type::ASSIGN_EQ },
        { ">", Token::Type::OP_GT },
        { "?", Token::Type::GEN_QUESTION },
        { "[", Token::Type::L_BRACKET },
        { "]", Token::Type::R_BRACKET },
        { "{", Token::Type::L_BRACE },
        { "}", Token::Type::R_BRACE },
        // clang-format on
    },
    m_lReSpec {
        { std::regex { "^[a-zA-Z_][a-zA-Z0-9_]*" }, Token::Type::GEN_NAME },
    }
{
}

TokenSpec::~TokenSpec()
{
}

Token* TokenSpec::match(const Source& acSource)
{
    std::string_view lsStr;

    // Look for a symbol
    for (const Symbol& lcSymbol : m_lSymbols)
    {
        if (startswith(acSource.remaining_text(), lcSymbol.sLexeme))
        {
            return new Token(lcSymbol.eTokenType, lcSymbol.sLexeme, acSource.pos());
        }
    }

    // Look for single-quote string
    if (acSource.remaining_text().at(0) == '\'')
    {
        size_t lnEnd = acSource.remaining_text().find('\'', 1);
        lsStr = acSource.remaining_text().substr(0, lnEnd + 1);
        return new Token(Token::Type::STRING, std::string { lsStr }, acSource.pos());
    }

    // Look for double-quote string or comment
    if (acSource.remaining_text().at(0) == '"')
    {
        if (acSource.column() == acSource.indent())
        {
            lsStr = acSource.remaining_line();
            return new Token(Token::Type::COMMENT, std::string { lsStr }, acSource.pos());
        }

        for (size_t i = 1; i < acSource.remaining_text().size(); i++)
        {
            switch (acSource.remaining_text().at(i))
            {
                case '"':
                    // TODO (gh-4): Add support for escaped quotes within a string token
                    lsStr = acSource.remaining_text().substr(0, i + 1);
                    return new Token(Token::Type::STRING, std::string { lsStr }, acSource.pos());
                case '\n':
                    // TODO (gh-54): Trailing comments with more than one " are tokenized as strings
                    lsStr = acSource.remaining_text().substr(0, i);
                    return new Token(Token::Type::COMMENT, std::string { lsStr }, acSource.pos());
                default:
                    continue;
            }
        }
    }

    // Look for a float
    if (startswith_float(acSource.remaining_text(), lsStr))
    {
        return new Token(Token::Type::FLOAT, std::string { lsStr }, acSource.pos());
    }

    // Look for an integer
    if (startswith_int(acSource.remaining_text(), lsStr))
    {
        return new Token(Token::Type::INTEGER, std::string { lsStr }, acSource.pos());
    }

    // Look for a keyword
    for (const Keyword& lcKeyword : m_lKeywords)
    {
        if (startswith(acSource.remaining_text(), lcKeyword.sFull, "! \n\t"))
        {
            return new Token(lcKeyword.eTokenType, lcKeyword.sFull, acSource.pos());
        }
    }

    // Match on a regular expression (slow - last resort)
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
