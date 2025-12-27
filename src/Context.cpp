#include "Context.h"

Context::Context() :
    m_pScope { new Scope() }
{
}

Context::~Context()
{
    for (Scope* pScope = pop_scope(); pScope != nullptr; pScope = pop_scope())
    {
        delete pScope;
    }
}

Settings& Context::settings()
{
    return m_cSettings;
}

void Context::add_path(const std::string& asPath)
{
    m_cSource.read_file(asPath);
}

void Context::add_text(const std::string& asText)
{
    m_cSource.read_text(asText);
}

const Source& Context::source() const
{
    return m_cSource;
}

Scope* Context::curr_scope()
{
    return m_pScope;
}

void Context::push_scope()
{
    m_pScope = new Scope(m_pScope);
}

Scope* Context::pop_scope()
{
    Scope* pOld = m_pScope;
    pOld->set_parent(nullptr);
    m_pScope = pOld->parent();
    return pOld;
}
