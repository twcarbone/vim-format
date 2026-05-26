#pragma once

#include <vector>

#include "ASTVisitor.h"
#include "doc.h"

class DocBuilder : public ASTVisitor
{
public:
#define X(name) virtual void visit(const ast::name* ast);
    VISITED_AST_NODES
#undef X

    doc::Doc* root() const;

private:
    void push_text(const std::string& text);
    void push_text(char c, size_t count = 1);
    void push_line(size_t width = 0);
    void push_deferred_line(size_t width = 0);
    void push_break();

    void push_nest();
    void push_group();
    void pop();

    void push_str_expr(const ast::StrExpr* str_expr);

    void visit_children(const ast::Node* parent, const std::string& delimiter = "");

    size_t m_nDeferredLine = 0;
    std::vector<doc::Doc*> m_lDocStack;
};
