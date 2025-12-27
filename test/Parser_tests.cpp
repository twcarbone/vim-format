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
    std::string m_sText;
    std::vector<const Node*> m_lNodes;

    void parse_str(const std::string& asText)
    {
        m_sText = asText;

        Context lcContext;
        lcContext.add_text(asText);

        Lexer lcLexer(lcContext);
        lcLexer.tokenize();

        Parser lcParser(lcContext, lcLexer.tokens());
        lcParser.parse();

        NodeVisitor lcNodeVisitor;
        m_lNodes = lcNodeVisitor.accumulate(lcParser.root(), NodeVisitor::Order::PRE);
    }

    void expect_nodes(size_t anCount)
    {
        const size_t expected = anCount;
        const size_t actual = m_lNodes.size();
        EXPECT_EQ(actual, expected) << "Text: " << m_sText;
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
