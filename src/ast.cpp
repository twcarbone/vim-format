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
// Stmt
//

void Stmt::set_leading_empty_lines(size_t anCount)
{
    m_nLeadingEmptyLines = anCount;
}

size_t Stmt::leading_empty_lines() const
{
    return m_nLeadingEmptyLines;
}

std::string Stmt::toString() const
{
    return str_a() + " <" + str_b() + ">";
}

std::string Stmt::str_b() const
{
    return "lvwp = " + std::to_string(m_nLeadingEmptyLines);
}

//
// ExprList
//

void ExprList::push(Expr* apExpr)
{
    m_lChildren.push_back(apExpr);
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

const Token* ExprCmd::cmd() const
{
    return m_pCmd;
}

const Expr* ExprCmd::expr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

void ExprCmd::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string ExprCmd::str_a() const
{
    return "ExprCmd " + m_pCmd->str();
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

const Expr* IfStmt::condition() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const StmtList* IfStmt::then_stmts() const
{
    return static_cast<StmtList*>(m_lChildren[1]);
}

const StmtList* IfStmt::else_stmts() const
{
    return static_cast<StmtList*>(m_lChildren[2]);
}

void IfStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string IfStmt::str_a() const
{
    return "IfStmt";
}

//
// WhileStmt
//

WhileStmt::WhileStmt(ast::Expr* apCondition, ast::StmtList* apStmtList)
{
    m_lChildren.push_back(apCondition);
    m_lChildren.push_back(apStmtList);
}

WhileStmt::~WhileStmt()
{
}

const Expr* WhileStmt::condition() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const StmtList* WhileStmt::stmts() const
{
    return static_cast<StmtList*>(m_lChildren[1]);
}

void WhileStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string WhileStmt::str_a() const
{
    return "WhileStmt";
}

//
// ForStmt
//

ForStmt::ForStmt(ast::Expr* apItem, ast::Expr* apItems, ast::StmtList* apStmts)
{
    m_lChildren.push_back(apItem);
    m_lChildren.push_back(apItems);
    m_lChildren.push_back(apStmts);
}

ForStmt::~ForStmt()
{
}

const Expr* ForStmt::item() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* ForStmt::items() const
{
    return static_cast<Expr*>(m_lChildren[1]);
}

const StmtList* ForStmt::stmts() const
{
    return static_cast<StmtList*>(m_lChildren[2]);
}

void ForStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string ForStmt::str_a() const
{
    return "ForStmt";
}

//
// JumpStmt
//

JumpStmt::JumpStmt(Token* apToken, Expr* apExpr) :
    m_pToken { apToken }
{
    m_lChildren.push_back(apExpr);
}

JumpStmt::~JumpStmt()
{
}

const Token* JumpStmt::token() const
{
    return m_pToken;
}

const Expr* JumpStmt::expr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

void JumpStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string JumpStmt::str_a() const
{
    return "JumpStmt " + m_pToken->str();
}

//
// FnParamList
//

FnParamList::~FnParamList()
{
}

std::string FnParamList::toString() const
{
    return "FnParamList";
}

void FnParamList::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// FnArgList
//

FnArgList::~FnArgList()
{
}

std::string FnArgList::toString() const
{
    return "FnArgList";
}

void FnArgList::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// FnParam
//

FnParam::FnParam(Var* apName, Expr* apDefaultExpr)
{
    m_lChildren.push_back(apName);
    m_lChildren.push_back(apDefaultExpr);
}

FnParam::~FnParam()
{
}

const Var* FnParam::name() const
{
    return static_cast<Var*>(m_lChildren[0]);
}

const Expr* FnParam::default_value() const
{
    return static_cast<Expr*>(m_lChildren[1]);
}

std::string FnParam::toString() const
{
    return "FnParam";
}

void FnParam::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// FnStmt
//

FnStmt::FnStmt(Token* apName,
               Token* apBang,
               FnParamList* apFnParamList,
               const std::vector<Token*>& alModifiers,
               StmtList* apBody) :
    m_pName { apName },
    m_pBang { apBang },
    m_lModifiers { alModifiers }
{
    m_lChildren.push_back(apFnParamList);
    m_lChildren.push_back(apBody);
}

FnStmt::~FnStmt()
{
}

const Token* FnStmt::name() const
{
    return m_pName;
}

const Token* FnStmt::bang() const
{
    return m_pBang;
};

const FnParamList* FnStmt::params() const
{
    return static_cast<FnParamList*>(m_lChildren[0]);
};

const std::vector<Token*>& FnStmt::modifiers() const
{
    return m_lModifiers;
};

const StmtList* FnStmt::body() const
{
    return static_cast<StmtList*>(m_lChildren[1]);
};

std::string FnStmt::str_a() const
{
    std::string lsStr = "FnStmt " + m_pName->str();

    if (m_pBang != nullptr)
    {
        lsStr += " " + m_pBang->str();
    }

    for (Token* pModifier : m_lModifiers)
    {
        lsStr += " " + pModifier->str();
    }

    return lsStr;
}

void FnStmt::accept(ASTVisitor& acASTVisitor) const
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

const Expr* BinaryOp::lexpr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* BinaryOp::rexpr() const
{
    return static_cast<Expr*>(m_lChildren[1]);
}

std::string BinaryOp::toString() const
{
    return "BinaryOp " + m_pOp->str();
}

void BinaryOp::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// CasedBinaryOp
//

CasedBinaryOp::CasedBinaryOp(Token* apOp, Expr* apLeft, Expr* apRight, Token* apCaseSensitivity) :
    BinaryOp(apOp, apLeft, apRight),
    m_pCaseSensitivity { apCaseSensitivity }
{
}

CasedBinaryOp::~CasedBinaryOp()
{
}

const Token* CasedBinaryOp::case_sensitivity() const
{
    return m_pCaseSensitivity;
}

std::string CasedBinaryOp::toString() const
{
    std::string lsStr = "CasedBinaryOp " + m_pOp->str();

    if (m_pCaseSensitivity != nullptr)
    {
        lsStr += m_pCaseSensitivity->str();
    }

    return lsStr;
}

void CasedBinaryOp::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// DictEntry
//

DictEntry::DictEntry(Expr* apKey, Expr* apValue)
{
    m_lChildren.push_back(apKey);
    m_lChildren.push_back(apValue);
}

DictEntry::~DictEntry()
{
}

const Expr* DictEntry::key() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* DictEntry::value() const
{
    return static_cast<Expr*>(m_lChildren[1]);
};

std::string DictEntry::toString() const
{
    return "DictEntry";
}

void DictEntry::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// DictExpr
//

DictExpr::~DictExpr()
{
}

void DictExpr::push(DictEntry* apEntry)
{
    m_lChildren.push_back(apEntry);
}

std::vector<const DictEntry*> DictExpr::entries() const
{
    std::vector<const DictEntry*> lEntries;

    for (Node* pNode : m_lChildren)
    {
        lEntries.push_back(static_cast<DictEntry*>(pNode));
    }

    return lEntries;
}

std::string DictExpr::toString() const
{
    return "DictExpr";
}

void DictExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// ListExpr
//

ListExpr::~ListExpr()
{
}

void ListExpr::push(Expr* apExpr)
{
    m_lChildren.push_back(apExpr);
}

std::vector<const Expr*> ListExpr::exprs() const
{
    std::vector<const Expr*> lExprs;

    for (Node* pNode : m_lChildren)
    {
        lExprs.push_back(static_cast<Expr*>(pNode));
    }

    return lExprs;
}

std::string ListExpr::toString() const
{
    return "ListExpr";
}

void ListExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// CallExpr
//

CallExpr::CallExpr(ast::Expr* apCallable, FnArgList* apFnArgList)
{
    m_lChildren.push_back(apCallable);
    m_lChildren.push_back(apFnArgList);
}

CallExpr::~CallExpr()
{
}

const Expr* CallExpr::callable() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const FnArgList* CallExpr::args() const
{
    return static_cast<FnArgList*>(m_lChildren[1]);
}

std::string CallExpr::toString() const
{
    return "CallExpr";
}

void CallExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// MethodCallExpr
//

MethodCallExpr::MethodCallExpr(Token* apOp, Expr* apReceiver, Expr* apCallExpr) :
    m_pOp { apOp }
{
    m_lChildren.push_back(apReceiver);
    m_lChildren.push_back(apCallExpr);
}

MethodCallExpr::~MethodCallExpr()
{
}

const Token* MethodCallExpr::op() const
{
    return m_pOp;
}

const Expr* MethodCallExpr::receiver() const
{
    return static_cast<Expr*>(m_lChildren[0]);
};

const Expr* MethodCallExpr::call() const
{
    return static_cast<Expr*>(m_lChildren[1]);
};

std::string MethodCallExpr::toString() const
{
    return "MethodCallExpr " + m_pOp->str();
}

void MethodCallExpr::accept(ASTVisitor& acASTVisitor) const
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
    return "Literal " + m_pToken->str();
}

void Literal::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// IndexExpr
//

IndexExpr::IndexExpr(Expr* apIndexable, Expr* apStart, Expr* apStop)
{
    m_lChildren.push_back(apIndexable);
    m_lChildren.push_back(apStart);
    m_lChildren.push_back(apStop);
}

IndexExpr::~IndexExpr()
{
}

std::string IndexExpr::toString() const
{
    return "IndexExpr";
}

const Expr* IndexExpr::indexable() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* IndexExpr::start() const
{
    return static_cast<Expr*>(m_lChildren[1]);
};

const Expr* IndexExpr::stop() const
{
    return static_cast<Expr*>(m_lChildren[2]);
};

void IndexExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// TernaryOp
//

TernaryOp::TernaryOp(Token* apLeftOp,
                     Token* apRightOp,
                     Expr* apLeftExpr,
                     Expr* apMiddleExpr,
                     Expr* apRightExpr) :
    m_pLeftOp { apLeftOp },
    m_pRightOp { apRightOp }
{
    m_lChildren.push_back(apLeftExpr);
    m_lChildren.push_back(apMiddleExpr);
    m_lChildren.push_back(apRightExpr);
}

TernaryOp::~TernaryOp()
{
}

const Token* TernaryOp::lop() const
{
    return m_pLeftOp;
}

const Token* TernaryOp::rop() const
{
    return m_pRightOp;
}

const Expr* TernaryOp::lexpr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* TernaryOp::mexpr() const
{
    return static_cast<Expr*>(m_lChildren[1]);
}

const Expr* TernaryOp::rexpr() const
{
    return static_cast<Expr*>(m_lChildren[2]);
}

std::string TernaryOp::toString() const
{
    return "TernaryOp " + m_pLeftOp->str() + " " + m_pRightOp->str();
}

void TernaryOp::accept(ASTVisitor& acASTVisitor) const
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

const Expr* UnaryOp::rexpr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

std::string UnaryOp::toString() const
{
    return "UnaryOp " + m_pOp->str();
}

void UnaryOp::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// Var
//

Var::Var(Token* apScope, Token* apName) :
    m_pScope { apScope },
    m_pName { apName }
{
}

Var::~Var()
{
}

const Token* Var::scope() const
{
    return m_pScope;
}

const Token* Var::name() const
{
    return m_pName;
}

std::string Var::toString() const
{
    if (m_pScope != nullptr)
    {
        return "Var " + m_pScope->str() + ":" + m_pName->str();
    }
    else
    {
        return "Var " + m_pName->str();
    }
}

void Var::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

//
// AssignStmt
//

AssignStmt::AssignStmt(Token* apOp, Var* apVar, Expr* apExpr) :
    m_pOp { apOp }
{
    m_lChildren.push_back(apVar);
    m_lChildren.push_back(apExpr);
}

AssignStmt::~AssignStmt()
{
}

const Token* AssignStmt::op() const
{
    return m_pOp;
}

const Var* AssignStmt::var() const
{
    return static_cast<Var*>(m_lChildren[0]);
}

const Expr* AssignStmt::expr() const
{
    return static_cast<Expr*>(m_lChildren[1]);
}

void AssignStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string AssignStmt::str_a() const
{
    return "AssignStmt " + m_pOp->str();
}

//
// CommentStmt
//

CommentStmt::CommentStmt(Token* apComment, bool abTrailing) :
    m_bTrailing { abTrailing },
    m_pComment { apComment }
{
}

bool CommentStmt::trailing() const
{
    return m_bTrailing;
}

const Token* CommentStmt::comment() const
{
    return m_pComment;
}

CommentStmt::~CommentStmt()
{
}

std::string CommentStmt::str_a() const
{
    return "CommentStmt " + m_pComment->str();
}

std::string CommentStmt::str_b() const
{
    std::string lsStr = Stmt::str_b();

    lsStr += "; trailing = ";
    lsStr += m_bTrailing ? "yes" : "no";

    return lsStr;
}

void CommentStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}
