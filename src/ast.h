#pragma once

#include <string>
#include <vector>

#include "ASTVisitor.h"
#include "Token.h"

namespace ast
{

class Node
{  // {{{
public:
    virtual ~Node();

    const std::vector<Node*>& children() const;

    virtual std::string toString() const = 0;
    virtual void accept(ASTVisitor& visitor) const = 0;

protected:
    std::vector<Node*> m_lChildren;
};  // }}}

class Stmt : public Node
{  // {{{
public:
    virtual ~Stmt() = default;

    virtual std::string toString() const;

protected:
    virtual std::string str_b() const;

private:
    virtual std::string str_a() const = 0;
};  // }}}

class EmptyStmt : public Stmt
{  // {{{
public:
    virtual ~EmptyStmt();

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;
};  // }}}

class Expr : public Node
{  // {{{
public:
    virtual ~Expr() = default;
};  // }}}

class GroupExpr : public Expr
{  // {{{
public:
    GroupExpr(Expr* expr);
    virtual ~GroupExpr();

    const Expr* expr() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class ExprList : public Node
{  // {{{
public:
    virtual ~ExprList() = default;

    void push(Expr* expr);
};  // }}}

class ExprCmd : public Stmt
{  // {{{
public:
    ExprCmd(Token* cmd, Expr* expr);
    virtual ~ExprCmd();

    const Token* ex_cmd() const;
    const Expr* expr() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pExCmd;
};  // }}}

class StmtList : public Node
{  // {{{
public:
    StmtList() = default;
    virtual ~StmtList();

