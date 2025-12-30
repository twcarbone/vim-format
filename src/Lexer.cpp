#include <exception>
#include <string_view>

#include "Lexer.h"
#include "Token.h"

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
    return m_lTokens;
}

const Source& Lexer::source() const
{
    return m_cSource;
}

const Token& Lexer::token(size_t anIdx) const
{
    // TODO (gh-7): throw IndexError
    return *m_lTokens.at(anIdx);
}

Token* Lexer::next()
{
    Token* pToken = do_next();

    while (pToken->delimiting_wp())
    {
        pToken = do_next();
    }

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
            throw std::runtime_error("Error: unrecognized token.\n\n" + m_cSource.traceback());
        }

        if (pToken->ambiguous() && !disambiguate(pToken))
        {
            throw std::runtime_error("Error: cannot disambiguate.\n\n" + m_cSource.traceback());
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

/**
 *  @notes
 *      1.  :h expr9 suggests unary operators apply to List, Dictionary. But we actually
 *          get E745, E728.
 */
bool Lexer::disambiguate(Token* apCurrentToken)
{
    //
    // GEN_MINUS
    //

    if (apCurrentToken->type() == Token::Type::GEN_MINUS)
    {
        if (m_lTokens.empty())
        {
            apCurrentToken->setType(Token::Type::OP_UNARY_MINUS);
        }
        else
        {
            switch (m_lTokens.back()->type())
            {
                case Token::Type::FLOAT:
                case Token::Type::INTEGER:
                case Token::Type::STRING:
                case Token::Type::R_PAREN:
                case Token::Type::IDENTIFIER:
                    apCurrentToken->setType(Token::Type::OP_SUB);
                    break;
                default:
                    // See @note 1
                    apCurrentToken->setType(Token::Type::OP_UNARY_MINUS);
            }
        }
    }

    //
    // GEN_PLUS
    //

    else if (apCurrentToken->type() == Token::Type::GEN_PLUS)
    {
        if (m_lTokens.empty())
        {
            apCurrentToken->setType(Token::Type::OP_UNARY_PLUS);
        }
        else
        {
            switch (m_lTokens.back()->type())
            {
                case Token::Type::FLOAT:
                case Token::Type::INTEGER:
                case Token::Type::STRING:
                case Token::Type::R_PAREN:
                case Token::Type::IDENTIFIER:
                    apCurrentToken->setType(Token::Type::OP_ADD);
                    break;
                default:
                    // See @note 1
                    apCurrentToken->setType(Token::Type::OP_UNARY_PLUS);
            }
        }
    }

    //
    // GEN_QUESTION
    //

    else if (apCurrentToken->type() == Token::Type::GEN_QUESTION)
    {
        if (m_lTokens.empty())
        {
            // Nothing
        }
        else
        {
            switch (m_lTokens.back()->type())
            {
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
    }

    //
    // GEN_NAME
    //

    else if (apCurrentToken->type() == Token::Type::GEN_NAME)
    {
        if (m_lTokens.empty())
        {
            apCurrentToken->setType(Token::Type::IDENTIFIER);
        }
        else
        {
            switch (m_lTokens.back()->type())
            {
                case Token::Type::OP_OPTION:
                    apCurrentToken->setType(Token::Type::OPTION);
                    break;
                default:
                    apCurrentToken->setType(Token::Type::IDENTIFIER);
            }
        }
    }

    //
    // GEN_EXCLAMATION
    //

    else if (apCurrentToken->type() == Token::Type::GEN_EXCLAMATION)
    {
        if (m_lTokens.empty())
        {
            apCurrentToken->setType(Token::Type::OP_LOGICAL_NOT);
        }
        else
        {
            switch (m_lTokens.back()->type())
            {
                case Token::Type::FUNCTION:
                    apCurrentToken->setType(Token::Type::OP_BANG);
                    break;
                default:
                    apCurrentToken->setType(Token::Type::OP_LOGICAL_NOT);
            }
        }
    }

    //
    // GEN_COLON
    //

    else if (apCurrentToken->type() == Token::Type::GEN_COLON)
    {
        for (auto rit = m_lTokens.crbegin(); rit != m_lTokens.crend(); rit++)
        {
            switch ((*rit)->type())
            {
                case Token::Type::L_BRACKET:
                    apCurrentToken->setType(Token::Type::OP_SLICE);
                    break;
                case Token::Type::OP_TERNARY_IF:
                    apCurrentToken->setType(Token::Type::OP_TERNARY_ELSE);
                    break;
                default:
                    break;
            }
        }
    }

    return !apCurrentToken->ambiguous();
}
