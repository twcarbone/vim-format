#ifndef NODEVISITOR_H
#define NODEVISITOR_H

#include <vector>

class Node;
class RuleNode;
class TokenNode;

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

    virtual void visit(const RuleNode* rule_node);
    virtual void visit(const TokenNode* rule_node);

protected:
    virtual void print(const Node* node);
};

#endif  // NODEVISITOR_H
