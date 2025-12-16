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
