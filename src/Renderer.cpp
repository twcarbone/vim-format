#include "Renderer.h"
#include "Settings.h"
#include "doc.h"

Renderer::Renderer(std::ostream& acOutStream) :
    DocVisitor(acOutStream),
    m_eMode { Mode::FLAT },
    m_nLevel { 0 },
    m_nColumn { 0 }
{
}

void Renderer::visit(const doc::Text* apDoc)
{
    write(apDoc->sText);
}

void Renderer::visit(const doc::Line* apLine)
{
    switch (m_eMode)
    {
        case Mode::FLAT:
            write(' ', apLine->nWidth);
            break;
        case Mode::BREAK:
            write(' ');  // Intentionally add trailing whitespace before newline
            write('\n');
            write(' ', m_nLevel * 4);
            write('\\');
    }
}

void Renderer::visit(const doc::Break* apDoc)
{
    write('\n');
    write(' ', m_nLevel * 4);
}

void Renderer::visit(const doc::Group* apDoc)
{
    Mode eOldMode = m_eMode;

    if (fits(apDoc))
    {
        m_eMode = Mode::FLAT;
    }
    else
    {
        m_eMode = Mode::BREAK;
    }

    for (const doc::Doc* pChildDoc : apDoc->lChildren)
    {
        pChildDoc->accept(*this);
    }

    m_eMode = eOldMode;
}

void Renderer::visit(const doc::Nest* apDoc)
{
    m_nLevel++;

    for (const doc::Doc* pChildDoc : apDoc->lChildren)
    {
        pChildDoc->accept(*this);
    }

    m_nLevel--;
}

bool Renderer::fits(const doc::Doc* apDoc)
{
    return apDoc->nWidth < Settings::ColumnLimit;
}

void Renderer::write(const std::string_view asText)
{
    m_cOutStream << asText;
    m_nColumn += asText.size();
}

void Renderer::write(char anChar, size_t anCount)
{
    m_cOutStream << std::string(anCount, anChar);

    switch (anChar)
    {
        case '\n':
            m_nColumn = 0;
            break;
        default:
            m_nColumn += anCount;
    }
}
