#include <string>

#include "Token.h"

Token::Token() :
    Token(Type::NONE, "null", 0)
{
}

Token::Token(Token::Type aeType, const std::string& asStr, int anSourcePos) :
    m_nSourcePos { anSourcePos },
    m_eType { aeType },
    m_sStr { asStr }
{
}

Token::~Token()
{
}

bool Token::ambiguous() const
{
    switch (m_eType)
    {
        case Type::GEN_NAME:
        case Type::GEN_PLUS:
        case Type::GEN_MINUS:
        case Type::GEN_QUESTION:
        case Type::GEN_EXCLAMATION:
            return true;
        default:
            return false;
    }
}

bool Token::delimiting_wp() const
{
    switch (m_eType)
    {
        case Type::TAB:
        case Type::SPACE:
            return true;
        default:
            return false;
    }
}

bool Token::structural_wp() const
{
    return m_eType == Type::NEWLINE;
}

int Token::source_pos() const
{
    return m_nSourcePos;
}

Token::Type Token::type() const
{
    return m_eType;
}

void Token::setType(Type aeType)
{
    m_eType = aeType;
}

std::string Token::str() const
{
    return m_sStr;
}

std::string Token::toString() const
{
    std::string lsTmp = "[Token] " + TypeToStr(m_eType);

    if (!delimiting_wp() && !structural_wp())
    {
        lsTmp += " " + m_sStr;
    }

    return lsTmp;
}

std::string Token::TypeToStr(Type aeType)
{
    switch (aeType)
    {
        // clang-format off
#define X(name) case Token::Type::name: return #name;
    TOKEN_TYPE_LIST
#undef X
        // clang-format on
    }

    return "NONE";
}

bool operator==(const Token& lhs, const Token& rhs)
{
    return lhs.m_eType == rhs.m_eType && lhs.m_sStr == rhs.m_sStr && lhs.m_nSourcePos == rhs.m_nSourcePos;
}

bool operator!=(const Token& lhs, const Token& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Token& obj)
{
    return os << obj.toString();
}
