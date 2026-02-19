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

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
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
// IfStmt
//

class IfStmt : public Stmt
{
public:
    IfStmt(Expr* condition, StmtList* then_stmts, StmtList* else_smts);
    virtual ~IfStmt();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// WhileStmt
//

class WhileStmt : public Stmt
{
public:
    WhileStmt(Expr* condition, StmtList* stmts);
    virtual ~WhileStmt();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// ForStmt
//

class ForStmt : public Stmt
{
public:
    ForStmt(Expr* item, Expr* items, StmtList* stmts);
    virtual ~ForStmt();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;
};

//
// JumpStmt
//

class JumpStmt : public Stmt
{
public:
    JumpStmt(Token* token, Expr* expr);
    virtual ~JumpStmt();

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
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

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
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
// IndexExpr
//

class IndexExpr : public Expr
{
public:
    IndexExpr(Expr* indexable, Expr* start, Expr* stop);
    virtual ~IndexExpr();

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

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
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

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    bool m_bTrailing;
    Token* m_pComment;
};

};
