#ifndef SCOPE_H
#define SCOPE_H

#include <unordered_map>

#include "Symbol.h"

class Scope
{
public:
    Scope(Scope* parent = nullptr);
    ~Scope();

    Scope* parent() const;
    void set_parent(Scope* scope);

    void insert(const Symbol* symbol);
    void insert(const std::string& name, Symbol::Type type);
    const Symbol* lookup(const std::string& name) const;

private:
    Scope* m_pParent;
    std::unordered_map<std::string, const Symbol*> m_mSymbols;
};

#endif  // SCOPE_H
