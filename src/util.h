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
 *      Return base name of file (ie, filename up to the first dot).
 *
 *  @example
 *      bar             -> bar
 *      bar.txt         -> bar
 *      bar.tar.gz      -> bar
 *      /foo/bar.txt    -> bar
 *      /foo/bar.tar.gz -> bar
 */
std::string base_name(const std::filesystem::path& path);

};
