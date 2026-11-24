#include <exception>
#include <string_view>

#include "Exceptions.h"
#include "Lexer.h"
#include "Token.h"
#include "util.h"

// Tip 2-1: const and constexpr variables declared at global scope have internal linkage
// by default. They do not need to be marked static or wrapped in an anonymous namespace
// to hide from then linker.
const std::string g_sKeyWordDelimiters = "! \n\t";
const char DQUOTE = '"';
const char SQUOTE = '\'';

Lexer::Lexer(const Context& acContext) :
    // clang-format off
    m_eState { State::NONE },
    m_pEndMarker { nullptr },
    m_pCount { nullptr },
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
        { "break", Token::Type::EX_BREAK , "brea"},
        { "continue", Token::Type::EX_CONTINUE , "con"},
        { "finish", Token::Type::EX_FINISH , "fini"},
        { "echo", Token::Type::EX_ECHO , "ec"},
        { "else", Token::Type::EX_ELSE , "el"},
        { "elseif", Token::Type::EX_ELSEIF , "elsei"},
        { "endfor", Token::Type::EX_ENDFOR , "endfo"},
        { "endfunction", Token::Type::EX_ENDFUNCTION , "endf"},
        { "endif", Token::Type::EX_ENDIF , "en"},
        { "endwhile", Token::Type::EX_ENDWHILE , "endwh"},
        { "function", Token::Type::EX_FUNCTION , "fu"},
        { "return", Token::Type::EX_RETURN , "retu"},
        { "set", Token::Type::EX_SET , "se"},
        { "while", Token::Type::EX_WHILE , "wh"},
        { "for", Token::Type::EX_FOR},
        { "if", Token::Type::EX_IF },
        { "in", Token::Type::IN },
        { "let", Token::Type::EX_LET },
        { "unlet", Token::Type::EX_UNLET , "unl"},
        { "lockvar", Token::Type::EX_LOCKVAR , "lockv"},
        { "unlockvar", Token::Type::EX_UNLOCKVAR , "unlo"},
        { "try", Token::Type::EX_TRY },
        { "endtry", Token::Type::EX_ENDTRY , "endt"},
        { "catch", Token::Type::EX_CATCH , "cat"},
        { "finally", Token::Type::EX_FINALLY , "fina"},
        { "throw", Token::Type::EX_THROW , "th"},
        { "echon", Token::Type::EX_ECHON },
        { "echohl", Token::Type::EX_ECHOHL , "echoh"},
        { "echomsg", Token::Type::EX_ECHOMSG , "echom"},
        { "echoerr", Token::Type::EX_ECHOERR , "echoe"},
        { "echoconsole", Token::Type::EX_ECHOCONSOLE , "echoc"},
        { "echowindow", Token::Type::EX_ECHOWINDOW , "echow"},
        { "eval", Token::Type::EX_EVAL },
        { "execute", Token::Type::EX_EXECUTE , "exe"},
        { "const", Token::Type::EX_CONST , "cons"},
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
        { "abort", Token::Type::FN_ABORT },
        { "closure", Token::Type::FN_CLOSURE },
        { "dict", Token::Type::FN_DICT },
        { "in", Token::Type::IN },
        { "is", Token::Type::OP_IS },
        { "isnot", Token::Type::OP_ISNOT },
        { "range", Token::Type::FN_RANGE },
        { "trim", Token::Type::HD_TRIM },
        { "eval", Token::Type::HD_EVAL },
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
        { "$'", Token::Type::STR_INTERP_SQUOTE },
        { "$\"", Token::Type::STR_INTERP_DQUOTE },
        { "$", Token::Type::SIG_ENV },
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
    },
    m_lReSpec {
        { std::regex { "^[a-zA-Z_][a-zA-Z0-9_]*" }, Token::Type::IDENTIFIER },
    },
    m_gStateTransitions { {
        { State::NONE,                      State::NONE },
        { State::HEREDOC_START,             State::HEREDOC },
        { State::HEREDOC,                   State::NONE },
        { State::HEREDOC_EVAL_START,        State::HEREDOC_EVAL_STR },
        { State::HEREDOC_EVAL_STR,          State::HEREDOC_EVAL_EXP },
        { State::HEREDOC_EVAL_EXP,          State::HEREDOC_EVAL_STR },
        { State::INTERP_STR_SQUOTE,         State::INTERP_EXP_SQUOTE },
        { State::INTERP_STR_DQUOTE,         State::INTERP_EXP_DQUOTE },
        { State::INTERP_EXP_SQUOTE,         State::INTERP_STR_SQUOTE },
        { State::INTERP_EXP_DQUOTE,         State::INTERP_STR_DQUOTE },
        { State::STRING_CONSTANT,           State::NONE },
        { State::LITERAL_STRING_BEGIN,      State::LITERAL_STRING_END },
        { State::LITERAL_STRING_END,        State::NONE },
        { State::PATTERN,                   State::NONE },
    } }
