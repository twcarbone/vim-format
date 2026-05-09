#include <iterator>

#include "ast.h"

using namespace ast;

// Tip 1-4: The final "Sink". Store the vector as the destination type. The move is
// completed here into the class member.
Node::Node(std::vector<Node*>&& alChildren) :
    m_lChildren { std::move(alChildren) }
{
}

// Tip 1-5: delete on Node* relies on Tip 1-6.
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

Stmt::Stmt(std::vector<Node*>&& alChildren) :
    Node { std::move(alChildren) }
{
}

std::string Stmt::toString() const
{
    return str_a() + " <" + str_b() + ">";
}

std::string Stmt::str_b() const
{
    return "";
}

void EmptyStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string EmptyStmt::str_a() const
{
    return "EmptyStmt";
}

Expr::Expr(std::vector<Node*>&& alChildren) :
    Node { std::move(alChildren) }
{
}

GroupExpr::GroupExpr(Expr* apExpr)
{
    m_lChildren.push_back(apExpr);
}

const Expr* GroupExpr::expr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

std::string GroupExpr::toString() const
{
    return "GroupExpr";
}

void GroupExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

void ExprList::push(Expr* apExpr)
{
    m_lChildren.push_back(apExpr);
}

void StmtList::push(Stmt* apStmt)
{
    m_lChildren.push_back(apStmt);
}

void StmtList::take(StmtList* apStmtList)
{
    extend(apStmtList);
    apStmtList->m_lChildren.clear();
}

void StmtList::insert(size_t anPos, Stmt* apStmt)
{
    m_lChildren.insert(m_lChildren.begin() + anPos, apStmt);
}

void StmtList::extend(const StmtList* apStmtList)
{
    m_lChildren.insert(m_lChildren.end(), apStmtList->m_lChildren.begin(), apStmtList->m_lChildren.end());
}

std::string StmtList::toString() const
{
    return "StmtList";
}

void StmtList::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

ExprCmd::ExprCmd(Token* apCount, Token* apCmd, Expr* apExpr) :
    m_pCount { apCount },
    m_pExCmd { apCmd }
{
    m_lChildren.push_back(apExpr);
}

ExprCmd::ExprCmd(Token* apCount, Token* apCmd, std::vector<Expr*>&& alExprs) :
    Stmt({ std::make_move_iterator(alExprs.begin()), std::make_move_iterator(alExprs.end()) }),
    m_pCount { apCount },
    m_pExCmd { apCmd }
{
}

const Token* ExprCmd::count() const
{
    return m_pCount;
}

const Token* ExprCmd::ex_cmd() const
{
    return m_pExCmd;
}

void ExprCmd::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string ExprCmd::str_a() const
{
    std::string tmp = "ExprCmd";
    tmp += " ";
    tmp += m_pExCmd->str();
    tmp += " ";
    tmp += m_pCount == nullptr ? "-count" : "+count:" + m_pCount->str();
    return tmp;
}

Program::Program(ast::StmtList* apStmtList)
{
    m_lChildren.push_back(apStmtList);
}

std::string Program::toString() const
{
    return "Program";
}

void Program::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

IfBranch::IfBranch(Token* apToken, ast::Expr* apCondition, ast::StmtList* apBody) :
    m_pToken { apToken }
{
    m_lChildren.push_back(apCondition);
    m_lChildren.push_back(apBody);
}

const Token* IfBranch::token() const
{
    return m_pToken;
}

const Expr* IfBranch::condition() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const StmtList* IfBranch::body() const
{
    return static_cast<StmtList*>(m_lChildren[1]);
}

std::string IfBranch::toString() const
{
    return "IfBranch";
}

void IfBranch::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

IfStmt::IfStmt(const std::vector<IfBranch*>& alIfBranches, Token* apExEndIf) :
    m_pExEndIf { apExEndIf }
{
    for (IfBranch* pIfBranch : alIfBranches)
    {
        m_lChildren.push_back(pIfBranch);
    }
}

const Token* IfStmt::ex_endif() const
{
    return m_pExEndIf;
}

void IfStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string IfStmt::str_a() const
{
    return "IfStmt";
}

WhileStmt::WhileStmt(Token* apExWhile,
                     Token* apExEndWhile,
                     ast::Expr* apCondition,
                     ast::StmtList* apStmtList) :
    m_pExWhile { apExWhile },
    m_pExEndWhile { apExEndWhile }
{
    m_lChildren.push_back(apCondition);
    m_lChildren.push_back(apStmtList);
}

const Expr* WhileStmt::condition() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Token* WhileStmt::ex_cmd_while() const
{
    return m_pExWhile;
}

