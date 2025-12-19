#ifndef CONTEXT_H
#define CONTEXT_H

#include "Settings.h"

class Context
{
public:
    Context();
    ~Context();

    Settings& settings();

private:
    Settings m_cSettings;
};

#endif  // CONTEXT_H
