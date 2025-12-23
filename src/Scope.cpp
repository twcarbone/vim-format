#include "Scope.h"

Scope::Scope(Scope* apScope) :
    m_pParent { apScope }
{
}

void Scope::insert(const Symbol& acSymbol)
{
    m_mSymbols.insert({ acSymbol.name(), acSymbol });
}

const Symbol& Scope::lookup(const std::string& asName) const
{
    return m_mSymbols.at(asName);
}
