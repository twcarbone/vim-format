#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

class Symbol
{
public:
    enum class Type
    {
        Number,
        Float,
        String,
        List,
        Dictionary,
        Funcref,
        Special,
        Job,
        Channel,
        Blob
    };

    Symbol(Type type, const std::string& name);
    ~Symbol();

    std::string name() const;

    Symbol::Type type() const;

private:
    Type m_eType;
    std::string m_sName;
};

#endif  // SYMBOL_H
