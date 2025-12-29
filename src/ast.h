#pragma once

#include <string>

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
// BinaryOp
//

class BinaryOp : public Node
{
public:
    BinaryOp(Token* op, Node* left, Node* right);
    virtual ~BinaryOp();

    const Token* op() const;
    const Node* left() const;
    const Node* right() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pOp;
    Node* m_pLeft;
    Node* m_pRight;
};

//
// Literal
//

class Literal : public Node
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
// UnaryOp
//

class UnaryOp : public Node
{
public:
    UnaryOp(Token* op, Node* right);
    virtual ~UnaryOp();

    const Token* op() const;
    const Node* right() const;

    virtual std::string toString() const;
    virtual void accept(ASTVisitor& visitor) const;

private:
    Token* m_pOp;
    Node* m_pRight;
};

//
// Var
//

class Var : public Node
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
