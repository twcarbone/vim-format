#include <fstream>

#include "Source.h"
#include "util.h"

Source::Source()
{
}

Source::~Source()
{
}

int Source::line() const
{
    int lnLine = 0;

    // If we are at EOF, looking 1 behind will see the final newline of the file. So we
    // need to start looking 2 behind.
    for (int i = eof() ? m_nPos - 2 : m_nPos - 1; i >= 0; i--)
    {
        if (m_sText[i] == '\n')
        {
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

int Source::indent() const
{
    std::string_view lsView { m_sText };

    if (line() == 0)
    {
        return lsView.find_first_not_of(" \t");
    }

    size_t lnCurosrStart = m_nPos - 1;

    if (eof())
    {
        lnCurosrStart--;
    }

    size_t lnStartOfLine = lsView.rfind('\n', lnCurosrStart) + 1;
    size_t lnStartOfContent = lsView.find_first_not_of(" \t", lnStartOfLine);
    return lnStartOfContent - lnStartOfLine;
}

std::string Source::path() const
{
    if (m_cPath.empty())
    {
        return "stdin";
    }

    return m_cPath.string();
}

std::string Source::context() const
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
    m_cPath = acPath;
    m_sText = vf::read_file(acPath);
}
