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

TEST_F(SingleTokenTest, single_blob)
{
    do_test("0z", Token::Type::BLOB);
    do_test("0Z", Token::Type::BLOB);
    do_test("0Zaabb", Token::Type::BLOB);
    do_test("0Z1234", Token::Type::BLOB);
}

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

TEST_F(SingleTokenTest, single_whitespace)
{
    do_test("\n", Token::Type::NEWLINE);
}
