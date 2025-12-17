#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <string>

// clang-format off
#define TOKEN_TYPE_LIST \
        X(NONE) \
        X(CMD_LET) \
        X(CMD_ECHO) \
        X(CMD_SET) \
        X(GEN_PLUS) \
        X(GEN_MINUS) \
        X(GEN_QUESTION) \
        X(GEN_NAME) \
        X(OP_ADD) \
        X(OP_SUB) \
        X(OP_UNARY_PLUS) \
        X(OP_UNARY_MINUS) \
        X(OP_MUL) \
        X(OP_DIV) \
        X(OP_MODULO) \
        X(OP_EQUAL) \
        X(OP_NEQUAL) \
        X(OP_GT) \
        X(OP_GTE) \
        X(OP_LT) \
        X(OP_LTE) \
        X(OP_MATCH) \
        X(OP_NMATCH) \
        X(OP_IS) \
        X(OP_ISNOT) \
        X(OP_OR) \
        X(OP_AND) \
        X(OP_MATCH_CASE) \
        X(OP_IGNORE_CASE) \
        X(OP_TERNARY_IF) \
        X(OP_TERNARY_ELSE) \
        X(OP_FALSEY) \
        X(OP_LSHIFT) \
        X(OP_RSHIFT) \
        X(OP_CAT_OLD) \
        X(OP_CAT_NEW) \
        X(OP_LOGICAL_NOT) \
        X(OP_OPTION) \
        X(ASSIGN_ADD) \
        X(ASSIGN_MINUS) \
        X(ASSIGN_MUL) \
        X(ASSIGN_DIV) \
        X(ASSIGN_EQ) \
        X(ASSIGN_MODULO) \
        X(ASSIGN_CAT_OLD) \
        X(ASSIGN_CAT_NEW) \
        X(L_PAREN) \
        X(R_PAREN) \
        X(L_BRACKET) \
        X(R_BRACKET) \
        X(COMMA) \
        X(IF) \
        X(ELSEIF) \
        X(ELSE) \
        X(ENDIF) \
        X(WHILE) \
        X(ENDWHILE) \
        X(FOR) \
        X(IN) \
        X(ENDFOR) \
        X(BREAK) \
        X(CONTINUE) \
        X(END) \
        X(TAB) \
        X(NEWLINE) \
        X(SPACE) \
        X(OPTION) \
        X(IDENTIFIER) \
        X(STRING) \
        X(INTEGER) \
        X(FLOAT)

// clang-format on

class Token
{
public:
    enum class Type
    {
#define X(name) name,
        TOKEN_TYPE_LIST
#undef X
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
    friend std::ostream& operator<<(std::ostream& os, const Token& obj);

private:
    Type m_eType;
    std::string m_sStr;
};

#endif  // TOKEN_H
