#include <exception>
#include <string_view>

#include "Exceptions.h"
#include "Lexer.h"
#include "Token.h"
#include "util.h"

const std::string g_sKeyWordDelimiters = "! \n\t";

Lexer::Lexer(const Context& acContext) :
    // clang-format off
    m_cSource { acContext.source() },
    m_eState { Token::Type::NONE },
    m_lCommands {
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
        { "for", "", Token::Type::FOR },
        { "if", "", Token::Type::IF },
        { "in", "", Token::Type::IN },
        { "let", "", Token::Type::CMD_LET },
    },
    m_lKeywords {
        { "abort", "", Token::Type::FN_ABORT },
        { "closure", "", Token::Type::FN_CLOSURE },
        { "dict", "", Token::Type::FN_DICT },
        { "in", "", Token::Type::IN },
        { "is", "", Token::Type::OP_IS },
        { "isnot", "", Token::Type::OP_ISNOT },
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
        { "b:", Token::Type::SCOPE_B },
        { "w:", Token::Type::SCOPE_W },
        { "t:", Token::Type::SCOPE_T },
        { "g:", Token::Type::SCOPE_G },
        { "l:", Token::Type::SCOPE_L },
        { "s:", Token::Type::SCOPE_S },
        { "a:", Token::Type::SCOPE_A },
        { "v:", Token::Type::SCOPE_V },
        { "!", Token::Type::GEN_EXCLAMATION },
        { "#", Token::Type::OP_MATCH_CASE },
        { "%", Token::Type::OP_MODULO },
        { "&", Token::Type::SIG_OPT },
        { "(", Token::Type::L_PAREN },
        { ")", Token::Type::R_PAREN },
        { "*", Token::Type::OP_MUL },
        { "+", Token::Type::GEN_PLUS },
        { ",", Token::Type::COMMA },
        { "-", Token::Type::GEN_MINUS },
        { ".", Token::Type::GEN_DOT },
        { "/", Token::Type::GEN_SLASH },
        { ":", Token::Type::COLON },
        { "<", Token::Type::OP_LT },
        { "=", Token::Type::ASSIGN_EQ },
        { ">", Token::Type::OP_GT },
        { "?", Token::Type::GEN_QUESTION },
        { "[", Token::Type::L_BRACKET },
        { "]", Token::Type::R_BRACKET },
        { "{", Token::Type::L_BRACE },
        { "}", Token::Type::R_BRACE },
        { "$", Token::Type::SIG_ENV },
        { "@", Token::Type::SIG_REG },
        // clang-format on
    },
    m_lReSpec {
        { std::regex { "^[a-zA-Z_][a-zA-Z0-9_]*" }, Token::Type::IDENTIFIER },
    }
{
}

Lexer::~Lexer()
{
    freeTokens();
}

void Lexer::tokenize()
{
    freeTokens();

    while (true)
    {
        next();

        if (m_lTokens.back()->type() == Token::Type::END)
        {
            break;
        }
    }
}

std::vector<Token*> Lexer::tokens() const
{
    // Filter out delimiting whitespace tokens before returning.

    std::vector<Token*> llTokens;
    llTokens.reserve(m_lTokens.size());

    for (Token* pToken : m_lTokens)
    {
        if (!pToken->is_horizontal_wp())
        {
            llTokens.push_back(pToken);
        }
    }

    return llTokens;
}

const Source& Lexer::source() const
{
    return m_cSource;
}

const Token& Lexer::token(size_t anIdx) const
{
    // TODO (gh-7): throw IndexError
    return *tokens().at(anIdx);
}

Token* Lexer::next()
{
    Token* pToken = do_next();
    m_lTokens.push_back(pToken);
    return pToken;
}

