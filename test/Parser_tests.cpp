#include <gtest/gtest.h>

#include <vector>

#include "Node.h"
#include "Parser.h"
#include "NodeVisitor.h"

TEST(ParserTest, Simple)
{
    Parser lcParser;
    lcParser.parse("2+3*4\n");
    Node* pRoot = lcParser.root();

    NodeVisitor lcNodeVisitor;

    std::vector<const Node*> llNodes = lcNodeVisitor.accumulate(pRoot, NodeVisitor::Order::PRE);

    ASSERT_EQ(18, llNodes.size());
}
