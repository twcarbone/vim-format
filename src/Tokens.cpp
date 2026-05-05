#include "Tokens.h"

Tokens::~Tokens()
{
    clear();
}

Token* Tokens::at(size_t anPosition) const
{
    return m_lData.at(anPosition);
}

size_t Tokens::pos() const
{
    return m_nPos;
}

Token* Tokens::back() const
{
    return m_lData.back();
}

Token* Tokens::head() const
{
    return m_lData[m_nPos];
}

Tokens::const_iterator Tokens::begin() const
{
    return m_lData.begin();
}

Tokens::const_iterator Tokens::cbegin() const noexcept
{
    return m_lData.cbegin();
}

Tokens::const_iterator Tokens::end() const
{
    return m_lData.end();
}

Tokens::const_iterator Tokens::cend() const noexcept
{
    return m_lData.cend();
}

Tokens::const_reverse_iterator Tokens::crbegin() const noexcept
{
    return m_lData.crbegin();
}

Tokens::const_reverse_iterator Tokens::crend() const noexcept
{
    return m_lData.crend();
}

void Tokens::reserve(size_t anCapacity)
{
    m_lData.reserve(anCapacity);
}

bool Tokens::empty() const
{
    return m_lData.empty();
}

size_t Tokens::size() const
{
    return m_lData.size();
}

void Tokens::push_back(Token* apToken)
{
    m_lData.push_back(apToken);
}

void Tokens::clear()
{
    for (const Token* pToken : m_lData)
    {
        delete pToken;
    }

    m_lData.clear();
}

void Tokens::seek(size_t anPos)
{
    if (anPos < 0 || anPos > m_lData.size())
    {
        throw std::runtime_error("Tokens::seek: attempt to access index " + std::to_string(anPos)
                                 + " (size = " + std::to_string(m_lData.size()) + ")");
    }

    m_nPos = anPos;
}

Token* Tokens::peek(int anCount, int anFlags) const
{
    return m_lData[m_nPos + move(anCount, anFlags)];
}

Token* Tokens::advance(int anCount, int anFlags)
{
    return m_lData[m_nPos += move(anCount, anFlags)];
}

int Tokens::move(int anCount, int anFlags) const
{
    Token* pToken = nullptr;

    size_t i;
    size_t lnSteps = std::abs(anCount);

    for (i = 1; lnSteps > 0; i++)
    {
        const int lnOffset = anCount >= 0 ? i : -1 * i;
        const int lnPos = m_nPos + lnOffset;

        if (lnPos < 0 || lnPos > m_lData.size() - 1)
        {
            throw std::runtime_error("Tokens::move: attempt to access index " + std::to_string(lnPos)
                                     + " (size = " + std::to_string(m_lData.size()) + ")");
        }

        Token* pToken = m_lData[lnPos];

        switch (pToken->type())
        {
            case Token::Type::SPACE:
            case Token::Type::TAB:
                if (anFlags & Flags::skipws)
                {
                    continue;
                }

                break;
            case Token::Type::NEWLINE:
                if (anFlags & Flags::skipnewline)
                {
                    continue;
                }

                break;
            default:
                break;
        }

        lnSteps--;
    }

    i--;
    return anCount >= 0 ? i : -1 * i;
}
