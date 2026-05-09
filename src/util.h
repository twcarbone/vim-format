#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace vf
{

const std::string DIGITS_BASE_10 = "0123456789";
const std::string DIGITS_BASE_16 = "0123456789abcdefABCDEF";

std::string read_file(const std::filesystem::path& path);

/**
 *  @brief
 *      Return true if `str` begins with `prefix`, delimited by one of `delim`.
 *
 *  @example
 *      startswith("integer", "int")            => true
 *      startswith("integer", "int", " ")       => false
 *      startswith("int i", "int", " ")         => true
 */
bool startswith(std::string_view str, std::string_view prefix, std::string_view delim = "");

/**
 *  @brief
 *      Return true if `str` starts with a valid int and set `out` to the int.
 *
 *      If `base` is not nullptr and an integer is found, set `base`.
 *
 *  @example
 *      ```
 *      std::string_view out;
 *      int base;
 *      startswith_int("42abc", out, &base);    // true, out = "42", base = 10
 *      ```
 */
bool startswith_int(std::string_view str, std::string_view& out, int* base = nullptr);

/**
 *  @brief
 *      Split a string by a delimiter. Empty parts are kept.
 *
 *  @example
 *      split("hello", '.')             => { "hello" }
 *      split("a + b ", '+')            => { "a ", " b "}
 *      split("one,,two,three,", ',')   => { "one", "", "two", "three", ""}
 */
std::vector<std::string> split(const std::string& str, char delimiter);

/**
 *  @brief
 *      Return true if `str` starts with a valid float and set `out` to the float.
 */
bool startswith_float(std::string_view str, std::string_view& out);

inline bool is_one_of(std::string_view str, std::string_view any)
{
    return str.size() == 1 && str.find_first_of(any) != std::string_view::npos;
}

/**
 *  @brief
 *      The last slash-separated portion of the path.
 */
std::string name(const std::filesystem::path& path);

/**
 *  @brief
 *      The filename with the non-first dot and tail removed.
 */
std::string stem(const std::filesystem::path& path);

/**
 *  @brief
 *      Everything right of the non-first dot in the filename.
 */
std::string tail(const std::filesystem::path& path);

std::filesystem::path with_tail(const std::filesystem::path& path, const std::string& tail);

};
