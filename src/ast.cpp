#include "ast.h"

using namespace ast;

//
// StmtList
//

StmtList::~StmtList()
{
    for (Stmt* pStmt : m_lStmnts)
    {
        delete pStmt;
    }

    m_lStmnts.clear();
}

void StmtList::push(Stmt* apStmt)
{
    m_lStmnts.push_back(apStmt);
}

std::string StmtList::toString() const
{
    return "StmtList";
}

void StmtList::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// BinaryOp
//

BinaryOp::BinaryOp(Token* apOp, Expr* apLeft, Expr* apRight) :
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

const Expr* BinaryOp::left() const
{
    return m_pLeft;
}

const Expr* BinaryOp::right() const
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
// SliceExpr
//

SliceExpr::SliceExpr(Token* apOp, Expr* apLeft, Expr* apRight) :
    m_pOp { apOp },
    m_pLeft { apLeft },
    m_pRight { apRight }
{
}

SliceExpr::~SliceExpr()
{
    delete m_pLeft;
    m_pLeft = nullptr;

    delete m_pRight;
    m_pRight = nullptr;
}

const Token* SliceExpr::op() const
{
    return m_pOp;
}

const Expr* SliceExpr::left() const
{
    return m_pLeft;
}

const Expr* SliceExpr::right() const
{
    return m_pRight;
}

std::string SliceExpr::toString() const
{
    return "SliceExpr:" + m_pOp->str();
}

void SliceExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// UnaryOp
//

UnaryOp::UnaryOp(Token* apOp, Expr* apRight) :
    m_pOp { apOp },
    m_pRight { apRight }
{
}

const Token* UnaryOp::op() const
{
    return m_pOp;
}

const Expr* UnaryOp::right() const
{
    return m_pRight;
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
