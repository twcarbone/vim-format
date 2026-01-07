#include "ast.h"

using namespace ast;

//
// Node
//

Node::~Node()
{
    for (ast::Node* pNode : m_lChildren)
    {
        delete pNode;
    }

    m_lChildren.clear();
}

const std::vector<Node*>& Node::children() const
{
    return m_lChildren;
}

//
// StmtList
//

StmtList::~StmtList()
{
}

void StmtList::push(Stmt* apStmt)
{
    m_lChildren.push_back(apStmt);
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
// ExprCmd
//

ExprCmd::ExprCmd(Token* apCmd, Expr* apExpr) :
    m_pCmd { apCmd }
{
    m_lChildren.push_back(apExpr);
}

ExprCmd::~ExprCmd()
{
}

std::string ExprCmd::toString() const
{
    return "ExprCmd:" + m_pCmd->str();
}

void ExprCmd::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// Program
//

Program::Program(ast::StmtList* apStmtList)
{
    m_lChildren.push_back(apStmtList);
}

Program::~Program()
{
}

std::string Program::toString() const
{
    return "Program";
}

void Program::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// IfStmt
//

IfStmt::IfStmt(ast::Expr* apCondition, ast::StmtList* apThenStmtList, ast::StmtList* apElseStmtList)
{
    m_lChildren.push_back(apCondition);
    m_lChildren.push_back(apThenStmtList);
    m_lChildren.push_back(apElseStmtList);
}

IfStmt::~IfStmt()
{
}

std::string IfStmt::toString() const
{
    return "IfStmt";
}

void IfStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// BinaryOp
//

BinaryOp::BinaryOp(Token* apOp, Expr* apLeft, Expr* apRight) :
    m_pOp { apOp }
{
    m_lChildren.push_back(apLeft);
    m_lChildren.push_back(apRight);
}

BinaryOp::~BinaryOp()
{
}

const Token* BinaryOp::op() const
{
    return m_pOp;
}

const Expr* BinaryOp::left() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* BinaryOp::right() const
{
    return static_cast<Expr*>(m_lChildren[1]);
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
    m_pOp { apOp }
{
    m_lChildren.push_back(apLeft);
    m_lChildren.push_back(apRight);
}

SliceExpr::~SliceExpr()
{
}

const Token* SliceExpr::op() const
{
    return m_pOp;
}

const Expr* SliceExpr::left() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* SliceExpr::right() const
{
    return static_cast<Expr*>(m_lChildren[1]);
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
    m_pOp { apOp }
{
    m_lChildren.push_back(apRight);
}

UnaryOp::~UnaryOp()
{
}

const Token* UnaryOp::op() const
{
    return m_pOp;
}

const Expr* UnaryOp::right() const
{
    return static_cast<Expr*>(m_lChildren[0]);
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
