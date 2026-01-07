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
// BinaryOp
//

class BinaryOp : public Expr
{
public:
    BinaryOp(Token* op, Expr* left, Expr* right);
    virtual ~BinaryOp();

    const Token* op() const;
    const Expr* left() const;
    const Expr* right() const;

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
    SliceExpr(Token* op, Expr* left, Expr* right);
    virtual ~SliceExpr();

    const Token* op() const;
    const Expr* left() const;
    const Expr* right() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pOp;
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
    const Expr* right() const;

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
    Var(Token* token);
    virtual ~Var();

    const Token* token() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pToken;
};

};
