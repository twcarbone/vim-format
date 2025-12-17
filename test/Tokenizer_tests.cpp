#include <gtest/gtest.h>

#include <fstream>
#include <string>

#include "Token.h"
#include "Tokenizer.h"

class TokenizerTest : public testing::Test
{
protected:
    Tokenizer m_cTokenizer;

    void tokenize_file(const std::string& asPath)
    {
        std::string lsText;
        std::ifstream lcIfStream(asPath);

        char c;
        while (lcIfStream.get(c))
        {
            lsText += c;
        }

        tokenize_str(lsText);
    }

    void tokenize_str(const std::string& asText)
    {
        m_cTokenizer.tokenize(asText);
    }

    void expect_tokens(size_t anCount)
    {
        const size_t expected = anCount;
        const size_t actual = m_cTokenizer.tokens()->size();
        EXPECT_EQ(actual, expected) << "Text: " << m_cTokenizer.text();
    }

    void expect_token(size_t anIdx, const std::string& asTokenStr, const Token::Type& aeTokenType)
    {
        const Token expected(aeTokenType, asTokenStr);
        const Token& actual = m_cTokenizer.token(anIdx);
        EXPECT_EQ(actual, expected) << "At " << std::to_string(anIdx) << " from: " << m_cTokenizer.text();
    }
};

class SingleTokenTest : public TokenizerTest
{
protected:
    void do_test(const std::string& asTokenStr, const Token::Type& aeTokenType)
    {
        tokenize_str(asTokenStr);

        expect_tokens(2);
        expect_token(0, asTokenStr, aeTokenType);
        expect_token(1, "EOF", Token::Type::END);
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

TEST_F(TokenizerTest, bad_float)
{
    tokenize_str("2.2.2");
    expect_tokens(4);
    expect_token(0, "2", Token::Type::INTEGER);
    expect_token(1, ".", Token::Type::OP_CAT_OLD);
    expect_token(2, "2.2", Token::Type::FLOAT);

    tokenize_str("3.3 . 3");
    expect_tokens(4);
    expect_token(0, "3.3", Token::Type::FLOAT);
    expect_token(1, ".", Token::Type::OP_CAT_OLD);
    expect_token(2, "3", Token::Type::INTEGER);

    tokenize_str("4 . 4.4");
    expect_tokens(4);
    expect_token(0, "4", Token::Type::INTEGER);
    expect_token(1, ".", Token::Type::OP_CAT_OLD);
    expect_token(2, "4.4", Token::Type::FLOAT);

    tokenize_str("5.5e");
    expect_tokens(5);
    expect_token(0, "5", Token::Type::INTEGER);
    expect_token(1, ".", Token::Type::OP_CAT_OLD);
    expect_token(2, "5", Token::Type::INTEGER);
    expect_token(3, "e", Token::Type::IDENTIFIER);

    tokenize_str("-6.6");
    expect_tokens(3);
    expect_token(0, "-", Token::Type::OP_UNARY_MINUS);
    expect_token(1, "6.6", Token::Type::FLOAT);
}

//
// bad_integer
//

TEST_F(TokenizerTest, bad_integer)
{
    tokenize_str("0x");
    expect_tokens(3);
    expect_token(0, "0", Token::Type::INTEGER);
    expect_token(1, "x", Token::Type::IDENTIFIER);

    tokenize_str("0xag");
    expect_tokens(3);
    expect_token(0, "0xa", Token::Type::INTEGER);
    expect_token(1, "g", Token::Type::IDENTIFIER);

    tokenize_str("0b12");
    expect_tokens(3);
    expect_token(0, "0", Token::Type::INTEGER);
    expect_token(1, "b12", Token::Type::IDENTIFIER);
}

//
// select_stmt
//

TEST_F(TokenizerTest, select_stmt_01)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "endif");

    expect_tokens(8);
    expect_token(0, "if", Token::Type::IF);
    expect_token(1, "1", Token::Type::INTEGER);
    expect_token(3, "echo", Token::Type::CMD_ECHO);
    expect_token(4, "1", Token::Type::INTEGER);
    expect_token(6, "endif", Token::Type::ENDIF);
}

TEST_F(TokenizerTest, select_stmt_02)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "else\n"
                 "  echo 2\n"
                 "endif");

    expect_tokens(13);
    expect_token(0, "if", Token::Type::IF);
    expect_token(1, "1", Token::Type::INTEGER);
    expect_token(3, "echo", Token::Type::CMD_ECHO);
    expect_token(4, "1", Token::Type::INTEGER);
    expect_token(6, "else", Token::Type::ELSE);
    expect_token(8, "echo", Token::Type::CMD_ECHO);
    expect_token(9, "2", Token::Type::INTEGER);
    expect_token(11, "endif", Token::Type::ENDIF);
}

