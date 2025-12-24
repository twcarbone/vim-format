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
    virtual ~AST() = default;

    void set_token(Token* token);

protected:
    Token* m_pToken;
};

//
// CmdExpr
//

class CmdExpr : public AST
{
public:
    CmdExpr(AST* parent);
    virtual ~CmdExpr() = default;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// BinOp
//

class BinOp : public AST
{
public:
    BinOp(AST* parent);
    virtual ~BinOp() = default;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

//
// Literal
//

class Literal : public AST
{
public:
    Literal(AST* parent);
    virtual ~Literal() = default;

    virtual std::string toString() const override;
    virtual void accept(NodeVisitor& visitor) const override;
};

#endif  // AST_H
