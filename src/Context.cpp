#include "Context.h"

Context::Context()
{
}

Context::~Context()
{
}

Settings& Context::settings()
{
    return m_cSettings;
}
