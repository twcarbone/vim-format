#include "DocVisitor.h"
#include "doc.h"

DocVisitor::DocVisitor(std::ostream& acOutStream) :
    m_cOutStream { acOutStream },
    m_nDepth { 0 }
{
}

// clang-format off
#define X(name) void DocVisitor::visit(const doc::name* doc) { visit_doc(doc); }
    VISITED_DOC_NODES
#undef X

// clang-format on

void DocVisitor::visit_doc(const doc::Doc* apDoc)
{
    m_cOutStream << std::string(m_nDepth * 2, ' ');
    m_cOutStream << apDoc->to_string();
    m_cOutStream << std::endl;

    m_nDepth++;

    for (const doc::Doc* pDoc : apDoc->lChildren)
    {
        pDoc->accept(*this);
    }

    m_nDepth--;
}
