#ifndef NODEVISITOR_H
#define NODEVISITOR_H

#include <vector>

class Node;
class NonTerminal;
class Terminal;

class AST;
class BinOp;
class UnaryOp;
class CmdExpr;
class Literal;
class Program;
class StmtList;
class Var;
class Assign;
class ListExpr;

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
    virtual void visit(const NonTerminal* node);
    virtual void visit(const Terminal* node);

    void handle_ast(const AST* ast_node);
    virtual void visit(const Program* ast_node);
    virtual void visit(const StmtList* ast_node);
    virtual void visit(const CmdExpr* ast_node);
    virtual void visit(const Literal* ast_node);
    virtual void visit(const BinOp* ast_node);
    virtual void visit(const UnaryOp* ast_node);
    virtual void visit(const Var* ast_node);
    virtual void visit(const Assign* ast_node);
    virtual void visit(const ListExpr* ast_node);

protected:
    virtual void print(const Node* node);
};

#endif  // NODEVISITOR_H
