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

void Context::add_path(const std::string& asPath)
{
    m_cSource.read_file(asPath);
}

void Context::add_text(const std::string& asText)
{
    m_cSource.read_text(asText);
}

const Source& Context::source() const
{
    return m_cSource;
}
