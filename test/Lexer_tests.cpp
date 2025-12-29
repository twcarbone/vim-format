#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

#include "Context.h"
#include "Lexer.h"
#include "Token.h"

class LexerTest : public testing::Test
{
protected:
    Lexer* m_pLexer = nullptr;

    void TearDown() override
    {
        delete m_pLexer;
        m_pLexer = nullptr;
    }

    void tokenize_file(const std::string& asPath)
    {
        Context lcContext;
        lcContext.add_path(asPath);

        m_pLexer = new Lexer(lcContext);
        m_pLexer->tokenize();
    }

    void tokenize_str(const std::string& asText)
    {
        Context lcContext;
        lcContext.add_text(asText);

        m_pLexer = new Lexer(lcContext);
        m_pLexer->tokenize();
    }

    void expect_tokens(size_t anCount)
    {
        const size_t expected = anCount;
        const size_t actual = m_pLexer->tokens().size();
        EXPECT_EQ(actual, expected) << "Text: " << m_pLexer->source().text();
    }

    void expect_token(size_t anIdx,
                      const std::string& asTokenStr,
                      const Token::Type& aeTokenType,
                      int anSourcePos)
    {
        const Token expected(aeTokenType, asTokenStr, anSourcePos);
        const Token& actual = m_pLexer->token(anIdx);
        EXPECT_EQ(actual, expected) << "Text: " << m_pLexer->source().text();
    }
};

class SingleTokenTest : public LexerTest
{
protected:
    void do_test(const std::string& asTokenStr, const Token::Type& aeTokenType)
    {
        tokenize_str(asTokenStr);

        expect_tokens(2);
        expect_token(0, asTokenStr, aeTokenType, 0);
        expect_token(1, "EOF", Token::Type::END, asTokenStr.size());

        TearDown();
    }
};

//
// single_integer
//

TEST_F(SingleTokenTest, single_integer)
{
    do_test("4", Token::Type::INTEGER);
    do_test("42", Token::Type::INTEGER);
    do_test("423", Token::Type::INTEGER);
    do_test("004", Token::Type::INTEGER);
    do_test("0x42", Token::Type::INTEGER);
    do_test("0X42", Token::Type::INTEGER);
    do_test("0x42ff", Token::Type::INTEGER);
    do_test("0Xa42f", Token::Type::INTEGER);
    do_test("0o42", Token::Type::INTEGER);
    do_test("0O42", Token::Type::INTEGER);
    do_test("0b1101", Token::Type::INTEGER);
    do_test("0B1101", Token::Type::INTEGER);
}

//
// single_float
//

TEST_F(SingleTokenTest, single_float)
{
    do_test("1.0", Token::Type::FLOAT);
    do_test("55.0", Token::Type::FLOAT);
    do_test("123.456", Token::Type::FLOAT);
    do_test("1.234e03", Token::Type::FLOAT);
    do_test("1.234e-03", Token::Type::FLOAT);
    do_test("1.234e+03", Token::Type::FLOAT);
    do_test("1.234E03", Token::Type::FLOAT);
    do_test("1.234E-03", Token::Type::FLOAT);
    do_test("1.234E+03", Token::Type::FLOAT);
}

//
// single_whitespace
//

TEST_F(SingleTokenTest, single_whitespace)
{
    do_test("\n", Token::Type::NEWLINE);
}

//
// single_string
//

TEST_F(SingleTokenTest, single_string)
{
    do_test("\"abcd\"", Token::Type::STRING);
}

//
// bad_float
//

