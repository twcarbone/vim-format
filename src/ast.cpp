#include "ast.h"

using namespace ast;

//
// BinaryOp
//

BinaryOp::BinaryOp(Token* apOp, Node* apLeft, Node* apRight) :
    m_pOp { apOp },
    m_pLeft { apLeft },
    m_pRight { apRight }
{
}

BinaryOp::~BinaryOp()
{
    delete m_pLeft;
    m_pLeft = nullptr;

    delete m_pRight;
    m_pRight = nullptr;
}

const Token* BinaryOp::op() const
{
    return m_pOp;
}

const Node* BinaryOp::left() const
{
    return m_pLeft;
}

const Node* BinaryOp::right() const
{
    return m_pRight;
}

std::string BinaryOp::toString() const
{
    return "BinaryOp:" + m_pOp->str();
}

void BinaryOp::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// Literal
//

Literal::Literal(Token* apToken) :
    m_pToken { apToken }
{
}

Literal::~Literal()
{
    delete m_pToken;
    m_pToken = nullptr;
}

const Token* Literal::token() const
{
    return m_pToken;
}

std::string Literal::toString() const
{
    return "Literal:" + m_pToken->str();
}

void Literal::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// UnaryOp
//

UnaryOp::UnaryOp(Token* apOp, Node* apRight) :
    m_pOp { apOp },
    m_pRight { apRight }
{
}

const Token* UnaryOp::op() const
{
    return m_pOp;
}

UnaryOp::~UnaryOp()
{
    delete m_pRight;
    m_pRight = nullptr;
}

std::string UnaryOp::toString() const
{
    return "UnaryOp:" + m_pOp->str();
}

void UnaryOp::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// Var
//

Var::Var(Token* apToken) :
    m_pToken { apToken }
{
}

Var::~Var()
{
    delete m_pToken;
    m_pToken = nullptr;
}

const Token* Var::token() const
{
    return m_pToken;
}

std::string Var::toString() const
{
    return "Var:" + m_pToken->str();
}

void Var::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

