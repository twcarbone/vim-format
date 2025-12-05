#include <gtest/gtest.h>

#include "Token.h"
#include "Tokenizer.h"

class SingleTokenTest : public testing::Test
{
protected:
    void do_test(const std::string& asTokenStr, const Token::Type& aeTokenType)
    {
        Tokenizer lcTokenizer(asTokenStr);
        lcTokenizer.tokenize();

        EXPECT_EQ(lcTokenizer.tokens()->size(), 1);
        EXPECT_EQ(lcTokenizer.token(0), Token(aeTokenType, asTokenStr));
    }
};

TEST_F(SingleTokenTest, IntegerLiteral)
{
    do_test("42", Token::Type::LITERAL_INTEGER);
    do_test("-42", Token::Type::LITERAL_INTEGER);
    do_test("0x42", Token::Type::LITERAL_INTEGER);
    do_test("0X42", Token::Type::LITERAL_INTEGER);
    do_test("0o42", Token::Type::LITERAL_INTEGER);
    do_test("0O42", Token::Type::LITERAL_INTEGER);
    do_test("0b1101", Token::Type::LITERAL_INTEGER);
    do_test("0B1101", Token::Type::LITERAL_INTEGER);
}

TEST_F(SingleTokenTest, FloatLiteral)
{
    do_test("123.456", Token::Type::LITERAL_FLOAT);
    do_test("+0.0001", Token::Type::LITERAL_FLOAT);
    do_test("55.0", Token::Type::LITERAL_FLOAT);
    do_test("-0.123", Token::Type::LITERAL_FLOAT);
    do_test("1.234e03", Token::Type::LITERAL_FLOAT);
    do_test("1.0E-6", Token::Type::LITERAL_FLOAT);
    do_test("-3.1416e+88", Token::Type::LITERAL_FLOAT);
}

TEST_F(SingleTokenTest, StringLiteral)
{
    do_test("'abcd'", Token::Type::LITERAL_STRING);
    do_test("\"abcd\"", Token::Type::LITERAL_STRING);
}

TEST(SingleStatementTest, Assignment)
{
    Tokenizer lcTokenizer("let i = 42");
    lcTokenizer.tokenize();

    EXPECT_EQ(lcTokenizer.tokens()->size(), 7);
    EXPECT_EQ(lcTokenizer.token(0), Token(Token::Type::KEYWORD, "let"));
    EXPECT_EQ(lcTokenizer.token(1), Token(Token::Type::SPACE, " "));
    EXPECT_EQ(lcTokenizer.token(2), Token(Token::Type::IDENTIFIER, "i"));
    EXPECT_EQ(lcTokenizer.token(3), Token(Token::Type::SPACE, " "));
    EXPECT_EQ(lcTokenizer.token(4), Token(Token::Type::OPERATOR_ASSIGNMENT, "="));
    EXPECT_EQ(lcTokenizer.token(5), Token(Token::Type::SPACE, " "));
    EXPECT_EQ(lcTokenizer.token(6), Token(Token::Type::LITERAL_INTEGER, "42"));
}