TEST_F(LexerTest, bad_float)
{
    tokenize_str("2.2.2");
    expect_tokens(4);
    expect_token(0, "2", Token::Type::INTEGER, 0);
    expect_token(1, ".", Token::Type::OP_CAT_OLD, 1);
    expect_token(2, "2.2", Token::Type::FLOAT, 2);

    tokenize_str("3.3 . 3");
    expect_tokens(4);
    expect_token(0, "3.3", Token::Type::FLOAT, 0);
    expect_token(1, ".", Token::Type::OP_CAT_OLD, 4);
    expect_token(2, "3", Token::Type::INTEGER, 6);

    tokenize_str("4 . 4.4");
    expect_tokens(4);
    expect_token(0, "4", Token::Type::INTEGER, 0);
    expect_token(1, ".", Token::Type::OP_CAT_OLD, 2);
    expect_token(2, "4.4", Token::Type::FLOAT, 4);

    tokenize_str("5.5e");
    expect_tokens(5);
    expect_token(0, "5", Token::Type::INTEGER, 0);
    expect_token(1, ".", Token::Type::OP_CAT_OLD, 1);
    expect_token(2, "5", Token::Type::INTEGER, 2);
    expect_token(3, "e", Token::Type::IDENTIFIER, 3);

    tokenize_str("-6.6");
    expect_tokens(3);
    expect_token(0, "-", Token::Type::OP_UNARY_MINUS, 0);
    expect_token(1, "6.6", Token::Type::FLOAT, 1);
}

//
// bad_integer
//

TEST_F(LexerTest, bad_integer)
{
    tokenize_str("0x");
    expect_tokens(3);
    expect_token(0, "0", Token::Type::INTEGER, 0);
    expect_token(1, "x", Token::Type::IDENTIFIER, 1);

    tokenize_str("0xag");
    expect_tokens(3);
    expect_token(0, "0xa", Token::Type::INTEGER, 0);
    expect_token(1, "g", Token::Type::IDENTIFIER, 3);

    tokenize_str("0b12");
    expect_tokens(3);
    expect_token(0, "0", Token::Type::INTEGER, 0);
    expect_token(1, "b12", Token::Type::IDENTIFIER, 1);
}

//
// select_stmt
//

TEST_F(LexerTest, select_stmt_01)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "endif");

    expect_tokens(8);
    expect_token(0, "if", Token::Type::IF, 0);
    expect_token(1, "1", Token::Type::INTEGER, 3);
    expect_token(3, "echo", Token::Type::CMD_ECHO, 7);
    expect_token(4, "1", Token::Type::INTEGER, 12);
    expect_token(6, "endif", Token::Type::ENDIF, 14);
}

TEST_F(LexerTest, select_stmt_02)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "else\n"
                 "  echo 2\n"
                 "endif");

    expect_tokens(13);
    expect_token(0, "if", Token::Type::IF, 0);
    expect_token(1, "1", Token::Type::INTEGER, 3);
    expect_token(3, "echo", Token::Type::CMD_ECHO, 7);
    expect_token(4, "1", Token::Type::INTEGER, 12);
    expect_token(6, "else", Token::Type::ELSE, 14);
    expect_token(8, "echo", Token::Type::CMD_ECHO, 21);
    expect_token(9, "2", Token::Type::INTEGER, 26);
    expect_token(11, "endif", Token::Type::ENDIF, 28);
}

TEST_F(LexerTest, select_stmt_03)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "elseif 1\n"
                 "  echo 2\n"
                 "endif");

    expect_tokens(14);
    expect_token(0, "if", Token::Type::IF, 0);
    expect_token(1, "1", Token::Type::INTEGER, 3);
    expect_token(3, "echo", Token::Type::CMD_ECHO, 7);
    expect_token(4, "1", Token::Type::INTEGER, 12);
    expect_token(6, "elseif", Token::Type::ELSEIF, 14);
    expect_token(7, "1", Token::Type::INTEGER, 21);
    expect_token(9, "echo", Token::Type::CMD_ECHO, 25);
    expect_token(10, "2", Token::Type::INTEGER, 30);
    expect_token(12, "endif", Token::Type::ENDIF, 32);
}

