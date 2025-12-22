#ifndef CONTEXT_H
#define CONTEXT_H

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

private:
    Settings m_cSettings;
    Source m_cSource;
};

#endif  // CONTEXT_H