Token* Lexer::do_next()
{
    Token* pToken;

    if (m_cSource.eof())
    {
        // TODO (gh-6): throw exception if EOF has already been returned
        pToken = new Token(Token::Type::END, "EOF", m_cSource.pos());
    }
    else
    {
        pToken = match();

        if (pToken == nullptr)
        {
            throw std::runtime_error("Error: unrecognized token.\n\n" + m_cSource.context());
        }

        if (pToken->is_ambiguous() && !disambiguate(pToken))
        {
            throw std::runtime_error("Error: cannot disambiguate.\n\n" + m_cSource.context());
        }

        if (pToken->is_keyword())
        {
            retype_keyword(pToken);
        }

        m_cSource.advance(pToken->str().size());
    }

    return pToken;
}

Token* Lexer::match()
{
    std::string_view lsStr;

    // Look for a register
    if (!m_lTokens.empty() && m_lTokens.back()->type() == Token::Type::SIG_REG)
    {
        const char c = m_cSource.remaining_text().at(0);
        switch (c)
        {
            case '"':
            case '-':
            case '#':
            case '=':
            case '*':
            case '+':
            case '~':
            case '_':
            case '/':
            case '@':
                break;
            case ':':
            case '.':
            case '%':
                throw VimError("E354", m_cSource.context());
            default:
                if (!std::isalnum(c))
                {
                    throw VimError("E354", m_cSource.context());
                }
        }

        return new Token(Token::Type::REGISTER, std::string { c }, m_cSource.pos());
    }

    if (chk_comment())
    {
        lsStr = m_cSource.remaining_line();
        return new Token(Token::Type::COMMENT, std::string { lsStr }, m_cSource.pos());
    }

    const char c = m_cSource.remaining_text().at(0);

    if (c == '\'')
    {
        toggle_state(Token::Type::SQUOTE);
        return new Token(Token::Type::SQUOTE, std::string { c }, m_cSource.pos());
    }

    if (m_eState == Token::Type::SQUOTE)
    {
        size_t lnSize = m_cSource.remaining_text().find('\'', 1);
        lsStr = m_cSource.remaining_text().substr(0, lnSize);
        return new Token(Token::Type::STRING, std::string { lsStr }, m_cSource.pos());
    }

    if (c == '"')
    {
        toggle_state(Token::Type::DQUOTE);
        return new Token(Token::Type::DQUOTE, std::string { c }, m_cSource.pos());
    }

    if (m_eState == Token::Type::DQUOTE)
    {
        // TODO (gh-4): Add support for escaped quotes within a string token
        size_t lnSize = m_cSource.remaining_text().find('"', 1);
        lsStr = m_cSource.remaining_text().substr(0, lnSize);
        return new Token(Token::Type::STRING, std::string { lsStr }, m_cSource.pos());
    }

    // Look for a symbol
    for (const Symbol& lcSymbol : m_lSymbols)
    {
        if (vf::startswith(m_cSource.remaining_text(), lcSymbol.sLexeme))
        {
            return new Token(lcSymbol.eTokenType, lcSymbol.sLexeme, m_cSource.pos());
        }
    }

    // Look for a float
    if (vf::startswith_float(m_cSource.remaining_text(), lsStr))
    {
        return new Token(Token::Type::FLOAT, std::string { lsStr }, m_cSource.pos());
    }

    // Look for an integer
    if (vf::startswith_int(m_cSource.remaining_text(), lsStr))
    {
        return new Token(Token::Type::INTEGER, std::string { lsStr }, m_cSource.pos());
    }

    // Look for a command
    if (m_cSource.column() == m_cSource.indent())
    {
        for (const Command& lcCommand : m_lCommands)
        {
            if (vf::startswith(m_cSource.remaining_text(), lcCommand.sFull, g_sKeyWordDelimiters))
            {
                return new Token(lcCommand.eTokenType, lcCommand.sFull, m_cSource.pos());
            }
            else if (lcCommand.sAbrv.empty())
            {
                continue;
            }
            else if (vf::startswith(m_cSource.remaining_text(), lcCommand.sAbrv, g_sKeyWordDelimiters))
            {
                return new Token(lcCommand.eTokenType, lcCommand.sAbrv, m_cSource.pos());
            }
        }
    }

    // Look for a keyword
    for (const Keyword& lcKeyword : m_lKeywords)
    {
        if (vf::startswith(m_cSource.remaining_text(), lcKeyword.sFull, g_sKeyWordDelimiters))
        {
            return new Token(lcKeyword.eTokenType, lcKeyword.sFull, m_cSource.pos());
        }
        else if (lcKeyword.sAbrv.empty())
        {
            continue;
        }
        else if (vf::startswith(m_cSource.remaining_text(), lcKeyword.sAbrv, g_sKeyWordDelimiters))
        {
            return new Token(lcKeyword.eTokenType, lcKeyword.sAbrv, m_cSource.pos());
        }
    }

    // Match on a regular expression (slow - last resort)
    for (auto it = m_lReSpec.cbegin(); it != m_lReSpec.cend(); ++it)
    {
        const std::regex& lcRe = it->first;
        const Token::Type& leTokenType = it->second;

        std::smatch lcMatch;
        std::string lsText { m_cSource.remaining_text() };
        if (std::regex_search(lsText, lcMatch, lcRe))
        {
            return new Token(leTokenType, lcMatch.str(), m_cSource.pos());
        }
    }

    return nullptr;
}

