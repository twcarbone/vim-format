#include <string>

#include "Token.h"

Token::Token() :
    m_eType { Token::Type::NONE },
    m_sStr { "null" }
{
}

Token::Token(Token::Type aeType, const std::string& asStr) :
    m_eType { aeType },
    m_sStr { asStr }
{
}

Token::~Token()
{
}

Token::Type Token::type() const
{
    return m_eType;
}

std::string Token::str() const
{
    return m_sStr;
}

std::string Token::toString() const
{
    std::string lsTmp = "[" + TypeToStr(m_eType) + "]";

    switch (m_eType)
    {
        case Type::NEWLINE:
        {
            break;
        }
        default:
        {
            lsTmp += " " + m_sStr;
        }
    }

    return lsTmp;
}

std::string Token::TypeToStr(Type aeType)
{
    switch (aeType)
    {
        case Type::SPACE:
            return "SPACE";
        case Type::NEWLINE:
            return "NEWLINE";
        case Type::TAB:
            return "TAB";

        case Type::KEYWORD:
            return "KEYWORD";

        case Type::IDENTIFIER:
            return "IDENTIFIER";

        case Type::LITERAL_INTEGER:
            return "LITERAL_INTEGER";
        case Type::LITERAL_FLOAT:
            return "LITERAL_FLOAT";
        case Type::LITERAL_CHAR:
            return "LITERAL_CHAR";
        case Type::LITERAL_STRING:
            return "LITERAL_STRING";

        case Type::OPERATOR_ARITHMETIC:
            return "OPERATOR_ARITHMETIC";
        case Type::OPERATOR_RELATIONAL:
            return "OPERATOR_RELATIONAL";
        case Type::OPERATOR_LOGICAL:
            return "OPERATOR_LOGICAL";
        case Type::OPERATOR_ASSIGNMENT:
            return "OPERATOR_ASSIGNMENT";
        case Type::OPERATOR_BITWISE:
            return "OPERATOR_BITWISE";
        case Type::OPERATOR_UNARY:
            return "OPERATOR_UNARY";

        case Type::SPECIAL_PAREN_OPEN:
            return "SPECIAL_PAREN_OPEN";
        case Type::SPECIAL_PAREN_CLOSED:
            return "SPECIAL_PAREN_CLOSED";
        case Type::SPECIAL_BRACKET_OPEN:
            return "SPECIAL_BRACKET_OPEN";
        case Type::SPECIAL_BRACKET_CLOSED:
            return "SPECIAL_BRACKET_CLOSED";
        case Type::SPECIAL_BRACE_OPEN:
            return "SPECIAL_BRACE_OPEN";
        case Type::SPECIAL_BRACE_CLOSED:
            return "SPECIAL_BRACE_CLOSED";
        case Type::SPECIAL_SEMICOLON:
            return "SPECIAL_SEMICOLON";

        case Type::NONE:
        default:
            break;
    }

    return "NONE";
}

bool operator==(const Token& lhs, const Token& rhs)
{
    return lhs.m_eType == rhs.m_eType && lhs.m_sStr == rhs.m_sStr;
}

bool operator!=(const Token& lhs, const Token& rhs)
{
    return !(lhs == rhs);
}
