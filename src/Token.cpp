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

bool Token::ambiguous() const
{
    switch (m_eType)
    {
        case Type::PLUS:
        case Type::MINUS:
            return true;
        default:
            return false;
    }
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
    std::string lsTmp = "[Token] " + TypeToStr(m_eType);

    if (ambiguous())
    {
        lsTmp += " (A)";
    }

    switch (m_eType)
    {
        case Type::NEWLINE:
        case Type::END:
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
        case Type::END:
            return "END";

        case Type::KEYWORD:
            return "KEYWORD";

        case Type::IDENTIFIER:
            return "IDENTIFIER";

        case Type::INTEGER:
            return "INTEGER";
        case Type::FLOAT:
            return "FLOAT";
        case Type::CHAR:
            return "CHAR";
        case Type::STRING:
            return "STRING";

        case Type::OP_MUL:
            return "OP_MUL";
        case Type::OP_DIV:
            return "OP_DIV";
        case Type::OP_ADD:
            return "OP_ADD";
        case Type::OP_SUB:
            return "OP_SUB";
        case Type::OP_PLUS:
            return "OP_PLUS";
        case Type::OP_MINUS:
            return "OP_MINUS";
        case Type::OP_ASSIGN:
            return "OP_ASSIGN";

        case Type::L_PAREN:
            return "L_PAREN";
        case Type::R_PAREN:
            return "R_PAREN";
        case Type::L_BRACKET:
            return "L_BRACKET";
        case Type::R_BRACKET:
            return "R_BRACKET";
        case Type::L_BRACE:
            return "L_BRACE";
        case Type::R_BRACE:
            return "R_BRACE";
        case Type::SEMICOLON:
            return "SEMICOLON";

        case Type::PLUS:
            return "PLUS";
        case Type::MINUS:
            return "MINUS";

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