void Lexer::freeTokens()
{
    for (const Token* pToken : m_lTokens)
    {
        delete pToken;
    }

    m_lTokens.clear();
}

bool Lexer::disambiguate(Token* apCurrentToken)
{
    // 1. Try disambiguating depending on preceding tokens.
    for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
    {
        const Token* pPrevToken = *rit;

        switch (apCurrentToken->type())
        {
            case Token::Type::GEN_MINUS:
                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                        continue;
                    case Token::Type::FLOAT:
                    case Token::Type::STRING:
                    case Token::Type::INTEGER:
                    case Token::Type::R_PAREN:
                    case Token::Type::IDENTIFIER:
                        apCurrentToken->setType(Token::Type::OP_SUB);
                        break;
                    default:
                        apCurrentToken->setType(Token::Type::OP_UNARY_MINUS);
                }

                break;
            case Token::Type::GEN_PLUS:
                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                        continue;
                    case Token::Type::FLOAT:
                    case Token::Type::STRING:
                    case Token::Type::INTEGER:
                    case Token::Type::R_PAREN:
                    case Token::Type::IDENTIFIER:
                        apCurrentToken->setType(Token::Type::OP_ADD);
                        break;
                    default:
                        apCurrentToken->setType(Token::Type::OP_UNARY_PLUS);
                }

                break;
            case Token::Type::GEN_QUESTION:
                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                        continue;
                    case Token::Type::OP_EQUAL:
                    case Token::Type::OP_NEQUAL:
                    case Token::Type::OP_GT:
                    case Token::Type::OP_GTE:
                    case Token::Type::OP_LT:
                    case Token::Type::OP_LTE:
                    case Token::Type::OP_MATCH:
                    case Token::Type::OP_NMATCH:
                    case Token::Type::OP_IS:
                    case Token::Type::OP_ISNOT:
                        apCurrentToken->setType(Token::Type::OP_IGNORE_CASE);
                        break;
                    default:
                        apCurrentToken->setType(Token::Type::OP_TERNARY_IF);
                }

                break;
            case Token::Type::GEN_EXCLAMATION:
                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                        continue;
                    case Token::Type::FUNCTION:
                        apCurrentToken->setType(Token::Type::OP_BANG);
                        break;
                    default:
                        apCurrentToken->setType(Token::Type::OP_LOGICAL_NOT);
                }

                break;
            case Token::Type::GEN_DOT:
                switch (pPrevToken->type())
                {
                    case Token::Type::IDENTIFIER:
                        apCurrentToken->setType(Token::Type::OP_DOT);
                        break;
                    default:
                        apCurrentToken->setType(Token::Type::OP_CAT_OLD);
                }

                break;
            case Token::Type::GEN_SLASH:
                switch (pPrevToken->type())
                {
                    case Token::Type::SIG_REG:
                        // Search pattern register
                        apCurrentToken->setType(Token::Type::IDENTIFIER);
                        break;
                    default:
                        apCurrentToken->setType(Token::Type::OP_DIV);
                }

                break;
            default:
                break;
        }

        // 2. Stop looking at preceding tokens if the current token was disambiguated.
        if (!apCurrentToken->is_ambiguous())
        {
            break;
        }
    }

    // 3. If the token is still not disambiguated, it may because there were zero
    //    preceding tokens, or all preceding tokens were whitespace. Some tokens can be
    //    disambiguated in this case.
    switch (apCurrentToken->type())
    {
        case Token::Type::GEN_MINUS:
            apCurrentToken->setType(Token::Type::OP_UNARY_MINUS);
            break;
        case Token::Type::GEN_PLUS:
            apCurrentToken->setType(Token::Type::OP_UNARY_PLUS);
            break;
        case Token::Type::GEN_EXCLAMATION:
            apCurrentToken->setType(Token::Type::OP_LOGICAL_NOT);
            break;
        default:
            break;
    }

    // 4. Return true only if the current token was disambiguated.
    return !apCurrentToken->is_ambiguous();
}

