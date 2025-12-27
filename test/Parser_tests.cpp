#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "Lexer.h"
#include "Node.h"
#include "NodeVisitor.h"
#include "Parser.h"

class ParserTest : public testing::Test
{
protected:
    Lexer* m_pLexer = nullptr;
    Parser* m_pParser = nullptr;
    NodeVisitor* m_pVisitor = nullptr;

    void TearDown() override
    {
        delete m_pLexer;
        m_pLexer = nullptr;

        delete m_pParser;
        m_pParser = nullptr;

        delete m_pVisitor;
        m_pVisitor = nullptr;
    }

    void parse_str(const std::string& asText)
    {
        Context lcContext;
        lcContext.add_text(asText);

        m_pLexer = new Lexer(lcContext);
        m_pLexer->tokenize();

        m_pParser = new Parser(lcContext, m_pLexer->tokens());
        m_pParser->parse();
    }

    void expect_nodes(size_t anCount)
    {
        const size_t expected = anCount;
        const size_t actual = m_pVisitor->accumulate(m_pParser->root()).size();
        EXPECT_EQ(actual, expected) << "Text: " << m_pLexer->source().text();
    }
};

TEST_F(ParserTest, Test01)
{
    parse_str("echo 1 + 2 - 3 . 4 .. 5\n");
    expect_nodes(46);
}

//
// function_stmt
//

TEST_F(ParserTest, function_stmt_01)
{
    parse_str("function! foo() range abort dict closure\nendfunction\n");
    expect_nodes(18);
}

TEST_F(ParserTest, function_stmt_02)
{
    parse_str("function! foo(...)\nendfunction\n");
    expect_nodes(16);
}

TEST_F(ParserTest, function_stmt_03)
{
    parse_str("function! foo(...,)\nendfunction\n");
    expect_nodes(17);
}

TEST_F(ParserTest, function_stmt_04)
{
    parse_str("function! foo(a)\nendfunction\n");
    expect_nodes(16);
}

TEST_F(ParserTest, function_stmt_05)
{
    parse_str("function! foo(a,)\nendfunction\n");
    expect_nodes(17);
}

TEST_F(ParserTest, function_stmt_06)
{
    parse_str("function! foo(a, ...)\nendfunction\n");
    expect_nodes(18);
}

TEST_F(ParserTest, function_stmt_07)
{
    parse_str("function! foo(a, b = 1)\nendfunction\n");
    expect_nodes(31);
}
