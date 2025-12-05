#include <exception>
#include <string>

#include "Token.h"
#include "Tokenizer.h"

Tokenizer::Tokenizer(const std::string& asText) :
    m_nCursor { 0 },
    m_sText { asText }
{
    m_pTokens = new std::vector<Token*>;
    m_pTokenSpec = new TokenSpec();
}

Tokenizer::~Tokenizer()
{
    for (const Token* pToken : *m_pTokens)
    {
        delete pToken;
    }
    m_pTokens->clear();

    delete m_pTokenSpec;
}

void Tokenizer::tokenize()
{
    while (hasMoreTokens())
    {
        Token* pToken = next();

        if (pToken == nullptr)
        {
            throw std::runtime_error("Unrecognized token");
        }

        m_pTokens->push_back(pToken);
    }
}

std::vector<Token*>* Tokenizer::tokens() const
{
    return m_pTokens;
}

Token* Tokenizer::next()
{
    const std::string& lsRemainingText = m_sText.substr(m_nCursor, -1);

    Token* pToken = m_pTokenSpec->match(lsRemainingText);

    if (pToken != nullptr)
    {
        m_nCursor += pToken->str().size();
    }

    return pToken;
}

bool Tokenizer::hasMoreTokens() const
{
    return m_nCursor < m_sText.size();
}