// clang-format on
{
}

Lexer::~Lexer()
{
}

void Lexer::tokenize()
{
    m_lTokens.clear();

    while (true)
    {
        extract_tokens();

        if (m_lTokens.back()->type() == Token::Type::END)
        {
            break;
        }
    }
}

Tokens Lexer::take_tokens()
{
    return std::move(m_lTokens);
}

const Tokens& Lexer::tokens() const
{
    return m_lTokens;
}

const Source& Lexer::source() const
{
    return m_cSource;
}

Token Lexer::token(size_t anIdx) const
{
    return *m_lTokens.at(anIdx);
}

/**
 *  Extract one or many tokens.
 */
void Lexer::extract_tokens()
{
    if (m_cSource.eof())
    {
        push_token(Token::Type::END, std::string { "EOF" });
        return;
    }

    const char c = m_cSource.remaining_text().at(0);

    switch (m_eState)
    {
        case State::HEREDOC_EVAL_START:
        case State::HEREDOC_START:
            if (std::isupper(c))
            {
                push_token(Token::Type::ENDMARKER, m_cSource.word());
                m_pEndMarker = m_lTokens.back();
                expect_token(Token::Type::NEWLINE, "\n");
                next_state();
                return;
            }

            [[fallthrough]];
        case State::INTERP_EXP_SQUOTE:
            if (c == SQUOTE)
            {
                throw VimError("E1279", m_cSource.context());
            }

            [[fallthrough]];
        case State::INTERP_EXP_DQUOTE:
            if (c == DQUOTE)
            {
                throw VimError("E1279", m_cSource.context());
            }

            [[fallthrough]];
        case State::HEREDOC_EVAL_EXP:
            switch (c)
            {
                case '{':
                    m_nBraceLevel++;
                    push_token(Token::Type::L_BRACE, c);
                    return;
                case '}':
                    if (m_nBraceLevel == 0)
                    {
                        next_state();
                    }
                    else
                    {
                        m_nBraceLevel--;
                    }

                    push_token(Token::Type::R_BRACE, c);
                    return;
            }

            [[fallthrough]];
        case State::NONE:
            if (try_push_register() || try_push_comment() || try_push_continuation())
            {
                return;
            }

            if (try_push_symbol())
            {
                switch (m_lTokens.back()->type())
                {
                    case Token::Type::ASSIGN_HEREDOC:
                        m_eState = State::HEREDOC_START;
                        break;
                    case Token::Type::DQUOTE:
                        push_string_constant();
                        return;
                    case Token::Type::SQUOTE:
                        push_literal_string();
                        return;
                    case Token::Type::STR_INTERP_DQUOTE:
                        push_interp_string_constant();
                        return;
                    case Token::Type::STR_INTERP_SQUOTE:
                        push_interp_literal_string();
                        return;
                    case Token::Type::SLASH:
                        // TODO (gh-141): Add support for escaped forward slash in patterns
                        // TODO (gh-170): :catch /{pattern}/ can be delimited with any character
                        m_eState = State::PATTERN;
                        break;
                    default:
                        return;
                }

                return;
            }

            if (try_push_keyword())
            {
                switch (m_lTokens.back()->type())
                {
                    case Token::Type::HD_EVAL:
                        m_eState = State::HEREDOC_EVAL_START;
                        break;
                    default:
                        break;
                }

                return;
            }

            if (try_push_blob() || try_push_number() || try_push_command() || try_push_regex())
            {
                return;
            }

            break;
        case State::HEREDOC:
            if (!try_push_heredoc_endmarker())
            {
                push_heredoc_string();
            }

            return;
        case State::INTERP_STR_SQUOTE:
            push_interp_literal_string();
            return;
        case State::INTERP_STR_DQUOTE:
            push_interp_string_constant();
            return;
        case State::HEREDOC_EVAL_STR:
            if (!try_push_heredoc_endmarker())
            {
                push_heredoc_eval_string();
            }

            return;
        default:
            throw std::runtime_error("Lexer::match(): fall-thru to unknown state");
    }

    throw std::runtime_error("Error: unrecognized token.\n\n" + m_cSource.context());
}

