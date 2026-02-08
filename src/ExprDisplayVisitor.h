#pragma once

#include <string>

#include "ASTVisitor.h"

namespace ast
{
class BinaryOp;
class Expr;
class Literal;
class SliceExpr;
class UnaryOp;
class Var;
};

class ExprDisplayVisitor : public ASTVisitor
{
public:
    ExprDisplayVisitor() = default;
    ~ExprDisplayVisitor() = default;

    /**
     *  @brief
     *      Return the expression AST `root` in prefix notation.
     *
     *  @example
     *      1 + 2 * 3               => (+ 1 (* 2 3))
     *      -1 + 2 * -3 / 4 - 5     => (- (+ (- 1) (/ (* 2 (- 3)) 4)) 5)
     */
    std::string display(ast::Expr* expr);

    virtual void visit(const ast::BinaryOp* ast) override;
    virtual void visit(const ast::Literal* ast) override;
    virtual void visit(const ast::SliceExpr* ast) override;
    virtual void visit(const ast::TernaryOp* ast) override;
    virtual void visit(const ast::UnaryOp* ast) override;
    virtual void visit(const ast::Var* ast) override;

private:
    std::string m_sDisplay;
};
