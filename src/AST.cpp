#include "AST.h"

//
// AST
//

AST::AST(AST* apParent) :
    Node(apParent),
    m_pToken { nullptr }
{
}

void AST::set_token(Token* apToken)
{
    m_pToken = apToken;
}

//
// CmdExpr
//

CmdExpr::CmdExpr(AST* apParent) :
    AST(apParent)
{
}

std::string CmdExpr::toString() const
{
    return "CmdExp:" + m_pToken->str();
}

void CmdExpr::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}

//
// BinOp
//

BinOp::BinOp(AST* apParent) :
    AST(apParent)
{
}

std::string BinOp::toString() const
{
    return "BinOp:" + m_pToken->str();
}

void BinOp::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}

//
// Literal
//

Literal::Literal(AST* apParent) :
    AST(apParent)
{
}

std::string Literal::toString() const
{
    return "Literal:" + m_pToken->str();
}

void Literal::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}
