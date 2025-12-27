#include "Scope.h"

Scope::Scope(Scope* apScope) :
    m_pParent { apScope }
{
}

Scope::~Scope()
{
    for (auto it = m_mSymbols.cbegin(); it != m_mSymbols.cend(); it++)
    {
        delete it->second;
    }

    m_mSymbols.clear();
}

Scope* Scope::parent() const
{
    return m_pParent;
}

void Scope::set_parent(Scope* apScope)
{
    m_pParent = apScope;
}

void Scope::insert(const Symbol* apSymbol)
{
    m_mSymbols.insert({ apSymbol->name(), apSymbol });
}

void Scope::insert(const std::string& asName, Symbol::Type aeType)
{
    Symbol* pSymbol = new Symbol(aeType, asName);
    insert(pSymbol);
}

const Symbol* Scope::lookup(const std::string& asName) const
{
    auto it = m_mSymbols.find(asName);

    if (it != m_mSymbols.end())
    {
        return it->second;
    }
    else if (m_pParent == nullptr)
    {
        return nullptr;
    }

    return m_mSymbols.at(asName);
}
