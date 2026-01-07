#pragma once

namespace ast
{
class BinaryOp;
class ExprCmd;
class IfStmt;
class Literal;
class Node;
class Program;
class SliceExpr;
class StmtList;
class UnaryOp;
class Var;
};

class ASTVisitor
{
public:
    ASTVisitor();
    ~ASTVisitor() = default;

    virtual void visit(const ast::BinaryOp* ast);
    virtual void visit(const ast::ExprCmd* ast);
    virtual void visit(const ast::IfStmt* ast);
    virtual void visit(const ast::Literal* ast);
    virtual void visit(const ast::Program* ast);
    virtual void visit(const ast::SliceExpr* ast);
    virtual void visit(const ast::StmtList* ast);
    virtual void visit(const ast::UnaryOp* ast);
    virtual void visit(const ast::Var* ast);

private:
    int m_nDepth;

    void print(const ast::Node* ast);
};
