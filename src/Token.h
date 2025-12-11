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

        INTEGER,
        FLOAT,
        CHAR,
        STRING,

        OP_MUL,    // Binary *
        OP_DIV,    // Binary /
        OP_ADD,    // Binary +
        OP_SUB,    // Binary -
        OP_PLUS,   // Unary +
        OP_MINUS,  // Unary -
        OP_ASSIGN,

        L_PAREN,
        R_PAREN,
        L_BRACKET,
        R_BRACKET,
        L_BRACE,
        R_BRACE,
        SEMICOLON,

        MINUS,  // Ambiguous
        PLUS,   // Ambiguous
    };

    Token();
    Token(Type type, const std::string& str);

    ~Token();

    bool ambiguous() const;

    Type type() const;
    void setType(Type type);

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
