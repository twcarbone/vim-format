#ifndef CONTEXT_H
#define CONTEXT_H

#include "Scope.h"
#include "Settings.h"
#include "Source.h"

class Context
{
public:
    Context();
    ~Context();

    const Source& source() const;
    Settings& settings();

    void add_path(const std::string& path);
    void add_text(const std::string& text);

    void push_scope();
    Scope* curr_scope();
    Scope* pop_scope();

private:
    Settings m_cSettings;
    Source m_cSource;
    Scope* m_pScope;
};

#endif  // CONTEXT_H
