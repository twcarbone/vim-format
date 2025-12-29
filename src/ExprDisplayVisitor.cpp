#include "ExprDisplayVisitor.h"
#include "ast.h"

std::string ExprDisplayVisitor::display(ast::Node* apRoot)
{
    m_sDisplay.clear();

    apRoot->accept(*this);

    // Remove leading whitespace
    return m_sDisplay.substr(1);
}

void ExprDisplayVisitor::visit(const ast::BinaryOp* apBinaryOp)
{
    m_sDisplay += " (" + apBinaryOp->op()->str();
    apBinaryOp->left()->accept(*this);
    apBinaryOp->right()->accept(*this);
    m_sDisplay += ")";
}

void ExprDisplayVisitor::visit(const ast::Literal* apLiteral)
{
    m_sDisplay += " " + apLiteral->token()->str();
}

void ExprDisplayVisitor::visit(const ast::UnaryOp* apUnaryOp)
{
    m_sDisplay += " (" + apUnaryOp->op()->str();
    apUnaryOp->right()->accept(*this);
    m_sDisplay += ")";
}

void ExprDisplayVisitor::visit(const ast::Var* apVar)
{
    m_sDisplay += " " + apVar->token()->str();
}
