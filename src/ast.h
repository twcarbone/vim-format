#pragma once

#include <string>
#include <vector>

#include "ASTVisitor.h"
#include "Token.h"

namespace ast
{

//
// Node
//

class Node
{
public:
    virtual ~Node();

    const std::vector<Node*>& children() const;

    virtual std::string toString() const = 0;
    virtual void accept(ASTVisitor& visitor) const = 0;

protected:
    std::vector<Node*> m_lChildren;
};

//
// Stmt
//

class Stmt : public Node
{
public:
    virtual ~Stmt() = default;

    virtual std::string toString() const;

protected:
    virtual std::string str_b() const;

private:
    virtual std::string str_a() const = 0;
};

//
// EmptyStmt
//

class EmptyStmt : public Stmt
{
public:
    virtual ~EmptyStmt();

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;
};

//
// Expr
//

class Expr : public Node
{
public:
    virtual ~Expr() = default;
};

//
// GroupExpr
//

class GroupExpr : public Expr
{
public:
    GroupExpr(Expr* expr);
    virtual ~GroupExpr();

    const Expr* expr() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// ExprList
//

class ExprList : public Node
{
public:
    virtual ~ExprList() = default;

    void push(Expr* expr);
};

//
// ExprCmd
//

class ExprCmd : public Stmt
{
public:
    ExprCmd(Token* cmd, Expr* expr);
    virtual ~ExprCmd();

    const Token* cmd() const;
    const Expr* expr() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pCmd;
};

//
// StmtList
//

class StmtList : public Node
{
public:
    StmtList() = default;
    virtual ~StmtList();

    void push(Stmt* stmt);
    void take(StmtList* stmts);
    void insert(size_t pos, Stmt* stmt);
    void extend(const StmtList* stmts);

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// Program
//

class Program : public Node
{
public:
    Program(ast::StmtList* stmts);
    virtual ~Program();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// IfBranch
//

class IfBranch : public Node
{
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
};

//
// IfStmt
//

class IfStmt : public Stmt
{
public:
    IfStmt();
    virtual ~IfStmt();

    void push(IfBranch* if_branch);

    std::vector<const IfBranch*> branches() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;
};

//
// WhileStmt
//

class WhileStmt : public Stmt
{
public:
    WhileStmt(Expr* condition, StmtList* stmts);
    virtual ~WhileStmt();

    const Expr* condition() const;
    const StmtList* stmts() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;
};

//
// ForStmt
//

class ForStmt : public Stmt
{
public:
    ForStmt(Expr* item, Expr* items, StmtList* stmts);
    virtual ~ForStmt();

    const Expr* item() const;
    const Expr* items() const;
    const StmtList* stmts() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;
};

//
// JumpStmt
//

class JumpStmt : public Stmt
{
public:
    JumpStmt(Token* token, Expr* expr);
    virtual ~JumpStmt();

    const Token* token() const;
    const Expr* expr() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pToken;
};

//
// FnParamList
//

class FnParamList : public ExprList
{
public:
    FnParamList() = default;
    virtual ~FnParamList();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// FnArgList
//

// TODO (gh-37): Rename xxList to xxSeq to distinguish from a ListExpr
class FnArgList : public ExprList
{
public:
    FnArgList() = default;
    virtual ~FnArgList();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// FnParam
//

class FnParam : public Expr
{
public:
    FnParam(Var* name, Expr* default_value);
    virtual ~FnParam();

    const Var* name() const;
    const Expr* default_value() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// FnStmt
//

class FnStmt : public Stmt
{
public:
    FnStmt(Token* name,
           Token* bang,
           FnParamList* params,
           const std::vector<Token*>& modifiers,
           StmtList* body);
    virtual ~FnStmt();

    const Token* name() const;
    const Token* bang() const;
    const FnParamList* params() const;
    const std::vector<Token*>& modifiers() const;
    const StmtList* body() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pName;
    Token* m_pBang;
    std::vector<Token*> m_lModifiers;
};

//
// BinaryOp
//

class BinaryOp : public Expr
{
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
};

//
// CasedBinaryOp
//

class CasedBinaryOp : public BinaryOp
{
public:
    CasedBinaryOp(Token* op, Expr* left, Expr* right, Token* case_sensitivity);
    virtual ~CasedBinaryOp();

    const Token* case_sensitivity() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pCaseSensitivity;
};

//
// DictEntry
//

class DictEntry : public Node
{
public:
    DictEntry(Expr* key, Expr* value);
    virtual ~DictEntry();

    const Expr* key() const;
    const Expr* value() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// DictExpr
//

class DictExpr : public Expr
{
public:
    DictExpr() = default;
    virtual ~DictExpr();

    void push(DictEntry* entry);

    std::vector<const DictEntry*> entries() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// ListExpr
//

class ListExpr : public Expr
{
public:
    ListExpr() = default;
    virtual ~ListExpr();

    void push(Expr* expr);

    std::vector<const Expr*> exprs() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// CallExpr
//

class CallExpr : public Expr
{
public:
    CallExpr(Expr* callable, FnArgList* args);
    virtual ~CallExpr();

    const Expr* callable() const;
    const FnArgList* args() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// MethodCallExpr
//

class MethodCallExpr : public Expr
{
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
};

//
// Literal
//

class Literal : public Expr
{
public:
    Literal(Token* token);
    virtual ~Literal();

    const Token* token() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pToken;
};

//
// SliceExpr
//

class SliceExpr : public Expr
{
public:
    SliceExpr(Expr* sliceable, Expr* start, Expr* stop);
    virtual ~SliceExpr();

    const Expr* sliceable() const;
    const Expr* start() const;
    const Expr* stop() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// IndexExpr
//

class IndexExpr : public Expr
{
public:
    IndexExpr(Expr* indexable, Expr* index);
    virtual ~IndexExpr();

    const Expr* indexable() const;
    const Expr* index() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// TernaryOp
//

class TernaryOp : public Expr
{
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
};

//
// UnaryOp
//

class UnaryOp : public Expr
{
public:
    UnaryOp(Token* op, Expr* right);
    virtual ~UnaryOp();

    const Token* op() const;
    const Expr* rexpr() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pOp;
};

//
// Var
//

class Var : public Expr
{
public:
    Var(Token* scope, Token* name);
    virtual ~Var();

    const Token* scope() const;
    const Token* name() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pScope;
    Token* m_pName;
};

//
// AssignStmt
//

class AssignStmt : public Stmt
{
public:
    AssignStmt(Token* op, Var* var, Expr* expr);
    virtual ~AssignStmt();

    const Token* op() const;
    const Var* var() const;
    const Expr* expr() const;

    virtual void accept(ASTVisitor& visitor) const;

private:
    virtual std::string str_a() const;

    Token* m_pOp;
};

//
// CommentStmt
//

class CommentStmt : public Stmt
{
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
};

};