void Lexer::next_state()
{
    m_eState = m_gStateTransitions[static_cast<int>(m_eState)][1];
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
                    case Token::Type::CONTINUATION:
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
                    case Token::Type::CONTINUATION:
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
                    case Token::Type::CONTINUATION:
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
                    case Token::Type::EX_FUNCTION:
                    case Token::Type::EX_UNLET:
                    case Token::Type::EX_LOCKVAR:
                    case Token::Type::EX_UNLOCKVAR:
                    case Token::Type::CONTINUATION:
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
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                    case Token::Type::CONTINUATION:
                        continue;
                    case Token::Type::EX_CATCH:
                        apCurrentToken->setType(Token::Type::SLASH);
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

bool Lexer::try_push_comment()
{
    if (m_cSource.remaining_text().at(0) != '"')
    {
        return false;
    }

    if (m_cSource.column() == m_cSource.indent())
    {
        // If " is the first non-whitespace of a line, it's a comment
        push_token(Token::Type::COMMENT, m_cSource.remaining_line());
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

    push_token(Token::Type::COMMENT, m_cSource.remaining_line());
    return true;
}

bool Lexer::try_push_register()
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
            push_token(Token::Type::REGISTER, c);
            return true;
        case ':':
        case '.':
        case '%':
            // test/error/E354_1.out
            throw VimError("E354", m_cSource.context());
        default:
            if (std::isalnum(c))
            {
                push_token(Token::Type::REGISTER, c);
                return true;
            }
    }

    // test/error/E354_2.out
    throw VimError("E354", m_cSource.context());
}

bool Lexer::try_push_continuation()
{
    // A line continuation consists of a newline, optional whitespace, and a backslash.

    if (m_cSource.remaining_text().at(0) != '\n')
    {
        return false;
    }

    for (size_t i = m_cSource.pos() + 1; i < m_cSource.size(); i++)
    {
        switch (m_cSource[i])
        {
            case ' ':
            case '\t':
                continue;
            case '\\':
            {
                std::string_view lsText = m_cSource.remaining_text().substr(0, i - m_cSource.pos() + 1);
                push_token(Token::Type::CONTINUATION, lsText);
                return true;
            }
            default:
                return false;
        }
    }

    return false;
}

bool Lexer::try_push_heredoc_endmarker()
{
    if (m_pEndMarker != nullptr && (m_cSource.line_text() == m_pEndMarker->str()))
    {
        push_token(Token::Type::ENDMARKER, m_cSource.word());
        m_pEndMarker = nullptr;
        m_eState = State::NONE;
        return true;
    }

    return false;
}

bool Lexer::try_push_symbol()
{
    for (const Symbol& lcSymbol : m_lSymbols)
    {
        if (vf::startswith(m_cSource.remaining_text(), lcSymbol.sLexeme))
        {
            push_token(lcSymbol.eTokenType, lcSymbol.sLexeme);
            return true;
        }
    }

    return false;
}

