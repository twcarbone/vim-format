#ifndef NODEVISITOR_H
#define NODEVISITOR_H

#include <vector>

#include "Node.h"

class NodeVisitor
{
public:
    enum class Order
    {
        PRE,
        POST
    };

    /**
     *  @brief
     *      Accumulate Nodes from a depth-first 'order' traversal.
     */
    std::vector<const Node*> accumulate(const Node* node, Order order);
};

#endif  // NODEVISITOR_H
