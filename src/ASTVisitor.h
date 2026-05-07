#pragma once

#include <iostream>
#include <string_view>
#include <vector>

// clang-format off
#define VISITED_AST_NODES   \
    X(AssignStmt)           \
    X(BinaryOp)             \
    X(CallExpr)             \
    X(CasedBinaryOp)        \
    X(CommentStmt)          \
    X(DictEntry)            \
    X(DictExpr)             \
    X(EmptyStmt)            \
    X(ExprCmd)              \
    X(FnArgList)            \
    X(FnParam)              \
    X(FnParamList)          \
    X(FnStmt)               \
    X(ForStmt)              \
    X(GroupExpr)            \
    X(HereDocExpr)          \
    X(IfBranch)             \
    X(IfStmt)               \
    X(IndexExpr)            \
    X(InterpStr)            \
    X(JumpStmt)             \
    X(ListAssignExpr)       \
    X(ListExpr)             \
    X(Literal)              \
    X(LiteralStr)           \
    X(LockVarStmt)          \
    X(MethodCallExpr)       \
    X(Program)              \
    X(SliceExpr)            \
    X(StmtList)             \
    X(StrConst)             \
    X(TernaryOp)            \
    X(UnaryOp)              \
    X(UnletStmt)            \
    X(Var)                  \
    X(VarQueryStmt)         \
    X(WhileStmt)

// clang-format on

namespace ast
{
#define X(name) class name;
VISITED_AST_NODES
#undef X
class Node;
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

#define X(name) virtual void visit(const ast::name* ast);
    VISITED_AST_NODES
#undef X

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
