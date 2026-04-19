#include <exception>
#include <string_view>

#include "Exceptions.h"
#include "Lexer.h"
#include "Token.h"
#include "util.h"

const std::string g_sKeyWordDelimiters = "! \n\t";

Lexer::Lexer(const Context& acContext) :
    // clang-format off
    m_eState { State::NONE },
    m_pCurrToken { nullptr },
    m_pEndMarker { nullptr },
    m_nBraceLevel { 0 },
    m_cSource { acContext.source() },
    m_lCommands {
        // Commands may have an abbreviation, and must appear in the first column.
        //
        // Example:
        //
        //  for item in items
        //  ^~~
        //
        //  let item = 1
        //  ^~~
        { "break", "brea", Token::Type::BREAK },
        { "continue", "con", Token::Type::CONTINUE },
        { "echo", "ec", Token::Type::EX_ECHO },
        { "else", "el", Token::Type::ELSE },
        { "elseif", "elsei", Token::Type::ELSEIF },
        { "endfor", "endfo", Token::Type::ENDFOR },
        { "endfunction", "endf", Token::Type::ENDFUNCTION },
        { "endif", "en", Token::Type::ENDIF },
        { "endwhile", "endwh", Token::Type::ENDWHILE },
        { "function", "fu", Token::Type::FUNCTION },
        { "return", "retu", Token::Type::RETURN },
        { "set", "se", Token::Type::EX_SET },
        { "while", "wh", Token::Type::WHILE },
        { "for", "", Token::Type::FOR },
        { "if", "", Token::Type::IF },
        { "in", "", Token::Type::IN },
        { "let", "", Token::Type::EX_LET },
        { "unlet", "unl", Token::Type::EX_UNLET },
    },
    m_lKeywords {
        // Keywords have no abbreviation, and can appear in any position.
        //
        // Example:
        //
        //  for item in items
        //           ^~
        //
        //  let items =<< trim END
        //                ^~~~
        { "abort", "", Token::Type::FN_ABORT },
        { "closure", "", Token::Type::FN_CLOSURE },
        { "dict", "", Token::Type::FN_DICT },
        { "in", "", Token::Type::IN },
        { "is", "", Token::Type::OP_IS },
        { "isnot", "", Token::Type::OP_ISNOT },
        { "range", "", Token::Type::FN_RANGE },
        { "trim", "", Token::Type::HD_TRIM },
        { "eval", "", Token::Type::HD_EVAL },
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
        { "=<<", Token::Type::ASSIGN_HEREDOC },
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
        { "/", Token::Type::OP_DIV },
        { ";", Token::Type::SEMICOLON },
        { ":", Token::Type::COLON },
        { "<", Token::Type::OP_LT },
        { "=", Token::Type::ASSIGN_EQ },
        { ">", Token::Type::OP_GT },
        { "?", Token::Type::GEN_QUESTION },
        { "[", Token::Type::L_BRACKET },
        { "]", Token::Type::R_BRACKET },
        { "{", Token::Type::L_BRACE },
        { "}", Token::Type::R_BRACE },
        { "@", Token::Type::SIG_REG },
        { "'", Token::Type::SQUOTE },
        { "\"", Token::Type::DQUOTE },
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

void Lexer::next()
{
    if (m_cSource.eof())
    {
        // TODO (gh-6): throw exception if EOF has already been returned
        push_token(Token::Type::END, std::string { "EOF" });
    }
    else
    {
        if (!match())
        {
            throw std::runtime_error("Error: unrecognized token.\n\n" + m_cSource.context());
        }

        if (m_pCurrToken->is_ambiguous() && !disambiguate(m_pCurrToken))
        {
            throw std::runtime_error("Error: cannot disambiguate.\n\n" + m_cSource.context());
        }

        if (m_pCurrToken->is_keyword())
        {
            retype_keyword(m_pCurrToken);
        }

        m_cSource.advance(m_pCurrToken->str().size());
    }

    m_lTokens.push_back(m_pCurrToken);
}

bool Lexer::match()
{
    const char c = m_cSource.remaining_text().at(0);

    switch (m_eState)
    {
        case State::HEREDOC:
            if (m_cSource.line_text() == m_pEndMarker->str())
            {
                push_token(Token::Type::ENDMARKER, m_cSource.word());
                m_eState = State::NONE;
                m_pEndMarker = nullptr;
                return true;
            }
            else if (c == '\n')
            {
                if (m_lTokens.back()->type() == Token::Type::NEWLINE)
                {
                    return push_token(Token::Type::STRING, std::string { "" });
                }

                return push_token(Token::Type::NEWLINE, '\n');
            }
            else
            {
                // FIXME (gh-100): Verify this makes a valid string
                return push_token(Token::Type::STRING, m_cSource.remaining_line());
            }

        case State::HEREDOC_START:
            if (std::isupper(c))
            {
                push_token(Token::Type::ENDMARKER, m_cSource.word());
                m_eState = State::HEREDOC;
                m_pEndMarker = m_pCurrToken;
                return true;
            }

            // fall-thru intentional
        case State::INTERP_EXP:
            switch (c)
            {
                case '{':
                    m_nBraceLevel++;
                    return push_token(Token::Type::L_BRACE, c);
                case '}':
                    if (m_nBraceLevel == 0)
                    {
                        m_eState = State::INTERP_STR;
                    }
                    else
                    {
                        m_nBraceLevel--;
                    }

                    return push_token(Token::Type::R_BRACE, c);
                case '"':
                    m_eState = State::NONE;
                    return push_token(Token::Type::DQUOTE, c);
                case '\'':
                    m_eState = State::NONE;
                    return push_token(Token::Type::SQUOTE, c);
            }

            // fall-thru intentional
        case State::NONE:
            switch (c)
            {
                case '$':
                    if (m_cSource.remaining_text().size() > 1)
                    {
                        const char c_next = m_cSource.remaining_text().at(1);
                        if (c_next == '"' || c_next == '\'')
                        {
                            return push_token(Token::Type::STR_INTERP, c);
                        }
                    }

                    return push_token(Token::Type::SIG_ENV, c);
            }

            // These have special logic and need to happen before symbol matching.
            if (push_register() || push_comment())
            {
                return true;
            }

            // If we match a symbol, it may require a state change.
            if (push_symbol())
            {
                switch (m_pCurrToken->type())
                {
                    case Token::Type::DQUOTE:
                        if (m_lTokens.back()->type() == Token::Type::STR_INTERP)
                        {
                            m_eState = State::INTERP_STR;
                        }
                        else
                        {
                            m_eState = State::STRING_CONSTANT;
                        }

                        break;
                    case Token::Type::SQUOTE:
                        if (m_lTokens.back()->type() == Token::Type::STR_INTERP)
                        {
                            m_eState = State::INTERP_STR;
                        }
                        else
                        {
                            m_eState = State::LITERAL_STRING;
                        }

                        break;
                    case Token::Type::ASSIGN_HEREDOC:
                        m_eState = State::HEREDOC_START;
                        break;
                    default:
                        break;
                }

                return true;
            }

            // Checking for a RegEx is slow and intentionally a last resort.
            if (push_number() || push_command() || push_keyword() || push_regex())
            {
                return true;
            }

            break;
        case State::INTERP_STR:
            switch (c)
            {
                case '{':
                    m_eState = State::INTERP_EXP;
                    return push_token(Token::Type::L_BRACE, c);
                case '}':
                    // test/error/E1278_1.out
                    throw VimError("E1278", m_cSource.context());
                case '"':
                    m_eState = State::NONE;
                    return push_token(Token::Type::DQUOTE, c);
                case '\'':
                    m_eState = State::NONE;
                    return push_token(Token::Type::SQUOTE, c);
            }

            return push_string("{}\"'");
        case State::LITERAL_STRING:
            if (c == '\'')
            {
                m_eState = State::NONE;
                return push_token(Token::Type::SQUOTE, c);
            }

            return push_string("'");
        case State::STRING_CONSTANT:
            if (c == '"')
            {
                m_eState = State::NONE;
                return push_token(Token::Type::DQUOTE, c);
            }

            // TODO (gh-4): Add support for escaped quotes within a string token
            return push_string("\"");
        default:
            throw std::runtime_error("Lexer::match(): fall-thru to unknown state");
    }

    return false;
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
                        apCurrentToken->setType(Token::Type::OP_LOGICAL_NOT);
                        break;
                    case Token::Type::FUNCTION:
                    case Token::Type::EX_UNLET:
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

bool Lexer::push_comment()
{
    if (m_cSource.remaining_text().at(0) != '"')
    {
        return false;
    }

    if (m_cSource.column() == m_cSource.indent())
    {
        // If " is the first non-whitespace of a line, it's a comment
        return push_token(Token::Type::COMMENT, m_cSource.remaining_line());
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

    return push_token(Token::Type::COMMENT, m_cSource.remaining_line());
}

bool Lexer::push_register()
{
    if (m_lTokens.empty() || m_lTokens.back()->type() != Token::Type::SIG_REG)
    {
        return false;
    }

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
            return push_token(Token::Type::REGISTER, c);
        case ':':
        case '.':
        case '%':
            // test/error/E354_1.out
            throw VimError("E354", m_cSource.context());
        default:
            if (std::isalnum(c))
            {
                return push_token(Token::Type::REGISTER, c);
            }
    }

    // test/error/E354_2.out
    throw VimError("E354", m_cSource.context());
}

bool Lexer::push_symbol()
{
    for (const Symbol& lcSymbol : m_lSymbols)
    {
        if (vf::startswith(m_cSource.remaining_text(), lcSymbol.sLexeme))
        {
            return push_token(lcSymbol.eTokenType, lcSymbol.sLexeme);
        }
    }

    return false;
}

bool Lexer::push_command()
{
    if (m_cSource.column() == m_cSource.indent())
    {
        for (const Command& lcCommand : m_lCommands)
        {
            if (vf::startswith(m_cSource.remaining_text(), lcCommand.sFull, g_sKeyWordDelimiters))
            {
                return push_token(lcCommand.eTokenType, lcCommand.sFull);
            }
            else if (lcCommand.sAbrv.empty())
            {
                continue;
            }
            else if (vf::startswith(m_cSource.remaining_text(), lcCommand.sAbrv, g_sKeyWordDelimiters))
            {
                return push_token(lcCommand.eTokenType, lcCommand.sAbrv);
            }
        }
    }

    return false;
}

bool Lexer::push_keyword()
{
    for (const Keyword& lcKeyword : m_lKeywords)
    {
        if (vf::startswith(m_cSource.remaining_text(), lcKeyword.sFull, g_sKeyWordDelimiters))
        {
            return push_token(lcKeyword.eTokenType, lcKeyword.sFull);
        }
        else if (lcKeyword.sAbrv.empty())
        {
            continue;
        }
        else if (vf::startswith(m_cSource.remaining_text(), lcKeyword.sAbrv, g_sKeyWordDelimiters))
        {
            return push_token(lcKeyword.eTokenType, lcKeyword.sAbrv);
        }
    }

    return false;
}

bool Lexer::push_regex()
{
    for (auto it = m_lReSpec.cbegin(); it != m_lReSpec.cend(); ++it)
    {
        const std::regex& lcRe = it->first;
        const Token::Type& leTokenType = it->second;

        std::smatch lcMatch;
        std::string lsText { m_cSource.remaining_text() };
        if (std::regex_search(lsText, lcMatch, lcRe))
        {
            return push_token(leTokenType, lcMatch.str());
        }
    }

    return false;
}

bool Lexer::push_number()
{
    std::string_view lsLexeme;

    // float
    if (vf::startswith_float(m_cSource.remaining_text(), lsLexeme))
    {
        return push_token(Token::Type::FLOAT, std::string { lsLexeme });
    }

    // integer
    if (vf::startswith_int(m_cSource.remaining_text(), lsLexeme))
    {
        return push_token(Token::Type::INTEGER, std::string { lsLexeme });
    }

    return false;
}

bool Lexer::push_token(Token::Type aeTokenType, char asLexeme)
{
    return push_token(aeTokenType, std::string { asLexeme });
}

bool Lexer::push_token(Token::Type aeTokenType, std::string_view asLexeme)
{
    return push_token(aeTokenType, std::string { asLexeme });
}

bool Lexer::push_token(Token::Type aeTokenType, const std::string& asLexeme)
{
    m_pCurrToken = new Token(aeTokenType, asLexeme, m_cSource.pos());
    return true;
}

bool Lexer::push_string(const std::string& asRightDelimiters)
{
    size_t lnSize = m_cSource.remaining_text().find_first_of(asRightDelimiters, 1);
    std::string_view lsStr = m_cSource.remaining_text().substr(0, lnSize);
    return push_token(Token::Type::STRING, lsStr);
}
