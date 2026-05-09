#include <iostream>

#include "ASTVisitor.h"
#include "ast.h"

ASTVisitor::ASTVisitor(std::ostream& acOutStream) :
    m_nDepth { 0 },
    m_cOutStream { acOutStream }
{
}

// clang-format off
#define X(name) void ASTVisitor::visit(const ast::name* ast) { handle_node(ast); }
    VISITED_AST_NODES
#undef X

// clang-format on

void ASTVisitor::handle_node(const ast::Node* apNode)
{
    write_line(apNode->toString());

    m_lVisitedNodes.push_back({ m_nDepth, apNode });

    m_nDepth++;

    for (ast::Node* pNode : apNode->children())
    {
        if (pNode == nullptr)
        {
            write_line("NULL");
        }
        else
        {
            pNode->accept(*this);
        }
    }

    m_nDepth--;
}

void ASTVisitor::write_line(std::string_view asText)
{
    for (size_t i = 0; i < m_nDepth; i++)
    {
        m_cOutStream << "  ";
    }

    m_cOutStream << asText << std::endl;
}
