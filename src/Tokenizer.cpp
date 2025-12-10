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

    do
    {
        m_pTokens->push_back(next());
    } while (m_pTokens->back()->type() != Token::Type::END);
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
    if (!hasMoreTokens())
    {
        // TODO: throw exception if EOF has already been returned
        return new Token(Token::Type::END, "EOF");
    }

    const std::string& lsRemainingText = m_sText.substr(m_nCursor, -1);

    Token* pToken = m_pTokenSpec->match(lsRemainingText);

    if (pToken == nullptr)
    {
        throw std::runtime_error("Unrecognized token");
    }

    m_nCursor += pToken->str().size();

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
