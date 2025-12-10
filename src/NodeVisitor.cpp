#include <iostream>

#include "Node.h"
#include "NodeVisitor.h"

NodeVisitor::NodeVisitor() :
    m_nDepth { 0 }
{
}

void NodeVisitor::map(const Node* apNode)
{
    std::cout << std::string(2 * m_nDepth, ' ');
    std::cout << apNode->toString() << std::endl;

    m_nDepth++;

    for (const Node* pChildNode : apNode->children())
    {
        map(pChildNode);
    }

    m_nDepth--;
}
