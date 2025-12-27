#ifndef AST_H
#define AST_H

#include "Analyzer.h"
#include "Node.h"
#include "NodeVisitor.h"
#include "Token.h"

//
// AST
//

class AST : public Node
{
public:
    AST(AST* parent);

    void set_token(Token* token);

protected:
    Token* m_pToken;
};

//
// Program
//

class Program : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// StmtList
//

class StmtList : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// CmdExpr
//

class CmdExpr : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// BinOp
//

class BinOp : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// UnaryOp
//

class UnaryOp : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// Literal
//

class Literal : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// Var
//

class Var : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// Assign
//

class Assign : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// ListExpr
//

class ListExpr : public AST
{
public:
    using AST::AST;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

#endif  // AST_H
