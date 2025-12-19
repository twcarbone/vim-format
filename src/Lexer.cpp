#include <exception>
#include <string_view>

#include "Lexer.h"
#include "Token.h"

Lexer::Lexer() :
    Lexer("")
{
}

Lexer::Lexer(std::string_view asText) :
    m_nCursor { 0 },
    m_sText { asText }
{
    m_pTokenSpec = new TokenSpec();
}

Lexer::~Lexer()
{
    freeTokens();
    delete m_pTokenSpec;
}

void Lexer::tokenize(std::string_view asText)
{
    freeTokens();
    m_nCursor = 0;

    if (!asText.empty())
    {
        m_sText = asText;
    }

    while (true)
    {
        next();

        if (m_lTokens.back()->type() == Token::Type::END)
        {
            break;
        }
    }
}

std::string_view Lexer::text() const
{
    return m_sText;
}

std::vector<Token*> Lexer::tokens() const
{
    return m_lTokens;
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

    if (!hasMoreTokens())
    {
        // TODO (gh-6): throw exception if EOF has already been returned
        pToken = new Token(Token::Type::END, "EOF");
    }
    else
    {
        pToken = m_pTokenSpec->match(m_sText.substr(m_nCursor, -1));

        if (pToken == nullptr)
        {
            throw std::runtime_error("Unrecognized token at " + std::to_string(m_nCursor));
        }

        if (pToken->ambiguous() && !disambiguate(pToken))
        {
            throw std::runtime_error("Cannot disambiguate " + pToken->toString());
        }

        m_nCursor += pToken->str().size();
    }

    return pToken;
}

bool Lexer::hasMoreTokens() const
{
    return m_nCursor < m_sText.size();
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
                // See @note 1
                case Token::Type::R_PAREN:
                    apCurrentToken->setType(Token::Type::OP_SUB);
                    break;
                default:
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
                // See @note 1
                case Token::Type::R_PAREN:
                    apCurrentToken->setType(Token::Type::OP_ADD);
                    break;
                default:
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
    return !apCurrentToken->ambiguous();
}
