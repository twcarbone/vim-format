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
// SingleTokenTest
//

TEST_F(SingleTokenTest, IntegerLiteral)
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

TEST_F(SingleTokenTest, FloatLiteral)
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

TEST_F(SingleTokenTest, WhiteSpace)
{
    do_test("\n", Token::Type::NEWLINE);
}

TEST_F(SingleTokenTest, StringLiteral)
{
    do_test("\"abcd\"", Token::Type::STRING);
}

//
// TokenizerTest
//

TEST_F(TokenizerTest, FloatTokenTest)
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

TEST_F(TokenizerTest, IntegerTokenTest)
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

TEST_F(TokenizerTest, Test01)
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