TEST_F(TokenizerTest, select_stmt_03)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "elseif 1\n"
                 "  echo 2\n"
                 "endif");

    expect_tokens(14);
    expect_token(0, "if", Token::Type::IF);
    expect_token(1, "1", Token::Type::INTEGER);
    expect_token(3, "echo", Token::Type::CMD_ECHO);
    expect_token(4, "1", Token::Type::INTEGER);
    expect_token(6, "elseif", Token::Type::ELSEIF);
    expect_token(7, "1", Token::Type::INTEGER);
    expect_token(9, "echo", Token::Type::CMD_ECHO);
    expect_token(10, "2", Token::Type::INTEGER);
    expect_token(12, "endif", Token::Type::ENDIF);
}

TEST_F(TokenizerTest, select_stmt_04)
{
    tokenize_str("if 1\n"
                 "  echo 1\n"
                 "elseif 1\n"
                 "  echo 2\n"
                 "else\n"
                 "  echo 3\n"
                 "endif");

    expect_tokens(19);
    expect_token(0, "if", Token::Type::IF);
    expect_token(1, "1", Token::Type::INTEGER);
    expect_token(3, "echo", Token::Type::CMD_ECHO);
    expect_token(4, "1", Token::Type::INTEGER);
    expect_token(6, "elseif", Token::Type::ELSEIF);
    expect_token(7, "1", Token::Type::INTEGER);
    expect_token(9, "echo", Token::Type::CMD_ECHO);
    expect_token(10, "2", Token::Type::INTEGER);
    expect_token(12, "else", Token::Type::ELSE);
    expect_token(14, "echo", Token::Type::CMD_ECHO);
    expect_token(15, "3", Token::Type::INTEGER);
    expect_token(17, "endif", Token::Type::ENDIF);
}

//
// iteration_stmt
//

TEST_F(TokenizerTest, iteration_stmt_01)
{
    tokenize_str("while 1\n"
                 "  echo 1\n"
                 "  break\n"
                 "endwhile");

    expect_tokens(10);
    expect_token(0, "while", Token::Type::WHILE);
    expect_token(1, "1", Token::Type::INTEGER);
    expect_token(3, "echo", Token::Type::CMD_ECHO);
    expect_token(4, "1", Token::Type::INTEGER);
    expect_token(6, "break", Token::Type::BREAK);
    expect_token(8, "endwhile", Token::Type::ENDWHILE);
}

TEST_F(TokenizerTest, iteration_stmt_02)
{
    tokenize_str("for item in items\n"
                 "  continue\n"
                 "endfor");

    expect_tokens(9);
    expect_token(0, "for", Token::Type::FOR);
    expect_token(1, "item", Token::Type::IDENTIFIER);
    expect_token(2, "in", Token::Type::IN);
    expect_token(3, "items", Token::Type::IDENTIFIER);
    expect_token(5, "continue", Token::Type::CONTINUE);
    expect_token(7, "endfor", Token::Type::ENDFOR);
}

//
// expr1
//

TEST_F(TokenizerTest, expr1_01)
{
    tokenize_str("1 ?? 2 ? 3 : 4");

    expect_tokens(8);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "??", Token::Type::OP_FALSEY);
    expect_token(2, "2", Token::Type::INTEGER);
    expect_token(3, "?", Token::Type::OP_TERNARY_IF);
    expect_token(4, "3", Token::Type::INTEGER);
    expect_token(5, ":", Token::Type::OP_TERNARY_ELSE);
    expect_token(6, "4", Token::Type::INTEGER);
}

//
// expr2
//

