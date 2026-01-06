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
    virtual ~Node() = default;

    virtual std::string toString() const = 0;
    virtual void accept(ASTVisitor& visitor) const = 0;
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

private:
    std::vector<Stmt*> m_lStmnts;
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

private:
    StmtList* m_pStmtList;
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

private:
    Expr* m_pCondition;
    StmtList* m_pThenStmtList;
    StmtList* m_pElseStmtList;
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
    Expr* m_pLeft;
    Expr* m_pRight;
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
    Expr* m_pLeft;
    Expr* m_pRight;
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
    Expr* m_pRight;
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