TEST_F(LexerTest, select_stmt_04)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "elseif 1\n"
                 "  echo 2\n"
                 "else\n"
                 "  echo 3\n"
                 "endif");

    expect_tokens(19);
    expect_token(0, "if", Token::Type::IF, 0);
    expect_token(1, "1", Token::Type::INTEGER, 3);
    expect_token(3, "echo", Token::Type::CMD_ECHO, 7);
    expect_token(4, "1", Token::Type::INTEGER, 12);
    expect_token(6, "elseif", Token::Type::ELSEIF, 14);
    expect_token(7, "1", Token::Type::INTEGER, 21);
    expect_token(9, "echo", Token::Type::CMD_ECHO, 25);
    expect_token(10, "2", Token::Type::INTEGER, 30);
    expect_token(12, "else", Token::Type::ELSE, 32);
    expect_token(14, "echo", Token::Type::CMD_ECHO, 39);
    expect_token(15, "3", Token::Type::INTEGER, 44);
    expect_token(17, "endif", Token::Type::ENDIF, 46);
}

//
// iteration_stmt
//

TEST_F(LexerTest, iteration_stmt_01)
{
    tokenize_str("while 1\n"
                 "  echo 1\n"
                 "  break\n"
                 "endwhile");

    expect_tokens(10);
    expect_token(0, "while", Token::Type::WHILE, 0);
    expect_token(1, "1", Token::Type::INTEGER, 6);
    expect_token(3, "echo", Token::Type::CMD_ECHO, 10);
    expect_token(4, "1", Token::Type::INTEGER, 15);
    expect_token(6, "break", Token::Type::BREAK, 19);
    expect_token(8, "endwhile", Token::Type::ENDWHILE, 25);
}

TEST_F(LexerTest, iteration_stmt_02)
{
    tokenize_str("for item in items\n"
                 "  continue\n"
                 "endfor");

    expect_tokens(9);
    expect_token(0, "for", Token::Type::FOR, 0);
    expect_token(1, "item", Token::Type::IDENTIFIER, 4);
    expect_token(2, "in", Token::Type::IN, 9);
    expect_token(3, "items", Token::Type::IDENTIFIER, 12);
    expect_token(5, "continue", Token::Type::CONTINUE, 20);
    expect_token(7, "endfor", Token::Type::ENDFOR, 29);
}

//
// function_stmt
//

TEST_F(LexerTest, function_stmt_01)
{
    tokenize_str("function! foo(a, b = 1, ...) range abort dict closure\n"
                 "  echo 1\n"
                 "endfunction");

    expect_tokens(22);
    expect_token(0, "function", Token::Type::FUNCTION, 0);
    expect_token(1, "!", Token::Type::OP_BANG, 8);
    expect_token(2, "foo", Token::Type::IDENTIFIER, 10);
    expect_token(3, "(", Token::Type::L_PAREN, 13);
    expect_token(4, "a", Token::Type::IDENTIFIER, 14);
    expect_token(5, ",", Token::Type::COMMA, 15);
    expect_token(6, "b", Token::Type::IDENTIFIER, 17);
    expect_token(7, "=", Token::Type::ASSIGN_EQ, 19);
    expect_token(8, "1", Token::Type::INTEGER, 21);
    expect_token(9, ",", Token::Type::COMMA, 22);
    expect_token(10, "...", Token::Type::FN_ELLIPSES, 24);
    expect_token(11, ")", Token::Type::R_PAREN, 27);
    expect_token(12, "range", Token::Type::FN_RANGE, 29);
    expect_token(13, "abort", Token::Type::FN_ABORT, 35);
    expect_token(14, "dict", Token::Type::FN_DICT, 41);
    expect_token(15, "closure", Token::Type::FN_CLOSURE, 46);
    expect_token(17, "echo", Token::Type::CMD_ECHO, 56);
    expect_token(18, "1", Token::Type::INTEGER, 61);
    expect_token(20, "endfunction", Token::Type::ENDFUNCTION, 63);
}

//
// expr1
//

TEST_F(LexerTest, expr1_01)
{
    tokenize_str("1 ?? 2 ? 3 : 4");

    expect_tokens(8);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "??", Token::Type::OP_FALSEY, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
    expect_token(3, "?", Token::Type::OP_TERNARY_IF, 7);
    expect_token(4, "3", Token::Type::INTEGER, 9);
    expect_token(5, ":", Token::Type::OP_TERNARY_ELSE, 11);
    expect_token(6, "4", Token::Type::INTEGER, 13);
}

