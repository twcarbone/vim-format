#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token
{
public:
    enum class Type
    {
        NONE,

        KEYWORD_NONE,

        IDENTIFIER_NONE,

        LITERAL_INTEGER,
        LITERAL_FLOAT,
        LITERAL_CHAR,
        LITERAL_STRING,

        OPERATOR_ARITHMETIC,
        OPERATOR_RELATIONAL,
        OPERATOR_LOGICAL,
        OPERATOR_ASSIGNMENT,
        OPERATOR_BITWISE,
        OPERATOR_UNARY,

        SPECIAL_PAREN_OPEN,
        SPECIAL_PAREN_CLOSED,
        SPECIAL_BRACKET_OPEN,
        SPECIAL_BRACKET_CLOSED,
        SPECIAL_BRACE_OPEN,
        SPECIAL_BRACE_CLOSED,
        SPECIAL_SEMICOLON
    };

    Token();
    Token(Type type, const std::string& str);

    ~Token();

    Type type() const;
    std::string str() const;

    std::string toString() const;

private:
    Type m_eType;
    std::string m_sStr;

    static std::string TypeToStr(Type type);
};

#endif  // TOKEN_H
