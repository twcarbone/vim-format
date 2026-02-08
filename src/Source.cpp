#include <fstream>

#include "Source.h"

Source::Source()
{
}

Source::~Source()
{
}

int Source::pos() const
{
    return m_nPos;
}

int Source::line() const
{
    int lnLine = 0;
    for (int i = 0; i <= m_nPos; i++)
    {
        if (m_sText[i] == '\n' && i < m_sText.size() - 1)
        {
            // Advance the line only after \n that is not the last character
            lnLine++;
        }
    }

    return lnLine;
}

int Source::column() const
{
    int lnColumn = 0;
    for (int i = 1; i <= m_nPos; i++)
    {
        if (m_sText[i - 1] == '\n' && i < m_sText.size() - 1)
        {
            // Reset the column only after \n that is not the last character
            lnColumn = 0;
        }
        else if (i != m_sText.size())
        {
            lnColumn++;
        }
    }

    return lnColumn;
}

bool Source::eof() const
{
    return m_nPos == m_sText.size();
}

std::string Source::path() const
{
    if (m_cPath.empty())
    {
        return "stdin";
    }

    return m_cPath.string();
}

std::string Source::traceback() const
{
    std::string lsTraceback;

    std::string lsLineNum = std::to_string(line() + 1);
    std::string lsColumnNum = std::to_string(column());

    lsTraceback += path() + ":" + lsLineNum + ":" + lsColumnNum;
    lsTraceback += "\n " + lsLineNum + " | " + std::string(line_text());
    lsTraceback += "\n " + std::string(lsLineNum.size(), ' ') + " | " + std::string(column(), ' ') + "^";

    return lsTraceback;
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

void Source::seek(int anPos)
{
    m_nPos = anPos;
}

void Source::advance(int anCount)
{
    m_nPos += anCount;
}

void Source::read_text(const std::string& asText)
{
    m_sText = asText;
}

void Source::read_file(const fs::path& acPath)
{
    // TODO (gh-39): Use vf::read_file() to read files in Source

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
