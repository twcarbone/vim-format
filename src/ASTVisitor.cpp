#include <iostream>

#include "ASTVisitor.h"
#include "ast.h"

ASTVisitor::ASTVisitor() :
    m_nDepth { 0 }
{
}

void ASTVisitor::visit(const ast::BinaryOp* apBinaryOp)
{
    print(apBinaryOp);
}

void ASTVisitor::visit(const ast::ExprCmd* apExprCmd)
{
    print(apExprCmd);
}

void ASTVisitor::visit(const ast::IfStmt* apIfStmt)
{
    print(apIfStmt);
}

void ASTVisitor::visit(const ast::Literal* apLiteral)
{
    print(apLiteral);
}

void ASTVisitor::visit(const ast::Program* apProgram)
{
    print(apProgram);
}

void ASTVisitor::visit(const ast::StmtList* apStmtList)
{
    print(apStmtList);
}

void ASTVisitor::visit(const ast::SliceExpr* apSliceExpr)
{
    print(apSliceExpr);
}

void ASTVisitor::visit(const ast::TernaryOp* apTernaryOp)
{
    print(apTernaryOp);
}

void ASTVisitor::visit(const ast::UnaryOp* apUnaryOp)
{
    print(apUnaryOp);
}

void ASTVisitor::visit(const ast::Var* apVar)
{
    print(apVar);
}

void ASTVisitor::print(const ast::Node* apNode)
{
    for (size_t i = 0; i < m_nDepth; i++)
    {
        std::cout << "  ";
    }

    std::cout << apNode->toString() << std::endl;

    m_nDepth++;

    for (ast::Node* pNode : apNode->children())
    {
        pNode->accept(*this);
    }

    m_nDepth--;
}