//
// expr2
//

TEST_F(LexerTest, expr2_01)
{
    tokenize_str("1 || 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "||", Token::Type::OP_OR, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
}

//
// expr3
//

TEST_F(LexerTest, expr3_01)
{
    tokenize_str("1 && 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "&&", Token::Type::OP_AND, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
}

//
// expr4
//

TEST_F(LexerTest, expr4_01)
{
    tokenize_str("1 == 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "==", Token::Type::OP_EQUAL, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
}

TEST_F(LexerTest, expr4_02)
{
    tokenize_str("1 != 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "!=", Token::Type::OP_NEQUAL, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
}

TEST_F(LexerTest, expr4_03)
{
    tokenize_str("1 > 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, ">", Token::Type::OP_GT, 2);
    expect_token(2, "2", Token::Type::INTEGER, 4);
}

TEST_F(LexerTest, expr4_04)
{
    tokenize_str("1 >= 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, ">=", Token::Type::OP_GTE, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
}

TEST_F(LexerTest, expr4_05)
{
    tokenize_str("1 < 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "<", Token::Type::OP_LT, 2);
    expect_token(2, "2", Token::Type::INTEGER, 4);
}

TEST_F(LexerTest, expr4_06)
{
    tokenize_str("1 <= 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "<=", Token::Type::OP_LTE, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
}

TEST_F(LexerTest, expr4_07)
{
    tokenize_str("1 =~# 2");

    expect_tokens(5);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "=~", Token::Type::OP_MATCH, 2);
    expect_token(2, "#", Token::Type::OP_MATCH_CASE, 4);
    expect_token(3, "2", Token::Type::INTEGER, 6);
}

TEST_F(LexerTest, expr4_08)
{
    tokenize_str("1 !~? 2");

    expect_tokens(5);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "!~", Token::Type::OP_NMATCH, 2);
    expect_token(2, "?", Token::Type::OP_IGNORE_CASE, 4);
    expect_token(3, "2", Token::Type::INTEGER, 6);
}

TEST_F(LexerTest, expr4_09)
{
    tokenize_str("1 is 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "is", Token::Type::OP_IS, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
}

TEST_F(LexerTest, expr4_10)
{
    tokenize_str("1 isnot 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "isnot", Token::Type::OP_ISNOT, 2);
    expect_token(2, "2", Token::Type::INTEGER, 8);
}

//
// expr5
//

TEST_F(LexerTest, expr5_01)
{
    tokenize_str("1 << 2 >> 3");

    expect_tokens(6);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "<<", Token::Type::OP_LSHIFT, 2);
    expect_token(2, "2", Token::Type::INTEGER, 5);
    expect_token(3, ">>", Token::Type::OP_RSHIFT, 7);
    expect_token(4, "3", Token::Type::INTEGER, 10);
}

//
// expr6
//

TEST_F(LexerTest, expr6_01)
{
    tokenize_str("1 + 2 - 3 . 4 .. 5");

    expect_tokens(10);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "+", Token::Type::OP_ADD, 2);
    expect_token(2, "2", Token::Type::INTEGER, 4);
    expect_token(3, "-", Token::Type::OP_SUB, 6);
    expect_token(4, "3", Token::Type::INTEGER, 8);
    expect_token(5, ".", Token::Type::OP_CAT_OLD, 10);
    expect_token(6, "4", Token::Type::INTEGER, 12);
    expect_token(7, "..", Token::Type::OP_CAT_NEW, 14);
    expect_token(8, "5", Token::Type::INTEGER, 17);
}

TEST_F(LexerTest, expr6_02)
{
    tokenize_str("a + b - c . d .. e");

    expect_tokens(10);
    expect_token(0, "a", Token::Type::IDENTIFIER, 0);
    expect_token(1, "+", Token::Type::OP_ADD, 2);
    expect_token(2, "b", Token::Type::IDENTIFIER, 4);
    expect_token(3, "-", Token::Type::OP_SUB, 6);
    expect_token(4, "c", Token::Type::IDENTIFIER, 8);
    expect_token(5, ".", Token::Type::OP_CAT_OLD, 10);
    expect_token(6, "d", Token::Type::IDENTIFIER, 12);
    expect_token(7, "..", Token::Type::OP_CAT_NEW, 14);
    expect_token(8, "e", Token::Type::IDENTIFIER, 17);
}

//
// expr7
//

TEST_F(LexerTest, expr7_01)
{
    tokenize_str("1 * 2 / 3 % 4");

    expect_tokens(8);
    expect_token(0, "1", Token::Type::INTEGER, 0);
    expect_token(1, "*", Token::Type::OP_MUL, 2);
    expect_token(2, "2", Token::Type::INTEGER, 4);
    expect_token(3, "/", Token::Type::OP_DIV, 6);
    expect_token(4, "3", Token::Type::INTEGER, 8);
    expect_token(5, "%", Token::Type::OP_MODULO, 10);
    expect_token(6, "4", Token::Type::INTEGER, 12);
}

//
// expr9
//

TEST_F(LexerTest, expr9_01)
{
    tokenize_str("!1");
    expect_tokens(3);
    expect_token(0, "!", Token::Type::OP_LOGICAL_NOT, 0);
    expect_token(1, "1", Token::Type::INTEGER, 1);
}

TEST_F(LexerTest, expr9_02)
{
    tokenize_str("-1");
    expect_tokens(3);
    expect_token(0, "-", Token::Type::OP_UNARY_MINUS, 0);
    expect_token(1, "1", Token::Type::INTEGER, 1);
}

TEST_F(LexerTest, expr9_03)
{
    tokenize_str("+1");
    expect_tokens(3);
    expect_token(0, "+", Token::Type::OP_UNARY_PLUS, 0);
    expect_token(1, "1", Token::Type::INTEGER, 1);
}

//
// list_expr
//

TEST_F(LexerTest, list_expr_01)
{
    tokenize_str("[1, 2, 3]");

    expect_tokens(8);
    expect_token(0, "[", Token::Type::L_BRACKET, 0);
    expect_token(1, "1", Token::Type::INTEGER, 1);
    expect_token(2, ",", Token::Type::COMMA, 2);
    expect_token(3, "2", Token::Type::INTEGER, 4);
    expect_token(4, ",", Token::Type::COMMA, 5);
    expect_token(5, "3", Token::Type::INTEGER, 7);
    expect_token(6, "]", Token::Type::R_BRACKET, 8);
}

TEST_F(LexerTest, list_expr_02)
{
    tokenize_str("[1,]");

    expect_tokens(5);
    expect_token(0, "[", Token::Type::L_BRACKET, 0);
    expect_token(1, "1", Token::Type::INTEGER, 1);
    expect_token(2, ",", Token::Type::COMMA, 2);
    expect_token(3, "]", Token::Type::R_BRACKET, 3);
}

TEST_F(LexerTest, list_expr_03)
{
    tokenize_str("[]");

    expect_tokens(3);
    expect_token(0, "[", Token::Type::L_BRACKET, 0);
    expect_token(1, "]", Token::Type::R_BRACKET, 1);
}

//
// file
//

TEST_F(LexerTest, file_01)
{
    tokenize_file("../../test/sample/01.vim");

    expect_tokens(6);
    expect_token(0, "let", Token::Type::CMD_LET, 0);
    expect_token(1, "i", Token::Type::IDENTIFIER, 4);
    expect_token(2, "=", Token::Type::ASSIGN_EQ, 6);
    expect_token(3, "42", Token::Type::INTEGER, 8);
    expect_token(4, "\n", Token::Type::NEWLINE, 10);
    expect_token(5, "EOF", Token::Type::END, 11);
}
