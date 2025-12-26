#ifndef NODEVISITOR_H
#define NODEVISITOR_H

#include <vector>

class Node;
class RuleNode;
class TokenNode;

class AST;
class BinOp;
class CmdExpr;
class Literal;
class Program;
class StmtList;

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
    std::vector<const Node*> accumulate(const Node* node, Order order = Order::PRE);

    /**
     *  @brief
     *      Get the node at the given `path` index, starting from `root`.
     */
    const Node* get_node(const Node* root, const std::vector<int>& path);

    void handle_node(const Node* node);
    virtual void visit(const RuleNode* node);
    virtual void visit(const TokenNode* node);

    void handle_ast(const AST* ast_node);
    virtual void visit(const Program* ast_node);
    virtual void visit(const StmtList* ast_node);
    virtual void visit(const CmdExpr* ast_node);
    virtual void visit(const Literal* ast_node);
    virtual void visit(const BinOp* ast_node);

protected:
    virtual void print(const Node* node);
};

#endif  // NODEVISITOR_H