TEST_F(TokenizerTest, expr2_01)
{
    tokenize_str("1 || 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "||", Token::Type::OP_OR);
    expect_token(2, "2", Token::Type::INTEGER);
}

//
// expr3
//

TEST_F(TokenizerTest, expr3_01)
{
    tokenize_str("1 && 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "&&", Token::Type::OP_AND);
    expect_token(2, "2", Token::Type::INTEGER);
}

//
// expr4
//

TEST_F(TokenizerTest, expr4_01)
{
    tokenize_str("1 == 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "==", Token::Type::OP_EQUAL);
    expect_token(2, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_02)
{
    tokenize_str("1 != 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "!=", Token::Type::OP_NEQUAL);
    expect_token(2, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_03)
{
    tokenize_str("1 > 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, ">", Token::Type::OP_GT);
    expect_token(2, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_04)
{
    tokenize_str("1 >= 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, ">=", Token::Type::OP_GTE);
    expect_token(2, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_05)
{
    tokenize_str("1 < 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "<", Token::Type::OP_LT);
    expect_token(2, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_06)
{
    tokenize_str("1 <= 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "<=", Token::Type::OP_LTE);
    expect_token(2, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_07)
{
    tokenize_str("1 =~# 2");

    expect_tokens(5);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "=~", Token::Type::OP_MATCH);
    expect_token(2, "#", Token::Type::OP_MATCH_CASE);
    expect_token(3, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_08)
{
    tokenize_str("1 !~? 2");

    expect_tokens(5);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "!~", Token::Type::OP_NMATCH);
    expect_token(2, "?", Token::Type::OP_IGNORE_CASE);
    expect_token(3, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_09)
{
    tokenize_str("1 is 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "is", Token::Type::OP_IS);
    expect_token(2, "2", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr4_10)
{
    tokenize_str("1 isnot 2");

    expect_tokens(4);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "isnot", Token::Type::OP_ISNOT);
    expect_token(2, "2", Token::Type::INTEGER);
}

//
// expr5
//

TEST_F(TokenizerTest, expr5_01)
{
    tokenize_str("1 << 2 >> 3");

    expect_tokens(6);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "<<", Token::Type::OP_LSHIFT);
    expect_token(2, "2", Token::Type::INTEGER);
    expect_token(3, ">>", Token::Type::OP_RSHIFT);
    expect_token(4, "3", Token::Type::INTEGER);
}

//
// expr6
//

TEST_F(TokenizerTest, expr6_01)
{
    tokenize_str("1 + 2 - 3 . 4 .. 5");

    expect_tokens(10);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "+", Token::Type::OP_ADD);
    expect_token(2, "2", Token::Type::INTEGER);
    expect_token(3, "-", Token::Type::OP_SUB);
    expect_token(4, "3", Token::Type::INTEGER);
    expect_token(5, ".", Token::Type::OP_CAT_OLD);
    expect_token(6, "4", Token::Type::INTEGER);
    expect_token(7, "..", Token::Type::OP_CAT_NEW);
    expect_token(8, "5", Token::Type::INTEGER);
}

//
// expr7
//

TEST_F(TokenizerTest, expr7_01)
{
    tokenize_str("1 * 2 / 3 % 4");

    expect_tokens(8);
    expect_token(0, "1", Token::Type::INTEGER);
    expect_token(1, "*", Token::Type::OP_MUL);
    expect_token(2, "2", Token::Type::INTEGER);
    expect_token(3, "/", Token::Type::OP_DIV);
    expect_token(4, "3", Token::Type::INTEGER);
    expect_token(5, "%", Token::Type::OP_MODULO);
    expect_token(6, "4", Token::Type::INTEGER);
}

//
// expr9
//

TEST_F(TokenizerTest, expr9_01)
{
    tokenize_str("!1");
    expect_tokens(3);
    expect_token(0, "!", Token::Type::OP_LOGICAL_NOT);
    expect_token(1, "1", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr9_02)
{
    tokenize_str("-1");
    expect_tokens(3);
    expect_token(0, "-", Token::Type::OP_UNARY_MINUS);
    expect_token(1, "1", Token::Type::INTEGER);
}

TEST_F(TokenizerTest, expr9_03)
{
    tokenize_str("+1");
    expect_tokens(3);
    expect_token(0, "+", Token::Type::OP_UNARY_PLUS);
    expect_token(1, "1", Token::Type::INTEGER);
}

//
// list_expr
//

TEST_F(TokenizerTest, list_expr_01)
{
    tokenize_str("[1, 2, 3]");

    expect_tokens(8);
    expect_token(0, "[", Token::Type::L_BRACKET);
    expect_token(1, "1", Token::Type::INTEGER);
    expect_token(2, ",", Token::Type::COMMA);
    expect_token(3, "2", Token::Type::INTEGER);
    expect_token(4, ",", Token::Type::COMMA);
    expect_token(5, "3", Token::Type::INTEGER);
    expect_token(6, "]", Token::Type::R_BRACKET);
}

TEST_F(TokenizerTest, list_expr_02)
{
    tokenize_str("[1,]");

    expect_tokens(5);
    expect_token(0, "[", Token::Type::L_BRACKET);
    expect_token(1, "1", Token::Type::INTEGER);
    expect_token(2, ",", Token::Type::COMMA);
    expect_token(3, "]", Token::Type::R_BRACKET);
}

TEST_F(TokenizerTest, list_expr_03)
{
    tokenize_str("[]");

    expect_tokens(3);
    expect_token(0, "[", Token::Type::L_BRACKET);
    expect_token(1, "]", Token::Type::R_BRACKET);
}

//
// file
//

TEST_F(TokenizerTest, file_01)
{
    tokenize_file("../../test/sample/01.vim");

    expect_tokens(6);
    expect_token(0, "let", Token::Type::CMD_LET);
    expect_token(1, "i", Token::Type::IDENTIFIER);
    expect_token(2, "=", Token::Type::ASSIGN_EQ);
    expect_token(3, "42", Token::Type::INTEGER);
    expect_token(4, "\n", Token::Type::NEWLINE);
    expect_token(5, "EOF", Token::Type::END);
}
