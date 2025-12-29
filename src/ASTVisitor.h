#pragma once

namespace ast
{
class BinaryOp;
class Literal;
class UnaryOp;
class Var;
};

class ASTVisitor
{
public:
    virtual void visit(const ast::BinaryOp* ast);
    virtual void visit(const ast::Literal* ast);
    virtual void visit(const ast::UnaryOp* ast);
    virtual void visit(const ast::Var* ast);
};