void Lexer::retype_keyword(Token* apCurrentToken)
{
    switch (apCurrentToken->type())
    {
        case Token::Type::FN_ABORT:
        case Token::Type::FN_CLOSURE:
        case Token::Type::FN_DICT:
        case Token::Type::FN_RANGE:
            for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
            {
                Token* pPrevToken = *rit;

                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                    case Token::Type::FN_ABORT:
                    case Token::Type::FN_CLOSURE:
                    case Token::Type::FN_DICT:
                    case Token::Type::FN_RANGE:
                        continue;
                    case Token::Type::R_PAREN:
                        return;
                    default:
                        apCurrentToken->setType(Token::Type::IDENTIFIER);
                        return;
                }
            }
        case Token::Type::IN:
            for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
            {
                Token* pPrevToken = *rit;

                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                        continue;
                    case Token::Type::R_BRACKET:
                    case Token::Type::IDENTIFIER:
                        return;
                    default:
                        apCurrentToken->setType(Token::Type::IDENTIFIER);
                        return;
                }
            }
        case Token::Type::OP_IS:
        case Token::Type::OP_ISNOT:
            for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
            {
                Token* pPrevToken = *rit;

                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                        continue;
                    case Token::Type::IDENTIFIER:
                    case Token::Type::INTEGER:
                    case Token::Type::SQUOTE:
                    case Token::Type::DQUOTE:
                    case Token::Type::FLOAT:
                    case Token::Type::R_BRACKET:
                    case Token::Type::R_PAREN:
                        return;
                    default:
                        apCurrentToken->setType(Token::Type::IDENTIFIER);
                        return;
                }
            }
        default:
            throw std::runtime_error("Error: cannot re-type.\n\n" + m_cSource.context());
    }
}

void Lexer::toggle_state(Token::Type aeState)
{
    if (m_eState == Token::Type::NONE)
    {
        m_eState = aeState;
    }
    else
    {
        m_eState = Token::Type::NONE;
    }
}

bool Lexer::chk_comment() const
{
    if (m_eState == Token::Type::DQUOTE)
    {
        // If we are building a string constant, it's not a comment
        return false;
    }

    if (m_cSource.remaining_text().at(0) != '"')
    {
        // A comment must start with a "
        return false;
    }

    if (m_cSource.column() == m_cSource.indent())
    {
        // If " is the first non-whitespace of a line, it's a comment
        return true;
    }

    for (size_t i = 1; i < m_cSource.remaining_text().size(); i++)
    {
        const char c = m_cSource.remaining_text().at(i);

        if (c == '"')
        {
            // TODO (gh-54): Trailing comments with more than one " are tokenized as strings
            return false;
        }

        if (c == '\n')
        {
            break;
        }
    }

    return true;
}
