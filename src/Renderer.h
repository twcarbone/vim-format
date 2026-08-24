#pragma once

#include <iostream>
#include <vector>

#include "DocVisitor.h"

class Renderer : public DocVisitor
{
public:
    Renderer(std::ostream& os = std::cout);
    ~Renderer() = default;

#define X(name) virtual void visit(const doc::name* doc);
    VISITED_DOC_NODES
#undef X

private:
    enum class Mode
    {
        FLAT,
        BREAK,
    };

    bool fits(const doc::Doc* doc);

    void write(const std::string_view text);
    void write(char c, size_t count = 1);

    Mode m_eMode = Mode::FLAT;
    size_t m_nLevel = 0;
    size_t m_nColumn = 0;
    std::vector<Mode> m_lModeStack;
};
