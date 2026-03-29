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
class CommentStmt;
class DictEntry;
class DictExpr;
class EmptyStmt;
class ExprCmd;
class FnArgList;
class FnParam;
class FnParamList;
class FnStmt;
class ForStmt;
class GroupExpr;
class IfBranch;
class IfStmt;
class IndexExpr;
class JumpStmt;
class ListExpr;
class Literal;
class MethodCallExpr;
class Node;
class Program;
class ScopeExpr;
class SliceExpr;
class StmtList;
class LiteralStr;
class StrConst;
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
    virtual void visit(const ast::CommentStmt* ast);
    virtual void visit(const ast::DictEntry* ast);
    virtual void visit(const ast::DictExpr* ast);
    virtual void visit(const ast::EmptyStmt* ast);
    virtual void visit(const ast::ExprCmd* ast);
    virtual void visit(const ast::FnArgList* ast);
    virtual void visit(const ast::FnParam* ast);
    virtual void visit(const ast::FnParamList* ast);
    virtual void visit(const ast::FnStmt* ast);
    virtual void visit(const ast::ForStmt* ast);
    virtual void visit(const ast::GroupExpr* ast);
    virtual void visit(const ast::IfBranch* ast);
    virtual void visit(const ast::IfStmt* ast);
    virtual void visit(const ast::IndexExpr* ast);
    virtual void visit(const ast::ScopeExpr* ast);
    virtual void visit(const ast::SliceExpr* ast);
    virtual void visit(const ast::JumpStmt* ast);
    virtual void visit(const ast::ListExpr* ast);
    virtual void visit(const ast::Literal* ast);
    virtual void visit(const ast::MethodCallExpr* ast);
    virtual void visit(const ast::Program* ast);
    virtual void visit(const ast::StmtList* ast);
    virtual void visit(const ast::LiteralStr* ast);
    virtual void visit(const ast::StrConst* ast);
    virtual void visit(const ast::TernaryOp* ast);
    virtual void visit(const ast::UnaryOp* ast);
    virtual void visit(const ast::Var* ast);
    virtual void visit(const ast::WhileStmt* ast);

    const std::vector<VisitedNode>& nodes() const;

    const VisitedNode& node(size_t idx) const;

protected:
    std::ostream& m_cOutStream;

private:
    int m_nDepth;
    std::vector<VisitedNode> m_lVisitedNodes;

    void handle_node(const ast::Node* ast);
    void write_line(std::string_view text);
};
