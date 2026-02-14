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

    int pos() const;
    int line() const;
    int column() const;
    int indent() const;
    bool eof() const;
    std::string path() const;
    std::string traceback() const;
    std::string_view text() const;
    std::string_view line_text() const;
    std::string_view remaining_text() const;

    void read_text(const std::string& text);
    void read_file(const fs::path& path);

    void seek(int pos);
    void advance(int count);

private:
    int m_nPos = 0;
    fs::path m_cPath;
    std::string m_sText;
};

#endif  // SOURCE_H
