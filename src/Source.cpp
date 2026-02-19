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
        if (view().at(i) == '\n')
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
        if (view().at(i - 1) == '\n' && i < view().size() - 1)
        {
            // Reset the column only after \n that is not the last character
            lnColumn = 0;
        }
        else if (i != view().size())
        {
            lnColumn++;
        }
    }

    return lnColumn;
}

int Source::indent() const
{
    if (line() == 0)
    {
        return view().find_first_not_of(" \t");
    }

    size_t lnCurosrStart = m_nPos - 1;

    if (eof())
    {
        lnCurosrStart--;
    }

    size_t lnStartOfLine = view().rfind('\n', lnCurosrStart) + 1;
    size_t lnStartOfContent = view().find_first_not_of(" \t", lnStartOfLine);
    return lnStartOfContent - lnStartOfLine;
}

int Source::bol() const
{
    if (line() == 0)
    {
        return 0;
    }
    else if (eof())
    {
        return view().rfind('\n', m_nPos - 2) + 1;
    }

    return view().rfind('\n', m_nPos - 1) + 1;
}

int Source::eol() const
{
    if (eof())
    {
        return m_nPos - 1;
    }

    size_t lnEOL = view().find('\n', m_nPos);

    if (lnEOL == std::string_view::npos)
    {
        return view().size();
    }

    return lnEOL;
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
    std::string lsColumnNum = std::to_string(column() + 1);

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
    // Start looking from one character behind in case we are on a newline
    size_t lnStart = view().rfind('\n', m_nPos - 1);
    size_t lnEnd = view().find('\n', m_nPos);

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
        lnEnd = view().size();
    }

    return view().substr(lnStart, lnEnd - lnStart);
}

std::string_view Source::remaining_text() const
{
    return view().substr(m_nPos);
}

std::string_view Source::remaining_line() const
{
    return view().substr(m_nPos, eol() - m_nPos);
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
