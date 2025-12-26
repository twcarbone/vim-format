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
// Program
//

std::string Program::toString() const
{
    return "Program(" + std::to_string(m_lChildren.size()) + ")";
}

void Program::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}

//
// StmtList
//

std::string StmtList::toString() const
{
    return "StmtList(" + std::to_string(m_lChildren.size()) + ")";
}

void StmtList::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}

//
// CmdExpr
//

std::string CmdExpr::toString() const
{
    return "CmdExpr(" + std::to_string(m_lChildren.size()) + "):" + m_pToken->str();
}

void CmdExpr::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}

//
// BinOp
//

std::string BinOp::toString() const
{
    return "BinOp(" + std::to_string(m_lChildren.size()) + "):" + m_pToken->str();
}

void BinOp::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}

//
// UnaryOp
//

std::string UnaryOp::toString() const
{
    return "UnaryOp(" + std::to_string(m_lChildren.size()) + "):" + m_pToken->str();
}

void UnaryOp::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}

//
// Literal
//

std::string Literal::toString() const
{
    return "Literal(" + std::to_string(m_lChildren.size()) + "):" + m_pToken->str();
}

void Literal::accept(NodeVisitor& acNodeVisitor) const
{
    acNodeVisitor.visit(this);
}