bool Lexer::try_push_command()
{
    // A command must appear either as the first non-whitespace of the line, or preceded
    // by optional whitespace plus a line continuation or integer (eg, [n]echow).

    if (m_cSource.column() != m_cSource.indent())
    {
        for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
        {
            const Token* pPrevToken = *rit;

            switch (pPrevToken->type())
            {
                case Token::Type::TAB:
                case Token::Type::SPACE:
                    continue;
                case Token::Type::INTEGER:
                case Token::Type::CONTINUATION:
                    goto maybe_excmd;
                default:
                    return false;
            }
        }
    }

maybe_excmd:

    // We passed the initial checks. Now see if string matches.

    for (const Command& lcCommand : m_lCommands)
    {
        if (vf::startswith(m_cSource.remaining_text(), lcCommand.sFull, g_sKeyWordDelimiters))
        {
            push_token(lcCommand.eTokenType, lcCommand.sFull);
            return true;
        }
        else if (lcCommand.sAbrv.empty())
        {
            continue;
        }
        else if (vf::startswith(m_cSource.remaining_text(), lcCommand.sAbrv, g_sKeyWordDelimiters))
        {
            push_token(lcCommand.eTokenType, lcCommand.sAbrv);
            return true;
        }
    }

    return false;
}

bool Lexer::try_push_keyword()
{
    if (m_cSource.column() == m_cSource.indent())
    {
        return false;
    }

    Token* pToken = nullptr;

    for (const Keyword& lcKeyword : m_lKeywords)
    {
        if (vf::startswith(m_cSource.remaining_text(), lcKeyword.sFull, g_sKeyWordDelimiters))
        {
            pToken = new Token(lcKeyword.eTokenType, lcKeyword.sFull, m_cSource.pos());
            break;
        }
    }

    if (pToken == nullptr)
    {
        return false;
    }

    // We matched a keyword. Check the context to see if it stays a keyword.
    //
    // Example:
    //
    //  let is = 1     <-- 'is' is an identifier
    //  echo a is b    <-- 'is' is a keyword

    switch (pToken->type())
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
                    case Token::Type::CONTINUATION:
                    case Token::Type::FN_ABORT:
                    case Token::Type::FN_CLOSURE:
                    case Token::Type::FN_DICT:
                    case Token::Type::FN_RANGE:
                        continue;
                    case Token::Type::R_PAREN:
                        goto end;
                    default:
                        pToken->setType(Token::Type::IDENTIFIER);
                        goto end;
                }
            }

            break;
        case Token::Type::IN:
            for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
            {
                Token* pPrevToken = *rit;

                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                    case Token::Type::CONTINUATION:
                        continue;
                    case Token::Type::R_BRACKET:
                    case Token::Type::IDENTIFIER:
                        goto end;
                    default:
                        pToken->setType(Token::Type::IDENTIFIER);
                        goto end;
                }
            }

            break;
        case Token::Type::OP_IS:
        case Token::Type::OP_ISNOT:
            for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
            {
                Token* pPrevToken = *rit;

                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                    case Token::Type::CONTINUATION:
                        continue;
                    case Token::Type::IDENTIFIER:
                    case Token::Type::INTEGER:
                    case Token::Type::SQUOTE:
                    case Token::Type::DQUOTE:
                    case Token::Type::FLOAT:
                    case Token::Type::R_BRACKET:
                    case Token::Type::R_PAREN:
                        goto end;
                    default:
                        pToken->setType(Token::Type::IDENTIFIER);
                        goto end;
                }
            }

            break;
        case Token::Type::HD_EVAL:
        case Token::Type::HD_TRIM:
            for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
            {
                Token* pPrevToken = *rit;

                switch (pPrevToken->type())
                {
                    case Token::Type::TAB:
                    case Token::Type::SPACE:
                    case Token::Type::CONTINUATION:
                    case Token::Type::HD_EVAL:
                    case Token::Type::HD_TRIM:
                        continue;
                    case Token::Type::ASSIGN_HEREDOC:
                        goto end;
                    default:
                        pToken->setType(Token::Type::IDENTIFIER);
                        goto end;
                }
            }

            break;
        default:
            throw std::runtime_error("Error: cannot re-type.\n\n" + m_cSource.context());
    }

end:

    push_token(pToken);
    return true;
}

