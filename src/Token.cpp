#include <string>

#include "Token.h"

Token::Token() :
    Token(Type::NONE, "null")
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
        case Type::GEN_NAME:
        case Type::GEN_PLUS:
        case Type::GEN_MINUS:
        case Type::GEN_QUESTION:
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
        case Type::CMD_LET:
            return "CMD_LET";
        case Type::CMD_ECHO:
            return "CMD_ECHO";
        case Type::CMD_SET:
            return "CMD_SET";
        case Type::GEN_PLUS:
            return "GEN_PLUS";
        case Type::GEN_MINUS:
            return "GEN_MINUS";
        case Type::GEN_QUESTION:
            return "GEN_QUESTION";
        case Type::GEN_NAME:
            return "GEN_NAME";
        case Type::OP_ADD:
            return "OP_ADD";
        case Type::OP_SUB:
            return "OP_SUB";
        case Type::OP_UNARY_PLUS:
            return "OP_UNARY_PLUS";
        case Type::OP_UNARY_MINUS:
            return "OP_UNARY_MINUS";
        case Type::OP_MUL:
            return "OP_MUL";
        case Type::OP_DIV:
            return "OP_DIV";
        case Type::OP_MODULO:
            return "OP_MODULO";
        case Type::OP_EQUAL:
            return "OP_EQUAL";
        case Type::OP_NEQUAL:
            return "OP_NEQUAL";
        case Type::OP_GT:
            return "OP_GT";
        case Type::OP_GTE:
            return "OP_GTE";
        case Type::OP_LT:
            return "OP_LT";
        case Type::OP_LTE:
            return "OP_LTE";
        case Type::OP_MATCH:
            return "OP_MATCH";
        case Type::OP_NMATCH:
            return "OP_NMATCH";
        case Type::OP_IS:
            return "OP_IS";
        case Type::OP_ISNOT:
            return "OP_ISNOT";
        case Type::OP_OR:
            return "OP_OR";
        case Type::OP_AND:
            return "OP_AND";
        case Type::OP_MATCH_CASE:
            return "OP_MATCH_CASE";
        case Type::OP_IGNORE_CASE:
            return "OP_IGNORE_CASE";
        case Type::OP_TERNARY_IF:
            return "OP_TERNARY_IF";
        case Type::OP_TERNARY_ELSE:
            return "OP_TERNARY_ELSE";
        case Type::OP_FALSEY:
            return "OP_FALSEY";
        case Type::OP_LSHIFT:
            return "OP_LSHIFT";
        case Type::OP_RSHIFT:
            return "OP_RSHIFT";
        case Type::OP_CAT_OLD:
            return "OP_CAT_OLD";
        case Type::OP_CAT_NEW:
            return "OP_CAT_NEW";
        case Type::OP_LOGICAL_NOT:
            return "OP_LOGICAL_NOT";
        case Type::OP_OPTION:
            return "OP_OPTION";
        case Type::ASSIGN_ADD:
            return "ASSIGN_ADD";
        case Type::ASSIGN_MINUS:
            return "ASSIGN_MINUS";
        case Type::ASSIGN_MUL:
            return "ASSIGN_MUL";
        case Type::ASSIGN_DIV:
            return "ASSIGN_DIV";
        case Type::ASSIGN_EQ:
            return "ASSIGN_EQ";
        case Type::ASSIGN_MODULO:
            return "ASSIGN_MODULO";
        case Type::ASSIGN_CAT_OLD:
            return "ASSIGN_CAT_OLD";
        case Type::ASSIGN_CAT_NEW:
            return "ASSIGN_CAT_NEW";
        case Type::L_PAREN:
            return "L_PAREN";
        case Type::R_PAREN:
            return "R_PAREN";
        case Type::L_BRACKET:
            return "L_BRACKET";
        case Type::R_BRACKET:
            return "R_BRACKET";
        case Type::COMMA:
            return "COMMA";
        case Type::IF:
            return "IF";
        case Type::ELSEIF:
            return "ELSEIF";
        case Type::ELSE:
            return "ELSE";
        case Type::ENDIF:
            return "ENDIF";
        case Type::WHILE:
            return "WHILE";
        case Type::ENDWHILE:
            return "ENDWHILE";
        case Type::FOR:
            return "FOR";
        case Type::IN:
            return "IN";
        case Type::ENDFOR:
            return "ENDFOR";
        case Type::BREAK:
            return "BREAK";
        case Type::CONTINUE:
            return "CONTINUE";
        case Type::END:
            return "END";
        case Type::TAB:
            return "TAB";
        case Type::NEWLINE:
            return "NEWLINE";
        case Type::SPACE:
            return "SPACE";
        case Type::OPTION:
            return "OPTION";
        case Type::IDENTIFIER:
            return "IDENTIFIER";
        case Type::STRING:
            return "STRING";
        case Type::INTEGER:
            return "INTEGER";
        case Type::FLOAT:
            return "FLOAT";
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
