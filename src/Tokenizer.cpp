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

std::vector<Token*>* Tokenizer::tokens() const
{
    return m_pTokens;
}

const Token& Tokenizer::token(size_t anIdx) const
{
    // TODO: throw IndexError
    return *m_pTokens->at(anIdx);
}

Token* Tokenizer::next()
{
    Token* pToken;

    if (!hasMoreTokens())
    {
        // TODO: throw exception if EOF has already been returned
        pToken = new Token(Token::Type::END, "EOF");
    }
    else
    {
        const std::string& lsRemainingText = m_sText.substr(m_nCursor, -1);

        pToken = m_pTokenSpec->match(lsRemainingText);

        if (pToken == nullptr)
        {
            throw std::runtime_error("Unrecognized token");
        }

        if (pToken->ambiguous() && !disambiguate(pToken))
        {
            throw std::runtime_error("Cannot disambiguate " + pToken->toString());
        }

        m_nCursor += pToken->str().size();
    }

    m_pTokens->push_back(pToken);
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

bool Tokenizer::disambiguate(Token* apCurrentToken)
{
    // MINUS

    if (apCurrentToken->type() == Token::Type::MINUS)
    {
        if (m_pTokens->empty())
        {
            apCurrentToken->setType(Token::Type::OP_MINUS);
        }
        else
        {
            switch (m_pTokens->back()->type())
            {
                case Token::Type::FLOAT:
                case Token::Type::INTEGER:
                case Token::Type::R_PAREN:
                    apCurrentToken->setType(Token::Type::OP_SUB);
                    break;
                default:
                    apCurrentToken->setType(Token::Type::OP_MINUS);
            }
        }
    }

    // PLUS

    else if (apCurrentToken->type() == Token::Type::PLUS)
    {
        if (m_pTokens->empty())
        {
            apCurrentToken->setType(Token::Type::OP_PLUS);
        }
        else
        {
            switch (m_pTokens->back()->type())
            {
                case Token::Type::FLOAT:
                case Token::Type::INTEGER:
                case Token::Type::R_PAREN:
                    apCurrentToken->setType(Token::Type::OP_ADD);
                    break;
                default:
                    apCurrentToken->setType(Token::Type::OP_PLUS);
            }
        }
    }

    return !apCurrentToken->ambiguous();
}
