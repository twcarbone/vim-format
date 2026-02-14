#include <exception>
#include <string_view>

#include "Lexer.h"
#include "Token.h"
#include "util.h"

Lexer::Lexer(const Context& acContext) :
    m_cSource { acContext.source() }
{
    m_pTokenSpec = new TokenSpec();
}

Lexer::~Lexer()
{
    freeTokens();
    delete m_pTokenSpec;
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
        if (!pToken->delimiting_wp())
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
        pToken = m_pTokenSpec->match(m_cSource);

        if (pToken == nullptr)
        {
            throw std::runtime_error("Error: unrecognized token.\n\n" + m_cSource.context());
        }

        if (pToken->ambiguous() && !disambiguate(pToken))
        {
            throw std::runtime_error("Error: cannot disambiguate.\n\n" + m_cSource.context());
        }

        m_cSource.advance(pToken->str().size());
    }

    return pToken;
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

        if (apCurrentToken->type() == Token::Type::GEN_MINUS)
        {
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
        }

        else if (apCurrentToken->type() == Token::Type::GEN_PLUS)
        {
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
        }

        else if (apCurrentToken->type() == Token::Type::GEN_QUESTION)
        {
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
        }

        else if (apCurrentToken->type() == Token::Type::GEN_NAME)
        {
            switch (pPrevToken->type())
            {
                case Token::Type::TAB:
                case Token::Type::SPACE:
                    continue;
                case Token::Type::OP_OPTION:
                    apCurrentToken->setType(Token::Type::OPTION);
                    break;
                default:
                    apCurrentToken->setType(Token::Type::IDENTIFIER);
            }
        }

        else if (apCurrentToken->type() == Token::Type::GEN_EXCLAMATION)
        {
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
        }

        else if (apCurrentToken->type() == Token::Type::GEN_DOT)
        {
            switch (pPrevToken->type())
            {
                case Token::Type::IDENTIFIER:
                    apCurrentToken->setType(Token::Type::OP_DOT);
                    break;
                default:
                    apCurrentToken->setType(Token::Type::OP_CAT_OLD);
            }
        }

        else if (apCurrentToken->type() == Token::Type::GEN_COLON)
        {
            switch (pPrevToken->type())
            {
                case Token::Type::TAB:
                case Token::Type::SPACE:
                    continue;
                case Token::Type::IDENTIFIER:
                    if (rit == m_lTokens.crbegin() && vf::is_one_of(pPrevToken->str(), "bwtglsav"))
                    {
                        // Previous token must be non-whitespace, single-character, valid
                        // identifier to make this colon a scope resolution operator
                        apCurrentToken->setType(Token::Type::OP_SCOPE);
                    }
                    else
                    {
                        apCurrentToken->setType(Token::Type::COLON);
                    }

                    break;
                default:
                    apCurrentToken->setType(Token::Type::COLON);
            }
        }

        // 2. Stop looking at preceding tokens if the current token was disambiguated.
        if (!apCurrentToken->ambiguous())
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
        case Token::Type::GEN_NAME:
            apCurrentToken->setType(Token::Type::IDENTIFIER);
            break;
        case Token::Type::GEN_EXCLAMATION:
            apCurrentToken->setType(Token::Type::OP_LOGICAL_NOT);
            break;
        default:
            break;
    }

    // 4. Return true only if the current token was disambiguated.
    return !apCurrentToken->ambiguous();
}
