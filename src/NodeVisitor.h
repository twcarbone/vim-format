#ifndef NODEVISITOR_H
#define NODEVISITOR_H

class Node;

class NodeVisitor
{
public:
    NodeVisitor();
    ~NodeVisitor() = default;

    /**
     *  @brief
     *      Traverse and print all descendants of 'node'.
     */
    void map(const Node* node);

private:
    int m_nDepth;
};

#endif  // NODEVISITOR_H
