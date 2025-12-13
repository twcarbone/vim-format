#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token
{
public:
    enum class Type
    {
        NONE,

        CMD_LET,   // let
        CMD_ECHO,  // echo
        CMD_SET,   // set

        GEN_PLUS,      // +
        GEN_MINUS,     // -
        GEN_QUESTION,  // ?
        GEN_NAME,      // [a-zA-Z_][a-zA-Z0-9_]+

        OP_ADD,           // +
        OP_SUB,           // -
        OP_UNARY_PLUS,    // +
        OP_UNARY_MINUS,   // -
        OP_MUL,           // *
        OP_DIV,           // /
        OP_MODULO,        // %
        OP_EQUAL,         // ==
        OP_NEQUAL,        // !=
        OP_GT,            // >
        OP_GTE,           // >=
        OP_LT,            // <
        OP_LTE,           // <=
        OP_MATCH,         // =~
        OP_NMATCH,        // !~
        OP_IS,            // is
        OP_ISNOT,         // isnot
        OP_OR,            // ||
        OP_AND,           // &&
        OP_MATCH_CASE,    // #
        OP_IGNORE_CASE,   // ?
        OP_TERNARY_IF,    // ?
        OP_TERNARY_ELSE,  // :
        OP_FALSEY,        // ??
        OP_LSHIFT,        // <<
        OP_RSHIFT,        // >>
        OP_CAT_OLD,       // .
        OP_CAT_NEW,       // ..
        OP_LOGICAL_NOT,   // !
        OP_OPTION,        // &

        ASSIGN_ADD,      // +=
        ASSIGN_MINUS,    // -=
        ASSIGN_MUL,      // *=
        ASSIGN_DIV,      // /=
        ASSIGN_EQ,       // =
        ASSIGN_MODULO,   // %=
        ASSIGN_CAT_OLD,  // .=
        ASSIGN_CAT_NEW,  // ..=

        L_PAREN,    // (
        R_PAREN,    // )
        L_BRACKET,  // [
        R_BRACKET,  // ]
        COMMA,      // ,

        IF,        // if
        ELSEIF,    // elseif
        ELSE,      // else
        ENDIF,     // endif
        WHILE,     // while
        ENDWHILE,  // endwhile
        FOR,       // for
        IN,        // in
        ENDFOR,    // endfor
        BREAK,     // break
        CONTINUE,  // continue

        END,      // End of file
        TAB,      // 0x09
        NEWLINE,  // 0x0a
        SPACE,    // 0x20

        OPTION,      // [a-z]+
        IDENTIFIER,  // [a-zA-Z_][a-zA-Z0-9_]+
        STRING,      // "..." or '...'
        INTEGER,     // 42, 0xff, 0b1101, etc.
        FLOAT,       // 3.14, 6.022e+23, -1.4E-5, etc.
    };

    Token();
    Token(Type type, const std::string& str);

    ~Token();

    bool ambiguous() const;
    bool delimiting_wp() const;
    bool structural_wp() const;

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