const Token* WhileStmt::ex_cmd_endwile() const
{
    return m_pExEndWhile;
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

ForStmt::ForStmt(ast::Expr* apItem, ast::Expr* apItems, ast::StmtList* apStmts, Token* apExEndFo) :
    m_pExEndFo { apExEndFo }
{
    m_lChildren.push_back(apItem);
    m_lChildren.push_back(apItems);
    m_lChildren.push_back(apStmts);
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

const Token* ForStmt::ex_endfo() const
{
    return m_pExEndFo;
}

void ForStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string ForStmt::str_a() const
{
    return "ForStmt";
}

JumpStmt::JumpStmt(Token* apToken, Expr* apExpr) :
    m_pExCmd { apToken }
{
    m_lChildren.push_back(apExpr);
}

const Token* JumpStmt::ex_cmd() const
{
    return m_pExCmd;
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
    return "JumpStmt " + m_pExCmd->str();
}

std::string FnParamList::toString() const
{
    return "FnParamList";
}

void FnParamList::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string FnArgList::toString() const
{
    return "FnArgList";
}

void FnArgList::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

FnParam::FnParam(Var* apName, Expr* apDefaultExpr)
{
    m_lChildren.push_back(apName);
    m_lChildren.push_back(apDefaultExpr);
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

FnStmt::FnStmt(Token* apExFu,
               Token* apExEndFu,
               Token* apName,
               Token* apBang,
               FnParamList* apFnParamList,
               const std::vector<Token*>& alModifiers,
               StmtList* apBody) :
    m_pExFu { apExFu },
    m_pName { apName },
    m_pBang { apBang },
    m_pExEndFu { apExEndFu },
    m_lModifiers { alModifiers }
{
    m_lChildren.push_back(apFnParamList);
    m_lChildren.push_back(apBody);
}

const Token* FnStmt::ex_fu() const
{
    return m_pExFu;
}

const Token* FnStmt::ex_endfu() const
{
    return m_pExEndFu;
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

BinaryOp::BinaryOp(Token* apOp, Expr* apLeft, Expr* apRight) :
    m_pOp { apOp }
{
    m_lChildren.push_back(apLeft);
    m_lChildren.push_back(apRight);
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

CasedBinaryOp::CasedBinaryOp(Token* apOp, Expr* apLeft, Expr* apRight, Token* apCaseSensitivity) :
    BinaryOp(apOp, apLeft, apRight),
    m_pCaseSensitivity { apCaseSensitivity }
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

DictEntry::DictEntry(Expr* apKey, Expr* apValue)
{
    m_lChildren.push_back(apKey);
    m_lChildren.push_back(apValue);
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

DictExpr::DictExpr(std::vector<DictEntry*>&& alEntries) :
    Expr({ std::make_move_iterator(alEntries.begin()), std::make_move_iterator(alEntries.end()) })
{
}

std::string DictExpr::toString() const
{
    return "DictExpr";
}

void DictExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

// Tip 1-2: Accept by r-value reference (&&) to enforce the "Move-Only" contract.
ListExpr::ListExpr(std::vector<Expr*>&& alExprs) :
    //  Tip 1-3: Use a "converting move".
    //
    //  1.  Iterator-range construction. Call std::vector constructor that takes 2
    //      iterators.
    //
    //  2.  Move-iterators. Use std::make_move_iterator to return an r-value reference to
    //      each member. That is, returns Expr*&&.
    //
    //  3.  Implicit upcasting. Expr* implicitly converts to Node*
    //
    //  Note: "moving" a pointer is the same operation as copying. The purpose of move
    //  here is semantic - signalling that this vector is empty and Node owns the
    //  addresses.
    Expr({ std::make_move_iterator(alExprs.begin()), std::make_move_iterator(alExprs.end()) })
{
}

void ListExpr::push(Expr* apExpr)
{
    m_lChildren.push_back(apExpr);
}

std::string ListExpr::toString() const
{
    return "ListExpr";
}

void ListExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

ListAssignExpr::ListAssignExpr(std::vector<Expr*>&& alNames, Token* apSemi) :
    Expr({ std::make_move_iterator(alNames.begin()), std::make_move_iterator(alNames.end()) }),
    m_pSemi { apSemi }
{
}

const Token* ListAssignExpr::semi() const
{
    return m_pSemi;
}

std::string ListAssignExpr::toString() const
{
    std::string tmp = "ListAssignExpr";

    if (m_pSemi != nullptr)
    {
        tmp += " with lastname";
    }

    return tmp;
}

void ListAssignExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

CallExpr::CallExpr(ast::Expr* apCallable, FnArgList* apFnArgList)
{
    m_lChildren.push_back(apCallable);
    m_lChildren.push_back(apFnArgList);
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

MethodCallExpr::MethodCallExpr(Token* apOp, Expr* apReceiver, Expr* apCallExpr) :
    m_pOp { apOp }
{
    m_lChildren.push_back(apReceiver);
    m_lChildren.push_back(apCallExpr);
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

void InterpStr::push(Expr* apExpr)
{
    m_lChildren.push_back(apExpr);
}

std::string InterpStr::toString() const
{
    return "InterpStr";
}

void InterpStr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

StrExpr::StrExpr(Token* apStr, Token* apLeftDelim, Token* apRightDelim) :
    m_pStr { apStr },
    m_pLDelim { apLeftDelim },
    m_pRDelim { apRightDelim }
{
}

const Token* StrExpr::str() const
{
    return m_pStr;
}

const Token* StrExpr::ldelim() const
{
    return m_pLDelim;
}

const Token* StrExpr::rdelim() const
{
    return m_pRDelim;
}

std::string StrExpr::toString(const std::string& asName) const
{
    std::string tmp = asName + " ";
    tmp += ";" + str_repr(m_pLDelim) + ";";
    tmp += str_repr(m_pStr);
    tmp += ";" + str_repr(m_pRDelim) + ";";
    return tmp;
}

std::string StrExpr::str_repr(Token* apStr) const
{
    if (apStr == nullptr)
    {
        return "";
    }

    if (apStr->str() == "\n")
    {
        return "\\n";
    }

    return apStr->str();
}

std::string LiteralStr::toString() const
{
    return StrExpr::toString("LiteralStr");
}

void LiteralStr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string StrConst::toString() const
{
    return StrExpr::toString("StrConst");
}

void StrConst::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string Pattern::toString() const
{
    return StrExpr::toString("Pattern");
}

void Pattern::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

Literal::Literal(Token* apToken) :
    m_pToken { apToken }
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

SliceExpr::SliceExpr(Expr* apSliceable, Expr* apStart, Expr* apStop)
{
    m_lChildren.push_back(apSliceable);
    m_lChildren.push_back(apStart);
    m_lChildren.push_back(apStop);
}

std::string SliceExpr::toString() const
{
    return "SliceExpr";
}

const Expr* SliceExpr::sliceable() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* SliceExpr::start() const
{
    return static_cast<Expr*>(m_lChildren[1]);
};

const Expr* SliceExpr::stop() const
{
    return static_cast<Expr*>(m_lChildren[2]);
};

void SliceExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

IndexExpr::IndexExpr(Expr* apIndexable, Expr* apIndex)
{
    m_lChildren.push_back(apIndexable);
    m_lChildren.push_back(apIndex);
}

const Expr* IndexExpr::indexable() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* IndexExpr::index() const
{
    return static_cast<Expr*>(m_lChildren[1]);
}

std::string IndexExpr::toString() const
{
    return "IndexExpr";
}

void IndexExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

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

UnaryOp::UnaryOp(Token* apOp, Expr* apRight) :
    m_pOp { apOp }
{
    m_lChildren.push_back(apRight);
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

Var::Var(Token* apSigil, Token* apScope, Token* apName) :
    m_pSigil { apSigil },
    m_pScope { apScope },
    m_pName { apName }
{
}

const Token* Var::sigil() const
{
    return m_pSigil;
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
    std::string tmp = "Var";
    tmp += " ";
    tmp += m_pSigil == nullptr ? "" : m_pSigil->str();
    tmp += m_pScope == nullptr ? "" : m_pScope->str();
    tmp += m_pName == nullptr ? "" : m_pName->str();
    return tmp;
}

void Var::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

AssignStmt::AssignStmt(Token* apExCmd, Token* apOp, Expr* apLeft, Expr* apRight) :
    m_pExCmd { apExCmd },
    m_pOp { apOp }
{
    m_lChildren.push_back(apLeft);
    m_lChildren.push_back(apRight);
}

const Token* AssignStmt::ex_cmd() const
{
    return m_pExCmd;
}

const Token* AssignStmt::op() const
{
    return m_pOp;
}

const Expr* AssignStmt::lexpr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

const Expr* AssignStmt::rexpr() const
{
    return static_cast<Expr*>(m_lChildren[1]);
}

void AssignStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string AssignStmt::str_a() const
{
    return "AssignStmt " + m_pExCmd->str() + " " + m_pOp->str();
}

VarQueryStmt::VarQueryStmt(Token* apExCmd, std::vector<Expr*>&& alNames) :
    Stmt({ std::make_move_iterator(alNames.begin()), std::make_move_iterator(alNames.end()) }),
    m_pExCmd { apExCmd }
{
}

const Token* VarQueryStmt::ex_cmd() const
{
    return m_pExCmd;
}

void VarQueryStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string VarQueryStmt::str_a() const
{
    return "VarQueryStmt " + m_pExCmd->str();
}

HereDocExpr::HereDocExpr(std::vector<Expr*>&& alLines,
                         std::vector<Token*>&& alModifiers,
                         Token* apEndMarker) :
    Expr({ std::make_move_iterator(alLines.begin()), std::make_move_iterator(alLines.end()) }),
    m_pEndMarker { apEndMarker },
    m_lModifiers { alModifiers }
{
}

const std::vector<Token*>& HereDocExpr::modifiers() const
{
    return m_lModifiers;
}

const Token* HereDocExpr::endmarker() const
{
    return m_pEndMarker;
};

std::string HereDocExpr::toString() const
{
    std::string tmp = "HereDocExpr";

    for (const Token* pModifier : m_lModifiers)
    {
        tmp += " " + pModifier->str();
    }

    tmp += " " + m_pEndMarker->str();
    return tmp;
}

void HereDocExpr::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

UnletStmt::UnletStmt(Token* apExUnlet, Token* apBang, Expr* apExpr) :
    m_pExUnlet { apExUnlet },
    m_pBang { apBang }
{
    m_lChildren.push_back(apExpr);
}

const Token* UnletStmt::ex_unlet() const
{
    return m_pExUnlet;
}

const Token* UnletStmt::bang() const
{
    return m_pBang;
}

const Expr* UnletStmt::expr() const
{
    return static_cast<Expr*>(m_lChildren[0]);
}

void UnletStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string UnletStmt::str_a() const
{
    std::string lsStr = "UnletStmt";

    if (m_pBang != nullptr)
    {
        lsStr += " " + m_pBang->str();
    }

    return lsStr;
}

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

std::string CommentStmt::str_a() const
{
    return "CommentStmt " + m_pComment->str();
}

std::string CommentStmt::str_b() const
{
    std::string lsStr = Stmt::str_b();

    if (!lsStr.empty())
    {
        lsStr += "; ";
    }

    lsStr += "trailing = ";
    lsStr += m_bTrailing ? "yes" : "no";

    return lsStr;
}

void CommentStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

LockVarStmt::LockVarStmt(Token* apExCmd, Token* apBang, Token* apDepth, std::vector<Expr*>&& alNames) :
    Stmt({ std::make_move_iterator(alNames.begin()), std::make_move_iterator(alNames.end()) }),
    m_pExCmd { apExCmd },
    m_pBang { apBang },
    m_pDepth { apDepth }
{
}

const Token* LockVarStmt::ex_cmd() const
{
    return m_pExCmd;
}

const Token* LockVarStmt::bang() const
{
    return m_pBang;
}

const Token* LockVarStmt::depth() const
{
    return m_pDepth;
}

void LockVarStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string LockVarStmt::str_a() const
{
    std::string tmp = "LockVarStmt";
    tmp += " ";
    tmp += m_pExCmd->str();
    tmp += " ";
    tmp += m_pBang == nullptr ? "-bang" : "+bang";
    tmp += " ";
    tmp += m_pDepth == nullptr ? "-depth" : "+depth:" + m_pDepth->str();
    return tmp;
}

TryBranch::TryBranch(Token* apExCmd, Pattern* apPattern, StmtList* apBody) :
    m_pExCmd { apExCmd }
{
    m_lChildren.push_back(apPattern);
    m_lChildren.push_back(apBody);
}

const Token* TryBranch::ex_cmd() const
{
    return m_pExCmd;
}

const Pattern* TryBranch::pattern() const
{
    return static_cast<Pattern*>(m_lChildren[0]);
};

const StmtList* TryBranch::body() const
{
    return static_cast<StmtList*>(m_lChildren[1]);
};

std::string TryBranch::toString() const
{
    std::string tmp = "TryBranch";
    tmp += m_pExCmd == nullptr ? "" : " " + m_pExCmd->str();
    return tmp;
}

void TryBranch::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

TryStmt::TryStmt(std::vector<TryBranch*>&& alTryBranches, Token* apExEndTry) :
    Stmt({ std::make_move_iterator(alTryBranches.begin()), std::make_move_iterator(alTryBranches.end()) }),
    m_pExEndTry { apExEndTry }
{
}

const Token* TryStmt::ex_endtry() const
{
    return m_pExEndTry;
}

void TryStmt::accept(ASTVisitor& acASTVisitor) const
{
    acASTVisitor.visit(this);
}

std::string TryStmt::str_a() const
{
    return "TryStmt";
}
