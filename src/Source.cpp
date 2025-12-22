#include <fstream>

#include "Source.h"

Source::Source()
{
}

Source::~Source()
{
}

bool Source::eof() const
{
    return m_nPos == m_sText.size();
}

size_t Source::pos() const
{
    return m_nPos;
}

size_t Source::line() const
{
    return m_nLine;
}

size_t Source::column() const
{
    return m_nColumn;
}

std::string_view Source::text() const
{
    return m_sText;
}

std::string_view Source::line_text() const
{
    std::string_view lsView { m_sText };

    // Start looking from one character behind in case we are on a newline
    size_t lnStart = lsView.rfind('\n', m_nPos - 1);
    size_t lnEnd = lsView.find('\n', m_nPos);

    if (lnStart == std::string_view::npos)
    {
        lnStart = 0;
    }
    else
    {
        lnStart++;
    }

    if (lnEnd == std::string_view::npos)
    {
        lnEnd = m_sText.size();
    }

    return lsView.substr(lnStart, lnEnd - lnStart);
}

std::string_view Source::remaining_text() const
{
    return std::string_view(m_sText).substr(m_nPos);
}

void Source::advance(size_t anCount)
{
    while (m_nPos < m_sText.size() && anCount > 0)
    {
        if (m_sText[m_nPos] != '\n')
        {
            m_nColumn++;
        }
        else if (m_nPos < m_sText.size() - 1)
        {
            // Do not increment the line/column at the final newline
            m_nLine++;
            m_nColumn = 0;
        }

        anCount--;
        m_nPos++;
    }
}

void Source::read_text(const std::string& asText)
{
    m_sText = asText;
}

void Source::read_file(const fs::path& acPath)
{
    m_cPath = acPath;

    std::ifstream lcIfStream { m_cPath };

    if (!lcIfStream.is_open())
    {
        throw std::runtime_error("Error: cannot open for reading: " + m_cPath.string());
    }

    char c;
    while (lcIfStream.get(c))
    {
        m_sText += c;
    }
}
