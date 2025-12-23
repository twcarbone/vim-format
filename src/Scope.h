#ifndef SCOPE_H
#define SCOPE_H

#include <unordered_map>

#include "Symbol.h"

class Scope
{
public:
    Scope(Scope* parent = nullptr);
    ~Scope();

    void insert(const Symbol& symbol);
    const Symbol& lookup(const std::string& name) const;

private:
    Scope* m_pParent;
    std::unordered_map<std::string, Symbol> m_mSymbols;
};

#endif  // SCOPE_H
