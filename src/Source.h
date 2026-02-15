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

    // 0-based index of cursor.
    // Can be equal to string size if at EOF.
    [[nodiscard]] int pos() const;

    // 0-based index of current line.
    int line() const;

    // 0-based index within current line.
    // Does not increment when on newline or EOF.
    int column() const;

    // Number of leading whitespace characters in current line.
    int indent() const;

    // True if cursor is at EOF.
    [[nodiscard]] bool eof() const;

    // 0-based index of the first character of the current line.
    [[nodiscard]] int bol() const;

    // 0-based index of the next newline ahead of the cursor.
    // If on EOF, index of preceding character.
    [[nodiscard]] int eol() const;

    // Path to source file.
    std::string path() const;

    // Basic printout of current line context.
    std::string context() const;

    // All text from 0 to EOF minus one.
    std::string_view text() const;

    // Text of current line.
    // If on newline or EOF, text is from preceding characters.
    std::string_view line_text() const;

    // All text from cursor to EOF minus one.
    std::string_view remaining_text() const;

    // All text from cursor to the next newline (exclusive).
    std::string_view remaining_line() const;

    // Move cursor to 0-based `pos`.
    void seek(int pos);

    // Advance the cursor forward `count` positions.
    void advance(int count);

    void read_text(const std::string& text);
    void read_file(const fs::path& path);

private:
    int m_nPos = 0;
    fs::path m_cPath;
    std::string m_sText;
};

inline int Source::pos() const
{
    return m_nPos;
}

inline bool Source::eof() const
{
    // This does not check for '\0' on purpose! std::string can contain embedded '\0'.
    return m_nPos == m_sText.size();
}

#endif  // SOURCE_H