bool Lexer::try_push_regex()
{
    for (auto it = m_lReSpec.cbegin(); it != m_lReSpec.cend(); ++it)
    {
        const std::regex& lcRe = it->first;
        const Token::Type& leTokenType = it->second;

        std::smatch lcMatch;
        std::string lsText { m_cSource.remaining_text() };
        if (std::regex_search(lsText, lcMatch, lcRe))
        {
            push_token(leTokenType, lcMatch.str());
            return true;
        }
    }

    return false;
}

bool Lexer::try_push_number()
{
    std::string_view lsLexeme;

    // float
    if (vf::startswith_float(m_cSource.remaining_text(), lsLexeme))
    {
        push_token(Token::Type::FLOAT, std::string { lsLexeme });
        return true;
    }

    // integer
    int lnBase;
    if (vf::startswith_int(m_cSource.remaining_text(), lsLexeme, &lnBase))
    {
        push_token(Token::Type::INTEGER, std::string { lsLexeme });

        if (m_cSource.column() == m_cSource.indent() && lnBase == 10)
        {
            // This integer may be a [count] preceding an ex command (eg :4echow 'foo').
            m_pCount = m_lTokens.back();
        }

        return true;
    }

    return false;
}

bool Lexer::try_push_blob()
{
    if (!vf::startswith(m_cSource.remaining_text(), "0z")
        && !(vf::startswith(m_cSource.remaining_text(), "0Z")))
    {
        return false;
    }

    size_t lnEnd = m_cSource.remaining_text().find_first_not_of(vf::DIGITS_BASE_16, 2);

    if (lnEnd == std::string_view::npos)
    {
        // 0[zZ] by itself is a valid blob-literal
    }
    else if (lnEnd % 2 != 0)
    {
        // test/error/E973_1.out
        throw VimError("E973", m_cSource.context());
    }

    push_token(Token::Type::BLOB, m_cSource.remaining_text().substr(0, lnEnd));
    return true;
}

void Lexer::push_token(Token::Type aeTokenType, char asLexeme)
{
    push_token(aeTokenType, std::string { asLexeme });
}

void Lexer::push_token(Token::Type aeTokenType, std::string_view asLexeme)
{
    push_token(aeTokenType, std::string { asLexeme });
}

void Lexer::push_token(Token::Type aeTokenType, const std::string& asLexeme)
{
    push_token(new Token(aeTokenType, asLexeme, m_cSource.pos()));
}

void Lexer::push_token(Token* apToken)
{
    if (apToken->is_ambiguous() && !disambiguate(apToken))
    {
        throw std::runtime_error("Error: cannot disambiguate.\n\n" + m_cSource.context());
    }

    if (apToken->type() != Token::Type::END)
    {
        m_cSource.advance(apToken->str().size());
    }

    m_lTokens.push_back(apToken);
}

void Lexer::expect_token(Token::Type aeTokenType, const std::string& asLexeme)
{
    if (!vf::startswith(m_cSource.remaining_text(), asLexeme))
    {
        throw std::runtime_error("Expected " + vf::sanitize(asLexeme) + ".\n\n" + m_cSource.context());
    }

    push_token(aeTokenType, asLexeme);
}

/**
 *  KD 1: String tokenizing
 *
 *  String processing has a lot of shared functionality. For example, tokenizing a
 *  literal string vs. interpolated literal string are essentially the same thing. In the
 *  latter case, curly braces act as delimiters in addition to single quotes. At one
 *  point (Nov 2026), string processing was handled by one giant function. It was nice
 *  that the shared logic was co-located, but interleaving different states, delimiters,
 *  and error conditions became too much to keep track of. It was decided to handle
 *  string processing in separate functions, even though the logic is somewhat repeated.
 */

void Lexer::push_literal_string()
{
    std::string_view lsRemainingText = m_cSource.remaining_text();

    for (size_t i = 0; i < lsRemainingText.size(); i++)
    {
        const char c = lsRemainingText[i];

        if (c != '\'')
        {
            continue;
        }
        else if (lsRemainingText.size() == i + 1 || lsRemainingText[i + 1] != c)
        {
            push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
            push_token(Token::Type::SQUOTE, c);
            return;
        }
        else
        {
            i++;
        }
    }

    throw VimError("E115", m_cSource.context());
}

