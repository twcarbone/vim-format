#pragma once

#include <iostream>

// clang-format off

#define VISITED_DOC_NODES   \
    X(Text)                 \
    X(Line)                 \
    X(Break)                \
    X(Group)                \
    X(Nest)

// clang-format on

namespace doc
{
#define X(name) class name;
VISITED_DOC_NODES
#undef X
class Doc;
};

class DocVisitor
{
public:
    DocVisitor(std::ostream& os = std::cout);
    ~DocVisitor() = default;

#define X(name) virtual void visit(const doc::name* doc);
    VISITED_DOC_NODES
#undef X

protected:
    std::ostream& m_cOutStream;

private:
    int m_nDepth;

    void visit_doc(const doc::Doc* doc);
};
