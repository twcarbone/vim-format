#pragma once

#include <iostream>
#include <string_view>
#include <vector>

namespace ast
{
class AssignStmt;
class BinaryOp;
class CallExpr;
class CasedBinaryOp;
class ExprCmd;
class FnArgList;
class FnParam;
class FnParamList;
class FnStmt;
class ForStmt;
class IfStmt;
class JumpStmt;
class ListExpr;
class Literal;
class MethodCallExpr;
class Node;
class Program;
class SliceExpr;
class StmtList;
class TernaryOp;
class UnaryOp;
class Var;
class WhileStmt;
};

struct VisitedNode
{
    int nLevel = -1;
    const ast::Node* pNode = nullptr;
};

class ASTVisitor
{
public:
    ASTVisitor(std::ostream& os = std::cout);
    ~ASTVisitor() = default;

    virtual void visit(const ast::AssignStmt* ast);
    virtual void visit(const ast::BinaryOp* ast);
    virtual void visit(const ast::CallExpr* ast);
    virtual void visit(const ast::CasedBinaryOp* ast);
    virtual void visit(const ast::ExprCmd* ast);
    virtual void visit(const ast::FnArgList* ast);
    virtual void visit(const ast::FnParam* ast);
    virtual void visit(const ast::FnParamList* ast);
    virtual void visit(const ast::FnStmt* ast);
    virtual void visit(const ast::ForStmt* ast);
    virtual void visit(const ast::IfStmt* ast);
    virtual void visit(const ast::JumpStmt* ast);
    virtual void visit(const ast::ListExpr* ast);
    virtual void visit(const ast::Literal* ast);
    virtual void visit(const ast::MethodCallExpr* ast);
    virtual void visit(const ast::Program* ast);
    virtual void visit(const ast::SliceExpr* ast);
    virtual void visit(const ast::StmtList* ast);
    virtual void visit(const ast::TernaryOp* ast);
    virtual void visit(const ast::UnaryOp* ast);
    virtual void visit(const ast::Var* ast);
    virtual void visit(const ast::WhileStmt* ast);

    const std::vector<VisitedNode>& nodes() const;

    const VisitedNode& node(size_t idx) const;

private:
    int m_nDepth;
    std::ostream& m_cOutStream;
    std::vector<VisitedNode> m_lVisitedNodes;

    void handle_node(const ast::Node* ast);
    void write_line(std::string_view text);
};