    void push(Stmt* stmt);
    void take(StmtList* stmts);
    void insert(size_t pos, Stmt* stmt);
    void extend(const StmtList* stmts);

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class Program : public Node
{  // {{{
public:
    Program(ast::StmtList* stmts);
    virtual ~Program();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class IfBranch : public Node
{  // {{{
public:
    IfBranch(Token* token, Expr* expr, StmtList* body);
    virtual ~IfBranch();

    const Token* token() const;
    const Expr* condition() const;
    const StmtList* body() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pToken;
};  // }}}

class IfStmt : public Stmt
{  // {{{
public:
    IfStmt(const std::vector<IfBranch*>& ifbranches, Token* ex_endif);
    virtual ~IfStmt();

    std::vector<const IfBranch*> branches() const;
    const Token* ex_endif() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pExEndIf;
};  // }}}

class WhileStmt : public Stmt
{  // {{{
public:
    WhileStmt(Token* ex_while, Token* ex_endwhile, Expr* condition, StmtList* stmts);
    virtual ~WhileStmt();

    const Expr* condition() const;
    const Token* ex_cmd_while() const;
    const Token* ex_cmd_endwile() const;
    const StmtList* stmts() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pExWhile;
    Token* m_pExEndWhile;
};  // }}}

class ForStmt : public Stmt
{  // {{{
public:
    ForStmt(Expr* item, Expr* items, StmtList* stmts, Token* ex_endfo);
    virtual ~ForStmt();

    const Expr* item() const;
    const Expr* items() const;
    const StmtList* stmts() const;
    const Token* ex_endfo() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pExEndFo;
};  // }}}

class JumpStmt : public Stmt
{  // {{{
public:
    JumpStmt(Token* token, Expr* expr);
    virtual ~JumpStmt();

    const Token* ex_cmd() const;
    const Expr* expr() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pExCmd;
};  // }}}

class FnParamList : public ExprList
{  // {{{
public:
    FnParamList() = default;
    virtual ~FnParamList();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

// TODO (gh-37): Rename xxList to xxSeq to distinguish from a ListExpr
class FnArgList : public ExprList
{  // {{{
public:
    FnArgList() = default;
    virtual ~FnArgList();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class FnParam : public Expr
{  // {{{
public:
    FnParam(Var* name, Expr* default_value);
    virtual ~FnParam();

    const Var* name() const;
    const Expr* default_value() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class FnStmt : public Stmt
{  // {{{
public:
    FnStmt(Token* ex_fu,
           Token* ex_endf,
           Token* name,
           Token* bang,
           FnParamList* params,
           const std::vector<Token*>& modifiers,
           StmtList* body);
    virtual ~FnStmt();

    const Token* ex_fu() const;
    const Token* ex_endfu() const;
    const Token* name() const;
    const Token* bang() const;
    const FnParamList* params() const;
    const std::vector<Token*>& modifiers() const;
    const StmtList* body() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pExFu;
    Token* m_pName;
    Token* m_pBang;
    Token* m_pExEndFu;
    std::vector<Token*> m_lModifiers;
};  // }}}

class BinaryOp : public Expr
{  // {{{
public:
    BinaryOp(Token* op, Expr* left, Expr* right);
    virtual ~BinaryOp();

    const Token* op() const;
    const Expr* lexpr() const;
    const Expr* rexpr() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

protected:
    Token* m_pOp;
};  // }}}

class CasedBinaryOp : public BinaryOp
{  // {{{
public:
    CasedBinaryOp(Token* op, Expr* left, Expr* right, Token* case_sensitivity);
    virtual ~CasedBinaryOp();

    const Token* case_sensitivity() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pCaseSensitivity;
};  // }}}

class DictEntry : public Node
{  // {{{
public:
    DictEntry(Expr* key, Expr* value);
    virtual ~DictEntry();

    const Expr* key() const;
    const Expr* value() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class DictExpr : public Expr
{  // {{{
public:
    DictExpr() = default;
    virtual ~DictExpr();

    void push(DictEntry* entry);

    std::vector<const DictEntry*> entries() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class ListExpr : public Expr
{  // {{{
public:
    ListExpr() = default;
    virtual ~ListExpr();

    void push(Expr* expr);

    std::vector<const Expr*> exprs() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class CallExpr : public Expr
{  // {{{
public:
    CallExpr(Expr* callable, FnArgList* args);
    virtual ~CallExpr();

    const Expr* callable() const;
    const FnArgList* args() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class MethodCallExpr : public Expr
{  // {{{
public:
    MethodCallExpr(Token* op, Expr* receiver, Expr* call);
    virtual ~MethodCallExpr();

    const Token* op() const;
    const Expr* receiver() const;
    const Expr* call() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pOp;
};  // }}}

class InterpStr : public Expr
{  // {{{
public:
    InterpStr() = default;
    virtual ~InterpStr();

    void push(Expr* expr);

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class StrExpr : public Expr
{  // {{{
public:
    StrExpr(Token* str, Token* ldelim, Token* rdelim);
    virtual ~StrExpr();

    const Token* str() const;
    const Token* ldelim() const;
    const Token* rdelim() const;

protected:
    Token* m_pStr;
    Token* m_pLDelim;
    Token* m_pRDelim;
};  // }}}

class LiteralStr : public StrExpr
{  // {{{
public:
    using StrExpr::StrExpr;
    virtual ~LiteralStr();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class StrConst : public StrExpr
{  // {{{
public:
    using StrExpr::StrExpr;
    virtual ~StrConst();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class Literal : public Expr
{  // {{{
public:
    Literal(Token* token);
    virtual ~Literal();

    const Token* token() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pToken;
};  // }}}

class SliceExpr : public Expr
{  // {{{
public:
    SliceExpr(Expr* sliceable, Expr* start, Expr* stop);
    virtual ~SliceExpr();

    const Expr* sliceable() const;
    const Expr* start() const;
    const Expr* stop() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class IndexExpr : public Expr
{  // {{{
public:
    IndexExpr(Expr* indexable, Expr* index);
    virtual ~IndexExpr();

    const Expr* indexable() const;
    const Expr* index() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};  // }}}

class TernaryOp : public Expr
{  // {{{
public:
    TernaryOp(Token* left_op, Token* right_op, Expr* left, Expr* middle, Expr* right);
    virtual ~TernaryOp();

    const Token* lop() const;
    const Token* rop() const;
    const Expr* lexpr() const;
    const Expr* mexpr() const;
    const Expr* rexpr() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pLeftOp;
    Token* m_pRightOp;
};  // }}}

class UnaryOp : public Expr
{  // {{{
public:
    UnaryOp(Token* op, Expr* right);
    virtual ~UnaryOp();

    const Token* op() const;
    const Expr* rexpr() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pOp;
};  // }}}

class ScopeExpr : public Expr
{  // {{{
public:
    ScopeExpr(Token* scope);
    virtual ~ScopeExpr();

    const Token* scope() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pScope;
};  // }}}

class Var : public Expr
{  // {{{
public:
    Var(Token* sigil, ScopeExpr* scope, Token* name);
    virtual ~Var();

    const Token* sigil() const;
    const ScopeExpr* scope() const;
    const Token* name() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pSigil;
    Token* m_pName;
};  // }}}

class AssignStmt : public Stmt
{  // {{{
public:
    AssignStmt(Token* op, Expr* left, Expr* right);
    virtual ~AssignStmt();

    const Token* op() const;
    const Expr* lexpr() const;
    const Expr* rexpr() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pOp;
};  // }}}

class CommentStmt : public Stmt
{  // {{{
public:
    CommentStmt(Token* comment, bool trailing = false);
    virtual ~CommentStmt();

    const Token* comment() const;

    bool trailing() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;
    virtual std::string str_b() const;

    bool m_bTrailing;
    Token* m_pComment;
};  // }}}

};

// vim: set foldmethod=marker :