void Lexer::push_interp_literal_string()
{
    std::string_view lsRemainingText = m_cSource.remaining_text();

    for (size_t i = 0; i < lsRemainingText.size(); i++)
    {
        const char c = lsRemainingText[i];

        if (c != '\'' && c != '{' && c != '}')
        {
            continue;
        }

        if (lsRemainingText.size() == i + 1 || lsRemainingText[i + 1] != c)
        {
            switch (c)
            {
                case '\'':
                    push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
                    push_token(Token::Type::SQUOTE, c);
                    m_eState = State::NONE;
                    return;
                case '{':
                    push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
                    push_token(Token::Type::L_BRACE, c);
                    m_eState = State::INTERP_EXP_SQUOTE;
                    return;
                case '}':
                    // test/error/E1278_1.out
                    throw VimError("E1278", m_cSource.context());
            }
        }
        else
        {
            i++;
        }
    }

    throw VimError("E115", m_cSource.context());
}

void Lexer::push_string_constant()
{
    std::string_view lsRemainingText = m_cSource.remaining_text();

    for (size_t i = 0; i < lsRemainingText.size(); i++)
    {
        const char c = lsRemainingText[i];

        if (c != DQUOTE)
        {
            continue;
        }
        if (i == 0 || lsRemainingText[i - 1] != '\\')
        {
            push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
            push_token(Token::Type::DQUOTE, c);
            return;
        }
    }

    throw VimError("E114", m_cSource.context());
}

void Lexer::push_interp_string_constant()
{
    std::string_view lsRemainingText = m_cSource.remaining_text();

    for (size_t i = 0; i < lsRemainingText.size(); i++)
    {
        const char c = lsRemainingText[i];

        if (c == '"')
        {
            // " can only be escaped by a preceding \.

            if (i == 0 || lsRemainingText[i - 1] != '\\')
            {
                push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
                push_token(Token::Type::DQUOTE, c);
                m_eState = State::NONE;
                return;
            }
        }
        else if (c == '{' || c == '}')
        {
            // { and } can be escaped by a preceding \ or doubling.

            bool bBackSlashEscape = i > 0 && lsRemainingText[i - 1] == '\\';
            bool bDoublingEscape = lsRemainingText.size() > i + 1 && lsRemainingText[i + 1] == c;

            if (!bBackSlashEscape && !bDoublingEscape)
            {
                switch (c)
                {
                    case '{':
                        push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
                        push_token(Token::Type::L_BRACE, c);
                        m_eState = State::INTERP_EXP_DQUOTE;
                        return;
                    case '}':
                        // test/error/E1278_1.out
                        throw VimError("E1278", m_cSource.context());
                }
            }
            else if (bDoublingEscape)
            {
                i++;
            }
        }
    }

    throw VimError("E114", m_cSource.context());
}

void Lexer::push_heredoc_string()
{
    std::string_view lsRemainingText = m_cSource.remaining_text();

    for (size_t i = 0; i < lsRemainingText.size(); i++)
    {
        if (lsRemainingText[i] == '\n')
        {
            push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
            push_token(Token::Type::NEWLINE, lsRemainingText[i]);
            return;
        }
    }
}

void Lexer::push_heredoc_eval_string()
{
    std::string_view lsRemainingText = m_cSource.remaining_text();

    for (size_t i = 0; i < lsRemainingText.size(); i++)
    {
        const char c = lsRemainingText[i];

        if (c != '\n' && c != '{' && c != '}')
        {
            continue;
        }

        if (lsRemainingText.size() == i + 1 || lsRemainingText[i + 1] != c)
        {
            switch (c)
            {
                case '\n':
                    push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
                    push_token(Token::Type::NEWLINE, c);
                    return;
                case '{':
                    push_token(Token::Type::STRING, lsRemainingText.substr(0, i));
                    push_token(Token::Type::L_BRACE, c);
                    m_eState = State::HEREDOC_EVAL_EXP;
                    return;
                case '}':
                    // test/error/E1278_1.out
                    throw VimError("E1278", m_cSource.context());
            }
        }
        else
        {
            i++;
        }
    }
}
