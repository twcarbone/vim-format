#include <iostream>
#include <vector>

#include "Node.h"
#include "NodeVisitor.h"

std::vector<const Node*> NodeVisitor::accumulate(const Node* apNode, NodeVisitor::Order aeOrder)
{
    std::vector<const Node*> llNodes;

    if (aeOrder == Order::PRE)
    {
        llNodes.push_back(apNode);
    }

    for (const Node* pChild : apNode->children())
    {
        std::vector<const Node*> llAncestors = accumulate(pChild, aeOrder);
        llNodes.insert(llNodes.end(), llAncestors.begin(), llAncestors.end());
    }

    if (aeOrder == Order::POST)
    {
        llNodes.push_back(apNode);
    }

    return llNodes;
}

void NodeVisitor::print(const Node* apNode)
{
    std::cout << std::string(2 * apNode->level(), ' ') << apNode->toString() << std::endl;
}

void NodeVisitor::visit(const RuleNode* apRuleNode)
{
    std::cout << "Rule  | ";
    print(apRuleNode);

    for (Node* pNode : apRuleNode->children())
    {
        pNode->accept(*this);
    }
}

void NodeVisitor::visit(const TokenNode* apRuleNode)
{
    std::cout << "Token | ";
    print(apRuleNode);

    for (Node* pNode : apRuleNode->children())
    {
        pNode->accept(*this);
    }
}
