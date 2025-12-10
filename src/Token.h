#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token
{
public:
    enum class Type
    {
        NONE,

        TAB,
        END,  // EOF
        SPACE,
        NEWLINE,

        KEYWORD,

        IDENTIFIER,

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

        L_PAREN,
        R_PAREN,
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

    static std::string TypeToStr(Type type);

    friend bool operator==(const Token& lhs, const Token& rhs);
    friend bool operator!=(const Token& lhs, const Token& rhs);

private:
    Type m_eType;
    std::string m_sStr;
};

#endif  // TOKEN_H
