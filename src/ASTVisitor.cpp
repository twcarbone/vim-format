#include <iostream>

#include "ASTVisitor.h"
#include "ast.h"

ASTVisitor::ASTVisitor() :
    m_nDepth { 0 }
{
}

void ASTVisitor::visit(const ast::AssignStmt* apAssignStmt)
{
    handle_node(apAssignStmt);
}

void ASTVisitor::visit(const ast::BinaryOp* apBinaryOp)
{
    handle_node(apBinaryOp);
}

void ASTVisitor::visit(const ast::ExprCmd* apExprCmd)
{
    handle_node(apExprCmd);
}

void ASTVisitor::visit(const ast::ForStmt* apForStmt)
{
    handle_node(apForStmt);
}

void ASTVisitor::visit(const ast::FuncArg* apFuncArg)
{
    handle_node(apFuncArg);
}

void ASTVisitor::visit(const ast::FuncStmt* apFuncStmt)
{
    handle_node(apFuncStmt);
}

void ASTVisitor::visit(const ast::IfStmt* apIfStmt)
{
    handle_node(apIfStmt);
}

void ASTVisitor::visit(const ast::JumpStmt* apJumpStmt)
{
    handle_node(apJumpStmt);
}

void ASTVisitor::visit(const ast::ListExpr* apListExpr)
{
    handle_node(apListExpr);
}

void ASTVisitor::visit(const ast::Literal* apLiteral)
{
    handle_node(apLiteral);
}

void ASTVisitor::visit(const ast::Program* apProgram)
{
    handle_node(apProgram);
}

void ASTVisitor::visit(const ast::StmtList* apStmtList)
{
    handle_node(apStmtList);
}

void ASTVisitor::visit(const ast::SliceExpr* apSliceExpr)
{
    handle_node(apSliceExpr);
}

void ASTVisitor::visit(const ast::TernaryOp* apTernaryOp)
{
    handle_node(apTernaryOp);
}

void ASTVisitor::visit(const ast::UnaryOp* apUnaryOp)
{
    handle_node(apUnaryOp);
}

void ASTVisitor::visit(const ast::Var* apVar)
{
    handle_node(apVar);
}

void ASTVisitor::visit(const ast::WhileStmt* apWhileStmt)
{
    handle_node(apWhileStmt);
}

void ASTVisitor::handle_node(const ast::Node* apNode)
{
    print_line(apNode->toString());

    m_nDepth++;

    for (ast::Node* pNode : apNode->children())
    {
        if (pNode == nullptr)
        {
            // Example: left side of SliceExpr '[:3]'
            print_line("NULL");
        }
        else
        {
            pNode->accept(*this);
        }
    }

    m_nDepth--;
}

void ASTVisitor::print_line(std::string_view asText)
{
    for (size_t i = 0; i < m_nDepth; i++)
    {
        std::cout << "  ";
    }

    std::cout << asText << std::endl;
}
