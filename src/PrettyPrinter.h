#pragma once

#include <iostream>

#include "ASTVisitor.h"

class Indent
{
public:
    Indent operator++(int);
    Indent operator--(int);
    friend std::ostream& operator<<(std::ostream& os, const Indent& indent);

private:
    size_t m_nLevel = 0;
    size_t m_nTabStop = 4;
};

class PrettyPrinter : public ASTVisitor

{
public:
    PrettyPrinter(std::ostream& os = std::cout);
    ~PrettyPrinter() = default;

    virtual void visit(const ast::AssignStmt* ast);
    virtual void visit(const ast::BinaryOp* ast);
    virtual void visit(const ast::CallExpr* ast);
    virtual void visit(const ast::CasedBinaryOp* ast);
    virtual void visit(const ast::CommentStmt* ast);
    virtual void visit(const ast::DictEntry* ast);
    virtual void visit(const ast::DictExpr* ast);
    virtual void visit(const ast::ExprCmd* ast);
    virtual void visit(const ast::FnArgList* ast);
    virtual void visit(const ast::FnParam* ast);
    virtual void visit(const ast::FnParamList* ast);
    virtual void visit(const ast::FnStmt* ast);
    virtual void visit(const ast::ForStmt* ast);
    virtual void visit(const ast::IfStmt* ast);
    virtual void visit(const ast::IndexExpr* ast);
    virtual void visit(const ast::JumpStmt* ast);
    virtual void visit(const ast::ListExpr* ast);
    virtual void visit(const ast::Literal* ast);
    virtual void visit(const ast::MethodCallExpr* ast);
    virtual void visit(const ast::Program* ast);
    virtual void visit(const ast::StmtList* ast);
    virtual void visit(const ast::TernaryOp* ast);
    virtual void visit(const ast::UnaryOp* ast);
    virtual void visit(const ast::Var* ast);
    virtual void visit(const ast::WhileStmt* ast);

private:
    void write_ln(const std::string& line);
    void write_bol();
    void write_eol();
    void write_txt(const std::string& text);

    Indent m_cIndent;
};
