#include <gtest/gtest.h>

#include <vector>

#include "Node.h"
#include "NodeVisitor.h"
#include "Parser.h"

class ParserTest : public testing::Test
{
protected:
    Parser m_cParser;
    NodeVisitor m_cNodeVisitor;
    std::vector<const Node*> m_lNodes;

    void parse_str(const std::string& asText)
    {
        m_cParser.parse(asText);
        m_lNodes = m_cNodeVisitor.accumulate(m_cParser.root(), NodeVisitor::Order::PRE);
    }

    void expect_nodes(size_t anCount)
    {
        const size_t expected = anCount;
        const size_t actual = m_lNodes.size();
        EXPECT_EQ(actual, expected) << "Text: " << m_cParser.text();
    }
};

TEST_F(ParserTest, Test01)
{
    parse_str("echo 1 + 2 - 3 . 4 .. 5\n");
    expect_nodes(46);
}
