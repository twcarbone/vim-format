#include "Symbol.h"

Symbol::Symbol(Symbol::Type aeType, const std::string& asName) :
    m_eType { aeType },
    m_sName { asName }
{
}

std::string Symbol::name() const
{
    return m_sName;
}

Symbol::Type Symbol::type() const
{
    return m_eType;
}
