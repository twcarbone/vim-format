#pragma once

#include <iostream>

#include "ASTVisitor.h"

class Indent
{
public:
    Indent(size_t tabstop);

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

#define X(name) virtual void visit(const ast::name* ast);
    VISITED_AST_NODES
#undef X

private:
    void write_bol();
    void write_eol();
    void write(const std::string& text);
    void write(char c, size_t count = 1);

    bool m_bNewlinePending;
    Indent m_cIndent;
};
