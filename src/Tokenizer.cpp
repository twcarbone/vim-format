#include <exception>
#include <string>

#include "Token.h"
#include "Tokenizer.h"

Tokenizer::Tokenizer() :
    Tokenizer("")
{
}

Tokenizer::Tokenizer(const std::string& asText) :
    m_nCursor { 0 },
    m_sText { asText }
{
    m_pTokens = new std::vector<Token*>;
    m_pTokenSpec = new TokenSpec();
}

Tokenizer::~Tokenizer()
{
    freeTokens();
    delete m_pTokenSpec;
}

void Tokenizer::tokenize(const std::string& asText)
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

        if (m_pTokens->back()->type() == Token::Type::END)
        {
            break;
        }
    }
}

const std::string& Tokenizer::text() const
{
    return m_sText;
}

std::vector<Token*>* Tokenizer::tokens() const
{
    return m_pTokens;
}

const Token& Tokenizer::token(size_t anIdx) const
{
    // TODO (gh-7): throw IndexError
    return *m_pTokens->at(anIdx);
}

Token* Tokenizer::next()
{
    Token* pToken = do_next();

    while (pToken->delimiting_wp())
    {
        pToken = do_next();
    }

    m_pTokens->push_back(pToken);
    return pToken;
}

Token* Tokenizer::do_next()
{
    Token* pToken;

    if (!hasMoreTokens())
    {
        // TODO (gh-6): throw exception if EOF has already been returned
        pToken = new Token(Token::Type::END, "EOF");
    }
    else
    {
        const std::string& lsRemainingText = m_sText.substr(m_nCursor, -1);

        pToken = m_pTokenSpec->match(lsRemainingText);

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

bool Tokenizer::hasMoreTokens() const
{
    return m_nCursor < m_sText.size();
}

void Tokenizer::freeTokens() const
{
    for (const Token* pToken : *m_pTokens)
    {
        delete pToken;
    }

    m_pTokens->clear();
}

/**
 *  @notes
 *      1.  :h expr9 suggests unary operators apply to List, Dictionary. But we actually
 *          get E745, E728.
 */
bool Tokenizer::disambiguate(Token* apCurrentToken)
{
    //
    // GEN_MINUS
    //

    if (apCurrentToken->type() == Token::Type::GEN_MINUS)
    {
        if (m_pTokens->empty())
        {
            apCurrentToken->setType(Token::Type::OP_UNARY_MINUS);
        }
        else
        {
            switch (m_pTokens->back()->type())
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
        if (m_pTokens->empty())
        {
            apCurrentToken->setType(Token::Type::OP_UNARY_PLUS);
        }
        else
        {
            switch (m_pTokens->back()->type())
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
        if (m_pTokens->empty())
        {
            // Nothing
        }
        else
        {
            switch (m_pTokens->back()->type())
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
        if (m_pTokens->empty())
        {
            apCurrentToken->setType(Token::Type::IDENTIFIER);
        }
        else
        {
            switch (m_pTokens->back()->type())
            {
                case Token::Type::OP_OPTION:
                    apCurrentToken->setType(Token::Type::OPTION);
                    break;
                default:
                    apCurrentToken->setType(Token::Type::IDENTIFIER);
            }
        }
    }

    return !apCurrentToken->ambiguous();
}
