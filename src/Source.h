#ifndef SOURCE_H
#define SOURCE_H

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class Source
{
public:
    Source();
    ~Source();

    bool eof() const;
    size_t pos() const;
    size_t line() const;
    size_t column() const;
    std::string path() const;
    std::string traceback() const;
    std::string_view text() const;
    std::string_view line_text() const;
    std::string_view remaining_text() const;

    void read_text(const std::string& text);
    void read_file(const fs::path& path);

    void advance(size_t count);

private:
    size_t m_nPos = 0;
    size_t m_nLine = 0;
    size_t m_nColumn = 0;
    fs::path m_cPath;
    std::string m_sText;
};

#endif  // SOURCE_H
