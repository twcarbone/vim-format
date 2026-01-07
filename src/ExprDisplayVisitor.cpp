#include "ExprDisplayVisitor.h"
#include "ast.h"

std::string ExprDisplayVisitor::display(ast::Expr* apExpr)
{
    m_sDisplay.clear();

    apExpr->accept(*this);

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

void ExprDisplayVisitor::visit(const ast::SliceExpr* apSliceExpr)
{
    m_sDisplay += " (" + apSliceExpr->op()->str();

    if (apSliceExpr->left() == nullptr)
    {
        m_sDisplay += " <begin>";
    }
    else
    {
        apSliceExpr->left()->accept(*this);
    }

    if (apSliceExpr->right() == nullptr)
    {
        m_sDisplay += " <end>";
    }
    else
    {
        apSliceExpr->right()->accept(*this);
    }

    m_sDisplay += ")";
}

void ExprDisplayVisitor::visit(const ast::TernaryOp* apTernaryOp)
{
    m_sDisplay += " (" + apTernaryOp->lop()->str() + apTernaryOp->rop()->str();
    apTernaryOp->lexpr()->accept(*this);
    apTernaryOp->mexpr()->accept(*this);
    apTernaryOp->rexpr()->accept(*this);
    m_sDisplay += ")";
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
