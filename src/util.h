#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace vf
{

std::string read_file(const std::filesystem::path& path);

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

};
