#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string_view>

#include "Context.h"

class Controller
{
public:
    Controller(Context context);
    ~Controller();

    void compile();
    void compile(std::string_view text);

private:
    Context m_cContext;
};

#endif  // CONTROLLER_H
