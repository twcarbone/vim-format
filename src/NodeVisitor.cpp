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

const Node* NodeVisitor::get_node(const Node* apRoot, const std::vector<int>& alPath)
{
    const Node* pCurrent = apRoot;

    for (int i : alPath)
    {
        pCurrent = pCurrent->children().at(i);
    }

    return pCurrent;
}

void NodeVisitor::print(const Node* apNode)
{
    for (size_t i = 0; i < apNode->level(); i++)
    {
        std::cout << "|  ";
    }

    std::cout << apNode->toString() << std::endl;
}

void NodeVisitor::handle_node(const Node* apNode)
{
    std::cout << "Node ";
    print(apNode);

    for (Node* pNode : apNode->children())
    {
        pNode->accept(*this);
    }
}

void NodeVisitor::visit(const NonTerminal* apNonTerminal)
{
    handle_node(apNonTerminal);
}

void NodeVisitor::visit(const Terminal* apNonTerminal)
{
    handle_node(apNonTerminal);
}
